#!/usr/bin/env python3
"""
bit_classifier.py — Train and run a 0/1 bit classifier for microcode extraction

- Folder-based training via torchvision.datasets.ImageFolder with subdirs '0' and '1' (must pre-sort ~1000 bits)
- Saves best model and labels.json
- Predictor accepts: single image, a directory of images, OR a .zip containing images
- Optional CSV output for predictions

This software is released into the public domain as it was cobbled together from various tutorials.

NOTE!!: How you install pytorch is important. If you just do 'pip install torch' you will get the CPU-accellerated (slow) version. 
        Follow the instructions on pytorch's website if you have an nVidia GPU so you get CUDA accelleration.

Usage
  Train:
    python bit_classifier.py train --data ./data --out ./model_out --img-size 64 --grayscale
  Predict (dir / file):
    python bit_classifier.py predict --model-dir ./model_out --input ./some_dir --img-size 64 --grayscale
  Predict (zip):
    python bit_classifier.py predict --model-dir ./model_out --input ./images.zip --img-size 64 --grayscale --out-csv predictions.csv
"""

import argparse
import csv
import io
import json
import os
from dataclasses import dataclass
from pathlib import Path
from typing import Tuple, List, Iterable

import numpy as np
from PIL import Image
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Subset
from torchvision import datasets, transforms, models


# ----------------------------
# Config / Utilities
# ----------------------------

@dataclass
class Config:
    data_dir: Path
    out_dir: Path
    img_size: int = 64
    batch_size: int = 64
    epochs: int = 15
    lr: float = 1e-3
    weight_decay: float = 1e-4
    train_split: float = 0.8
    grayscale: bool = True
    num_workers: int = 4
    seed: int = 42
    model: str = "cnn"      # "cnn" or "resnet18"
    freeze_backbone: bool = True  # if using resnet18
    aug: bool = True


def set_seed(seed: int):
    import random
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.deterministic = False
    torch.backends.cudnn.benchmark = True


# ----------------------------
# Models
# ----------------------------

class TinyCNN(nn.Module):
    def __init__(self, in_ch: int, num_classes: int = 2):
        super().__init__()
        self.features = nn.Sequential(
            nn.Conv2d(in_ch, 32, 3, padding=1), nn.ReLU(inplace=True),
            nn.Conv2d(32, 32, 3, padding=1), nn.ReLU(inplace=True),
            nn.MaxPool2d(2),

            nn.Conv2d(32, 64, 3, padding=1), nn.ReLU(inplace=True),
            nn.Conv2d(64, 64, 3, padding=1), nn.ReLU(inplace=True),
            nn.MaxPool2d(2),

            nn.Conv2d(64, 128, 3, padding=1), nn.ReLU(inplace=True),
            nn.MaxPool2d(2),
        )
        # classifier will be adapted to img_size at runtime
        self.classifier = nn.Identity()

    def adapt_fc(self, img_size: int, num_classes: int = 2):
        dummy = torch.zeros(1, self.features[0].in_channels, img_size, img_size)
        with torch.no_grad():
            feat = self.features(dummy)
            n = feat.numel()
        self.classifier = nn.Sequential(
            nn.Flatten(),
            nn.Linear(n, 128),
            nn.ReLU(inplace=True),
            nn.Dropout(0.25),
            nn.Linear(128, num_classes),
        )

    def forward(self, x):
        x = self.features(x)
        return self.classifier(x)


def build_model(cfg: Config, in_channels: int, num_classes: int = 2) -> nn.Module:
    if cfg.model == "resnet18":
        m = models.resnet18(weights=models.ResNet18_Weights.DEFAULT)
        if cfg.grayscale:
            old = m.conv1
            m.conv1 = nn.Conv2d(1, old.out_channels, kernel_size=old.kernel_size,
                                stride=old.stride, padding=old.padding, bias=False)
            with torch.no_grad():
                m.conv1.weight[:] = old.weight.mean(dim=1, keepdim=True)
        if cfg.freeze_backbone:
            for p in m.parameters():
                p.requires_grad = False
            for p in m.layer4.parameters():
                p.requires_grad = True
        m.fc = nn.Linear(m.fc.in_features, num_classes)
        return m
    else:
        m = TinyCNN(in_channels, num_classes)
        m.adapt_fc(cfg.img_size, num_classes)
        return m


# ----------------------------
# Data & Transforms
# ----------------------------

def make_transforms(cfg: Config) -> Tuple[transforms.Compose, transforms.Compose]:
    to_gray = [transforms.Grayscale(num_output_channels=1)] if cfg.grayscale else []

    train_tf = [*to_gray, transforms.Resize((cfg.img_size, cfg.img_size))]
    if cfg.aug:
        train_tf += [
            transforms.RandomApply([transforms.RandomRotation(5)], p=0.3),
            transforms.RandomApply([transforms.ColorJitter(brightness=0.1, contrast=0.1)], p=0.3),
        ]
    train_tf += [transforms.ToTensor()]

    val_tf = [*to_gray, transforms.Resize((cfg.img_size, cfg.img_size)), transforms.ToTensor()]
    return transforms.Compose(train_tf), transforms.Compose(val_tf)


def make_dataloaders(cfg: Config) -> Tuple[DataLoader, DataLoader, List[str], torch.Tensor]:
    train_tf, val_tf = make_transforms(cfg)

    # Base just to get samples/targets and class names
    base = datasets.ImageFolder(str(cfg.data_dir), transform=None)
    class_names = base.classes
    targets = np.array([s[1] for s in base.samples])

    class_counts = np.bincount(targets, minlength=len(class_names))
    weights = 1.0 / np.maximum(class_counts, 1)
    class_weights = torch.tensor(weights / weights.sum() * len(weights), dtype=torch.float32)

    indices = np.arange(len(base))
    rng = np.random.default_rng(cfg.seed)
    rng.shuffle(indices)
    split = int(cfg.train_split * len(indices))
    train_idx, val_idx = indices[:split], indices[split:]

    train_ds = datasets.ImageFolder(str(cfg.data_dir), transform=train_tf)
    val_ds = datasets.ImageFolder(str(cfg.data_dir), transform=val_tf)

    train_subset = Subset(train_ds, train_idx.tolist())
    val_subset = Subset(val_ds, val_idx.tolist())

    train_loader = DataLoader(train_subset, batch_size=cfg.batch_size, shuffle=True,
                              num_workers=cfg.num_workers, pin_memory=True)
    val_loader = DataLoader(val_subset, batch_size=cfg.batch_size, shuffle=False,
                            num_workers=cfg.num_workers, pin_memory=True)
                            
    return train_loader, val_loader, class_names, class_weights


# ----------------------------
# Metrics
# ----------------------------

def confusion_matrix(y_true: np.ndarray, y_pred: np.ndarray, num_classes: int = 2) -> np.ndarray:
    cm = np.zeros((num_classes, num_classes), dtype=int)
    for t, p in zip(y_true, y_pred):
        cm[t, p] += 1
    return cm


def metrics_from_logits(logits: torch.Tensor, y: torch.Tensor):
    preds = logits.argmax(dim=1)
    correct = (preds == y).sum().item()
    total = y.numel()

    tp = ((preds == 1) & (y == 1)).sum().item()
    fp = ((preds == 1) & (y == 0)).sum().item()
    fn = ((preds == 0) & (y == 1)).sum().item()

    precision = tp / (tp + fp) if (tp + fp) else 0.0
    recall = tp / (tp + fn) if (tp + fn) else 0.0
    f1 = 2 * precision * recall / (precision + recall) if (precision + recall) else 0.0
    return correct, total, precision, recall, f1, preds


# ----------------------------
# EarlyStopper
# ----------------------------

class EarlyStopper:
    def __init__(self, patience: int = 3, min_delta: float = 0.0):
        self.patience = patience
        self.min_delta = min_delta
        self.best = float("-inf")
        self.num_bad = 0

    def step(self, metric: float) -> bool:
        """Return True if we should stop (no improvement for `patience` steps)."""
        if metric > self.best + self.min_delta:
            self.best = metric
            self.num_bad = 0
        else:
            self.num_bad += 1
        return self.num_bad >= self.patience


# ----------------------------
# Train / Eval
# ----------------------------

def train_one_epoch(model, loader, device, optimizer, criterion):
    model.train()
    total_loss = 0.0
    total_correct = 0
    total_count = 0
    ps, rs, fs = [], [], []
    for x, y in loader:
        x, y = x.to(device), y.to(device)
        optimizer.zero_grad(set_to_none=True)
        logits = model(x)
        loss = criterion(logits, y)
        loss.backward()
        optimizer.step()

        total_loss += loss.item() * y.size(0)
        correct, count, p, r, f1, _ = metrics_from_logits(logits, y)
        total_correct += correct
        total_count += count
        ps.append(p); rs.append(r); fs.append(f1)

    return {
        "loss": total_loss / max(total_count, 1),
        "acc": total_correct / max(total_count, 1),
        "precision": float(np.mean(ps)) if ps else 0.0,
        "recall": float(np.mean(rs)) if rs else 0.0,
        "f1": float(np.mean(fs)) if fs else 0.0,
    }


def evaluate(model, loader, device, criterion):
    model.eval()
    total_loss = 0.0
    total_correct = 0
    total_count = 0
    ps, rs, fs = [], [], []
    all_preds = []
    all_labels = []
    with torch.no_grad():
        for x, y in loader:
            x, y = x.to(device), y.to(device)
            logits = model(x)
            loss = criterion(logits, y)
            total_loss += loss.item() * y.size(0)
            correct, count, p, r, f1, preds = metrics_from_logits(logits, y)
            total_correct += correct
            total_count += count
            ps.append(p); rs.append(r); fs.append(f1)
            all_preds.append(preds.cpu().numpy())
            all_labels.append(y.cpu().numpy())

    cm = confusion_matrix(np.concatenate(all_labels), np.concatenate(all_preds)) if total_count else np.zeros((2,2), dtype=int)
    return {
        "loss": total_loss / max(total_count, 1),
        "acc": total_correct / max(total_count, 1),
        "precision": float(np.mean(ps)) if ps else 0.0,
        "recall": float(np.mean(rs)) if rs else 0.0,
        "f1": float(np.mean(fs)) if fs else 0.0,
        "confusion_matrix": cm.tolist(),
    }


def save_checkpoint(model, out_dir: Path, class_names: List[str], best_metric: float):
    out_dir.mkdir(parents=True, exist_ok=True)
    torch.save(model.state_dict(), out_dir / "model.pt")
    with open(out_dir / "labels.json", "w") as f:
        json.dump({"classes": class_names, "best_val_f1": best_metric}, f, indent=2)


# ----------------------------
# Prediction helpers (file/dir/zip)
# ----------------------------

IMG_EXTS = {".png", ".jpg", ".jpeg", ".bmp", ".tif", ".tiff"}

def load_tensor_from_pil(img: Image.Image, img_size: int, grayscale: bool) -> torch.Tensor:
    tf = transforms.Compose(
        ([transforms.Grayscale(1)] if grayscale else []) +
        [transforms.Resize((img_size, img_size)), transforms.ToTensor()]
    )
    return tf(img).unsqueeze(0)


def load_image_from_path(path: Path, img_size: int, grayscale: bool) -> torch.Tensor:
    img = Image.open(path).convert("RGB")
    return load_tensor_from_pil(img, img_size, grayscale)


def load_image_from_bytes(data: bytes, img_size: int, grayscale: bool) -> torch.Tensor:
    img = Image.open(io.BytesIO(data)).convert("RGB")
    return load_tensor_from_pil(img, img_size, grayscale)


def iter_images_in_dir(path: Path) -> Iterable[Path]:
    if path.is_dir():
        for ext in IMG_EXTS:
            yield from path.rglob(f"*{ext}")
    elif path.is_file() and path.suffix.lower() in IMG_EXTS:
        yield path


def iter_images_in_zip(zip_path: Path) -> Iterable[Tuple[str, bytes]]:
    import zipfile
    with zipfile.ZipFile(zip_path, 'r') as zf:
        for info in zf.infolist():
            name_lower = info.filename.lower()
            if any(name_lower.endswith(ext) for ext in IMG_EXTS):
                with zf.open(info, 'r') as f:
                    yield info.filename, f.read()


# ----------------------------
# CLI entry points
# ----------------------------

def train_main(args):
    cfg = Config(
        data_dir=Path(args.data),
        out_dir=Path(args.out),
        img_size=args.img_size,
        batch_size=args.batch_size,
        epochs=args.epochs,
        lr=args.lr,
        weight_decay=args.weight_decay,
        train_split=args.train_split,
        grayscale=args.grayscale,
        num_workers=args.num_workers,
        seed=args.seed,
        model=args.model,
        freeze_backbone=args.freeze_backbone,
        aug=not args.no_aug,
    )
    set_seed(cfg.seed)

    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    train_loader, val_loader, class_names, class_weights = make_dataloaders(cfg)

    in_ch = 1 if cfg.grayscale else 3
    model = build_model(cfg, in_ch, num_classes=2).to(device)

    criterion = nn.CrossEntropyLoss(weight=class_weights.to(device))
    optimizer = optim.AdamW(filter(lambda p: p.requires_grad, model.parameters()),
                            lr=cfg.lr, weight_decay=cfg.weight_decay)

    best_f1 = -1.0
    stopper = EarlyStopper(patience=args.patience, min_delta=args.min_delta)
    for epoch in range(1, cfg.epochs + 1):
        tr = train_one_epoch(model, train_loader, device, optimizer, criterion)
        va = evaluate(model, val_loader, device, criterion)
        print(f"[Epoch {epoch:02d}] "
              f"train: loss={tr['loss']:.4f} acc={tr['acc']:.4f} f1={tr['f1']:.4f} | "
              f"val: loss={va['loss']:.4f} acc={va['acc']:.4f} f1={va['f1']:.4f}")
        print(f"   val precision={va['precision']:.4f} recall={va['recall']:.4f} "
              f"cm={va['confusion_matrix']}")

        if va["f1"] > best_f1:
            best_f1 = va["f1"]
            save_checkpoint(model, cfg.out_dir, class_names, best_f1)

        if stopper.step(va["f1"]):
            print(f"Early stopping: no val F1 improvement >= {args.min_delta} for {args.patience} epoch(s).")
            break

    print(f"Best val F1: {best_f1:.4f}")
    print(f"Saved best model to: {cfg.out_dir/'model.pt'}")


def predict_main(args):
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model_dir = Path(args.model_dir)
    with open(model_dir / "labels.json", "r") as f:
        meta = json.load(f)
    classes = meta["classes"]

    grayscale = args.grayscale
    img_size = args.img_size

    arch = args.model
    in_ch = 1 if grayscale else 3
    model = build_model(Config(data_dir=Path("."), out_dir=Path("."), img_size=img_size,
                               grayscale=grayscale, model=arch), in_ch, num_classes=len(classes))
    model.load_state_dict(torch.load(model_dir / "model.pt", map_location=device))
    model.to(device).eval()

    input_path = Path(args.input)
    results = []  # list of (name, pred_label, p0, p1)

    with torch.no_grad():
        if input_path.suffix.lower() == ".zip":
            for name, data in iter_images_in_zip(input_path):
                x = load_image_from_bytes(data, img_size, grayscale).to(device)
                logits = model(x)
                probs = torch.softmax(logits, dim=1).cpu().numpy()[0]
                pred_idx = int(np.argmax(probs))
                results.append((name, classes[pred_idx], float(probs[0]), float(probs[1])))
        else:
            paths = list(iter_images_in_dir(input_path))
            if not paths:
                raise SystemExit(f"No images found in '{input_path}'.")
            for p in sorted(paths):
                x = load_image_from_path(p, img_size, grayscale).to(device)
                logits = model(x)
                probs = torch.softmax(logits, dim=1).cpu().numpy()[0]
                pred_idx = int(np.argmax(probs))
                results.append((str(p), classes[pred_idx], float(probs[0]), float(probs[1])))

    for name, pred, p0, p1 in results:
        print(f"{name}: pred={pred}  P(0)={p0:.3f}  P(1)={p1:.3f}")

    # Optional CSV
    if args.out_csv:
        with open(args.out_csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["name", "pred", "P0", "P1"])  # keep headers simple
            for row in results:
                w.writerow(row)
        print(f"Wrote CSV: {args.out_csv}")


def build_argparser():
    p = argparse.ArgumentParser(description="Train a 0/1 die-shot bit classifier.")
    sub = p.add_subparsers(required=True, dest="cmd")

    pt = sub.add_parser("train", help="Train the model")
    pt.add_argument("--data", required=True, help="Path with folders '0' and '1'")
    pt.add_argument("--out", default="model_out", help="Output dir for model + labels.json")
    pt.add_argument("--img-size", type=int, default=64)
    pt.add_argument("--batch-size", type=int, default=64)
    pt.add_argument("--epochs", type=int, default=15)
    pt.add_argument("--lr", type=float, default=1e-3)
    pt.add_argument("--weight-decay", type=float, default=1e-4)
    pt.add_argument("--train-split", type=float, default=0.8)
    pt.add_argument("--num-workers", type=int, default=4)
    pt.add_argument("--seed", type=int, default=42)
    pt.add_argument("--grayscale", action="store_true", help="Force 1-channel input")
    pt.add_argument("--model", choices=["cnn", "resnet18"], default="cnn")
    pt.add_argument("--freeze-backbone", action="store_true", help="Freeze resnet18 backbone")
    pt.add_argument("--no-aug", action="store_true", help="Disable data augmentation")
    pt.add_argument("--patience", type=int, default=3, help="Early stopping patience (epochs without val improvement)")
    pt.add_argument("--min-delta", type=float, default=0.0, help="Minimum F1 improvement to reset patience")

    pp = sub.add_parser("predict", help="Run inference on an image, directory, or .zip of images")
    pp.add_argument("--model-dir", default="model_out", help="Folder with model.pt and labels.json")
    pp.add_argument("--input", required=True, help="Image path, directory, or .zip archive")
    pp.add_argument("--img-size", type=int, default=64)
    pp.add_argument("--grayscale", action="store_true")
    pp.add_argument("--model", choices=["cnn", "resnet18"], default="cnn")
    pp.add_argument("--out-csv", default=None, help="Optional path to write a CSV of predictions")

    return p


def main():
    parser = build_argparser()
    args = parser.parse_args()
    if args.cmd == "train":
        train_main(args)
    elif args.cmd == "predict":
        predict_main(args)
    else:
        parser.print_help()


if __name__ == "__main__":
    main()

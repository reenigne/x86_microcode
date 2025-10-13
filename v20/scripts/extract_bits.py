#!/usr/bin/env python3
"""
Export square crops around MaskROMTool bit coordinates into a single ZIP file.

Usage:
  python export_bit_windows_zip.py \
      --json bits.json \
      --image die.jpg \
      --zip_out bits.zip \
      --window_size 38

Notes:
- The input JSON is expected to have a large "bits" array with fields: x, y, value, ambiguous.
- Bits are sorted *column-first*, *bottom-up*.
- Logical coordinates are assigned so that (0,0) is the top-left of the bit grid.
- Each crop (window_size × window_size) is saved as bit_X_Y.png inside the ZIP.
"""

import argparse
import json
import math
import os
import io
import zipfile
from dataclasses import dataclass
from statistics import mean
from typing import List, Tuple

from PIL import Image


@dataclass
class Bit:
    x: float
    y: float
    value: bool | None = None
    ambiguous: bool | None = None
    gx: int | None = None
    gy: int | None = None


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(description="Export bit-centered crops into a single ZIP archive.")
    p.add_argument("--json", required=True, help="Path to MaskROMTool JSON file.")
    p.add_argument("--image", required=True, help="Path to source JPEG (very high resolution).")
    p.add_argument("--zip_out", required=True, help="Output ZIP filename (e.g. bits.zip).")
    p.add_argument("--window_size", type=int, required=True, help="Window size (pixels).")
    p.add_argument("--tail_frac", type=float, default=0.05,
                   help="Fraction of largest x-diffs used for detecting column jumps (default: 0.05).")
    return p.parse_args()


def load_bits(json_path: str) -> List[Bit]:
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)
    bits_json = data.get("bits")
    if not isinstance(bits_json, list):
        raise ValueError("JSON missing 'bits' array.")
    return [Bit(
        x=float(b["x"]),
        y=float(b["y"]),
        value=bool(b.get("value")) if "value" in b else None,
        ambiguous=bool(b.get("ambiguous")) if "ambiguous" in b else None,
    ) for b in bits_json]


def estimate_column_threshold(x_diffs: List[float], tail_frac: float) -> float:
    if not x_diffs:
        return 0.0
    diffs = sorted(abs(d) for d in x_diffs)
    n = len(diffs)
    if n == 1:
        return diffs[0] / 2 if diffs[0] > 0 else 0.0
    cut = max(1, min(n - 1, int(math.floor((1.0 - tail_frac) * n))))
    small = diffs[:cut]
    large = diffs[cut:]
    if not large:
        return (mean(small) if small else 0.0) * 3.0
    return (mean(small) + mean(large)) / 2.0


def split_into_columns(bits: List[Bit], tail_frac: float) -> List[List[Bit]]:
    x_diffs = [bits[i].x - bits[i - 1].x for i in range(1, len(bits))]
    threshold = estimate_column_threshold(x_diffs, tail_frac)
    columns: List[List[Bit]] = []
    current_col: List[Bit] = [bits[0]]

    for i in range(1, len(bits)):
        dx = abs(bits[i].x - bits[i - 1].x)
        if dx > threshold:
            columns.append(current_col)
            current_col = [bits[i]]
        else:
            current_col.append(bits[i])
    columns.append(current_col)
    columns.sort(key=lambda col: mean(b.x for b in col))
    return columns


def assign_logical_grid(columns: List[List[Bit]]) -> Tuple[int, int]:
    grid_width = len(columns)
    grid_height_max = 0
    for gx, col in enumerate(columns):
        col_sorted = sorted(col, key=lambda b: b.y)  # top→bottom
        for gy, b in enumerate(col_sorted):
            b.gx = gx
            b.gy = gy
        grid_height_max = max(grid_height_max, len(col_sorted))
        columns[gx] = col_sorted
    return grid_width, grid_height_max


def crop_with_padding(img: Image.Image, cx: int, cy: int, size: int) -> Image.Image:
    half = size // 2
    left = cx - half
    top = cy - half
    right = left + size
    bottom = top + size
    src_w, src_h = img.size
    src_box = (max(left, 0), max(top, 0), min(right, src_w), min(bottom, src_h))
    out = Image.new("RGB", (size, size), (0, 0, 0))
    dst_left = max(0, -left)
    dst_top = max(0, -top)
    if src_box[0] < src_box[2] and src_box[1] < src_box[3]:
        region = img.crop(src_box)
        out.paste(region, (dst_left, dst_top))
    return out


def main():
    args = parse_args()
    bits = load_bits(args.json)
    print(f"[info] Loaded {len(bits)} bits.")

    columns = split_into_columns(bits, args.tail_frac)
    grid_w, grid_h_max = assign_logical_grid(columns)
    print(f"[info] Detected grid {grid_w}×{grid_h_max}")

    img = Image.open(args.image).convert("RGB")
    ws = args.window_size
    ordered_bits = [b for col in columns for b in col]

    with zipfile.ZipFile(args.zip_out, "w", compression=zipfile.ZIP_DEFLATED) as zf:
        for i, b in enumerate(ordered_bits, 1):
            cx, cy = int(round(b.x)), int(round(b.y))
            crop = crop_with_padding(img, cx, cy, ws)
            buf = io.BytesIO()
            crop.save(buf, format="PNG")
            zf.writestr(f"bit_{b.gx}_{b.gy}.png", buf.getvalue())
            if i % 500 == 0 or i == len(ordered_bits):
                print(f"[progress] {i}/{len(ordered_bits)} written")

    print(f"[done] {len(ordered_bits)} images saved to {args.zip_out}")


if __name__ == "__main__":
    main()
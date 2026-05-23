

## z386 ROM1 and PLA4 notes

For ROM1 (`pla_control` in z386), I feed it with:

```
{state[6:0], inst_byte[7:0], mode[4:0]}
```

`inst_byte` is either the opcode byte or the ModR/M byte, depending on the
decode state. The `mode` bits are:

```
mode[4] = 0F escape seen
mode[3] = addr32 && mod != 3 && r/m == 4, so a SIB byte follows
mode[2] = addr32 && mod == 0 && r/m == 5, so disp32-only addressing
mode[1] = addr16 && mod == 0 && r/m == 6, so disp16-only addressing
mode[0] = addr32
```

The ROM1 output is 12 bits (`abcdefghijkl`). In the opcode state z386 
interoperates it as:

```
l      structural decode complete, excluding trailing immediates/displacements
k      opcode has a W bit
j      0 means opcode[2:0] is an embedded register; j=1,l=0 means ModR/M follows
i      segment-register PUSH/POP class
h      ALU/update-flags class
c,d    prefix / ModR/M / two-operand classification
ab     immediate-size class for ModR/M instructions
abcdef immediate/layout class for non-ModR/M instructions
```

For ModR/M decoding, the most important output bits are:

```
f      SIB byte follows
e,d    displacement class
fed    000 none, 010 disp8, 011 disp16/32,
       100 SIB only, 101 SIB+disp8, 110 SIB+disp16/32,
       111 SIB+disp32
```

For PLA4 / ROM4 (`protection.sv`), I feed it with:

```
{state[15:6], const[5:0]}
```

`const[5:0]` is the `ABCDEF` field from the microcode word. The upper ten state
bits come from the small "tiny PLA" / muxing logic in front of ROM4. In z386 I
name those bits:

```
bit 15  p1   privilege comparison 1, depending on protection ALU op
bit 14  p2   privilege comparison 2, depending on protection ALU op
bit 13  b13  appears unused
bit 12  b12  appears unused
bit 11  p    descriptor present bit, descriptor G bit for the granularity test,
             or selector-null for selector-only tests
bit 10  u    descriptor S bit, or CR0.ET for FPU tests
bit  9  x    descriptor type[3], executable, or CR0.TS for FPU tests
bit  8  ce   descriptor type[2], conforming/expand-down, or CR0.EM for FPU tests
bit  7  rw   descriptor type[1], readable/writable, or CR0.MP for FPU tests
bit  6  a    descriptor type[0], accessed
```

The `p1/p2` formulas depend on the protection ALU op (`PTOVRR`, `PTSELA`,
`PTSELE`, `PTF`, etc.). The current formulas are enough for z386, but I would
still treat them as inferred.

For PLA4 output, the high four bits are the `KLMN` side-effect flags. z386 uses
them as:

```
K  stack operation / CPL update side effect
L  perform limit/type validation
M  descriptor or selector validation OK, safe to commit
N  set the descriptor accessed bit
```


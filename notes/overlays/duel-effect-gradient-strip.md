# Duel-effect gradient strip and projection reuse

`func_80156448` occupies the complete `0x80156448..0x801566D4` interval:
652 bytes of matching C in `gradient_strip.c`, using the unchanged
`gcc_2_8_1_g0_split` profile. Its first candidate matched every instruction;
no register pins, assembly, barriers or compiler adjustments were required.

The routine draws two projected G4 polygons for each of 32 cyclic segments
between three SVECTOR arrays. The first two vertices are black, while the
last two use half-intensity input color. Direct `setRGB3(&polygon, ...)`
retains the target's stack-relative field stores, and signed modulo indexing
is preserved.

Unlike the neighboring textured-strip routine, this routine rejects only a
negative projection flag, not negative raw depth. Zero bias selects the
existing generic packet helper. Otherwise it subtracts the unsigned-halfword
bias, clamps negative depth to zero, shifts by two, and truncates the
ordering-table priority to `u16`. Both submission paths use flags one.
The common G4/FT4 coordinate interface and real ordering-table data owner are
reused without new storage or local linker aliases.

Three independently verified, already accepted French functions are reused
without source changes:

| Source | Functions | Bytes |
|---|---|---:|
| `projected_wrappers.c` | `func_80151218`, `func_8015131C` | 476 |
| `matrix_setup.c` | `func_801513F4` | 200 |

Their existing group boundaries, definition order, SDK declarations and
named profile are retained. Spanish resident bindings for `MulMatrix2`,
`RotTrans` and `RotMatrix` agree with the resident linker-symbol inventory.
Projection callees now have compiled C owners rather than preserved assembly;
no absolute alias substitutes for their bodies.

Before promotion, a fresh independent full-bank link verified the gradient
strip and matrix helper, then the complete four-function / 1,328-byte batch
against all 40 accepted Spanish entries. The latter reproduces all 90,112
bank bytes with SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`:
44 C functions / 9,384 bytes, leaving 41 explicit assembly boundaries.
All configured Spanish overlays then contain 168/209 matching instances /
65,236 bytes. Production checks also verify complete module images and
terrain copies, exact C object/ELF extents, and real generated-data ownership.

The adjacent `func_801566D4` number renderer is deliberately not promoted.
Private experiments recovered its 20-vector workspace, eight halfword
digits, two FT4 packets, RECT texture records and mode-dependent submission,
but did not reproduce every instruction. Builtin absolute value, the SDK
vector macro and explicit shifted indexing resolved several mismatches;
the remaining call-sharing, register-allocation and depth-temporary
differences keep it in the assembly inventory. Its candidate snapshots and
per-instruction diagnostics remain local under `tmp/`.

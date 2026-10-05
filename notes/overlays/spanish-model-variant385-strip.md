# Spanish MODEL385 three-station strip

The independently recovered helper at `+0x1478..+0x1B00` adds 1,672 matching
instruction bytes in each of the eight MODEL385/535 physical images:
eight new C instances and 13,376 bytes. The complete images still match
their retail hashes. The [line recovery](spanish-model-variant385-lines.md),
all existing bindings, raw data, and other functions are preserved.

A fresh seven-region screen checked 5,977 configured C entries, with no
same-size accepted body. This is a retail-derived reconstruction, not a
French or other regional C port. It uses the existing GCC 2.8.1 / MASPSX 2.81
`gcc_2_8_1_g0_split` profile and naturally reproduces the 264-byte frame.

## Original ownership and behavior

Entry `+0xD90` calls the helper with the original `s2` context in its delay
slot. CFG reaching-definition analysis finds only entry `+0xC` as the
definition of that context at this call. The call lies in the descriptor
iteration loop, under the phase-positive gate at `+0xD78..+0xD80`. The helper
independently checks phase positive. Updates occur only on the final
descriptor iteration.

One `0x8C`-byte strip at context `+0x4E0` contains three stations, each with
upper, center, and lower eight-byte endpoints. The three endpoint banks are
at `+0`, `+0x18`, and `+0x30`; three packed screen-coordinate banks are at
`+0x48`, `+0x54`, and `+0x60`. Three projection flags occupy `+0x74..+0x80`,
followed by three signed depths at `+0x80..+0x8C`.

The selected `POLY_GT4` is the 52-byte packet at context `+0xE2C..+0xE60`.
Entry `+0x24` establishes its stable `s7` pointer. The packet is initialized
by `SetPolyGT4` at `+0x2E8`, made semi-transparent at `+0x324`, and passed
to `SetShadeTex` with zero at `+0x330`. The later packet beginning at
`+0xE60` is not substituted for this helper's packet.

The helper's origin, direction, and packed projected-vector fields lie at
`+0xFAC`, `+0xFC0`, and `+0xFD0`. Frame/time are `+0xFEC/+0xFF0`, the timing
pointer is `+0x1000`, iteration is `+0x1014`, radius/progress are signed
halfwords at `+0x1028/+0x102A`, and phase is `+0x1030`. The private header
describes the helper's `0x1034`-byte view, not the entire allocation.

Rotation uses `ratan2(projected.y, projected.x) + 2048`. Transverse extent
is `width * radius / 1024`, with `radius / 128` added on odd frames, then
narrowed to a signed halfword. Stations progress along the direction using
`progress * station / 2`. Existing SDK vector and coordinate expressions
preserve evaluation order, signed packed-coordinate extraction, and old-GCC
address scheduling.

The renderer projects three endpoint banks and emits two quads for each of
the two intervals. Outer edge color is RGB 0/32/192 and center edge color
is 192/192/192. Before each submission it clamps the **current station's**
negative depth to zero and explicitly clears that station's projection
flag. Its source retains the natural depth-and-flag gate; the compiler
eliminates the redundant test of the just-cleared flag. This does **not**
implement rejection based on the original GTE projection flag.

Submission uses the narrowed `u16` depth of the **next station**, not the
current station whose depth was clamped. That next depth is not guaranteed
to have been clamped yet. Both flag writes, both depth clamps, both calls,
their order, and this asymmetric priority choice are retained exactly.

## Actual descriptors

The common table at image `+0x2FB0` uses 76-byte records indexed by the
command modulo 1000. Both slots of each model use one descriptor iteration:

| Model | Command | Width | Start | Expanded | Fade | End |
| --- | --- | --- | --- | --- | --- | --- |
| 170 | 551003 | 32 | 58 | 64 | 100 | 180 |
| 406 | 551006 | 48 | 68 | 80 | 330 | 370 |
| 407 | 551004 | 32 | 56 | 64 | 160 | 180 |
| 513 | 551005 | 32 | 48 | 56 | 280 | 320 |

Entry initializes radius to 1024 and progress to zero. Phase one uses
unsigned time interpolation to expand progress to 1024 and then enters
phase two. After the fade threshold, radius decreases and clamps to zero.
Every actual expansion and fade denominator is positive. No fallback or
extra arithmetic behavior was introduced.

## Exact-match evidence

`spanish-model-variant385-strip-attempts.csv` records five compiler
experiments and eight terminal full-image matches. The first independent
reconstruction had the correct frame and 1,672 bytes, but 277 differing
words. Separating angle adjustment and group initialization recovered
1,668 bytes and left 138 positional differences. SDK coordinate macros
alone did not change those results.

Retaining both depth and flag predicates after the observed flag resets
recovered the lower-packet address scheduling and every instruction, without
forced registers, artificial stack padding, dummy operations, or extra
compiler flags. Slot one was independently compiled and linked.

Forty-two target-compiled layout constants, both private-header inclusion
orders, all eight original-context calls, initialized packet bounds, actual
descriptors, and stable pointer lifetimes are covered by focused tests.
Every selected C relocation is reconstructed: sixteen calls to twelve
resident callees and two local jumps. Six added SDK aliases preserve all
thirty-six previous names and addresses; the binding file now has forty-two
names for the same thirty-six addresses.

Family tests continue to verify every selected C and retained ASM owner,
raw-data ownership, all complete binaries, and the resident loader/callee/
context owners. The two helpers together contribute sixteen matching C
instances and 23,840 instruction bytes; thirty-two other function instances
remain generated ASM.

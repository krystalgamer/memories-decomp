# Spanish MODEL385 pulsing quads

The independently reconstructed helper at `+0x21B8..+0x26B4` contributes
1,276 matching instruction bytes in each of eight MODEL385/535 images:
eight new C instances and 10,208 bytes. Every complete 20,480-byte image
matches retail. The family now has twenty-four C instances (34,048 bytes),
three unique recovered routines, and twenty-four physical ASM functions.

A fresh screen of 6,079 configured regional C entries compared eight
same-size bodies and found no accepted normalized match. This recovery is
from retail instructions, not French or other regional C. It uses the
authoritative GCC 2.8.1 / MASPSX 2.81 `gcc_2_8_1_g0_split` profile and a
natural 272-byte frame, without padding, forced registers or assembly.

## Original call and ownership

Entry `+0xD70` calls this helper with `s2` in the argument delay slot.
CFG reaching-definition analysis finds only entry `+0xC`, which captures
the original context. The call is in the descriptor-iteration loop, after
the unsigned `time >= descriptor.start` gate at `+0xD58..+0xD68`.
Unlike the later line/strip calls, this call does not require a particular
phase. The callee itself has no initial start-time guard.

Two `0x90`-byte groups occupy context `+0x56C..+0x68C`. Each begins with
four rows of four eight-byte `SVECTOR` points, followed by signed size at
`+0x80`. Entry initializes the planar points using descriptor `+0x28`,
clears size, and advances by `0x90` for two groups. Opaque trailing fields
are not assigned guessed meanings.

The inherited 52-byte `POLY_GT4` is at `+0xE60..+0xE94`. Entry establishes
`s7 = context+0xE2C`, advances it by 52 at `+0x338`, calls `SetPolyGT4` at
`+0x33C`, enables semi-transparency at `+0x374`, and calls `SetShadeTex`
with zero at `+0x380`. The helper captures context in `s0` and initializes
its stable `s1` packet pointer only after the ordering-table call. It changes
coordinates and RGB, not inherited texture or command bytes.

Origin is at `+0xFAC`, direction at `+0xFC0`, frame/time at `+0xFEC/+0xFF0`,
step at `+0xFF8`, descriptor pointer at `+0x1000`, descriptor iteration at
`+0x1014`, signed halfword progress at `+0x102A`, and phase at `+0x1030`.
The private `0x1034`-byte view is not a claim about the full allocation.
The frame word is only tested for its low bit; this use does not establish
signedness of the game's frame counter.

## Rendering and update order

Odd frames add signed `size/8` to all three scale components; even frames
add zero. Group zero translates to origin. Group one translates to
`origin + direction*progress/1024`. Preserve both `RotMatrix` calls, the
coordinate/light-matrix sequence, `ReadRotMatrix`, `ScaleMatrix`, and
`SetRotMatrix`, even where an algebraic simplification looks plausible.

Each group draws four quads, selecting the same column from all four point
rows. Depth is signed `RotTransPers4(...)*8/10`, not rewritten as `*4/5`.
The first three vertices are RGB `(0,64,192)` and the last is
`(192,192,192)`. Submission requires nonnegative depth and nonnegative
projection flag; depth is then narrowed to an unsigned halfword.

Updates occur only when `iteration+1 == descriptor.count` (unsigned
halfword at `+0xC`). Group zero expands toward 4096 in phase zero, using
unsigned `(time-start)*4096/(end-start)` at descriptor `+0x2C/+0x30`.
Fade at `+0x38/+0x3C` is an **else** branch, not another check after an
expansion changes phase. It starts only when `fade_start <= time` and
size is positive. At zero size, phase three advances to four.

Group one is zero in nonpositive phases and 4096 in phase one. In phase
two it adds `step*1024`, clamping at 8192 and advancing to phase three.
In phase four it subtracts `step*64`, clamping at zero and advancing to
phase five. Other phases retain size. Group-zero phase changes are visible
to group one later in the same call.

Actual descriptors are at image `+0x2FB0 + (command%1000)*76`; all have
count one. The physical records and hashes remain in
[`spanish-model-variant385-instances.csv`](spanish-model-variant385-instances.csv).

| Model | Point extent | Start / end | Fade start / end |
| --- | --- | --- | --- |
| 170 | 128 | 0 / 58 | 100 / 180 |
| 406 | 192 | 20 / 68 | 330 / 370 |
| 407 | 128 | 0 / 56 | 160 / 180 |
| 513 | 128 | 0 / 48 | 280 / 320 |

## Proof and preservation

Thirty-seven target layout checks cover the private views and SDK fields,
including coexistence with both accepted MODEL385 headers. Tests pin the
original call/context, packet lifetime, group initialization, actual
descriptors, arithmetic and phase gates. Selected compiler ownership and
all sixteen relocations are checked: ten calls to nine resident addresses
and six local jumps. Existing family tests continue checking every retained
ASM/C function, raw-data owner, resident callee and context allocation.

The eleven-row attempt ledger retains the initial twelve-word mismatch,
the exact corrected candidate, an independently compiled slot-one wrapper,
and eight complete-image terminal matches. Delaying packet initialization
and using descriptor-first fade comparison recovered the original lifetime
and load order. Dependency fingerprints cover body, private header, then
the shared model-variant header.

Both accepted C helpers, their ledgers, the instance table, module records,
all 42 binding names and 36 resident addresses are unchanged. The header
and `+0x2EB4..+0x5000` tail remain raw data. No new SDK aliases or shared
declaration changes were needed.

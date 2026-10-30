# French MODEL31368 / MODEL31378 entry

Refs #6460. Both game-owned `0x2384`-byte entries are matching pure C under
`gcc_2_8_1_g0_split` (GCC 2.8.1 and MASPSX 2.81). Slot 1 includes the same body
through a wrapper that relocates its three image-tail symbols. Both normal
production builds reproduce the complete original 20,480-byte images. The
four-byte headers and raw suffixes beginning at image offset `0x2388` are
unchanged.

| Sector | Entry | Complete image SHA-256 |
|---|---|---|
| 31368 | `0x8013B004` | `c73e4097502ea759fc7f6e12ddd0652a40b20a4ffee7fa04f61cd727abf82192` |
| 31378 | `0x8017B004` | `7a5b47ace749a02490507779f0b8941f1b777f96a365b4d509684d2244b97200` |

Archive: `game/france/DATA/MODEL.MRG`, SHA-256
`0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3`.
Each entry preserves its native 1,760-byte frame and all 119 resident calls.

## Family evidence

The two bodies differ in exactly 15 words: three evidenced LUI producers for
the scale vector, two image records and command table, and 12 entry-bounded
direct jumps relocated by `0x40000`. The other 2,258 words and all resident
calls are identical. This is not a suffix-identity claim.

The actual native body shares the three 128-SVECTOR walks at offsets 20,
1,044 and 2,068 with the independently reproduced French MODEL52364 family.
Its 118 resident calls occur in the same order, with exactly one additional
`PopMatrix` on this family's pre-start return path. The local C/header from
commit `5c3fb33c00450cbf55f267c17d506f324089e830`, published in #8446, supplied
structure only; this family's command layout and behavioral differences were
recovered from its native instructions. Generic `variant789_entry.c` is a
different six-flare effect and was rejected as a donor despite the raw header
value 789. Header values are locators, not family identifiers.

## Recovered layout and behavior

The command stride is 56 bytes. Wing/ring RGB occupy bytes 0..5; byte 6 is the
main slot. Byte 7 is not read and remains `unknown_07`. Half-size/start depth
are at 8/10; particle count/radius/speed at 12/14/16; signed spin rate at 18;
travel divisor at 20; wing count/height/width at 22/24/28. Burst geometry is at
30..34, ring radii at 36/38, and spray radius/count at 40/42. Start/release are
signed halfwords at 44/46 and burst time is a word at 48. The unaccessed
trailing word at 52 remains `unknown_34`; no purpose is claimed.

The constructor initializes relative z to `direction * start_depth`, with x/y
zero. Runtime pushes and copies the view matrix before testing start time,
balancing the early return with `PopMatrix`. There is no separate rise or
launch interval. Release captures the slot and seeds the wings once; the
velocity-to-local-position copy belongs inside that state-zero transition,
as proven by the native branch directly over it for nonzero state.
Travel starts at release and arrival tests `direction * position.vz >= 450`,
not the absolute coordinate.

Main quads use texture 1, scalar fade, and UV bounds 127..254 by 0..127.
Their z rotation uses half the spin rate before state 2 and four times the
rate thereafter; the postburst path uses half speed. Each rate is narrowed
to signed short before multiplication by elapsed time. The initial particle
loop submits a box whenever its distance predicate passes, even for zero
projected depth; the separate postburst depth-zero guard remains.

State offsets through ring RGB at 4,081 match the independently recovered
particle/ring layout. The ring uses texture 0 and UV bounds 0..127 by 64..191.
It starts at release and samples the main slot, not byte 7. Scalar fade,
flash/burst decay, ring-color decay and completion retain their native rules.

## Experiments and acceptance

`french-model-image31368-entry-attempts.csv` records all seven experiments for
both images and both normal production proofs: 16 rows. All experiments
passed 49 independently recovered target ABI assertions. All 4,512 original
inputs were frozen before the two native baselines; only the seven intended
metadata files subsequently changed.

The first candidate was four bytes short with 645 differing aligned words
per image. Correcting the transition-local position copy and introducing a
depth walker left 650; moving walker initialization after projection left
649. A loop-owned cursor restored the native extent but left 53 words.
Moving initialization after packet setup canonicalized to the earlier
four-byte-short candidate. Direct indexed right-shift submission left 637.
Binding the actual projection depth buffer before `RotTransPersN`, then
indexing that buffer, let GCC generate the native independent base and
induction walker and matched both complete images exactly.

All 14 ordinary candidate links completed before binary diagnostics; no
rejected link was forced. Final acceptance used tracked manifests and normal
`overlay_build.py build` and `verify`, with one original-sized C owner in
each ELF. No retail/generated output, assembly tricks, pinned registers,
fabricated accesses or external reference declarations are included.

# Spanish MODEL438 target-clamped sheets

Independently recover the 1,304-byte renderer at `+0x3DE8..+0x4300`
in twelve physical MODEL438/MODEL588 images. Both load-slot candidates
match with the existing `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and
MASPSX 2.81. No accepted regional C body was used: the selected shape
is absent from 5,921 configured regional C entries, including thirty
same-size comparisons at recovery time.

The [physical-instance ledger](spanish-model-variant438-instances.csv)
records MODEL147, MODEL211 and MODEL610 stages 7/8, and MODEL263,
MODEL525 and MODEL632 stages 9/10. Their headers are 438/588 and their
command words are `604000..604003`. Model identifiers and archive
record indices differ for MODEL525, MODEL610 and MODEL632; the ledger
preserves both.

All twelve complete 20KiB images match. This adds twelve C instances
and 15,648 instruction bytes, while retaining 84 assembly instances.
Each image has eight contiguous game-owned functions:
`4, 1614, 1B5C, 23E8, 2A34, 3638, 3DE8, 4300`, ending at `4868`
(all offsets hexadecimal). The four-byte header and 1,944-byte tail
remain exact raw data. The closed helper at `+0x23E8` has no observed
entry-reachable call; it remains inventoried as game code.

## Original context and initialized storage

Entry `+0x1460` calls the renderer; delay slot `+0x1464` passes `s3`.
Control-flow analysis proves that only the original `a0` assignment at
entry `+0xC` reaches this call. Initialization reuses `s3` on other
paths, so a simple register-name comparison would not establish this.

Entry `+0x54` forms `context + 0x20D8` in `s7`. Its only intervening
write adds 52 at `+0x2C8`; `SetPolyGT4` at `+0x2CC` initializes the
selected packet at `+0x210C`, with the argument in delay slot `+0x2D0`.
All twelve color stores remain within this 52-byte packet.

The sheet pointer starts at `+0x1938`, saved at entry `+0x30`.
Initialization uses four arrays at record offsets `0, 20, 40, 60`,
advances records by `0x98`, and stops after two records. Target-compiled
checks verify 32 sizes and offsets, including:

| Storage | Offset / extent |
|---|---|
| Two 152-byte sheet records | `+0x1938..+0x1A68` |
| Selected GT4 | `+0x210C..+0x2140` |
| Word origin | `+0x2258` |
| Signed-short target | `+0x2264` |
| Signed word direction | `+0x226C` |
| Frame / unsigned time / step | `+0x2298 / +0x229C / +0x22A4` |
| G32 timing pointer | `+0x22B4` |
| Signed-short fixed size | `+0x22D8` |
| Signed progress / phase | `+0x22DC / +0x22E8` |

The `0x22EC` state view is partial, not a claim about allocation size.
Resident owner checks verify the real context pointers and their
separation from resident and overlay banks.

## Rendering and progression

Odd frames add signed `size / 8` to each sheet's uniform scale. The
first sheet uses the word origin. The second uses signed
`direction * progress / 1024` until progress reaches 1,024, then
switches to the independently stored signed-short target.

The original matrix sequence is preserved, including `ReadRotMatrix`,
the second `RotMatrix`, scaling and `SetRotMatrix`. Four quads per
sheet use inner RGB on three corners and outer RGB on the fourth.
Signed depth is scaled by eight tenths; nonnegative depth and flag
permit sorting with the original sixteen-bit priority narrowing.

At phase zero the first sheet grows through unsigned timing to 4,096,
then sets phase one. Otherwise it uses the signed-short fixed size.
The second sheet is zero in phase zero, otherwise 4,096 before phase
two. At phase two with completed translation, it grows by
`step * 1024` to 8,192. At phase three it shrinks by `step * 64`,
clamps to zero and sets phase four.

The timing table starts at image `+0x4964` with 72-byte rows selected
by the command remainder. The 36-byte C timing view covers only the
required prefix; unsigned growth fields are at `+0x1C/+0x20`.
The four observed pairs are `(0,60), (0,48), (0,102), (20,48)`.
The unsigned division and its original zero-divisor trap are retained.

## Exactness and ownership

Six regressions cover exhaustive physical identities and all 96
function instances, actual call context and initialization, target
layouts and terminal fingerprints, every selected/retained input and
linked owner, and resident function/context owners. The missing-input
regression verifies that the physical inventory skips cleanly in
cross-region jobs without the legal Spanish archive.

Each selected object has ten external calls to nine distinct addresses,
including two `RotMatrix` calls. Its seven local jumps and all
relocations are checked against retail. Thirty-seven resident bindings
are resolved for the complete images, not just the selected C.

The [fourteen-row attempt ledger](spanish-model-variant438-attempts.csv)
records two independent slot compilations and twelve complete-image
terminals, including source and transitive-header fingerprints.
Previously accepted modules and their sources are unchanged.

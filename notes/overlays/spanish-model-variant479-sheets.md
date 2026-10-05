# Spanish MODEL479 offset sheets

Independently recover the 1,292-byte helper at image `+0x2B90..+0x309C`,
`func_8013DB90` / `func_8017DB90`, with named `gcc_2_8_1_g0_split`
(GCC 2.8.1 / MASPSX 2.81). A fresh scan of 5,837 accepted regional C
registrations found no same-size candidate. This is new decompilation, not
a regional port.

The physical images, seven-function inventory, resident bindings and disabled
primary command are the same as [MODEL479 curtains](spanish-model-variant479.md):
MODEL379, record329, stages7/8, headers479/629, sectors90984/90994, request645000.
This recovery adds two C instances and 2,584 instruction bytes without
introducing another image or changing any function boundary. The curtain
sources remain unchanged; five functions per image remain assembly, including
the unreferenced helper at `+0x309C`. Headers and suffixes remain raw.

## Entry-proven layout

| Private sheet-helper field | Offset / extent |
|---|---|
| Five 592-byte offset-primary records | `+0x18D8..+0x2468` |
| Signed SVECTOR offset / signed active word | Primary record `+0x40 / +0x188` |
| Eight 152-byte sheets | `+0x2D98..+0x3258` |
| Four four-element corner arrays | Sheet `+0x00 / +0x20 / +0x40 / +0x60` |
| Outer RGB / inner RGB / signed size | Sheet `+0x80 / +0x84 / +0x88` |
| Reused 52-byte GT4 packet | `+0x3F7C..+0x3FB0` |
| Three 32-byte position views | `+0x4024..+0x4084`; translation in the first VECTOR |
| Frame / time / step | `+0x4164 / +0x4168 / +0x4170` |
| Stored G32 descriptor pointer / signed shared phase | `+0x4178 / +0x41EC` |

The first candidate's partial primary view began at the position field
`+0x1918`. It produced a correct 264-byte stack frame but an unnecessary
induction spill and 24 extra instruction bytes. Entry instead establishes
the record base at `+0x18D8` (`+0x1C`), addresses the active word at record
`+0x188` (`+0x6BC`), advances both record cursors by `0x250` and bounds the
loop at five (`+0x7B0..+0x7C4`). The next entry-owned array begins at `+0x2468`.
Using those actual boundaries, without changing the function body, reproduces
all 1,292 bytes. The second slot independently matches as well.

Entry separately initializes eight sheets with a 152-byte stride
(`+0x948..+0x95C`), with four corners in each of four arrays. No pointer
arithmetic crosses a corner-array boundary. These are private partial views,
not complete declarations of every surrounding context field.

## Dispatch and rendering

The entry call at `+0xE68` passes `s6` in its delay slot. A reaching-definition
walk identifies entry `+0xC`, the original incoming `a0`, as its only reaching
definition. Entry requires unsigned time at least descriptor `grow_begin`
and signed phase less than five; do not silently replace the latter with an
assumed nonnegative phase range.

All eight sheets project four quads using zero rotation and uniform
`size + extra` scale. On odd frames, `extra` is signed `size / 8`, truncating
toward zero. The first five translations combine the corresponding primary's
signed position with position `i % 3`; the final three use position `i - 5`
without that offset.

Each quad receives inner RGB on its first three corners and outer RGB on its
fourth. Depth is scaled by signed `depth * 8 / 10`. Sorting requires
nonnegative scaled depth and flag, plus a nonzero primary active word for
the first five sheets; the final three do not test a primary. Priority is
narrowed to 16 bits. Geometry projection and subsequent size updates still
occur when that active test prevents sorting.

Size updates happen after each sheet is drawn. For the first five, phase two
sets 4096; phase three adds `step * 512` below 8192, caps at 8192 and changes
the shared phase to four. Thus a phase write can affect subsequent sheets
within the same call. The final three grow toward 4096 in phase zero and set
the shared phase to one when reaching that cap.

Both groups fade when unsigned time reaches `fade_begin` and signed size is
positive: the first five use amplitude8192 / shift13, the last three use
amplitude4096 / shift12. The target uses unsigned time arithmetic and division,
then a signed size comparison to clamp nonpositive results to zero. Preserve
that evaluation and narrowing rather than introducing new arithmetic rules.

Entry proves 44-byte descriptors rooted at image `+0x3E1C`. The selected record
has growth times20..100 at descriptor `+0x18/+0x1C` and fade times360..440 at
`+0x24/+0x28`; both actual denominator differences are80.

## Conditional primary-bank overlap

The sheet helper's private view ends at `+0x41F0`, twenty bytes beyond the
curtain helper's view. With resident contexts `0x80136000/0x80176000`, this
extends **496 bytes into the primary-load bank**, but not into the secondary
executable. This is not evidence of blanket nonoverlap or allocation capacity.

The same actual command triple `(645000, 593000, -2)` and signed resident
guard at `0x80058BB0` skip the primary callback at `0x80058BEC`. Tests retain
the metadata-copy offsets, command value, primary-image stubs and resident
pointer owners established for the curtain recovery. The distinction is
between a command-disabled primary callback and the independently observed
sheet-helper view, not a generally unused memory bank.

## Exact-match evidence

Three archived comparisons preserve the initial 1,316-byte mismatch
(316 differing words), the entry-boundary correction (1,292 exact bytes) and
the independent second-slot match. Two production terminal records also
fingerprint the body and private header. No forced registers, artificial
frame padding, instruction edits, new compiler flags or reference declarations
are used.

Four sheet-specific regressions check the entry's record boundaries and
dispatch, 43 target-compiled layout constants, complete-image C owners,
ten resident-call relocations and three local-jump relocations per image,
and terminal source/header fingerprints. Existing MODEL479 tests retain
all C/assembly/raw owners, the curtain fingerprints and resident-bank evidence;
their reaching-definition walk now covers both selected entry calls.

# French MODEL143 entries

Eight complete20,480-byte runtime images contain exact4,080-byte C entries
and16-byte source-owned VECTOR literals, headers143/273. Models481/496
use stages7/8; models234/546 use stages9/10. Commands74000..74003 select
four descriptors directly. The [instance ledger](french-model-variant143-instances.csv)
records all loader sectors and independent hashes.

## Ownership and declarations

| Offset | Size | Owner |
| --- | --- | --- |
| 0 | 4 | Raw module header |
| 4 | 4080 | Matching C entry |
| 0xFF4 | 16 | C VECTOR literal |
| 0x1004 | 28 | Raw GsIMAGE view |
| 0x1020 | 16352 | Unclassified suffix |

This contributes32,640 C instruction bytes and128 literal bytes. The
130,816 suffix bytes remain unclassified. Reading the descriptor table does
not establish exhaustive suffix ownership or runtime completion.

The34-byte descriptor has eight bytes and thirteen signed halfwords.
Selected counts are16,32,44 and55. The local0x698-byte state conservatively
declares55 positions,55 targets and55 rotations, preserving three72-byte
unknown gaps instead of guessing64-element ownership. Five geometry
vertices, six three-pointer faces, six CVECTOR colors, one unsigned packed
texture, signed elapsed time and a completion byte complete the measured
view. Forty-one target-compiled assertions protect all relevant layouts.

Slot entries use the authoritative GsCOORDUNIT path through
Model_GetSlotDataEntry and GsGetLwUnit, not a guessed GsCOORDINATE2 view.
Existing local game and Psy-Q headers supply every call declaration.

## Native behavior

Initialization clears all four halfwords of each position. Targets use
`rand() % (spread * 2) - spread`, zero Y and
`direction * (rand() % spread + 450)` for Z. Rotation starts at0,0 and
`rand() % 4096`. Four face colors are graded by `(i+4)/8`; two retain full
descriptor color. Five pointed geometry vertices form faces
`[1,0,2]`, `[2,0,3]`, `[3,0,4]`, `[4,0,1]`, `[1,2,3]`, `[3,4,1]`.
The frame-step query also occurs on constructor calls.

Before each staggered start, the slot coordinate is copied into its position;
randomized Y is also retained in its target. Main geometry renders through
travel plus hold. After travel, the position's fourth halfword toggles
alternate-frame visibility. Triangle submission requires strictly positive
clip and depth, with a nonnegative projection flag.

Movement preserves signed16 narrowing: copy position into scratch, negate,
add target, divide by `(travel_duration-time)/step`, then add back.
The last movement step copies the target. Rotation increments use signed
modulo4096 without multiplying by frame step. All traversals stay within
the selected55-element views.

The second pass uses six scratch vertices to form adjacent right/left
quads. Color grows with elapsed sprite time; its four texture frames use
`time * 4 / sprite_duration`. The left quad replaces two projected points
and uses `(first_depth * 2 + depth1 + depth3) / 4`. Only the main geometry
matrix chain includes MulMatrix2.

After elapsed advances, completion pulses occur only for strict
`time > travel_duration`: earlier instances return4 and the last returns1.
The overall count/spacing/travel/hold threshold returns2. Otherwise opposing
slot animation5 or8 returns4, and other animation states return0.

## Experiment evidence

The [attempt ledger](french-model-variant143-attempts.csv) retains one initial
compile failure, eight paired source experiments and two canonical matches.
The failure records invalid use of a packet color macro on CVECTOR and the
missing ordering-table declaration; both were corrected using existing
headers and fields.

Independent vector cursors recovered the native frame and traversal.
Separate face-index/value lifetimes and captured RGB results recovered
evaluation ordering. Cursor-before-index epilogues resolved scheduling,
unsigned texture storage recovered LHU page extraction, and reusing the
secondary cursor in the sprite pass resolved its final four stack offsets.
No forced registers, artificial stores, fake dependencies, padding,
inline assembly or reference-project types are used.

All42 ordered calls to24 resident destinations match. Independent proofs
verified all eight complete images,16 actual C entry/literal contributions,
24 disjoint raw owners and the actual resident callee bodies before
canonical integration. The authoritative GCC2.8.1/MASPSX2.81
`gcc_2_8_1_g0_split` pipeline is unchanged.

Canonical sources are
`src/overlays/french_model_variant/variant143_entry.c`,
`variant143_entry.h` and `variant143_entry_slot1.c`.
Regressions are in `tools/project/tests/test_french_model_variant143.py`.

# Spanish MODEL464 descriptor-driven sheets

This independently recovered helper adds new matching C rather than porting
an accepted regional implementation. An exhaustive physical model-record
scan finds six images with headers 464/614: MODEL175 stages 7/8, MODEL182
stages 9/10, and MODEL244 stages 7/8. Model numbers equal compact record
indices for these three models.

| Model | Stages | Sectors | Request | Primary count |
| --- | --- | --- | --- | --- |
| 175 | 7/8 | 48480/48490 | 630001 | 5 |
| 182 | 9/10 | 50432/50442 | 630002 | 5 |
| 244 | 7/8 | 67524/67534 | 630000 | 2 |

Each pair loads at `0x8013B000/0x8017B000`. The selected helpers are
`func_8013CE94/func_8017CE94`, each 1,164 bytes, totaling 6,984 newly matched
instruction bytes. All six complete 20,480-byte images reproduce their
recorded retail hashes with named `gcc_2_8_1_g0_split`
(GCC 2.8.1/MASPSX 2.81).

The initial candidate already reproduced the instruction sequence but swapped
the two stack slots for the primary cursor and ordering-table pointer.
Declaration order resolved those six differing words. The ledger preserves
that comparison, the first exact comparison, independent second-slot
compilation, and six final production source/header fingerprints. No forced
registers, artificial padding, instruction patches, or new flags were used.

## Ownership and remaining work

All six images have the following closed, fully visited function CFGs:

| Offset range | Bytes | Selection |
| --- | --- | --- |
| `0x0004..0x1368` | 4,964 | Entry, generated assembly |
| `0x1368..0x18EC` | 1,412 | Helper, generated assembly |
| `0x18EC..0x1E94` | 1,448 | Helper, generated assembly |
| `0x1E94..0x2320` | 1,164 | Descriptor-driven sheets, matching C |
| `0x2320..0x2838` | 1,304 | Non-entry-reachable helper, generated assembly |
| `0x2838..0x2DAC` | 1,396 | Non-entry-reachable helper, generated assembly |

The entry directly calls the first three helpers. The last two closed
functions are **not entry-reachable**, but remain inventoried code owners.
Thirty function instances remain assembly. The four-byte headers and
8,788-byte suffixes at `+0x2DAC` remain raw owners. Each suffix is
**unclassified**, not assumed to be entirely data.

Delay-slot-aware reaching definitions prove that the original context
assignment at image `+0xC` reaches the sheet call at `+0x11D0`, despite
initialization reusing that saved register. The entry's unsigned comparison
at `+0x11C4` suppresses this call before the descriptor's growth start.

Regressions verify all 36 function owners and 12 raw owners in input objects
and final linked images, ten selected call relocations per image, 36 resident
callee owners, four loader/dispatcher owners, and eight resident pointer-storage
owners. The partial context views at `0x80136000/0x80176000` do not overlap
the known slot payloads. The view size `0x1C64` is not an allocation-capacity
claim or a description of every entry access.

## Layout, descriptors, and bounds

Entry `+0x18/+0x1C` establishes primary records at context `+0x4E0` and
sheets at `+0x774`. Its loops prove five 132-byte primary records and six
156-byte sheet records. Primary active is at record `+0x80`; sheet size
and shown are at `+0x88/+0x98`. The GT4 packet occupies
`+0x1AB4..+0x1AE8`.

Three 32-bit origin words begin at `+0x1BD8`, immediately followed by an
SVECTOR anchor at `+0x1BE4`. Do not declare the origin as a padded 16-byte
VECTOR overlapping that anchor. Frame, unsigned time, unsigned step, timing
pointer, and phase are respectively `+0x1C18`, `+0x1C1C`, `+0x1C24`,
`+0x1C2C`, and `+0x1C60`. The pointer uses `G32`. Thirty-six target-compiled
constants check the private and shared layouts.

Actual requests come from model-record sector 275, offset `0x110` for
stages 7/8 or `0x114` for stages 9/10. Entry arithmetic establishes a
72-byte descriptor stride from image `+0x2EA8`. Normalized indices 1, 2,
and 0 respectively select:

| Model | Descriptor | Count | Growth | Fade |
| --- | --- | --- | --- | --- |
| 175 | `+0x2EF0` | 5 | 30..60 | 160..180 |
| 182 | `+0x2F38` | 5 | 20..40 | 120..136 |
| 244 | `+0x2EA8` | 2 | 160..200 | 500..560 |

Growth fields are `+0x1C/+0x20`, fade fields `+0x28/+0x2C`, and signed
count is `+0x44`. The helper processes count plus one sheets: six for
MODEL175/182, but only three for MODEL244. Every actual interval is nonzero.
Negative counts retain the original no-iteration behavior.

Only the first count sheets access primary fields. The final sheet uses
the origin and skips the primary dereference. Its primary byte cursor is
rooted in the raw context, not the five-element typed array: with count 5
it reaches the sheet region, then finishes at `+0x7F8`, still within the
helper view. It never dereferences that non-primary position.

## Rendering and state changes

Each sheet projects four Gouraud-textured quads, using inner RGB on the
first three corners and outer RGB on the fourth. Odd frames add signed
size/8 uniformly on all axes. Non-final sheets use the fixed anchor; the
final sheet uses the three origin words. All sheets may sort regardless
of primary activation: sorting scales depth by 8/10, checks nonnegative
adjusted depth and projection flags, and narrows priority to 16 bits.

For the final sheet, phase 0 computes unsigned descriptor-timed growth,
clamps at 4096, and advances phase 1. Only the nonzero-phase branch permits
fade, and only when time is **strictly greater** than fade start.
Fade uses amplitude 4096; reaching zero advances phase 3.

Other sheets update only while primary active equals exactly 1. Before
shown is set, they grow by unsigned step times 4096 to 16384 and set shown.
Otherwise positive size decreases by step times 512 and clamps at zero.
Here the shared shown field records the completed growth stage; it is not
a gate on rendering.

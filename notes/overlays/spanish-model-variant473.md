# Spanish MODEL473 six-sheet helper

This independently recovered helper adds new matching C rather than porting
an accepted regional implementation. MODEL385 (compact record 335) loads
header 473 at stage 9 and header 623 at stage 10. A scan of all physical
model records finds exactly these two occurrences.

| Slot | Stage | Sector | Load address | Selected helper | Bytes |
| --- | --- | --- | --- | --- | --- |
| 0 | 9 | 92660 | `0x8013B000` | `func_8013DBE8` | 1,216 |
| 1 | 10 | 92670 | `0x8017B000` | `func_8017DBE8` | 1,216 |

Both complete 20,480-byte images reproduce their recorded retail hashes
with `gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81). The second source is a
symbol-only wrapper. The attempt ledger preserves five recovery comparisons
and two terminal production records with source/header dependency fingerprints.
No forced registers, artificial stack padding, or instruction edits were used.

## Ownership and remaining work

Each image contains seven closed, fully visited function CFGs:

| Offset range | Bytes | Selection |
| --- | --- | --- |
| `0x0004..0x104C` | 4,168 | Entry, generated assembly |
| `0x104C..0x15AC` | 1,376 | Helper, generated assembly |
| `0x15AC..0x1FB0` | 2,564 | Helper, generated assembly |
| `0x1FB0..0x2BE8` | 3,128 | Helper, generated assembly |
| `0x2BE8..0x30A8` | 1,216 | Six-sheet helper, matching C |
| `0x30A8..0x386C` | 1,988 | Non-entry-reachable helper, generated assembly |
| `0x386C..0x3CE4` | 1,144 | Helper, generated assembly |

The entry directly calls five helpers, **not** the closed function at
`+0x30A8`. That function is explicitly inventoried and retained as assembly;
entry reachability is not a prerequisite for preserving a code owner.
In total twelve function instances remain assembly. The four-byte header
and 4,892-byte suffix at `+0x3CE4` remain raw owners. The suffix is
**unclassified**, not asserted to consist entirely of data.

Delay-slot-aware reaching definitions prove that the original context
assignment at image `+0xC` reaches the sheet call at `+0xEE4`, despite
initialization reusing the saved register. The entry permits this call while
phase is less than 5.

Regression coverage checks all fourteen function owners and four raw owners
in input objects and final linked images, ten call relocations per selected
helper, 33 resident callee owners, four loader/dispatcher owners, and eight
resident pointer-storage owners. The helper views at `0x80136000/0x80176000`
do not overlap the known slot payloads. The view size `0x3C60` is **not**
an allocation-capacity claim or a description of every entry access.

## Recovered layout and bounded traversal

Entry instruction `+0x1C` establishes the primary-record base at context
`+0x18D8`. The loop bound at `+0x80C` and increment at `+0x810` prove
**five 556-byte records**, ending at `+0x23B4`. Position and active are at
record `+0x40` and `+0x188`, respectively.

Six 152-byte sheet records occupy `+0x2984..+0x2D14`. The GT4 packet occupies
`+0x3A04..+0x3A38`; a VECTOR origin starts at `+0x3AFC`. Frame, unsigned
time, unsigned step, timing pointer, and phase are at `+0x3BDC`, `+0x3BE0`,
`+0x3BE8`, `+0x3BF0`, and `+0x3C5C`. The stored pointer uses `G32`.
Thirty-four target-compiled constants verify the private declarations and
shared sheet/SDK layouts.

The renderer processes six sheets, but only the first five access a primary
record. Its cursor therefore starts from the **raw context byte pointer**,
not from a typed five-element array. Six increments reach `+0x25E0`, before
the sheet records; the sixth iteration never dereferences primary fields.
This avoids forming a typed array pointer beyond its permitted one-past end.

The initial guessed view started at the position member and incorrectly
described six primaries. Correcting the entry-owned array removed that
mistake but still produced an unnecessary induction pointer. The bounded
byte cursor recovered the exact size, and initializing the inner counter
before the first-corner cursor resolved the final two swapped instructions.

## Descriptor and behavior

The actual stage 9/10 request is 639000, at model-record sector 275 offset
`0x114`. Its normalized index 0 selects a 44-byte descriptor at image
`+0x3DE0`: growth endpoints 20 and 84 at descriptor `+0x18/+0x1C`, fade
endpoints 360 and 420 at `+0x24/+0x28`. Entry arithmetic establishes the
44-byte stride; the context-stored descriptor pointer is read by the helper.

Each sheet projects four Gouraud-textured quads, with inner RGB on the first
three corners and outer RGB on the fourth. Odd frames add signed size/8
uniformly on all axes. The first five sheets follow primary positions and
require a nonzero primary active field for sorting; the sixth uses the world
origin and bypasses that activation test. Sorting scales depth by 8/10,
rejects negative adjusted depth or projection flags, and narrows priority
to 16 bits. The original ReadRotMatrix/RotMatrix sequence is retained.

For the sixth sheet, phase 0 computes growth from the unsigned descriptor
interval, clamps at 4096, and advances phase 1. There is no additional
synthetic lower-time guard. Its independent timed fade uses amplitude 4096
and clamps at zero.

For the first five sheets, phase 2 sets size to 4096. Phase 3 adds unsigned
step times 512, clamps at 8192, and advances phase 4. Their independent timed
fade uses amplitude 8192 and clamps at zero. Fade checks remain separate
from the growth/phase branches rather than being restricted to one phase.

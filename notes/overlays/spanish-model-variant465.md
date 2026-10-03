# Spanish MODEL465/615 runtime helpers

This independently recovered Spanish family adds four actual MODEL images:
models 108 and 573, stages 9 and 10, slots zero and one. Recovery started
from accepted master `45d1a33db` without pending MODEL460 work. Before
integration the clean branch advanced to accepted `6a236f4ff`, preserving
every calibrated source, declaration, profile and Spanish binding fingerprint.
All 182 earlier Spanish modules remain unchanged.

The subsequent spiral follow-up adds four independently verified C owners
without changing the accepted canonical source. Current family ownership is
28 C instances / 34,848 bytes, with the entry and companion still assembly.

The [instance ledger](spanish-model-variant465-instances.csv) records each
real archive slice and distinct SHA-256. The loader compacts model 573 to
record 523; model 108 remains record 108. Records contain 276 sectors,
with these ten-sector images at offsets 200 and 210. Their load addresses
are `0x8013B000` and `0x8017B000`. The actual command at record sector 275,
byte `0x114`, is 631000. It selects descriptor zero from the table at
image `0x4450`, stride 52. The descriptor lies in preserved raw storage,
not in a reclassified code tail.

## Complete boundaries and real compiler ownership

| Offset | Bytes | Ownership | Direct entry-call path |
| --- | ---: | --- | --- |
| `0x0004` | 4,680 | Generated assembly entry | Entry |
| `0x124C` | 1,056 | Fan C | Yes |
| `0x166C` | 3,840 | Generated assembly companion | Yes |
| `0x256C` | 1,352 | Orbit C | Yes |
| `0x2AB4` | 1,376 | Web C | Not demonstrated |
| `0x3014` | 776 | Spoke C | Not demonstrated |
| `0x331C` | 892 | Ring C | Not demonstrated |
| `0x3698` | 868 | Quad C | Not demonstrated |
| `0x39FC` | 2,392 | Spiral C | Not demonstrated |

All nine complete boundaries were independently walked in each actual Spanish
image, including delay slots and local call targets. The last
function ends at `0x4354`; its 2,392 bytes are not hidden in the suffix.
The four-byte header and `0x4354..0x5000` unclassified suffix remain
separate real storage owners. No claim is made about additional runtime
paths to the retained helpers.

The initial recovery freshly compiled twelve objects, covering six helpers
in both slots, matching all 24 original Spanish C spans with zero differing words. The
[attempt ledger](spanish-model-variant465-attempts.csv) records the twelve
canonical source/profile experiments. The unchanged accepted regional fan
and orbit bodies use independently verified local declarations; the other
four wrappers retain the accepted US448 implementations. All use the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81. No C, header,
compiler profile, padding workaround or assembly substitution was added.

Four independent complete-image links now account for all 81,920 bytes:
**28 C owners / 34,848 bytes**, **eight assembly owners / 34,080 bytes** and
**eight header/tail owners / 12,992 bytes**. Function inventory additions
are all 36 instances, not only the C successes.

## Context, layouts and lifetimes

Fresh target compilation verifies 133 constants in 532 bytes of read-only
data, including pointer/scalar widths, shared geometry/packet declarations,
fan and orbit records, and narrow web records. The generic shared sheet
declaration's 152-byte size is not this family's runtime stride.

The fan is one 296-byte record at context zero. It contains 18 inner
`SVECTOR`s at zero, 17 outer points at `0x90`, three colors at
`0x118/0x11C/0x120`, and scale at `0x124`. Sixteen iterations use inner
indices zero, `j+1`, `j+2` and outer indices `j`, `j+1`, all within those
arrays. Its triangle and quad packets occupy `0x1948..0x1964` and
`0x1964..0x1988`. The descriptor growth interval is 40 to 140, with
nonzero denominator 100; shrinking begins at frame 220. Both depth and
projection flag must be nonnegative before custom sorting with low-16-bit
depth and fourth argument one.

The orbit is a distinct 296-byte record at `0x11F0..0x1318`: four rows of
four points at `0/0x20/0x40/0x60`, colors at `0x80/0x84`, and two
3-by-2-by-3 signed-word arrays at `0x98/0xE0`. The `0x88..0x98` interval
stays opaque in this view. Three 520-byte companion records occupy
`0xBD8..0x11F0`; their signed selectors at `0x200/0x204` enable lanes.
The entry clears those selectors and both 18-word orbit arrays.
The companion helper can change phase five to six; the orbit helper
changes phase six to seven after summing at least 36 completed states.

Each active orbit lane draws four GT4 quads through the one 52-byte
packet at `0x19BC..0x19F0`. Actual descriptor colors are inner
`(140,128,16)` and outer `(224,180,160)`. Both depth and projection flag
must be nonnegative; sorting uses the low 16 bits of depth.
The gate at entry `0x1090..0x10A8` is signed phase at least five, not
a descriptor-frame gate.

Three 416-byte web records occupy `0x6F8..0xBD8`, with two 4-by-6 point
grids at `0/0xC0`, color at `0x180`, scale at `0x194`, and done at
`0x198`. Six 144-byte rings start at `0x1318`, four 144-byte spoke
records at `0x1678`, and one 144-byte quad at `0x18B8`, ending exactly
at the triangle packet. Web and spoke timing reads use the prefix word
at `0x11F0+0x88`; this does not establish an array of generic sheets.
The three line helpers reuse a 20-byte `GsGLINE` at `0x1A84`.
Their direct color writes occupy bytes 12 through 17, while projection
writes the coordinate words at offsets four and eight. Rings require
depth strictly between zero and `0x800`; webs and spokes require
positive depth. The quad shares the fan's G4 packet and requires
positive depth for custom sorting.

Independent retail checks retain 449 literal anchors, five relocated
anchors and 27 complete register-write lifetimes per image. The entry
has a 248-byte frame and keeps original context in `s6`. Its `s2`
register is reused during initialization, so a linear whole-function
immutability assertion would be wrong. Delay-slot-aware reaching-definition
analysis proves that the original capture at image `0xC` reaches all
three call sites `0x1088/0x10A4/0x10BC`. The negative-command path
bypasses the initialization-only reassignments.

The fan keeps original context in `s7`, separate from advancing `s4`;
orbit uses `s5`, webs `s3`, spokes `s4`, and rings/quad `s2`.
Their frames are respectively 272, 304, 296, 272, 264 and 248 bytes.
Projection outputs occupy stack `208..216`, disjoint from all direct
stack stores and saved-register areas. Full write sets verify packet
bases, record cursor strides, web coordinate-pointer spills, and the
immutable web/spoke timing-pointer spills. These are observed access
and ABI-preserved lifetime proofs, not allocation or whole-game
isolation claims.

## Actual resident ownership and scope

All 37 bindings were independently assembled from accepted Spanish
MODEL435/338/442 metadata and the authoritative Spanish resident inventory.
This includes the extra 332-byte `func_8005F5C8` owner at `0x8004F5D8`.
A fresh matching Spanish resident independently verifies all 37 actual
callee owners, the three matching callers `func_8004CB0C`,
`func_800559D4` and `Model_LoadMonsterMerge`, and both real context-pointer
storage owners at `0x80010024/0x80010028`. Their values are
`0x80136000/0x80176000`. Every local and resident call in all nine
functions is accounted for.

The directly observed entry extent `0x1B50` is a minimum, not an
allocation size. It is disjoint from the selected primary, secondary
and MODEL loads. The unknown intervals, unmatched functions and raw
suffixes remain explicitly unclassified.

## Original integration checkpoint

Original configured Spanish coverage became **1,008/1,258 C instances /
1,176,332 C instruction bytes across 186 images**. This adds 24 C
instances without changing earlier accepted families or French metadata.
It does not establish exhaustive Spanish or seven-release runtime coverage.

Production acceptance passes all 186 complete Spanish overlays, a fresh
Spanish resident and the clean North American executable match. Actual
linked objects and sections retain all 24 C, 12 assembly and eight
header/tail owners, all 133 freshly compiled layout constants and all 37
resident callees. The final three caller and two context-pointer owners
are archived before the North American build replaces Spanish outputs.
All 60 focused and 1,340 full regressions passed without skips, together
with basic-type, attempt, declaration-visibility, notes, note-link and
metadata policies. CI and maintainer acceptance remain separate gates.

## Retained spiral follow-up

The unchanged accepted French465 wrappers around `variant448_spiral.c`
reproduce all four 2,392-byte Spanish functions with 304-byte frames.
Fresh complete-image substitutions preserve every one of the 24 previous
C objects byte-for-byte, both remaining assembly functions per image,
and all raw extents. Two terminal wrapper records extend the original
twelve ledger rows unchanged. This adds 9,568 C instruction bytes.

Fresh exact Spanish resident linking verifies all eleven actual spiral
imports, their selected sized input definitions and complete retail bodies,
including the 44-byte `RotTransPers` at `0x80087868`. The four local symbol
lists and shared family binding rename only the existing address-based
alias for that function; the address and 37 distinct resident owners do
not change. The eight retained assembly objects also reference that alias:
each production object is byte-identical to its archived original after
only `objcopy --redefine-sym func_french_80087868=RotTransPers`. Thus their
instruction bytes, relocation offsets/types and resolved targets are
preserved despite the intentional symbol-table spelling change.
All 24 earlier C objects remain byte-identical without normalization.
No shared C, header, profile or other family binding changes.
Both actual context-pointer data owners are verified again. Resident
output `.main` combines code and data, so data ownership is established
from selected non-executable input `.data`, exact contents and link-map
contributions, not from the aggregate output section's executable flag.

Forty-four fresh target-compiled constants verify the 124-byte arm, both
elements of its vector/projection/angle/width/color/depth/offset fields,
SDK geometry types and the 52-byte GT4. Twelve arms occupy `128..6F8`;
the reused packet occupies `1988..19BC`. Four closed CFGs cover all 598
instructions. The original descriptor/context regressions execute against
the real Spanish inputs. No direct entry-call path to the spiral is
demonstrated; retained code is not presented as a reachable animation.

Projection uses separate words at `sp+D0` and `sp+D4`, disjoint from all
direct stack stores. The original-context spill at `sp+D8` and projection
pointer spill at `sp+100` each have exactly one direct writer. The packet
base and flag pointer remain unchanged over their relevant loops, and all
direct stack writes fit the 304-byte frame. Each four-point call duplicates
the two spine inputs and their corresponding two projected-output pointers;
the subsequent single-point call writes the corresponding displaced point.
These intentional aliases stay within the measured two-element fields.
This is accessed-view and ABI-preserved lifetime evidence, not a complete
indirect-writer, allocation-capacity or whole-frame isolation proof.

Actual initialization clears the signed halfword at `1B28`; the entry
stores the byte-valued frame-step getter result at `1B08`. The getter has
two byte reads, so no stable-read assumption or stronger six-step bound
is required. Isolated retail growth/clamp execution, including branch and
load delays, agrees with an independent scalar oracle in 17,408 cases.
Given zero initialization, a byte step and no external corruption, the
seventeen sizes `0,64,..1024` are closed: growth adds `step * 64` and clamps
at 1024. An already saturated size is unchanged, with no phase transition.
Another 672 out-of-domain cases verify original signed-halfword wrapping;
twelve valid-instruction mutations are rejected. Those edge cases are not
a claim of portable defined C arithmetic for arbitrary inputs.

Existing geometry and signed positive-depth anchors remain checked.
No new whole-helper native/GTE/GPU rendering oracle, whole-animation
termination, caller reachability or complete writer audit is claimed.
Configured Spanish coverage after this follow-up is 1,372/1,612 C instances
and 1,856,256 instruction bytes across the unchanged 255 images, not
exhaustive runtime coverage.

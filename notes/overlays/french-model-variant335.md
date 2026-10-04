# French MODEL335 entry, ribbons, sheets, screen rings and streamers

Four complete `MODEL.MRG` images for models 44 and 558, stages 7 and 8,
contain the same slot-relative five-function group. Header words are 335
and 485. The selected command is 501000, so the controller initializes
the secondary handler with argument zero and subsequently calls it with
`-1`. The image instances and independent hashes are recorded in
[the instance table](french-model-variant335-instances.csv).

The entry at `0x4..0xB98`, ribbon helper at `0xB98..0x1534`, sheet helper at
`0x1534..0x1BC4`, ring helper at `0x1BC4..0x20D8` and streamer helper at
`0x20D8..0x28EC` are matching C: 2,964, 2,460, 1,680, 1,300 and
2,068 instruction bytes per image, respectively, using `gcc_2_8_1_g0_split`
(GCC 2.8.1/MASPSX 2.81). Slot 1 only renames the functions and the entry's
existing raw-suffix label. No compiler change, forced register, assembly,
artificial dependency, or source-local
external declaration is used. No shared source for another region changes.

## Ownership and remaining work

All five spans have closed, contiguous control-flow graphs:

| Offset | Bytes | Ownership |
| --- | ---: | --- |
| `0x4` | 2,964 | Resident-controller entry, C |
| `0xB98` | 2,460 | Entry-called ribbons, C |
| `0x1534` | 1,680 | Entry-called sheets, C |
| `0x1BC4` | 1,300 | Entry-called screen rings, C |
| `0x20D8` | 2,068 | Entry-called streamers, C |

The entry's calls at `0x9F0`, `0x9F8`, `0xA14` and `0xA1C`, with the incoming
context passed in their delay slots, establish the ribbon, sheet, final-helper
and ring callers. All 34 distinct external call targets across the five
functions are actual French resident function starts. The four-byte module
header and the complete `0x28EC..0x5000`
suffix retain separate raw owners. The suffix remains unclassified; it
is not asserted to contain only data or excluded from further research.

The suffix survey additionally establishes a closed 1,744-byte function at
`0x28EC..0x2FBC`, with a 304-byte frame and 11 resident imports in all four
images. It uses the six shorter records initialized by the entry. Fourteen
paired experiments remain nonexact; the best differs in 59 words per slot.
This function is still raw-owned and uninventoried, not excluded as data or
SDK code. A bounded image-local scan found no earlier direct transfer to its
span or literal word containing its entry address; that does not prove
unreachability. The remaining suffix also remains unresolved. Six native
28-byte texture descriptors have null pixel and CLUT pointers, which does
not establish an inline texture payload.

This registration contains 20 inventoried functions, all now C,
and 41,888 C instruction bytes. The streamer recovery adds 8,272 C bytes
while preserving the accepted entry, ribbons, sheets and rings, without adding images or
function spans. It does not establish exhaustive French
overlay coverage. A complete archive reconciliation against the preceding
270 registered variant instances found 2,214 unregistered instances
(2,098 distinct images), with no exact registered entry-span duplicates.
These four images are a bounded subset of that remaining work.

## Independently recovered views

The entry retains the accepted local MODEL336 control-flow and declaration
structure only where supported by MODEL335 instructions. Its two unused
`MATRIX` declarations preserve that common entry-template layout; their
unaccessed stack storage has no inferred runtime role. MODEL335 instead
uses a single part matrix, three ribbon records, four sheets, three rings,
six `0x334`-byte records and two `0x378`-byte streamers. The shorter records
fill `0x1214..0x254C`; the streamers end at `0x2C3C`. The matrix at `0x2DC8`
precedes three vectors at `0x2DE8/0x2DF8/0x2E08`. The entry's brightness,
slot and command fields extend the minimum observed context through
`0x2EB4`. This is not an allocation-capacity claim. Target compilation
checks 63 entry layout constants in addition to the 61 helper constants.

The two streamers at `0x254C` have stride `0x378`, independently agreeing
with the existing local `Variant321Streamer` declaration. Point rows are at
`0/0x110`, screen rows at `0x88/0x198`, angle/width columns at `0xCC/0x1DC`,
colours at `0x264`, projection flags/depths at `0x2AC/0x2F0`, and offset
halfwords at `0x334/0x356`. Their FT4 packet pair starts at `0x2D68`.
Seventeen points per record use extent `+0x2E88` divided by 64 for geometry;
projected width is capped at three above extent 512. Phase eight shrinks
the extent by 16 and advances to phase nine at zero. The previous-screen
iterator advances by exactly one record. No shared type is changed.

The measured configuration stride is `0x30`, with part byte `+4` and
unsigned start/end frames at `+0xC/+0x2C`. Configuration and six texture
descriptors are addressed within the existing raw suffix, using its
original label rather than declaring new storage owners. Seven native
SDK aliases are wired consistently in the family linker and all four
symbol lists: `GetClut`, `SetPolyFT4`, `SetPolyG3`, `SetPolyG4`, `Square0`,
`SquareRoot0` and `GsGetLwUnit`. Their actual resident input objects are
verified; resident inventory and SDK ownership do not change.

Three `0x394`-byte ribbon records fill context `0..0xABC`, exactly before
the sheet array. Each has 17-point rows at `0x0/0x110`, packed screen rows
at `0x88/0x198`, angle and width arrays at `0xCC/0x1DC`, colour at `0x220`,
depth and projection flags at `0x2C8/0x30C`, and signed-halfword offset
columns at `0x350/0x372`. Bytes `0x224..0x2C8` remain opaque in the ribbon
helper view; the entry view separately identifies the initial colour,
scale and control fields through `0x240`. The native
screen union exposes the same four bytes as a `DVECTOR` or `PSXLONG`.
The shared terminal halfword-column view addresses the measured fields
within the same record; it does not introduce another record or storage.
Two alternating 40-byte `POLY_FT4` packets fill `0x2D18..0x2D68`.

The existing resident binding at `0x80087868` is named `RotTransPers`
consistently in this family's linker and symbol lists. Its 44-byte SDK
body loads the two vector words, executes RTPS, writes SXY2, IR0 and FLAG
through arguments two through four, and returns SZ3 shifted right by two.
This directly verifies the local SDK declaration and is an alias correction,
not a resident inventory or SDK ownership change.

The entry initializes four `0x98`-byte sheet records at context `+0xABC`,
ending exactly at the ring array at `+0xD1C`. The helper projects four
quads per record from four rows of four eight-byte points. Rows start at
`+0x0/+0x20/+0x40/+0x60`; outer and inner colours are at `+0x80/+0x84`,
and signed scale is at `+0x88`. These independently measured offsets
agree with the existing `ModelVariantSheet` declaration. The sheet
`POLY_GT4` at `+0x2CB0` ends where the ring packet begins at `+0x2CE4`.
Target-compiler checks cover both record layouts and the projection and
transform types; no reference-project type or compiler claim is used.

The entry initializes three ring records at context `+0xD1C`, advancing
by `0x1A8` and generating 17 points in each of three rows. The ring helper
uses the same stride and bounds. Its local record contains:

| Field | Offset | Bytes |
| --- | ---: | ---: |
| Point row A | `0x0` | 136 |
| Point row B | `0x88` | 136 |
| Point row C | `0x110` | 136 |
| Inner colour | `0x198` | 4 |
| Outer colour | `0x19C` | 4 |
| Scale | `0x1A0` | 4 |
| Cycle count | `0x1A4` | 4 |

The fourth colour bytes are not read by this helper. The local record
view does not create additional storage or claim an allocation capacity.
Target-compiler layout constants verify the 424-byte stride, all seven
field offsets, the eight-byte `SVECTOR`, and the 52-byte `POLY_GT4`.
The quad is at `+0x2CE4`; signed translation halfwords are at
`+0x2E18/+0x2E1A/+0x2E1C`; step and phase words are at `+0x2E58`
and `+0x2EA0`. These helper accesses fit below context `+0x2EA4`.
The resident pointer table places secondary contexts at `0x80136000`
and `0x80176000`; these spans do not overlap their loaded model,
primary-handler, or secondary-handler images. This is a bounded helper
access proof, not a global allocation or lifetime proof.

## Reconstruction evidence

[The attempt ledger](french-model-variant335-attempts.csv) preserves five
paired entry experiments, thirteen
paired ribbon experiments, four paired sheet experiments and four paired
ring experiments, with their respective complete-production records.
It also preserves all 24 streamer experiments: 23 completed paired probes
and one slot-zero compile failure, followed by the production terminals.
The malformed do-loop experiment produced no object or comparison and is
recorded as such, not as a measured mismatch.

Streamer attempts 1 through 23 did not match; width/local-variable
variations alone did not resolve the native control flow. Investigation
of the retained suffix led back to the accepted local MODEL336 streamer
source. Keeping the screen-offset calculations inside each projection
branch, rather than manually factoring their common tail, recovered all
2,068 bytes and the 328-byte frame in attempt 24. This is meaningful
control-flow structure, not duplicate artificial work: the compiler
performs the native tail merging. Both slots and all four complete images
were independently proven with actual sized C owners before integration.

The first entry candidate had 3,064 bytes and a 216-byte frame. Its first
calibration also reversed the G3/FT4 and Square0/SquareRoot0 symbol names;
the ledger explicitly preserves that error and the corrected second
experiment. Sharing every loop index regressed to 3,084 bytes. Separating
the call-spanning sheet/ring geometry index from the record/streamer
counter recovered 2,968 bytes and the native 208-byte frame. Computing
both screen-delta components before either store and preserving
configuration-first comparison order recovered all 2,964 bytes in both
slots. No artificial dependency or forced register was needed.

Initialization uses a single part transform, target Y `-300`, target Z
`-450/+450`, sheet coordinates `+/-128`, and three 17-point ring rows
with the measured unsigned trigonometric shifts. Updates refresh both
the transform and screen projection only before the configuration end.
At or after its start, phases below seven call ribbons and sheets;
phases five and later call the final helper and rings. Phases six through
eight return four, phase nine returns one and advances to ten, and phase
eleven returns two. Brightness fades and animation-step accumulation
retain their native ordering.

The ribbon recovery progresses from 2,444 bytes/393 differing words to
2,460 bytes with the exact 304-byte frame. Genuine sheet-pointer declaration
order recovers spills; a promoted geometry index recovers point arithmetic.
The terminal offset-column view and native signed screen access recover
projection operations. A previous-screen iterator advances by the measured
record stride, rather than being recomputed each iteration. Initializing
the point index, projection-output reference and screen iterator in their
observed order recovers the final two setup instructions. The ledger retains
the intervening regressions, including the 312-byte-frame candidate.

The ribbons descend from the moving origin at `0x2DE8` to its X/Z copy
with Y zero at `0x2DF8`. Three angular phases and two sinusoidal components
shape each 17-point path; the copy is displaced along the recovered yaw
by a width derived from signed halfword `0x2E92`. The helper preserves the
second `ratan2` call whose return value is unused. Terminal projection uses
points 15/16; other segments use consecutive points. Nonnegative depth and
flags gate polygon sorting. Phase four uses unsigned timing interpolation
to grow signed count `0x2E8C` to 16 and advance to phase five. Phase six
derives width from the first sheet's signed scale divided by four, clamped
at zero. Wave counters at `0x2E98/0x2E9C` advance by step times 1150/200.

The first sheet candidate had the exact 1,680-byte size and 272-byte frame
but differed in 12 setup instructions. Splitting the bearing calculation
into a separate statement did not change the output. Reusing the radial
angle variable for the initial rotation bias corrected the setup sequence
but extended its lifetime, producing 95 register-colouring differences.
A distinct, genuinely used one-use rotation-bias scalar recovered every
instruction in both slots.

The first three sheets orbit the word-vector origin at `+0x2DE8`, using
half the signed radius at `+0x2E78`; the fourth uses the translation
halfwords at `+0x2E18`. The angle combines the bearing from
`+0x2E34/+0x2E3C`, rotation at `+0x2E74`, 3072, and one third-turn per
sheet. Rendering in phase zero is restricted to sheet zero; later phases
render all sheets, subject to nonnegative depth and projection flags.
The phase transitions retain the measured unsigned frame interpolation
through the timing-table pointer at `+0x2E60`. The fourth sheet grows to
16384 in phase five and later shrinks without setting phase seven; the
first three shrink from 4096 and set phase seven on completion. These
differences are preserved rather than generalized across the sheets.

The first ring candidate had the right size and frame but
34 differing words. Correcting the scale-update branch scope and the
measured spill ordering reduced this to 23. Moving the radius expression
after the transform fields added 24 bytes and regressed to 298 differing
words. Keeping `scale / 8` before transform setup and adding 512 in the
two point-coordinate expressions recovered every instruction in both
slots. The compiler hoists this genuine loop-invariant addition.

The retail branch skips rendering when the cycle count is nonpositive,
but still advances scale and checks completion. Its completion scalar
starts at one and is assigned one again in the terminal branch; the C
preserves that observed behaviour without introducing volatility.

Complete production images, sized linked C ownership, every retained
assembly/raw span, and the French resident executable are the acceptance
gates. Tests also cover loader sectors and commands, independent image
hashes, all five CFGs, caller arguments, local record layout, external
callee starts, and the slot wrappers.

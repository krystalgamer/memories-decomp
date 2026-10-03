# French MODEL headers 410 and 560

Two independently identified model 54 images, stages 7/8, retain exact quad,
ribbon, and ring helpers. The [instance ledger](french-model-variant410-instances.csv)
records their sectors, load addresses through slot identity, and complete
image hashes. They occupy ten sectors each at MODEL.MRG sectors 15084/15094,
loaded at `0x8013B000`/`0x8017B000`.

## Ownership and limits

| Offset range | Bytes per image | Owner |
|---|---:|---|
| `0..4` | 4 | raw header |
| `4..AAC` | 2728 | entry assembly |
| `AAC..F58` | 1196 | quads C |
| `F58..18B8` | 2400 | ribbons C |
| `18B8..1D04` | 1100 | rings C |
| `1D04..5000` | 13052 | unclassified raw suffix |

All four measured function spans in each image have fully reached, closed
control-flow graphs and one return. The entry calls the first two helpers,
but no direct call in the four measured spans reaches the ring helper.
Entry initialization establishes its accessed records; it does not prove
that this retained renderer executes. The suffix is not classified as
padding, data-only, unreachable, or non-code.

The six C owners contribute 9,392 instruction bytes, preserving the accepted
quad and ring owners. Two entry owners remain generated assembly,
totaling 5,456 bytes; four raw
header/suffix owners preserve 26,112 bytes. These twelve owners account for
both complete 20,480-byte images, not exhaustive runtime coverage.

## Independently measured ring views

Two 152-byte records start at context `0x1EF8`. Each has four rows of four
eight-byte `SVECTOR`s, at `0/0x20/0x40/0x60`; colors at `0x80/0x84`;
and signed scale at `0x88`. The last twelve bytes remain opaque to this
helper. Entry and helper independently advance records by `0x98`.
Entry initializes the inner color to `(255,255,255)` and outer color to
`(0,64,255)`; the fourth projected vector row is at the local origin.

The renderer uses the `POLY_GT4` at `0x20C4`. Entry establishes a packet
pointer at `0x2090`, advances it by 52 bytes, and calls `SetPolyGT4` at
module offsets `0x238/0x23C/0x240`. This initializer evidence independently
supports the packet type, rather than merely relying on compatible fields.

Ring zero uses three word coordinates at `0x2184`; ring one uses signed
halfwords at `0x2190`. Frame parity is at `0x21C4`, unsigned elapsed time at
`0x21C8`, step at `0x21D0`, a `G32` configuration pointer at `0x21D8`, and
phase at `0x21F0`. The unsigned duration divisor is configuration `+0xC`.
Entry also compares elapsed time and duration unsigned.

The dedicated header's `0x21F4` extent is a partial accessed view, not
allocation capacity. No descriptor count or valid command range is inferred.
SDK types and declarations come from the existing project headers.

## Preserved ring behavior

On odd frames, signed scale divided by eight supplies a pulse; even frames
use zero. Zero rotation and the selected origin feed the original matrix
sequence: `RotMatrix`, `GsGetLs`, `GsSetLsMatrix`, `ReadRotMatrix`, another
`RotMatrix`, `ScaleMatrix`, then `SetRotMatrix`.

Four projections per record use the four vector rows. The first three
vertices receive color `+0x84`, and the fourth receives color `+0x80`.
Sorting requires strictly positive depth below 2048. The projection flag
is not used as an additional visibility condition.

For ring zero, phase zero computes scale as unsigned
`(elapsed << 12) / duration`, clamps at 4096, and advances phase to one.
Phase three shrinks it by `step * 32`, clamping at zero without advancing
phase. For ring one, phase two grows scale by `step * 512`, clamps at
16384, and advances phase to three. Its separate phase-three check can
then run in the same call, shrinking by `step * 96`; reaching zero sets
phase four. These are separate checks, not mutually exclusive branches.

## Accepted ring matching evidence

The accepted French/Spanish MODEL402 renderer supplied structural prior
art. MODEL410 independently requires different record layout, dynamic
colors, direct halfword destination coordinates, and phase transitions;
its dedicated source leaves the accepted implementation unchanged.

The first reconstruction matches both complete 1,100-byte functions with
272-byte frames under the existing `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. The
[append-only attempt ledger](french-model-variant410-attempts.csv) records
both calibrations and promoted source fingerprints. Slot one changes
only the function symbol.

Independent scratch validation checks 34 target-compiled layout constants,
all eight closed function spans, packet-initializer anchors in both slots,
and 34 actual resident input-function owners against complete retail
bodies. Both complete image relinks match without masked comparisons,
using the ring C objects and explicitly owned raw fallback spans.

Ring-only clean production acceptance for #6937 reproduced the complete
French resident and all291 configured French overlays at that checkpoint. Both new production images independently
relink with maps to identical production ELFs. Those twelve selected
inputs comprised two ring C owners, six assembly owners, and four raw
owners; both ring C object texts equaled their frozen candidates, and all 34 resident owners are reverified after the clean
build. The 289 prior overlay registrations remain unchanged.

All 90 focused regressions pass without skips, including seven dedicated
MODEL410 tests, both-slot packet/color initializer anchors, and the shared
MODEL402/MODEL435 and European MODEL408 regression fixtures. Family fixtures select modules from
their explicit instance ledgers, not a shared resident-binding filename;
reusing MODEL402 bindings does not make these MODEL410 images part of that
family. MODEL408 fixtures select model54's stages9/10 explicitly, without
including its distinct stages7/8. The original 60-test gate missed these
legacy-fixture interactions, which were exposed by full test discovery. Repository
metadata validation passed. At that checkpoint, French configured totals
were 1,587 C
instances out of 1,867 inventoried functions, with 2,124,588 C instruction
bytes. These counts do not establish exhaustive coverage; French #6460
stays open.

## Directly called quad helper

The helper at `0xAAC` is directly called by the entry and draws ten
four-vertex FT4 elements. Its independently measured `0x2FC` record begins
at context zero. Four rows of ten `SVECTOR`s start at
`0/0x50/0xA0/0xF0`; color is at `0x190`, ten signed scales at `0x194`,
ten angles at `0x1BC`, and ten done words at `0x1E4`. Unaccessed gaps
remain opaque. The one observed outer iteration does not establish a
larger allocation or descriptor capacity.

The quad packet is a distinct `POLY_FT4` at context `0x2028`, not the ring
renderer's GT4. Entry instructions at image `0x48`, `0x360`, and `0x364`
establish its pointer and call `SetPolyFT4`. The helper reads signed
target halfwords at `0x2190`, direction words at `0x2198`, screen-delta
halfwords at `0x21A8`, step at `0x21D0`, and phase at `0x21F0`.
The `0x21F4` partial view is not an allocation-size claim.

Retail reserves sixteen unaccessed stack bytes between rotation at
`sp+0x28` and scale at `sp+0x40`. Accepted French MODEL476's reused
`variant459_curtains.c` provides structural precedent for representing
measured unused stack storage as opaque bytes. The quad source retains
that independently measured gap without inventing a semantic object,
executed store, fake dependency, or forced register. It also retains
scalar results of the two observed `ratan2` calls; their unused values
are eliminated by the authoritative compiler.

Signed-halfword point angles begin at zero and advance by 1300 and 1700.
Trigonometric offsets of radius128 are added to the signed target.
Nonnegative scales draw; RGB channels fade above3072 by
`color * (4096-scale) / 1024`. Sorting requires nonnegative depth and
projection flag and scale strictly below4096.

Before phase three, scales below4096 grow by `step << 8`; a crossing
resets scale to zero, and phase one advances to two. Later phases grow
and clamp at4096, marking the corresponding done word. The final point
advances phase three to four when the sum of all ten done words is at
least ten. Existing terminal scales and done words are not reset.

Three paired experiments are preserved in the append-only ledger. The
first produced1196 bytes/frame328 with86 differing words. Retaining the
angle-result locals and moving done initialization after the OT call
reduced that to85 differing words, still frame328. Explicitly representing
the measured opaque gap produced both complete1196-byte functions with
frame344 and zero differences, using `gcc_2_8_1_g0_split`.

Independent quad proof covers both complete20KiB images, twelve actual
selected input owners,28 target-compiled layout constants, all eight
closed function spans, and34 actual resident input owners. That scratch
proof deliberately kept the then-pending rings as raw fallback. Final
clean production acceptance reproduces the complete French resident and
all293 configured overlays. Both complete images independently relink with
maps to identical production ELFs. Twelve actual selected inputs cover
all40,960 bytes: four C owners/4,592 bytes, four assembly owners/10,256 bytes,
and four raw owners/26,112 bytes. All four production C texts equal their
frozen quad/ring candidates; all34 resident input owners and complete
retail bodies are reverified.

All64 focused tests pass without skips, including ten MODEL410 tests;
full discovery passes1,658 tests with five skips. The resident was rebuilt
after report regeneration so ownership tests could inspect its complete
input objects as well as its linked ELF. Metadata passes. All293 accepted
registrations remain unchanged; the authoritative French inventories now
contain1,591 C instances out of1,883 functions and2,128,692 C instruction
bytes. At that checkpoint entry, ribbons, and both unclassified suffixes remained in scope;
French #6460 remains open.

## Directly called ribbon helper

The helper at `0xF58` draws nine ribbons, each with seventeen projected points
and sixteen G4 segments. Entry directly calls this helper. Its packet at
context `0x206C` is independently established by entry instructions at image
`0x40`, `0x1C8`, and `0x1CC`, including the `SetPolyG4` call.

Nine `0x31C`-byte records begin at context `0x2FC`. Two seventeen-element
`SVECTOR` arrays start at record offsets `0/0x110`; their four-byte screen
arrays at `0x88/0x198`; angles at `0xCC`; widths at `0x1DC`; inner/outer
colors at `0x220/0x224`; depths at `0x250`; signed-halfword offsets at
`0x294/0x2B6`; and progress words at `0x2D8`. The helper does not consume
the constructor's scale/shift fields in `0x228..0x250`, so that gap stays
opaque. The `0x21F4` context view is not an allocation-capacity claim.

Screen coordinates have two equivalent local views: canonical `DVECTOR`
signed halves for projection deltas and a canonical `PSXLONG` packed word
for signed high-halfword extraction while drawing. A four-byte union
expresses those accesses without inventing storage or a new SDK type.
It is not a claim about the original source declarations. Both projection
loop counters retain their observed signed-halfword normalization.

For nonnegative phase, negative progress is clamped to zero for geometry.
Runtime ribbon directions use angle1024 for ribbon zero, then alternating
`1024 + (i+1)*200` and `1024 - i*200`. This is deliberately distinct from
the entry constructor's `1024/10` angular step. Progress below1024 grows
by `step*40` and clamps; the first point crossing while phase one advances
to two. The yaw-derived width is eight. The second observed `ratan2`
call remains even though its returned pitch is unused.

Zero rotation, target translation, and the original coordinate setup feed
`RotMatrix`, `GsGetLs`, and `GsSetLsMatrix`; there is no `ScaleMatrix` call.
`RotTransPers4` projects the current/next pair, or predecessor/current at
terminal point16, and `RotTransPers` projects the corresponding second
edge. Signed deltas feed `ratan2`, then cosine/sine supply the offsets.
The final segment has black vertices1/3. Sorting requires nonnegative
depth and the corresponding projection flag, passes low16 depth and
final argument1. The ninth ribbon reaching terminal progress1024 in
phase two advances to three. Both animation counters update even for a
negative phase, by `step*384` and `step<<6`.

Five historical paired attempts produced2348/2364/2380/2364/2284 bytes
with466/379/393/379/423 differing words. The current-master replay preserved
the2364-byte/frame920 control. Direct `DVECTOR` arrays recovered the exact
2400-byte/frame920 size and all but six instructions: four signed draw-Y
loads and two operand-register assignments. Packed pointer extraction
recovered the signed loads but folded next-point addressing, producing
2396 bytes and74 differences. A signed coordinate temporary retained the
six-word result. The equivalent union views recover both complete functions
exactly under `gcc_2_8_1_g0_split`, GCC2.8.1/MASPSX2.81.

The append-only ledger preserves all ten paired experiments and promoted
fingerprints. Slot one changes only the function symbol. Independent
scratch proof checks44 target-compiled layout constants, all eight closed
function spans, both G4 initializer anchors, both complete20KiB image
relinks with twelve actual input owners, and34 resident input owners
against their complete retail bodies. The resident input objects were
rebuilt before that proof; an older surviving ELF/map alone was insufficient.
Scratch proof keeps the other functions as explicit raw fallback.
Clean production acceptance reproduces the complete French resident and
all293 configured overlays. Both complete images independently relink
with maps to identical production ELFs. Twelve actual inputs account for
all40,960 bytes: six C owners/9,392 bytes, two entry assembly owners/5,456
bytes, and four raw owners/26,112 bytes. All six production C texts equal
their frozen candidates, including the unchanged accepted quad/ring
objects. All34 resident input owners and complete retail bodies are
reverified after the clean build.

All96 focused regressions pass without skips, including thirteen MODEL410
tests, shared-binding MODEL402/MODEL435 and European MODEL408 fixtures,
overlay source wiring, and progress. Metadata passes. The293 registrations
remain unchanged; configured French totals are1,601 C instances out of
1,883 functions and2,146,372 C instruction bytes. No report surfaces are
refreshed by this matching change.

Entry and both13,052-byte unclassified suffixes remain in scope. No
family, region, or exhaustive runtime completion is claimed.

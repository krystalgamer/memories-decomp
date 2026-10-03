# French MODEL373 grid, point groups, strip, ribbons, and quads

Model707 stages7/8 load two distinct 20KiB images from the French
`MODEL.MRG`, with headers373/523 at sectors167712/167722 and load addresses
`0x8013B000`/`0x8017B000`. The instance ledger preserves each image hash.
The directly entry-called helpers at offset`0x1EE4`,
`func_8013CEE4` and `func_8017CEE4`, have856 instruction bytes per slot.
The strip helpers at`0x223C`, `func_8013D23C` and `func_8017D23C`,
add1,352 C instruction bytes per slot without changing the accepted points.
The ribbon helpers at`0x2784`, `func_8013D784` and `func_8017D784`,
add1,764 C instruction bytes per slot, preserving both accepted helpers.
The quad helpers at`0x2E68`, `func_8013DE68` and `func_8017DE68`,
add1,412 C instruction bytes per slot, preserving all three accepted helpers.
The entry-called grid helpers at`0x176C`, `func_8013C76C` and
`func_8017C76C`, add1,912 C instruction bytes per slot while preserving
all four accepted helpers.

## Ownership and retained scope

Each image retains these exact spans:

| Offset | Bytes | Owner |
| --- | ---: | --- |
| `0x0` | 4 | Raw header |
| `0x4` | 4268 | Entry assembly |
| `0x10B0` | 1724 | Assembly |
| `0x176C` | 1912 | Matching grid C |
| `0x1EE4` | 856 | Matching point-group C |
| `0x223C` | 1352 | Matching strip C |
| `0x2784` | 1764 | Matching ribbon C |
| `0x2E68` | 1412 | Matching quad C |
| `0x33EC` | 2820 | Assembly |
| `0x3EF0` | 4368 | Unclassified raw suffix |

Across both images, twenty actual inputs cover all40,960 bytes:
ten C owners/14,592 bytes, six assembly owners/17,624 bytes, and
four raw owners/8,744 bytes. The entry directly calls offsets`0x10B0`,
`0x176C`, and`0x1EE4`. No direct caller of the strip, ribbon, or quad helpers is
observed among these sixteen closed function spans. The three other function spans
per image remain assembly; the suffix is neither padding nor excluded game code.

The dedicated resident-binding file contains34 independently verified
function addresses. Accepted MODEL402 bindings supply their existing
canonical spellings except `GsGetActiveBuff`, supplied by accepted
MODEL476 bindings. Neither existing file alone covers all observed
calls. No accepted binding file or implementation changes.

## Independently measured point view

The helper traverses three180-byte records at context`0xD48`.
Each accesses sixteen `SVECTOR` points, a rotation at record`0xA0`, and
a signed scale at`0xA8`. Other gaps remain opaque.

Entry instructions at image`0x7C`, `0xA8`, `0x528`, and`0x52C` establish
the `POLY_FT4` at context`0x2364` and call `SetPolyFT4` on that packet.
The helper reads signed target halfwords at`0x23D0`, view-direction words
at`0x23EC`, a signed frame step at`0x2410`, and phase at`0x2484`.
The partial`0x2488` view is not an allocation-size or capacity claim.

Retail reserves sixteen unaccessed stack bytes between the coordinate
and color locals. The source preserves this measured gap as opaque
`u8` storage, following accepted French MODEL476's declaration structure
in `variant459_curtains.c`, without guessing a semantic object type or
adding executed stores. The record and context types were independently
recovered, not imported from the structural lead.

## Preserved point behavior

Positive-scale groups retain the `rsin` call even though its result is
unused. The initial `ratan2` call is likewise retained. Light color is
`(192,192,192)` below scale5120, then fades by
`(scale-5120)*192/1024`. The unused dark-color stores remain present.

After rotation, scaling, and the coordinate/light-matrix setup, each
point is projected separately. The shared FT4 packet receives a
16-by-16 screen square centered on that projection. Sorting requires
nonnegative depth and projection flag; there is no extra upper-depth
cutoff.

Scale below6144 grows by `step*192`. Crossing the limit subtracts6144
once, advances rotation X by `step*192` and Z by `step*400`, then clamps
scale to6144 when phase is at least seven. Negative scales advance
without drawing. A terminal6144 scale is still positive and follows the
zero-color drawing path on subsequent calls.

## Point matching evidence

The first independently declared candidate matches both entire856-byte
functions with272-byte frames using the authoritative
`gcc_2_8_1_g0_split` profile, GCC2.8.1 and MASPSX2.81.
The append-only attempt ledger records exact text and promoted
fingerprints. Slot one changes only the function symbol.

Independent scratch proof verifies both complete retail images,
twenty actual linked input owners,22 target-compiled layout constants,
all sixteen closed function spans, and34 actual resident input-function
owners against complete retail bodies. The proof base is independently
accepted`494241c9`; integration starts from accepted`d51b71b3`, whose
intervening US-only sheet match leaves all relevant French inputs,
types, profiles, and build tools unchanged.

Before publication, the verified change was reconciled on independently
accepted`c6d22987`, preserving all291 accepted registrations, including
MODEL410 rings and their fixture fixes. The authoritative combined
inventories contain293 images,1,589 C instances out of1,883 functions,
and2,126,300 C instruction bytes. All promoted source fingerprints remain
unchanged.

Clean final production acceptance matches the complete French resident
and all293 configured overlays. Both new production ELFs
independently relink with maps; all twenty selected input owners and34
resident input owners are reverified. Both C object texts equal the
frozen exact candidates. All60 focused tests pass without skips; full
discovery passes1,643 tests with five skips, and metadata passes.

## Independently measured strip view and behavior

The strip record at context`0xF64` is108 bytes: three rows of two
`SVECTOR` inputs, three pairs of projected words at record`0x30`,
and two signed depth words at`0x64`. The remaining record gap is opaque.
Both entries establish context`0x1DF8` at image`0x3C` and call
`SetPolyGT4` at`0x224`, passing that packet at`0x228`.

The partial context view ends at`0x2488`. It reads origin words at`0x23C4`,
delta words at`0x23D8`, signed angle halfwords at`0x23E8`, step at`0x2410`,
a `G32` configuration pointer at`0x2418`, index at`0x242C`, signed factor
and width halfwords at`0x2474`/`0x2476`, and phase at`0x2484`.
Only the configuration's unsigned count halfword at`0xC` is interpreted.
Neither partial view establishes allocation size, record capacity, or a
semantic interpretation for its opaque gaps.

OT acquisition and `ratan2(angle_delta[1],angle_delta[0])+2048` precede
the positive-phase gate. Width divided by32 retains the signed negative
correction. One observed record iteration projects two sets of three
vectors, using trig angles1024/3072 and a zero middle vector. The first
origin uses the three context words; the second adds `delta*factor/1024`.
Scale4096 and the original coordinate, light, rotation and projection
sequence remain intact. The scalar projection flag does not gate drawing.

One observed drawing iteration retains earlier packet coordinate/color
stores overwritten before the sole sort. These are observed retail stores,
not invented dependencies. The first vertex pair is `(128,0,128)`, the
last `(192,192,192)`. Separate signed tests require depth greater than zero
and less than2048; sorting passes the low16 bits.

Updates additionally require `index_242C+1 == config->count_0C`.
Phase one grows factor at or below1024 by `step*32`, clamps at1024 and
sets phase two. A separate phase-five check shrinks positive width by
`step*8`, clamping at zero.

Accepted Spanish MODEL414 bands were inspected first; accepted French
`variant402_strip.c` supplied closer control-flow/expression/declaration-order
structure. All strip/context/configuration fields above were independently
recovered rather than copied from donor types.

## Strip matching evidence

The first paired candidate already had1,352 bytes and a256-byte frame,
but two instruction words differed in each slot: a combined conjunction
folded into unsigned `(depth-1)<2047`. Nested signed guards instead preserve
retail `blez` and `slti` and match both complete bodies exactly with the
authoritative `gcc_2_8_1_g0_split` profile. No type change, artificial store,
fake dependency, register forcing, or inline assembly is involved.
The append-only ledger preserves both experiments and promoted fingerprints.
The wrapper changes only the slot-one function symbol.

Independent proof on accepted`9977e704` verifies34 target-compiled layout
constants, all sixteen closed function spans, both complete20KiB images,
twenty actual selected image inputs, and34 resident input owners against
complete retail function bodies. The accepted point C objects remain selected
and equal their frozen exact texts. Registrations, symbols, bindings, shared
SDK headers, and compiler profiles are unchanged.

Before production gates, integration was transferred to independently
accepted`a193a5f4`, which includes the accepted MODEL410 quads. The combined
French inventories now contain293 images,1,593 C instances out of1,883
functions, and2,131,396 C instruction bytes. The accepted Spanish totals
and all MODEL410 sources, metadata, and ownership tests remain unchanged.

Clean final production acceptance matches the complete French resident
and all293 configured overlays. Both production ELFs independently relink
with maps; all twenty selected image inputs and34 resident input owners
are reverified. All four point/strip C object texts equal their frozen
exact candidates. All72 focused tests pass without skips, including the
accepted MODEL410 ownership/shared-binding regression. Full discovery
passes1,661 tests with five skips, and metadata passes.

## Independently measured ribbon view and behavior

Eight observed108-byte records begin at context`0x1058`. Each contains two
input vectors at`0`, projected words at`0x10`, screen-angle words at`0x18`,
two edge vectors at`0x20`, edge projections at`0x30`, width words at`0x38`,
depth words at`0x5C`, and signed X/Y offset pairs at`0x64`/`0x68`.
The record gap`0x40..0x5C` remains opaque. A separate view at context`0xFD0`
reads only grayscale bytes`0x80`/`0x84`; intervening bytes are uninterpreted.

Both entries establish context`0x1DB8` at image`0x6C`, pass it at`0x1F0`,
and call `SetPolyG3` at`0x1F8`. The partial context view ends at`0x24A0`
with target alignment and includes the signed mode halfword at`0x249C`.
Other measured accesses include view-direction words`0x23EC`, flags`0x2404`,
scale`0x2444`, and a word angle at`0x2478`. Origin, delta, signed factor,
step, phase, configuration pointer and count/index fields retain the
independently measured offsets described above. These are minimum access
views, not allocation-size or capacity claims.

All four initial `ratan2` calls remain, including two unused results.
Turn is `ratan2(direction.z,direction.x)+3072`; tilt is
`ratan2(delta.z,delta.y)+1024`. Mode one negates turn.
Positive phase gates geometry, projection, and drawing. Each of eight
records generates two points at radii40/128, advancing its angle by512.
Axial spacing is `192+(flags&1)*32`; the edge offset uses length16.
Translation is `origin+delta*factor/1024`; scale is capped at4096.

Both projection iterations preserve repeated `RotTransPers4` inputs and
outputs, followed by edge projection, screen-angle calculation and signed
screen-width offsets. One drawing iteration forms a G3 triangle using the
outer/inner grayscale bytes. Separate signed depth guards require
`0 < depth < 2048`; the low16 bits and literal one are passed to the
accepted resident packet helper. There is no projection-flag visibility
test. Outside the phase gate, the count/index condition advances the word
angle by `step*50`.

Accepted Spanish `variant442_ribbons.c` supplied the initial structural
lead; accepted French `variant402_ribbons.c` supplied the phase/upper-depth
shape. Record, color, configuration, and context declarations were
independently recovered, not imported from donor types.

## Ribbon matching evidence

The first candidate matches both complete1,764-byte bodies with296-byte
frames under the authoritative `gcc_2_8_1_g0_split` profile. The ledger
preserves the exact paired experiment and promoted source fingerprints.
The wrapper changes only the function symbol. No artificial stores,
fake dependencies, forced registers, inline assembly, or invented stack
object is used.

Independent proof on accepted`a193a5f4` verifies43 target-compiled layout
constants, all sixteen closed function spans, both complete20KiB images,
twenty selected image inputs and34 resident input owners against complete
retail bodies. That proof preserves the accepted point C and does not
import the then-pending strip implementation. Integration starts from
independently accepted`58df911e`, preserving both point and strip C,
all293 registrations, symbols, bindings, SDK headers, and compiler profiles.
Combined French inventories contain1,595 C instances out of1,883 functions,
and2,134,924 C instruction bytes.

Combined production acceptance verifies the clean complete French resident
and all293 configured overlays byte-for-byte. Both complete production images
relink identically with maps: twenty selected inputs cover40,960 bytes,
including six C owners/7,944 bytes, ten assembly owners/24,272 bytes, and
four raw owners/8,744 bytes. All six point/strip/ribbon C texts equal their
frozen candidates; all34 resident input owners and complete retail bodies
are reverified. All74 focused tests pass without skips; full discovery passes
1,663 tests with five skips. Metadata policy checks pass.

## Independently measured quad view and behavior

One observed136-byte record at context`0xFD0` contains four rows of four
`SVECTOR` inputs and two four-byte colors at record`0x80`/`0x84`.
Both entries establish the prior GT4 at`0x1DF8`, advance52 bytes at image`0x278`,
then call `SetPolyGT4` at`0x27C` with the resulting packet at context`0x1E2C`.
The partial context ends at`0x2488`. Origin/delta are at`0x23C4`/`0x23D8`,
flags at`0x2404`, elapsed at`0x2408`, step at`0x2410`, the `G32`
configuration pointer at`0x2418`, index at`0x242C`, scale/brightness at
`0x2444`/`0x2448`, signed factor halfword at`0x2474`, and phase at`0x2484`.
Configuration reads an unsigned count halfword at`0xC` and unsigned
divisor word at`0x14`. Other gaps stay opaque; these views do not establish
allocation size or capacity.

Drawing is not gated on positive phase. Phase eight makes the first color
brightness grayscale and the second `(0,brightness*128/255,brightness)`.
Odd flags add signed scale/8 to each scale component. Translation remains
origin plus delta*factor/1024. The coordinate, light-matrix, rotation,
scaling and projection sequence is preserved. Four inner iterations project
one column from each vector row into the shared GT4. The first three
vertices use color one, the last uses color zero. Separate signed tests
require0<depth<2048; sorting uses low16 depth, without a projection-flag test.

Updates require index+1 equal to the configuration count. Phase zero,
while scale<4096, performs unsigned `(elapsed<<12)/duration`, preserving
the retail divide-by-zero trap rather than inventing a guard; reaching4096
clamps scale and sets phase one. Phase three grows factor<=1024 bystep48,
clamping1024 and setting phase four. Phases four/five grow scale bystep512
to8192. Phase six shrinks positive scale bystep32 to64 and sets phase seven.
Phase seven grows scale bystep2560 to10240. Phase eight shrinks positive
brightness bystep8 to zero and sets phase nine.

Accepted Spanish432 draw supplied structural evidence only. All record,
context and configuration views above were independently recovered.
Sixteen unaccessed stack bytes between rotation and scale remain opaque
storage, following the accepted structural precedent without invented
stores or a guessed semantic object.

## Quad matching evidence

The first paired candidate matches both complete1,412-byte bodies with
288-byte frames under authoritative GCC2.8.1/MASPSX2.81
`gcc_2_8_1_g0_split`. The ledger records exact text and promoted fingerprints;
the wrapper changes only the slot symbol. No inline assembly, forced
registers, artificial stores, fake dependencies, or source-local externs.

Independent proof on accepted`87afd641` verifies38 target-compiled layout
constants, sixteen closed function spans, both complete20KiB images,
twenty selected image inputs, and34 resident input owners against complete
retail bodies. Both accepted point and strip C texts equal their frozen
candidates. That proof deliberately excludes then-pending ribbon PR #6945.
Integration was subsequently transferred to independently accepted`6f507ef9`,
preserving all accepted point/strip/ribbon C and their fingerprints.
All293 registrations, bindings, symbols, shared SDK headers, profiles and
accepted Spanish totals remain unchanged. Combined French inventories
contain1,597 C instances out of1,883 functions and2,137,748 C instruction bytes.

Combined production acceptance verifies the clean complete French resident
and all293 configured overlays byte-for-byte. Both complete production images
relink identically with maps: twenty selected inputs cover40,960 bytes,
including eight C owners/10,768 bytes, eight assembly owners/21,448 bytes,
and four raw owners/8,744 bytes. All eight point/strip/ribbon/quad C texts
equal their frozen candidates; all34 resident input owners and complete
retail bodies are reverified. All64 focused tests pass without skips;
full discovery passes1,666 tests with five skips. Metadata checks pass.

## Independently measured grid view and behavior

The grid begins at context zero: nine rows of seventeen `SVECTOR` points
have136-byte row stride, followed by nine four-byte colors at`0x4C8`.
The view ends at`0x4EC`. Both entries establish context`0x21A0` at image`0x4C`,
pass it at`0x490`, and call `SetPolyGT4` at`0x494`; the eight-iteration
initializer advances52 bytes at`0x518`. Rendering uses eight observed GT4
packets, reusing each row's packet for all sixteen adjacent point segments.
These are access views, not claims about allocation size, exclusive storage
ownership, or non-overlap with other packet views.

The partial context reaches the signed orientation halfword at`0x249C`
and has target-aligned size`0x24A0`. Origin/delta are`0x23C4`/`0x23D8`,
unsigned elapsed`0x2408`, signed step`0x2410`, and the `G32` configuration
pointer`0x2418`. Scale, translation factor, word rotation, oscillation,
brightness, texture offset, and phase are respectively`0x2430`, `0x2434`,
`0x2438`, `0x243C`, `0x2440`, `0x2480`, and`0x2484`.
Configuration words`0x14`, `0x18`, `0x1C`, `0x20`, and`0x24` are unsigned
timeline boundaries. Gaps remain opaque.

Both unused initial `ratan2` calls remain. Orientation chooses the sign of
X rotation. Translation is origin plus delta*factor/1024; Y additionally
uses-256+factor/4. Phase five sets brightness to1024-(scale-8192)/8 and
updates nine blue-to-red gradient colors; phase six and later use
brightness/8 grayscale. Earlier phases leave the colors unchanged.
Each row's UV origin is texture offset minus16*row, with U32/95 and
Vorigin+127/origin+111. Top vertices use color[row], bottom vertices
color[row+1]. Sorting requires nonnegative depth and projection flag,
passes low16 depth, and has no upper-depth cutoff.

Texture offset at or below128 advances8 and wraps at128. Two unsigned
timeline divisions grow scale to4096/phase one and translation to1024/
phase three; the second additionally requires phase below three. Both
retain retail divide-by-zero traps. Phase three grows scale bystep512 to8192
and phase four. Phase four oscillates scale around6144 with amplitude2048,
advances its angle bystep64, and enters phase five after configuration`0x24`.
Phase five grows bystep256 to16384, entering phase six with brightness1024.
Phase six keeps growing scale while fading positive brightness bystep32;
zero brightness advances phase seven. After configuration`0x1C`, the
rotation word advances bystep64 independently of those phase branches.

Accepted Spanish445/415 bands were inspected first but have different
geometry and control flow. Accepted shared `variant459_grid.c` provides
closer projection, gradient, and later-phase structure only. Its
types, constant packet use, phase numbering, and translation behavior were
not imported.

## Grid matching evidence

Four paired experiments are retained in the append-only ledger. The first
has1,956 bytes and326 differing words; the second has1,924 bytes and271
differences. A translated-Y local, full-width UV row local, loop-update
evaluation, and deadline operand order recover the observed scheduling.
The third has the correct1,912 bytes but two differing words: positive16
induction/subtraction instead of negative16 induction/addition.
Expressing the descending row offset directly resolves both. The fourth
matches both entire1,912-byte functions with288-byte frames under
authoritative GCC2.8.1/MASPSX2.81 `gcc_2_8_1_g0_split`.
No forced registers, artificial stores, fake dependencies, inline assembly,
or source-local extern declarations are used.

Independent proof on accepted`e70cb8b2` verifies43 target-compiled layout
constants, sixteen closed function spans, both complete20KiB images,
twenty selected linked image inputs, and34 actual resident input owners
against complete retail bodies. All eight accepted point/strip/ribbon/quad
C texts still equal their frozen candidates. Registrations, symbols,
bindings, shared headers, compiler profiles, and other regional totals are
unchanged. Combined French inventories contain1,599 C instances out of1,883
functions and2,141,572 C instruction bytes.

Clean final production acceptance matches the complete French resident
and all293 configured overlays byte-for-byte. Both complete production images
independently relink to identical ELFs with maps. Twenty selected inputs cover
40,960 bytes: ten C owners/14,592 bytes, six assembly owners/17,624 bytes,
and four raw owners/8,744 bytes. All ten C object texts equal their frozen
candidates; all34 resident input owners and complete retail bodies are
reverified. All30 focused tests pass without skips; full discovery passes
1,669 tests with five skips. Metadata policy checks pass.

French #6460 stays open; these measured functions do not establish
exhaustive coverage.

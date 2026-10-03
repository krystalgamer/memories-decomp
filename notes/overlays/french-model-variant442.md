# French MODEL headers 442 and 592

Four distinct secondary images reuse the accepted
`src/overlays/model_variant/variant425_{sheet,spiral,rays,webs,ribbons,bands,spokes,rings,quad}.c` bodies
through eighteen French symbol-renaming wrappers. The ribbon and spiral wrappers
define `VERSION_FRENCH` for their measured source-order differences. The existing
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 pipeline is authoritative.
Other shared expressions, headers, G32 annotations and US profiles remain unchanged.
US header 425 is source provenance, not French byte-identity evidence.

## Loader and boundaries

Models 259 and 630 (compact records 259 and 580) use stages 7/8. Record
sectors 180/190 in 276-sector compact records select ten-sector images at
`0x8013B000`/`0x8017B000`. The resident controller calls image `+4` with
context and initial command or update `-1`. The
[instance ledger](french-model-variant442-instances.csv) records actual
nonnegative commands, slices and four distinct complete hashes. Other
stages and models are not covered.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1174` | 4464 | generated assembly | yes |
| `0x1174..0x1560` | 1004 | sheet C | yes |
| `0x1560..0x1F2C` | 2508 | spiral C | yes |
| `0x1F2C..0x2924` | 2552 | rays C | yes |
| `0x2924..0x2CE8` | 964 | webs C | yes |
| `0x2CE8..0x3334` | 1612 | ribbons C | no |
| `0x3334..0x3A40` | 1804 | bands C | no |
| `0x3A40..0x3D50` | 784 | spokes C | no |
| `0x3D50..0x40CC` | 892 | rings C | no |
| `0x40CC..0x4430` | 868 | quad C | no |

Strict walks cover all ten functions with one terminal return per span and
no unresolved indirect transfer. Entry reaches only the first five
functions. The sheet, spiral, rays and webs C helpers are entry-reachable; the other five C helpers
remain retained module-local code without a demonstrated entry execution
path. Real storage owners preserve each
four-byte header and 3,024-byte suffix at `0x4430..0x5000`; the suffix is
explicitly unclassified, not established non-code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. A single 456-byte `ModelVariantBand` begins
at context `+0x1CA0`, followed by one 152-byte sheet at `+0x1E68`, six
144-byte rings at `+0x1F00`, four 144-byte spoke records at `+0x2260`,
and one 144-byte quad at `+0x24A0`. Adjacent extents agree exactly. This is
the 456-byte band, not header435's 492-byte padded form.

Forty-four entry anchors check saved pointers, initialization counters,
increments, bounds, strides and cleared rotation. The band counter starts
zero and repeats only while nonpositive after its increment, establishing
one record. Its color rows at 324/360 each contain nine four-byte entries.
The quad loop also initializes one record; rings/spokes use bounds six/four.
These are accessed views, not whole-context allocation or extra execution
path evidence.

Ninety-five local layout constants are independently target-compiled for
this family. They cover band/sheet/ring/spoke/quad records and the SDK views
listed in the [companion422 evidence](french-model-variant422.md), including
four-byte target `long` and stored-pointer-bearing `GsCOORDINATE2`. The band
has point rows at 0/72/144, screen rows at 216/252/288 and depths at 420..452.

## Exact matching and preservation

The [attempt ledger](french-model-variant442-attempts.csv) records eight
initial terminal canonical-wrapper matches and two subsequent webs matches
after fresh current-header compilation.
Rebased candidate searches are not the acceptance gate: all four real
canonical links reproduce unmasked complete images. Thirty-six resident
bindings are independently checked; the named `RotTransPers3` address comes
from the accepted French Exodia manifest.

The combined422/442 production gate preserves all 144 previously accepted
registrations, reproduces all 152 French images and matches the clean
French resident. This family adds 16 C owners and 17,392 bytes, alongside
24 assembly owners, eight raw owners, 36 fresh-resident callee owners and
95 recompiled layouts. The 52,416 assembly bytes and 12,096 unclassified
suffix bytes remain untranslated.

Five inherited regressions check actual archive slices, commands, complete
hashes, canonical sources/fingerprints, raw extents, all ten control-flow
spans and 44 entry anchors. Together the two families add eight images,
32 C instances and 34,784 C bytes. Configured totals become 152 images,
668/1105 matching C instances and 602,540 C bytes, without claiming
exhaustive runtime coverage or seven-release completion. General progress
snapshots are separate.

## Entry-reachable smaller-web follow-up

The accepted `variant425_webs.c` compiles unchanged to 964 French bytes.
Two wrappers rename `func_8013D8F8` to `func_8013D924/func_8017D924`.
All four complete canonical images match after replacing only the assembly
owner at `0x2924..0x2CE8`, adding four C instances / 3,856 instruction
bytes. No module records, hashes, function boundaries, resident bindings,
symbol addresses or raw extents change. Final production validation
reproduces all 192 configured French images and the clean French resident.
The family has 20 section-defined C owners, 20 assembly owners and eight
raw owners; the former webs assembly object is absent from selected link
inputs. All 36 family callees and three caller/loader owners were checked
again, and all 25 layout constants were freshly recompiled. All 96 French
MODEL variant and 47 progress/global-usage regressions pass, together with
metadata, basic-type and G32/PSXLONG policy checks.

French entry captures `a0 -> s3 -> s6`, saves the web pointer at `sp + 0x94`,
and initializes three 512-byte records at context `0..0x600`. The next
record area begins at `+0x600`. Each record has two five-by-six `SVECTOR`
grids at 0 and `0xF0`, with 48-byte rows, color at `0x1E0` and scale at
`0x1F4`. These are not the 608-byte six-by-six webs of header 435.
Entry loop bounds and strides establish the smaller shape independently
of the reused header. Entry calls the helper at module `0xFF0`, passing
the same context through `a0 = s3`.

The helper uses `RotTransPers3`, the line packet at context `+0x266C`,
translation at `+0x26A8/+0x26AC/+0x26B0`, step at `+0x2700`, and state at
`+0x2748`. Twenty-five freshly target-compiled constants verify the local
web and SDK views, including packed `GsGLINE` color offsets 12/15.
Twenty-two retail anchors check entry/helper capture, record pointers,
grid and record bounds, strides and scale access in every image.

The selected descriptor is a 56-byte record at module
`+0x452C + (command % 1000) * 56`, within the single preserved suffix.
Every legal archive slice and command is independently checked. All eight
helper callees, including `RotTransPers3`, and the three matching resident
initializer/controller/loader owners agree with the fresh exact French
resident. The existing 36 family bindings already supply every name.

The fixed slot contexts `0x80136000/0x80176000`, passed through `field_DEC`,
have a direct-entry access minimum of `0x2760` (10,080 bytes). This is not
an allocated-capacity declaration. The selected model/primary/secondary
loads do not overlap that view; whole-game lifetime isolation and all
primary-context writes are not inferred.

The family becomes 20 matching C owners / 21,248 C bytes, 20 assembly
owners / 48,560 bytes, and eight raw owners / 12,112 bytes (including
the four headers). Configured French totals become 192 images,
792/1,343 matching C instances and 724,668 C instruction bytes.
Suffixes remain unclassified and counts are not exhaustive runtime coverage.

## Retained short ribbons

The [Family435 first-segment indexing recovery](french-model-variant435.md)
also produces this family's complete 1,612-byte helper and 296-byte frame.
Both slots and all four complete canonical images match. The shared
`variant425_ribbons.c` uses `VERSION_FRENCH` only for the two indexed screen
coordinate expressions inside `k == 0`; the original US path is unchanged.
Two guarded wrappers add four C instances / 6,448 bytes.

The [attempt ledger](french-model-variant442-attempts.csv) preserves the
rejected unchanged-body trial (1,600 bytes/frame 288/218 unequal words),
the rejected typed-screen trial (1,604/frame 288/215 unequal words), both
exact indexed-coordinate trials and the two canonical terminal records.
Semantic equivalence alone was never accepted.

Eight 108-byte ribbons occupy context `0x1940..0x1CA0`, immediately before
the existing band. Point, screen, angle, offset-point, offset-screen and
width fields agree with the 116-byte Family435 shape through offset 64.
The opaque middle extent is eight bytes shorter: depths are at 92 and
halfword offsets at 100/104. The record is not interchangeable with the
116-byte form. The helper draws a `POLY_G3` at `0x2530` for strictly
positive depth, unlike Family435's nonnegative-depth condition.

Scale is the sheet field at `0x1E68 + 0x88`. The selected 56-byte descriptor
is still `module + 0x452C + (command % 1000) * 56`; every command is
`608000`. Its halfword at `+0x18` is one and is compared with the signed
halfword at context `0x2728`, plus one. Equality advances the angle at
`0x2744` by the step at `0x2700` shifted left five. This comparison value
is not evidence of a whole-animation duration.

The combined independent proof compiles 45 local/SDK constants, checks
59 focused anchors per family and verifies every slice, command and
descriptor, all 11 ribbon callees and three resident caller/loader owners.
The 36 binding addresses do not change; `0x80087868` receives the accepted
`RotTransPers` name. The direct context minimum remains `0x2760`, not
allocation capacity or global lifetime isolation. Ribbons is retained-only.

This family becomes 24 C owners / 27,696 bytes, 16 assembly owners /
42,112 bytes and eight raw owners / 12,112 bytes. The independent combined
435/442 batch adds 48,360 C bytes across 30 existing registrations, with
27 distinct complete images. No registration, boundary, profile or suffix
changes. All 222 French and 263 US complete images and both clean residents
passed exact matching. Fresh production checks establish the combined
206 French C / 68 assembly / 60 raw owners, preserve all 30 US ribbon C
owners and recompile all 45 layout constants. All 130 French, 57 Spanish,
16 progress and five US toolchain regressions pass without skips, alongside
the repository policy gates. Inventory totals are not exhaustive runtime coverage.

### Accepted ownership reconciliation

The combined ribbon branch normally merges fixed accepted `e8c82d3d5`,
preserving all 252 registrations, 936 accepted C instances and the accepted
petal and Family445/433 webs owners. Only the original 30 ribbon instances
/ 48,360 bytes are added: totals are 966/1,581 C instances and 965,620 C
bytes. All 135 original paths remain; 131 files are byte-identical, with
aggregate progress, the two family notes and a regional ledger guard
updated. The guard keeps French experiments out of Spanish ledger checks.
One additional Spanish435 fixture preserves its original six-helper selection rather
than inheriting the French ribbon promotion. No Spanish source, inventory
or binding changes; the total scope is 136 paths. No pending French branch
is stacked.

The accepted additive header declarations are freshly calibrated in all
30 canonical images and 45 compiled layouts, alongside 59 anchors and
eleven callees per family. The baseline assembly's old projection alias
is retained only for the scratch relink at its unchanged verified address.
All 263 US and 252 French complete images and both clean residents pass
again. Production verification preserves thirty petals, twelve Family445
webs, four Family433 webs and all thirty US ribbon C owners, with the
combined ribbon families' 206 C, 68 assembly and 60 raw owners unchanged.
The 141 French, 72 Spanish, 16 progress and five US toolchain regressions
pass without skips, alongside metadata/basic-types/external-attempts/G32 checks.

Before publication, accepted Family402 strips at `ca50339b5` are merged
normally as well: four preserved C owners / 6,064 bytes, with no pending
Family402 ribbon branch stacked. The fixed accepted baseline has 940 C
instances; the unchanged thirty-ribbon addition gives 970/1,581 C instances
and 971,684 bytes across 252 images. Scope remains 136 paths, with 131
original files unchanged. US/shared/Spanish build inputs are identical
across this accepted strip-only delta. All 252 French images and the
clean resident pass again, with 142 French, 72 Spanish, 16 progress and
five US toolchain regressions and the policy gates. Production verification
proves the four accepted strip C owners alongside the previously checked
petal, webs and ribbon owners, and recompiles all 45 layout constants.

Requested accepted cutoff `53922e730` is merged next, retaining its
region-aware descriptor checks and Spanish442 standalone-ribbon fixture
semantics. The Spanish fixture explicitly lists its six accepted helpers
instead of appending a duplicate ribbon to the expanded French selection.
Its source, inventory and bindings remain unchanged. French wrapper macros
and experimental-ledger checks remain French-only. Scope becomes 137 paths,
with 130 original files unchanged; French totals remain 970/1,581 C
instances and 971,684 bytes. No pending Family402 ribbon branch is stacked.
All 263 US and 252 French complete images and both clean residents pass
again, with actual accepted strip/petal/webs owners, all original ribbon
owners and 45 freshly compiled layouts verified. The 142 French,
81 Spanish, 16 progress and five US toolchain regressions and repository
policy gates pass.

Newer requested cutoff `625cf9497` adds an independently checked
70-path US-only delta for header-404 bands and header-423 sheets. French
and Spanish build inputs and existing shared sources/headers/profiles
remain unchanged. The completed French image/resident proof is retained,
including hashed resident artifacts. Fresh acceptance passes for all
263 US images and the clean US resident. Production evidence proves the
twelve accepted header-404 bands, four header-423 sheets and thirty US
ribbon C owners, retains the unchanged French owners, and recompiles
45 layout constants. The 142 French, 81 Spanish, 16 progress and five US
toolchain regressions and repository policy gates pass.

Newly accepted Family402 ribbons at `3b77bdf0c` are normally merged next:
four more preserved C owners / 6,480 bytes, not a pending-branch stack.
The accepted baseline now has 944 C instances; the original thirty ribbons
remain the only additions, giving 974/1,581 C instances and 978,164 bytes
across 252 images. Its exact 24-path French-only delta preserves all US
inputs and the hashed US resident evidence. All 252 French images and the
clean French resident pass again. Production ownership preserves the four
accepted strips, four accepted Family402 ribbons and all other verified
French/US owners; 45 layout constants are recompiled. The broader regional
suite passes 199 French and 103 Spanish tests, including all 143 French and
81 Spanish MODEL-family tests, with 16 progress and five US toolchain tests
and policy gates passing. Scope remains 137 paths and 130 unchanged originals.

## Entry-called sheet follow-up

Two three-line canonical wrappers reuse accepted `variant425_sheet.c`
unchanged. Both French functions at `+0x1174..+0x1560` match 1,004 bytes
with a 256-byte frame, and actual scratch links reproduce all four
complete unmasked images. The sixteen original attempt rows remain
byte-identical, followed by two terminal canonical matches. No regional
guard, shared-source, header, compiler-profile or binding change is needed.

Independent evidence checks 30 target-compiled local/SDK constants and
184 instruction anchors. Entry initializes one 152-byte sheet at context
`0x1E68..0x1F00`. Four four-point `SVECTOR` rows start at `0/0x20/0x40/0x60`,
outer and inner colors at `0x80/0x84`, and size at `0x88`. The helper reuses
the `POLY_GT4` at `+0x25A4` for four quads. It scales projection depth by
eight tenths and requires both that depth and the stack flag at `+0xD4`
to be nonnegative before sorting the low sixteen depth bits.

All nine helper callees, 36 resident bindings and three caller owners
are verified against the clean French resident. Entry calls the helper at
`+0xFE0` with the original context. Command `608000` selects the 56-byte
descriptor at `+0x452C`; growth times at `+0x1C/0x20` are `44/80`, and
shrink times at `+0x2C/0x30` are `320/440`, with positive denominators.
During phase one at context `0x2748`, the value at `0x2734` advances by
the frame step at `0x2700` times sixteen, capped at `0x600`, then enters
phase two. The direct context minimum `0x2760` is not allocation capacity.
The `0x4430..0x5000` suffix remains unclassified.

The independent cutoff is accepted
`9d0f2b84ea8214e652f22ed21783ccc048cfa9fd`, not pending French415 bands.
This adds four C instances / 4,016 bytes while retaining all 252 images
and 1,032 accepted C instances. Configured totals become 1,036/1,581 C
instances and 1,080,956 bytes. Expected family ownership is 28 C owners /
31,712 bytes, preserving all 24 prior C owners / 27,696 bytes, alongside
twelve assembly owners / 38,096 bytes and eight real header/suffix owners /
12,112 bytes. Production acceptance passed all 252 complete French images
and the clean resident, with all 28 C owners verified in actual linked
ELF/object definitions. The 204 French, 112 Spanish, sixteen progress and
five US-toolchain regressions pass without skips, alongside G32 and
repository policy. Shared US/Spanish sources and profiles are unchanged;
no fresh US rebuild is claimed. The authored scope is eighteen paths.

An ordinary merge of accepted
`cea956d3679bda7a9d91edee564e85dc40766118` retains the maintainer-merged
French415 bands. Combined configured totals are 252 images, 1,046/1,581 C
instances and 1,100,156 bytes, preserving all 1,042 accepted C instances.
Sixteen original authored files remain byte-identical; only this note
and the progress fixture change. Reconciled acceptance passed all 252
complete French images and the clean resident, explicitly preserving all
forty accepted Family415 C owners / 59,080 bytes and all 28 Family442 C
owners / 31,712 bytes. The 204 French, 112 Spanish, sixteen progress and
five US-toolchain regressions pass without skips, alongside G32 and
repository policy. No shared US source or profile changes relative to
the accepted cutoff are introduced, and no fresh US rebuild is claimed.

## Entry-called spiral follow-up

The helper at `+0x1560..+0x1F2C` contributes 2,508 C bytes per image, using
GCC 2.8.1/MASPSX 2.81 and the named `gcc_2_8_1_g0_split` profile. Two
regional wrappers reuse accepted `variant425_spiral.c` and its existing
arm view. A narrow `VERSION_FRENCH` arm separates the retained turn
adjustment until after timing-pointer setup. The non-French source arm
retains the accepted expression and statement order.

The unchanged accepted source produced the correct 2,508-byte extent and
440-byte frame, but exchanged two stores around a signed-division branch
delay slot. Splitting the meaningful turn adjustment resolves both
instructions in both load slots without forced registers, extra stores,
or artificial dependencies. Four experiment rows preserve the
source/header/wrapper fingerprints: SHA-256 of the sorted
`body.c:sha256`, `header.h:sha256`, and `slot.c:sha256` lines, each followed
by a newline. The two terminal rows use the canonical wrapper's SHA-256,
following the existing family ledger convention.

Four independent complete-image links contain forty real, section-defined
function owners: eight C and two assembly per image. All 28 earlier C
objects remain unchanged, and eleven actual resident dependency definitions
are checked through selected input objects, linker-map placement, final
symbols, and complete retail function bytes. The four headers and four
suffixes retain real raw definitions. No suffix bytes are classified as C.

Independent target-compiled checks establish 24 local/SDK layout constants.
There are sixteen 124-byte arms at context `0x600..0xDC0`, each with two
`SVECTOR` points, projected positions, angles, widths, depths, and narrow
screen offsets. The stack projection-status array is exactly 128 bytes.
The helper reuses the 52-byte `POLY_GT4` at `0x2570..0x25A4`; timing scale
is at `0x1EF0`. Twenty instruction anchors per image verify the strides,
loop limits, state accesses, and matching store order. The required context
end `0x274C` stays within the existing measured `0x2760` extent and clear
of both slots' live image loads. These are required spans, not allocation
capacity or exclusive simultaneous lifetimes.

At accepted cutoff `94a19bafab7c3b81d2f2ba35d2589cfc9aa8c063`, this adds
four C instances and 10,032 bytes: configured French totals become
1,333/1,595 instances and 1,767,500 C bytes across 253 modules. Family442
has 32 C owners / 41,744 bytes; entry and the other drawing helper retain
eight assembly owners / 28,064 bytes. The 12,096 suffix bytes remain
unclassified, and these configured counts are not exhaustive runtime
coverage or completion of the French campaign.

Final acceptance passes the clean French resident and all 253 complete
overlays, with 52 focused French/Spanish/progress regressions without skips
and metadata, basic-types, source-contract, declaration-visibility and
notes gates. Production ownership verifies all 32 C owners / 41,744 bytes
and preserves all 28 earlier C objects. The non-French preprocessed
translation unit is unchanged, and the French function equals the frozen
exact candidate. This source-preservation check is not a fresh North
American binary match; exact-head CI remains required.

## Entry-called rays

The 2,552-byte helper at `+0x1F2C..+0x2924` now uses two thin renaming
wrappers around the unchanged accepted header-425 rays source and header.
Both load slots match immediately under the authoritative
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile, including the 456-byte
frame. No regional conditional, new type, compiler flag, artificial store,
source-local declaration or source-body change is needed. The two
function-only ledger fingerprints identify the frozen scratch body, whose
only differences are relative include paths; terminal fingerprints identify
the canonical wrappers. All 24 historical ledger rows are preserved.

Four complete scratch and canonical production images match. Mapped proof
ELFs equal the actual production ELFs, and all selected C objects appear in
the generated linker script and map with section-defined, sized function
owners and exact final bytes. All 32 earlier C objects remain byte-identical.
Family totals are **36 C owners / 51,952 bytes**, four assembly entry owners /
17,856 bytes, and eight raw header/suffix owners / 12,112 bytes. The 12,096
suffix bytes remain unclassified. This is not exhaustive runtime coverage.

Independent target compilation verifies 54 layout constants; 104 retail
instruction anchors verify their accesses. Sixteen 184-byte rays span context
`0xDC0..0x1940`, ending at the existing ribbon records. Each ray has three
eight-byte `SVECTOR` points at `0x18` and `0x48`, projected word rows at
`0x30` and `0x60`, angles at `0x3C`, widths at `0x6C`, depths at `0x94`,
and signed-halfword offsets at `0xA0`/`0xA6`. Unaccessed bytes stay opaque.
The helper reuses the existing 52-byte GT4 at context `0x2570`; translation,
orientation, timing and phase accesses remain within the established
`0x2760` required extent, not a new allocation-capacity claim.

The target's projection status array is 128 bytes at `sp+0xD0`, with
eight-byte rows. The third point uses `status[i][2]`: it
addresses the next row, and for the last ray reaches `sp+0x150`, aliasing
the projection `p` output. The separate flag is at `sp+0x154`. This
out-of-bounds source spelling reproduces measured retail behavior; it is
not evidence of a larger array or a safe-array guarantee. Drawing reads
only the first two status words per ray, accepts nonnegative depth and
status, and passes the low 16 depth bits to the existing three-argument
`GsSortPoly`. The last segment's far colors are zeroed. Growth adds
the context `0x2700` step times 256, clamps size at `0x1000`, and advances
phase 2 to 3.

All 36 existing resident bindings and the rays helper's eleven callees are
checked against real section-defined resident symbols and full retail
function bytes. Entry calls the helper at image `+0x100C`, passing its
original context from `s3` in the delay slot. No new address aliases or
reachability assumptions are introduced.

At accepted cutoff `a882159bf2fffaa3518a420b25c96f4cb49e4130`, this adds
four matching C instances / 10,208 bytes: configured French totals become
1,357/1,597 instances and 1,822,732 C bytes across 253 images. This independent
branch does not include the pending MODEL465 spiral change. Shared bodies,
headers and other regional registrations remain unchanged.

Rays final acceptance passed: the clean French resident and all 253
configured French overlays match. The four final mapped production ELFs
remain identical to the actual build ELFs, with all 32 earlier C objects
unchanged. All eleven callees have actual selected resident input
definitions, matching map placement, sized final symbols and complete
retail function bytes. All 55 focused French/Spanish/progress/toolchain
regressions pass without skips, together with metadata, attempt-ledger,
basic-type, header, matching-source, data-symbol, declaration-visibility
and notes policies.

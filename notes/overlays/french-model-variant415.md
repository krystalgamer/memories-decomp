# French MODEL headers 415 and 565

Ten distinct images at stages 9/10 for models 102, 282, 288, 642 and 645
reuse unchanged accepted `variant398_sheets.c` and `variant398_webs.c`
through four canonical renaming wrappers. The named
`gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81; US compiler
provenance is not a French compiler-selection rule.

## Loader and complete ownership

The matched MODEL loader selects ten 2,048-byte sectors from 276-sector
compact records, at record offsets 200/210. Loads are `0x8013B000` and
`0x8017B000`, with entry at `+4`. The controller supplies context and
the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant415-instances.csv) records
actual models, compact indices, commands, slices and complete hashes.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x12B4` | 4784 | generated assembly | yes |
| `0x12B4..0x18A4` | 1520 | generated assembly | yes |
| `0x18A4..0x2024` | 1920 | generated assembly | yes |
| `0x2024..0x2508` | 1252 | sheets C | yes |
| `0x2508..0x2A34` | 1324 | webs C | no |
| `0x2A34..0x2FB8` | 1412 | curtains C | yes |

Strict control-flow walks cover every instruction and delay slot in all
six spans, each with one terminal return and no unresolved indirect
transfer. Webs remains retained game code: no direct entry-call path is
claimed. The four-byte header and 8,264-byte suffix at
`0x2FB8..0x5000` have real storage owners. The suffix remains unclassified,
not proven non-code.

## Independently recovered views

Entry captures `a0 -> s3 -> s8`. It initializes two 152-byte sheets at
context `0x6A8..0x7D8`, saving the pointer at stack `0x84`. Four corner
arrays begin at record offsets `0/0x20/0x40/0x60`, outer/inner colors
at `0x80/0x84`, and size at `0x88`. Pointer reloads, the four-corner
bound, two-record bound and 152-byte advances establish the accessed view.

Three 416-byte narrow webs at context `0..0x4E0` have two 4-by-6
`SVECTOR` grids at `0/0xC0`, color at `0x180`, scale at `0x194` and
done at `0x198`. The grid walker is `s1`, unlike Family439's `s2`.
Entry clears done separately from scale; the helper updates this field
during its final phase. Eight-byte element, 48-byte row and 416-byte
record advances agree with the six/four/three bounds.

Real commands `581000`, `581001` and `581004` select 48-byte descriptors
at module `0x30B4 + (command % 1000) * 48`. Entry's shift/add sequence
independently establishes the stride; the resulting descriptor pointer
is stored at context `0x1A00`. The direct context-access minimum is
`0x1A44`. Loader/controller pointer evidence separates the accessed
context from the model, auxiliary and overlay loads. This minimum is
not whole-allocation capacity or proof of global isolation.

Forty retail instruction anchors and the batch's 50 target-compiled
local C/SDK constants support these views. All 36 resident callee
addresses and the loader/controller owners are checked against actual
resident ELF bytes; `ratan2` retains its established French address
`0x80089928`.

## Exactness and scope

The [attempt ledger](french-model-variant415-attempts.csv) records four
terminal canonical-wrapper matches. Both C helpers are linked together
in each complete unmasked image, not accepted via masked comparisons.
Production ownership comprises 20 C owners / 25,760 C bytes,
40 assembly owners / 96,360 assembly bytes, and 20 raw owners.

This family and [Family439](french-model-variant439.md) form an independent
batch from accepted master `e4f38060e`, preserving all 198 existing
French registrations. Together they add 24 images, 144 inventoried
function instances and 48 C instances / 57,960 C bytes. All 222 configured
images and the clean resident must match; configured totals are
890/1,521 C instances and 850,164 C instruction bytes.

Shared bodies, headers and profiles are unchanged. Initial acceptance
used `15558fb58`; the branch was refreshed after the independent
Family465 work was maintainer-merged, not stacked while pending.
Seven family regressions
cover legal slices, commands, hashes, sources, terminal fingerprints,
complete spans, reachability, binding addresses, raw extents and accessed
views. Untranslated functions, suffixes and other runtime areas remain;
these counts do not establish exhaustive coverage. Report snapshots are
separate.

## Entry-called curtains follow-up

Two three-line wrappers now reuse the unchanged accepted
`variant398_curtains.c` body for both French slots. Each canonical function
matches 1,412 bytes with a 304-byte frame, and independent actual links
reproduce all ten complete unmasked images before metadata promotion.
No shared-source guards, new types, compiler flags or binding changes
are needed. The four original terminal attempt rows remain byte-identical,
followed by two terminal canonical curtain matches.

Independent evidence checks 33 target-compiled layout constants and 119
instruction anchors. Entry initializes **four** 280-byte
`ModelVariantCurtain` records at context `0x13B4..0x1814`; this helper
processes only the **first three**, `0x13B4..0x16FC`. Each record has
seventeen-point `SVECTOR` rows at `0/0x88`, scale at `0x110` and count
at `0x114`. The differing initialization and helper bounds are preserved,
not normalized into one guessed capacity.

The helper reads the first sheet's size at `context + 0x730`, not a curtain
field, and uses the `POLY_GT4` packet at `+0x18F0`. Each shown curtain
submits sixteen adjacent-point quads. Projection depth and the stack flag
at frame `+0xD4` must both be nonnegative; sorting receives the low sixteen
depth bits. Rotation, step and phase remain at `0x1A2C/0x19F8/0x1A30`.
Entry calls the helper at `+0x1134` when phase is at least two, passing
the original context. Webs remains retained code without a direct
entry-call path.

All ten helper callees, all 36 existing resident bindings and three
resident caller owners were checked independently. Commands
`581000/581001/581004` still select the existing 48-byte descriptors at
`+0x30B4`; direct context extent `0x1A44` is a measured minimum, not an
allocation-capacity claim. Each image's `0x2FB8..0x5000` suffix remains
unclassified.

The independent cutoff is accepted
`a1520c70d8fd0b8a78d4bbb5ccc2c331f48aafe4`, not the pending French421
band branch. This adds ten C instances / 14,120 bytes, retaining all 252
registrations and 1,010 accepted C instances. Configured totals become
1,020/1,581 C instances and 1,053,372 bytes. Expected family ownership
is thirty C owners / 39,880 bytes, thirty assembly owners / 82,240 bytes,
and twenty real header/suffix owners / 82,680 bytes.
Production acceptance reproduces all 252 complete French images and the
clean French resident. Actual linked ELF/object evidence verifies all
thirty C owners, preserving the twenty previous sheets/webs C owners /
25,760 bytes. The 203 French, 112 Spanish, sixteen progress and five
US-toolchain regressions pass without skips, alongside G32 and repository
policy. The inherited Family439 fixture uses its own established curtain
bounds; its sources and overlay metadata remain unchanged. All shared
US/Spanish bodies, headers, compiler profiles and binding addresses are
unchanged. The authored scope is 37 paths.

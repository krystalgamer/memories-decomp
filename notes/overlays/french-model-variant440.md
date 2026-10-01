# French MODEL headers 440 and 590

4 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant423_*.c` sheets, webs, spokes, rings and quad
bodies through 10 three-line canonical renaming wrappers.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
The original retained-helper integration preserved shared bodies, headers,
G32/PSXLONG annotations and profiles from accepted master `eed176532`.
The accepted PSXLONG migration
invalidated earlier source fingerprints, so calibration, layouts and actual
canonical links were freshly rebuilt against the migrated sources.
The later sheet/web integration below uses fresh accepted `1dfa77d50`
fingerprints and leaves its shared sources and declarations unchanged.

## Loader and boundaries

Models 262, 631 use these images at stages 9/10.
The matched loader selects ten 2,048-byte sectors from 276-sector compact
records, at record offsets 200/210.
Loads are `0x8013B000`/`0x8017B000`, with entry at `+4`; the controller
supplies context and the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant440-instances.csv) records
independently checked indices, commands, slices and complete hashes.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xF14` | 3856 | generated assembly | yes |
| `0xF14..0x16E0` | 1996 | generated assembly | yes |
| `0x16E0..0x1BD4` | 1268 | sheets C | yes |
| `0x1BD4..0x2130` | 1372 | webs C | yes |
| `0x2130..0x2440` | 784 | spokes C | no |
| `0x2440..0x27BC` | 892 | rings C | no |
| `0x27BC..0x2B20` | 868 | quad C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. Spokes, rings and quad are
**retained code, not direct-entry reachable**. Sheets and webs are called
at `+0xD8C` and `+0xD94`, each receiving the original context.
Each four-byte header and 9,440-byte suffix at
`0x2B20..0x5000` has a real storage owner. Suffixes remain unclassified,
not proven non-code.

## Accessed layouts

Entry captures `a0 -> s3 -> s6`. Two 152-byte sheet views begin at
context `+0x5E8`, six 144-byte rings at `+0x718`, four 144-byte spoke records
at `+0xA78`, and one 144-byte quad at `+0xCB8`. Adjacent extents agree
exactly. Entry clears the first sheet's size at `+0x88`, consumed by
the spokes helper.

69 entry anchors check pointer capture/saves/reloads,
initialized counters, increments, bounds, strides and accessed fields.
Seventy-one freshly target-compiled local C/SDK constants check primitive
widths, ring/spoke/quad and sheet-prefix views, `SVECTOR`, `VECTOR`,
`MATRIX`, `GsCOORDINATE2`, `GsGLINE`, `POLY_G4` and `POLY_GT4`.
Unrelated generic type sizes do not establish runtime record layouts.
These are accessed-view observations, not whole-context allocation proof.

## Exactness and preservation

The original retained-helper integration recorded
6 terminal wrapper matches in the [attempt ledger](french-model-variant440-attempts.csv).
Candidate rebasing was
only a locator; actual links reproduce all 4 complete unmasked images.
Production verification checks selected object owners and final ELF bytes:
12 C owners / 10,176 C bytes,
16 assembly owners / 33,968 assembly bytes,
8 raw owners, 36 independently verified fresh-resident
callees, and all 71 recompiled layout constants.

Five inherited regressions check archive slices/commands/hashes, canonical
sources/fingerprints, all spans/local calls, entry anchors and raw extents.
The combined 431/440/465 batch preserves all 158 accepted French
registrations, reproduces 168 complete images and the clean
French resident, and adds 28 C instances / 23,656 instruction bytes.
Configured totals are 714/1231 C instances
and 641,460 C instruction bytes. The accepted #6668 images are preserved;
no unmerged PR is stacked. Untranslated functions, unclassified tails and unregistered
runtime areas remain; these totals do not establish exhaustive coverage.
General report snapshots remain separate.

## Entry-called sheets and webs

Against accepted master `1dfa77d50`, the unchanged accepted US sheets and
webs bodies reproduce both slots and all four complete unmasked images
through freshly compiled canonical wrappers. The sheet is 1,268 bytes
with a 256-byte frame; the web is 1,372 bytes with a 280-byte frame.
Four new terminal ledger rows retain all six accepted rows unchanged.
This adds eight C instances / 10,560 bytes, not another registration.
No pending French PR is stacked.

Independent original-image evidence checks 134 instruction anchors and
43 target-compiled C/SDK layout constants. Three 416-byte web records
cover `+0..0x4E0`, each with two four-by-six `SVECTOR` grids at `+0/+0xC0`,
color at `+0x180`, scale at `+0x194` and completion at `+0x198`.
The following 264-byte region remains untyped here. Two 152-byte sheet
views cover `+0x5E8..0x718`, with four vector rows at `+0/+0x20/+0x40/+0x60`,
outer/inner colors at `+0x80/+0x84` and size at `+0x88`.
The sheet packet is `POLY_GT4` at `+0xDBC`; the web packet is `GsGLINE`
at `+0xE84`. Both retain nonnegative depth/flag checks; sheet depth is
scaled by eight tenths before the eight-unit ordering-table bias.

Commands `606000` select the 48-byte descriptor at `+0x2C1C`, with
frame bounds `0..36` read through `+0xF00`. Original entry accesses prove
a minimum context extent `0xF38`, disjoint from the active package,
scratch and overlay regions, not its allocation capacity.
Frame `+0xEF0`, step `+0xEF8` and phase `+0xF24` drive both helpers;
sheet path/size inputs also use `+0xF1C/+0xF18`.
All 36 resident binding addresses and three resident caller owners are
checked independently, including eight web and nine sheet callees.
Only the verified `ratan2` and `RotTransPers3` binding names change.

Shared bodies, headers, compiler profiles and all US/Spanish source and
metadata are unchanged. Spanish440 explicitly retains its accepted three
helpers and existing descriptor/context regression instead of inheriting
the French promotions. Configured totals become 952/1,581 C instances
and 940,364 C bytes across 252 French images.

All 252 complete unmasked images and the clean French resident pass.
Actual production ELF/object verification proves 20 C owners / 20,736 bytes,
eight assembly owners / 23,408 bytes and eight raw owners / 37,776 bytes
in these four images, preserving all twelve prior C owners. The 43 layout
constants are recompiled and all 36 resident bindings and three callers
are checked against the fresh resident. The 199 French, 103 Spanish,
16 progress and five US toolchain regressions pass without skips, alongside
metadata/basic-types/external-attempts/G32 gates. The independent change
has 26 authored paths; no shared source, header or profile changes.

The subsequently accepted Family435/442 ribbons are preserved through an
ordinary merge of accepted `e687a79d5`, which includes their maintainer merge
at `1b2a2e2b8`. Its 974 accepted C instances plus the original eight
Family440 helpers give 982/1,581 instances and 988,724 bytes across the
same 252 images. No pending branch is stacked. Shared sheet/web bodies,
headers, profiles, all four wrappers and their independent evidence remain
unchanged. All 252 complete French images and the clean resident pass again.
Production objects preserve all thirty accepted ribbon C owners and all
twenty Family440 C owners, with the assembly/raw ownership unchanged.
All 43 layout constants are recompiled; 36 resident bindings and three
caller owners are verified again. The 200 French, 112 Spanish, 16 progress
and five US toolchain regressions and policy gates pass without skips.
All 26 authored paths remain; 24 files are byte-identical to the original
Family440 head, with only this note and the progress fixture reconciled.

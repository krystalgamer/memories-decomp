# French MODEL headers 460 and 610

These 28 secondary-handler images reuse the unchanged accepted
`src/overlays/model_variant/variant443_{ribbons,sheets,strand}.c` bodies. Six
three-line wrappers rename the functions for the two measured French load
addresses. The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1 and MASPSX
2.81. Shared bodies, local declarations, G32 annotations and US compiler
profiles remain unchanged. US header 443 establishes source provenance,
not French byte identity.

## Loader and instance evidence

The matched MODEL loader selects compact 276-sector records. Stages 7/8
occupy record sectors 180/190; stages 9/10 occupy 200/210. Each image is ten
2,048-byte sectors, loading at `0x8013B000` or `0x8017B000`.

| Stages | Models |
|---|---|
| 7/8 | 70, 125, 168, 460, 469, 704 |
| 9/10 | 44, 98, 161, 370, 400, 458, 462, 558 |

The [instance ledger](french-model-variant460-instances.csv) records actual
compact indices, nonnegative command words, archive slices and complete
hashes. All 28 images have distinct hashes. Model 704 is compact record 604:
the matched loader removes both absent 50-ID ranges, 300..349 and 650..699.
The shared regression fixture previously exercised only the first gap; its
record calculation now follows the loader's full compaction, including the
single absent ID 720. This fixture correction changes no production source
or archive slice.

The resident controller dispatches the secondary image at `+4`, passing its
context and command argument or update argument `-1`. Other stages and other
models are not covered by this registration.

## Boundaries and reachability

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xCFC` | 3320 | generated assembly | yes |
| `0xCFC..0x17EC` | 2800 | ribbons C | yes |
| `0x17EC..0x1D74` | 1416 | sheet-set C | yes |
| `0x1D74..0x20B4` | 832 | strand C | no |
| `0x20B4..0x28CC` | 2072 | generated assembly | no |

Every span has complete direct control flow, one terminal return and no
unresolved indirect transfer. Entry reaches offsets `0x4`, `0xCFC` and
`0x17EC`. Strand and the final assembly function are retained module-local
code; no entry execution path is demonstrated. Every image has real storage
owners for its four-byte header and 10,036-byte suffix at `0x28CC..0x5000`.
The suffix remains unclassified; a data segment does not prove it contains
no code.

## Accessed record evidence

Entry captures `a0 -> s6 -> s7`. Eight preceding objects have stride 744
bytes and end at `context + 0x1740`, the saved sheet pointer. Sixteen sheet
records have stride 156 and end at `+0x2100`. Strand generates six 124-byte
records with thirteen points each from `+0x2100` through `+0x23E8`; that
body's execution path is not established.

Entry reconstructs `load + 0x29C8`, adds `argument * 64` and stores the
configuration pointer at `context + 0x2E7C`. The actual command is read at
`(record * 276 + 275) * 2048 + 0x110 + ((stage - 7) // 2) * 4`;
its nonnegative remainder modulo 1000 selects the configuration. All 28
selected 64-byte views fit inside their real image suffixes.

The observed mode at record `+0x24` is always one. With `n` at `+0x20` and
kind at `+0x28`, the sheet body selects `(count, first)` as `(n+1, 1)` for
kind zero, `(n+2, 2)` for kind two, and `(2*n, n)` for kind one. Actual views
satisfy `0 < first <= 8`, `first <= count <= 16`, and `count-first <= 8`,
covering the initialized sheets, local slots and preceding objects. These
are command-specific accessed bounds, not total context allocation proof.

Seventeen entry/configuration instruction anchors were checked in every
image. Forty-nine target-compiled layout constants cover the 156-byte
`ModelVariantSheetSet` (including `shown` at 152), 124-byte
`ModelVariantStrand`, `SVECTOR`, `VECTOR`, `MATRIX`, `GsCOORDINATE2`,
`POLY_GT4`, 16-byte `GsLINE`, and four-byte target `long`/`s32`.

## Exact matching and preservation

The [attempt ledger](french-model-variant460-attempts.csv) records four
terminal canonical-wrapper matches. Both accepted bodies were recompiled
with the current local header, including the accepted sheet-set declaration.
Calibration rebasing located candidates only; actual links reproduced all
28 unmasked complete images. An initial limited binding pool lacked `rcos`;
the existing French model-408 binding resolved that setup failure without
changing a source or compiler candidate. `GsSortLine` comes from the accepted
French duel-effect bindings.

Production validation preserves all 116 previous French registrations and
reproduces all 144 complete images plus the clean French resident. Actual
selected objects provide 56 new sized C owners and 62,944 instruction bytes,
84 assembly owners, 56 raw header/suffix owners and 34 independently checked
fresh-resident callee owners. Target layouts are recompiled. The family's
229,376 assembly bytes and 281,008 unclassified suffix bytes remain untranslated.

Six regressions check complete hashes, actual commands and configuration
bounds, canonical wrappers and fingerprints, source selection, real storage
extents, entry anchors and all five control-flow boundaries. Existing family
fixtures remain covered, including both compact-record gaps.

Configured totals become 144 images, 636/1029 matching C instances and
567,756 C instruction bytes. These are not exhaustive runtime coverage or
seven-release completion. General progress snapshots remain separate.

## Entry-called ribbons follow-up

The unchanged accepted US443 ribbon body reproduces `+0xCFC..+0x17EC`,
2,800 instruction bytes and a 856-byte frame in both French slots. Two
three-line wrappers add 28 C instances / 78,400 bytes. Complete canonical
scratch links have 84 genuine C owners / 141,344 bytes, preserving all
56 accepted sheet/strand owners / 62,944 bytes. Entry and streamers remain
assembly: 56 owners / 150,976 bytes. The 56 raw header/suffix owners retain
281,120 bytes; the suffix remains unclassified.

The first unchanged-source calibration matched both slots and all 28
complete images. Two calibration rows and two canonical terminals follow
the four unchanged prior ledger rows. Shared source/header, US profiles
and all resident binding addresses are unchanged; only two established
SDK aliases become `ratan2` and `RotTransPers`. No `VERSION_FRENCH`
alternative is needed.

Fifty-one freshly target-compiled constants and 104 literal retail anchors
per image independently prove the local view and use sites. Eight
`0x2E8` records occupy context `0..0x1740`, ending at the accepted sheet
array. Each has seventeen-point grids at `0/0x110`, projected words at
`0x88/0x198`, angles at `0xCC`, widths at `0x1DC`, RGB at `0x220`,
signed count at `0x240`, state at `0x248`, length at `0x24C`, depth
at `0x260`, and signed halfword screen offsets at `0x2A4/0x2C6`.
Opaque intervals remain uninterpreted by this helper. Entry initializes
count to `-index * 16`, state to zero, length to `0x400`, and RGB
from descriptor bytes `4..6`.

All 28 actual 64-byte descriptors have counts 4, 5, 6 or 8 at `+0x20`.
These positive counts independently bound both the eight initialized
records and the helper's eight-by-seventeen flag array. Retail stack
anchors place that 544-byte array at `0xD0..0x2F0`, followed by
projection outputs at `0x2F0/0x2F4`. Entry calls at `+0xB54` after
unsigned clock `+0x2E6C` reaches descriptor `+0x30`; original context
is reloaded from its saved stack slot at `+0xB50`.

Two `POLY_FT4` packets occupy `+0x2BB4..+0x2C04`. Frame parity
selects whether packet alternation occurs before or after each draw;
the separate conditions remain unchanged. Signed nonnegative depth and
per-point flag gate sorting, with depth narrowed to sixteen bits.
The distinct terminal-point projection and unused bend computation are
retained, as is the unused-result second `ratan2` call.

Count grows by twice the frame step to sixteen, then state becomes one.
Length decays by `step << 6`; a finished record either retires with
state two or resets length to `0x400`, count to `-descriptor_count * 8`,
and state to zero. A completed count can move phase one to two; the
state sum can move phase four to five. The two wave clocks advance by
`step * 650` and `step << 7`.

All 28 legal archive slices and hashes, 34 resident binding owners,
eleven helper callees and three resident caller owners are independently
verified. The directly accessed entry minimum is `0x2ED8`, not allocation
capacity, and remains separated from the selected load spans.

Calibration began independently at accepted
`ec4d1162808586899a90dbc12323c41d5ee330f9`. Before integration, a
fast-forward to accepted `ec4bd2d99cb3a99251c3765355537b26345ad83b`
verified all 116 relevant source/header/metadata paths unchanged,
retaining the maintainer-merged French halos and Spanish additions.
No pending branch is included. Fixed-cutoff configured totals become
252 images, 1,234/1,581 C instances and 1,472,580 C instruction bytes.
General report snapshots remain separate; this is not French completion.

Production acceptance reproduces all 252 complete French overlay images
and the clean French resident. Defining objects and final linked ELFs
confirm the C owners, including all 48 accepted French338 owners /
78,656 bytes and 32 French422 owners / 39,984 bytes. All 243 French,
151 Spanish and 22 focused regressions pass without skips, together
with metadata, types, attempt-ledger and notes gates. Shared US bodies,
headers and profiles remain unchanged; this is not a fresh US rebuild.

# French MODEL headers 460 and 610

These 28 secondary-handler images reuse the unchanged accepted
`src/overlays/model_variant/variant443_{sheets,strand}.c` bodies. Four
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
| `0xCFC..0x17EC` | 2800 | generated assembly | yes |
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

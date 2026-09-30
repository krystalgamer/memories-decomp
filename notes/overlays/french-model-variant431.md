# French MODEL headers 431 and 581

2 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant414_*.c` spokes, rings
bodies through 4 three-line canonical renaming wrappers.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
Shared bodies, headers, G32/PSXLONG annotations and profiles are unchanged
from accepted master `eed176532`. The accepted PSXLONG migration
invalidated earlier source fingerprints, so calibration, layouts and actual
canonical links were freshly rebuilt against the migrated sources.

## Loader and boundaries

Models 401 use these images at stages 7/8.
The matched loader selects ten 2,048-byte sectors from 276-sector compact
records, at record offsets 180/190.
Loads are `0x8013B000`/`0x8017B000`, with entry at `+4`; the controller
supplies context and the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant431-instances.csv) records
independently checked indices, commands, slices and complete hashes.

Model 401 here means stages 7/8 and headers 431/581, not the separately
registered model-401 stages 9/10/header-432 renderer.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xF1C` | 3864 | generated assembly | yes |
| `0xF1C..0x1338` | 1052 | generated assembly | yes |
| `0x1338..0x17C8` | 1168 | generated assembly | yes |
| `0x17C8..0x2444` | 3196 | generated assembly | yes |
| `0x2444..0x28C4` | 1152 | generated assembly | yes |
| `0x28C4..0x2BCC` | 776 | spokes C | no |
| `0x2BCC..0x2F48` | 892 | rings C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. All 2 C helpers are
**retained code, not direct-entry reachable**; no additional execution path
is claimed. Each four-byte header and 8,376-byte suffix at
`0x2F48..0x5000` has a real storage owner. Suffixes remain unclassified,
not proven non-code.

## Accessed layouts

Entry captures `a0 -> s6`. Six 152-byte sheet views begin at context
`+0xA80`, six 144-byte rings at `+0xE10`, and four 144-byte spoke records
at `+0x1170`. Adjacent extents agree exactly. Entry clears the first sheet's
size at `+0x88`, consumed by the spokes helper. The separate five 116-byte
records at `+0x13B0` are **not** declared as `ModelVariantQuad`; this family
has no promoted quad helper.

57 entry anchors check pointer capture/saves/reloads,
initialized counters, increments, bounds, strides and accessed fields.
Seventy-one freshly target-compiled local C/SDK constants check primitive
widths, ring/spoke/quad and sheet-prefix views, `SVECTOR`, `VECTOR`,
`MATRIX`, `GsCOORDINATE2`, `GsGLINE`, `POLY_G4` and `POLY_GT4`.
Unrelated generic type sizes do not establish runtime record layouts.
These are accessed-view observations, not whole-context allocation proof.

## Exactness and preservation

The [attempt ledger](french-model-variant431-attempts.csv) records
4 terminal wrapper matches. Candidate rebasing was
only a locator; actual links reproduce all 2 complete unmasked images.
Production verification checks selected object owners and final ELF bytes:
4 C owners / 3,336 C bytes,
10 assembly owners / 20,864 assembly bytes,
4 raw owners, 36 independently verified fresh-resident
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

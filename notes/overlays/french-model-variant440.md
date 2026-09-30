# French MODEL headers 440 and 590

4 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant423_*.c` spokes, rings, quad
bodies through 6 three-line canonical renaming wrappers.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
Shared bodies, headers, G32/PSXLONG annotations and profiles are unchanged
from accepted master `eed176532`. The accepted PSXLONG migration
invalidated earlier source fingerprints, so calibration, layouts and actual
canonical links were freshly rebuilt against the migrated sources.

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
| `0x16E0..0x1BD4` | 1268 | generated assembly | yes |
| `0x1BD4..0x2130` | 1372 | generated assembly | yes |
| `0x2130..0x2440` | 784 | spokes C | no |
| `0x2440..0x27BC` | 892 | rings C | no |
| `0x27BC..0x2B20` | 868 | quad C | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. All 3 C helpers are
**retained code, not direct-entry reachable**; no additional execution path
is claimed. Each four-byte header and 9,440-byte suffix at
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

The [attempt ledger](french-model-variant440-attempts.csv) records
6 terminal wrapper matches. Candidate rebasing was
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

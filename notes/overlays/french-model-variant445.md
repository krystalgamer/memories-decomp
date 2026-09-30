# French MODEL headers 445 and 595

These twelve secondary-handler images reuse the unchanged accepted
`src/overlays/model_variant/variant428_{sheets,spokes,rings,quad}.c` bodies.
Eight three-line wrappers rename their function symbols for the two measured
French load addresses. All use the existing `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. Shared bodies, headers, stored-pointer annotations
and original US profiles remain unchanged. US header 428 is source
provenance, not proof of French image identity.

## Loader and instance evidence

The matched MODEL loader selects 276-sector compact records. Stages 7/8
occupy record sectors 180/190; stages 9/10 occupy 200/210. Each image is ten
2,048-byte sectors and loads at `0x8013B000` or `0x8017B000`.

| Stages | Models |
|---|---|
| 7/8 | 187, 596 |
| 9/10 | 239, 361, 368, 478 |

The [instance ledger](french-model-variant445-instances.csv) records compact
indices, actual nonnegative commands, archive slices and complete hashes.
All twelve instances have distinct hashes. Resident controllers dispatch
the chosen secondary image at `+4`, passing the context and initial command
or update argument `-1`. Other stages of these models are not covered here.

## Boundaries, ownership and reachability

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1034` | 4144 | generated assembly | yes |
| `0x1034..0x17A8` | 1908 | generated assembly | yes |
| `0x17A8..0x1C8C` | 1252 | sheets C | yes |
| `0x1C8C..0x21F0` | 1380 | generated assembly | yes |
| `0x21F0..0x24F8` | 776 | spokes C | no |
| `0x24F8..0x2874` | 892 | rings C | no |
| `0x2874..0x2BD8` | 868 | quad C | no |
| `0x2BD8..0x3594` | 2492 | generated assembly | yes |

Every span has complete direct control flow, one terminal return and no
unresolved indirect transfer. The entry calls offsets `0x1034`, `0x17A8`,
`0x1C8C` and `0x2BD8`. The initial boundary hypothesis stopped after the
quad helper at `0x2BD8`; strict call ownership rejected that hypothesis.
Walking the additional entry-called function established its full
2,492-byte extent through `0x3594`. It remains assembly, not raw tail data.
Spokes, rings and quad are retained module-local code, not proven entry
execution paths. Execution frequency and exhaustive coverage remain unknown.

Entry captures `a0 -> s3 -> s6` and initializes the records consumed by
the helpers: sheets at `context + 0xC78`, rings at `+0xDA8`, spokes at
`+0x1108`, and quad at `+0x1348`. Their saved pointers advance by 152, 144,
144 and 144 bytes. Sheet/ring/spoke initialization bounds are two/six/four
records; the rotation and shared phase words are cleared at `+0x15B8` and
`+0x15BC`. Fifteen instruction anchors were checked in all twelve images.

Seventy-one target-compiled layout constants verify accepted local sheet,
ring, spoke and quad records plus SDK structures, including `POLY_GT4`,
`POLY_G4` and stored-pointer-bearing `GsCOORDINATE2`. They establish accessed
views, not context allocation bounds. Each image has real storage owners for
its four-byte header and 6,764-byte suffix at `0x3594..0x5000`. That suffix
remains explicitly unclassified; a data segment does not prove absence of code.

## Exact matching and preservation

The [attempt ledger](french-model-variant445-attempts.csv) records eight
terminal canonical-wrapper matches. All four bodies compiled unchanged with
the authoritative French profile. Calibration searched 2,362 distinct French
secondary payloads. Internal J26 rebasing located candidates only; subsequent
actual links reproduced all twelve unmasked complete images with sized,
section-defined C function owners.

Production validation preserves all 104 accepted French registrations and
reproduces all 116 complete images plus the clean French resident. The new
family contributes 48 C owners and 45,456 instruction bytes, with 48 assembly
owners, 24 raw header/suffix owners, 36 independently verified fresh-resident
callee owners and 71 recompiled target layouts. Its 119,088 assembly bytes
and 81,168 unclassified suffix bytes remain untranslated.

Five inherited regression cases check slices, actual command words, complete
hashes, source selection, wrapper fingerprints, real storage extents, entry
anchors and all eight function boundaries. They check the additional
entry-called assembly function and distinguish sheets from retained helpers
without changing existing family fixtures.

Configured totals become 116 images, 554/889 matching C instances and
457,908 C instruction bytes. These figures are not exhaustive runtime
coverage or seven-release completion. General progress snapshots remain
separate.

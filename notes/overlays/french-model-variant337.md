# French MODEL variant 337/487 ring helper

Six independently verified French secondary images directly reuse the accepted
`src/overlays/spanish_model_variant/variant337_rings.c` body and existing
`variant337_rings_slot1.c` wrapper. No French C or header is added. Each uses
the authoritative `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
The ring helper at offset `0x1278` is 1,216 bytes in both slots.

| Model | Compact record | Stages | Sectors | Request / initial argument |
|---:|---:|---|---|---|
| 110 | 110 | 9/10 | 30560/30570 | 503001 / 1 |
| 159 | 159 | 9/10 | 44084/44094 | 503004 / 4 |
| 410 | 360 | 7/8 | 99540/99550 | 503007 / 7 |

The [instance ledger](french-model-variant337-instances.csv) records all six
distinct complete hashes. Each slice contains ten 2,048-byte sectors and
loads at `0x8013B000` or `0x8017B000`. Header identity, shared source and
matching instruction prefixes are not substitutes for independent image
verification. Model 410's stages 9/10 are a different family and are excluded.

## Boundaries and ownership

| Offset range | Bytes | Owner |
|---|---:|---|
| `0x4..0x9A4` | 2464 | generated assembly |
| `0x9A4..0x1278` | 2260 | generated assembly |
| `0x1278..0x1738` | 1216 | matching ring C |
| `0x1738..0x1F5C` | 2084 | generated assembly |

All four functions are reachable in the entry's direct call graph. Each
complete span has one terminal return and no unresolved indirect transfer.
The family retains eighteen assembly instances / 40,848 instruction bytes.
Each four-byte header and 12,452-byte suffix at `0x1F5C..0x5000` has a real
generated storage owner. All 74,712 suffix bytes remain unclassified, not
excluded code or C coverage.

The entry captures its context through `a0 -> s2 -> s6`, forms the ring base
at `context + 0x3A4`, advances it by 152 bytes and bounds the loop at two.
The helper's two ring records each contain four rows of four `SVECTOR`s,
colors at offsets 128/132 and signed scale at 136. It reuses the canonical
`POLY_GT4` at context `0xC38`.

The entry passes context `0xCCC` as the output view to the independently
checked resident SDK function at `0x8008A428`, corresponding to the accepted
matrix initialization call. The `MATRIX` is 32 bytes, with translation at
`0xCE0`; the next `SVECTOR` begins at `0xCEC`. The shared header's partial
state size `0xD70` is not an allocation bound: the entry accesses later
fields. No guessed allocation extent or overlapping translation vector is
introduced.

At offsets `0x70/0x74`, the entry reconstructs image base plus `0x2058`.
It multiplies the initial argument by 36 and stores the resulting pointer
at context `0xD34`. Actual French metadata requests modulo 1000 select
configuration views 1, 4 and 7. Each accessed 36-byte view lies within the
real suffix owner. These windows do not establish global array capacity
or exclusive runtime use. Fourteen entry/configuration instruction anchors
were independently checked in every image.

## Independent matching evidence

Calibration compiled the accepted Spanish body and scanned all 2,362 distinct
French secondary payloads. Actual subsequent complete-image links verified
all six images and the canonical symbol in each slot. Both source paths are
reused unchanged; the [attempt ledger](french-model-variant337-attempts.csv)
records their terminal fingerprints and profile.

Accepted master added `G32` to the stored configuration pointer between the
initial proof and integration. The updated accepted declarations were
recompiled: 57 target size/offset constants and all six canonical full-image
links still matched. No old header was restored and no annotation was
removed. The project `check-g32` gate is included in integration validation.

Fresh production validation reproduces all 104 configured French images and
the clean French resident. Selected-object and final-ELF checks cover six C
owners, eighteen assembly owners, twelve raw owners, all 33 fresh-resident
callee owners and the 57 target layout constants. The 98 previously accepted
module registrations are preserved. Source bodies, shared headers, profiles,
other-region metadata and the general report remain unchanged.

The shared Spanish fixture is parameterized without weakening its existing
checks; French tests add current-source fingerprints, bindings, entry
initialization, configuration arithmetic, actual requests and local-call
targets. Legal-image hashes, boundaries and source selection are checked
independently for each region.

Configured French totals become 104 images, 506/793 matching C instances and
412,452 C instruction bytes. These inventory figures are not exhaustive
runtime coverage or seven-release completion. Unclassified tails and
uninventoried game code remain separate work.

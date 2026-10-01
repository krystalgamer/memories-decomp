# French MODEL variant 337/487 ring and ribbon helpers

Six independently verified French secondary images directly reuse the accepted
`src/overlays/spanish_model_variant/variant337_rings.c` body and existing
`variant337_rings_slot1.c` wrapper. Two French ribbon wrappers also reuse
the accepted North American `variant320_ribbon.c` and its existing header.
Each uses
the authoritative `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
The ring helper at offset `0x1278` is 1,216 bytes and the ribbon at `0x9A4`
is 2,260 bytes in both slots. No new header or storage declaration is added.

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
| `0x9A4..0x1278` | 2260 | matching ribbon C |
| `0x1278..0x1738` | 1216 | matching ring C |
| `0x1738..0x1F5C` | 2084 | generated assembly |

All four functions are reachable in the entry's direct call graph. Each
complete span has one terminal return and no unresolved indirect transfer.
The family has twelve C instances / 20,856 instruction bytes, retaining
all six accepted rings. Twelve assembly instances / 27,288 instruction bytes
remain.
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

## Original ring integration evidence

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

## Ribbon evidence and regional difference

The accepted header-320 body initially compiled to 2,236 bytes with a
296-byte frame instead of the French 2,260 bytes / 304-byte frame. Both
slots differed in 338 aligned common words. Within the terminal `k == 16`
branch, keeping the current endpoint indexed by `k`, rather than literal
`16`, recovers the retail `record + 0x40` and `record + 0x20` cursors and
their spills. That sole source change makes every instruction exact.
`VERSION_FRENCH` selects this measured indexing difference; the default
North American branch is unchanged. The previous-point index remains `15`.
Both distinct experiments and both canonical terminals are retained in the
attempt ledger, after the unchanged historical ring records.

Independent target compilation verifies 46 layout constants against 107
literal retail instruction anchors in every image. The existing
`Variant320Ribbon` view covers one `0x3A4`-byte record at context zero:

| Field | Offset |
|---|---:|
| Seventeen source `SVECTOR`s / projected words | `0x000` / `0x088` |
| Screen angles | `0x0CC` |
| Seventeen displaced `SVECTOR`s / projected words | `0x110` / `0x198` |
| Projected widths | `0x1DC` |
| Shared RGB and uninterpreted fourth byte | `0x220` |
| Opaque bytes | `0x224..0x2D8` |
| Depth / projection flags | `0x2D8` / `0x31C` |
| Signed screen offsets X / Y | `0x360` / `0x382` |

The entry initializes RGB to 192. The next accepted ring view begins at
`0x3A4` and ends at `0x4D4`; no guessed backing allocation is introduced.
The renderer alternates two existing 40-byte `POLY_FT4` packets at
`0xC6C..0xCBC`. The direct context-access minimum is `0xD80`, not an
allocation capacity. All six accessed contexts are disjoint from their
selected model, primary and secondary image loads.

The entry calls the ribbon at `0x818`, passing the original context in its
delay slot, when unsigned clock `0xD24` reaches descriptor field `0x10`.
The selected 36-byte descriptors remain at image `0x2058 + command%1000*36`.
Their growth fields `0x14/0x18` and fade fields `0x1C/0x20` have positive
intervals in every actual request. In phase 1 the drawn length grows to
sixteen and advances to phase 2; in phase 3 the signed displacement shrinks
to zero. The helper always advances its two wave clocks, including when
the main draw guard is false. Signed depth and flags gate sorting, which
uses the low sixteen depth bits.

All eleven helper callees, all 33 resident bindings and three loader/caller
owners were checked against the complete exact French resident, with
accepted caller-source fingerprints and the runtime context-pointer table.
Four established SDK aliases replace address-based names without changing
addresses. Six complete combined links contain twelve genuine C owners:
the six new ribbons contribute 13,560 bytes and preserve 7,296 ring bytes.
No suffix byte is reclassified by this change.

Final production validation reproduces all 252 configured French images and
the clean French resident. Selected input objects and final ELF definitions
confirm all twelve C owners. Fresh normal-pipeline builds also reproduce all
six North American consumers of the shared ribbon body; their genuine C
owners are retained and the default source branch is unchanged. All 239
French, 141 Spanish and 35 focused regressions pass alongside repository
policy checks. Configured French totals become 1,182 / 1,581 matching
instances and 1,336,116 C instruction bytes, not exhaustive runtime coverage.

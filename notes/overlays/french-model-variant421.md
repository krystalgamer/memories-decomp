# French MODEL headers 421 and 571

These twelve secondary-handler instances reuse the unchanged accepted
`src/overlays/model_variant/variant404_{sheets,spokes,rings,quad}.c` bodies.
Eight three-line wrappers rename the four function symbols for the two
measured French load addresses. All entries use the existing
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81. The shared source
bodies, header and original US profiles are unchanged. US header 404 is
structural provenance, not a French header identity.

## Loader and instance evidence

The matched MODEL loader and phase callback select 276-sector compact
records. Stages 7/8 occupy record sectors 180/190; stages 9/10 occupy
200/210. Each image occupies ten 2,048-byte sectors and loads at
`0x8013B000` or `0x8017B000`.

| Stages | Models |
|---|---|
| 7/8 | 84, 162 |
| 9/10 | 88, 114, 184, 369 |

The [instance ledger](french-model-variant421-instances.csv) records the
compact indices, actual nonnegative commands, slice locations and complete
hashes. All twelve instances have distinct complete hashes. The matched
resident controllers dispatch the chosen secondary image at `+4`, passing
its context and initial command or the update argument `-1`.

## Boundaries, ownership and reachability

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x11D0` | 4556 | generated assembly | yes |
| `0x11D0..0x1860` | 1680 | generated assembly | yes |
| `0x1860..0x247C` | 3100 | generated assembly | yes |
| `0x247C..0x2C28` | 1964 | generated assembly | yes |
| `0x2C28..0x310C` | 1252 | sheets C | yes |
| `0x310C..0x367C` | 1392 | generated assembly | yes |
| `0x367C..0x398C` | 784 | spokes C | no |
| `0x398C..0x3D08` | 892 | rings C | no |
| `0x3D08..0x406C` | 868 | quad C | no |

Every span has complete direct control flow, one terminal return and no
unresolved indirect transfer. The entry's direct call graph reaches the
first six functions, including sheets. Spokes, rings and quad are retained
module-local code, not proven entry execution paths. This distinction does
not establish execution frequency or exhaustive runtime coverage.

The active entry captures `a0 -> s2 -> s6` and initializes the same records
consumed by the helpers: sheets at `context + 0xFD8`, rings at `+0x1108`,
spokes at `+0x1468`, and quad at `+0x16A8`. It advances the corresponding
saved pointers by 152, 144, 144 and 144 bytes, bounds the sheet/ring/spoke
loops at two/six/four records, and clears the rotation and shared phase
words at `+0x1958` and `+0x1960`. Fifteen instruction anchors were checked
independently in every image.

Seventy-one target-compiled layout constants verify the accepted local
sheet, ring, spoke and quad records and SDK structures, including
`POLY_GT4` and `POLY_G4`. These prove accessed layouts, not context allocation
bounds. Each image retains real storage owners for its four-byte header
and its 3,988-byte suffix at `0x406C..0x5000`. The suffix is explicitly
unclassified; a data segment does not establish absence of code.

## Exact matching and preservation

The [attempt ledger](french-model-variant421-attempts.csv) records eight
terminal canonical-wrapper matches. The four accepted bodies compiled
unchanged with the authoritative French profile. Calibration scanned 2,362
distinct French secondary payloads; internal J26 rebasing was only a
discovery aid. Subsequent actual links reproduced all twelve unmasked
complete images with section-defined C function owners.

Production validation preserves all 86 accepted French images and
reproduces all 98 configured complete images plus the clean French resident.
The new family contributes 48 C owners and 45,552 instruction bytes, with
60 assembly owners, 24 real header/suffix owners, 36 independently checked
fresh-resident callee owners and 71 recompiled target layouts. Its 152,304
assembly bytes and 47,856 suffix bytes remain untranslated or unclassified.

Five inherited regression cases check archive slices, actual command words,
complete hashes, C source selection, wrapper fingerprints, real storage
extents, entry anchors and all nine function boundaries. They preserve the
entry-reachable sheets versus retained-helper distinction without changing
the existing family fixtures.

Configured totals become 98 images, 474/769 matching C instances and 380,820
C instruction bytes. The larger denominator includes newly inventoried
unmatched code. These are configured inventory figures, not exhaustive
runtime coverage or seven-release completion. General progress snapshots
remain separate.

# French MODEL headers 421 and 571

These twelve secondary-handler instances reuse the unchanged accepted
`src/overlays/model_variant/variant404_{sheets,webs,spokes,rings,quad}.c` bodies.
Ten three-line wrappers rename the five function symbols for the two
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
| `0x310C..0x367C` | 1392 | webs C | yes |
| `0x367C..0x398C` | 784 | spokes C | no |
| `0x398C..0x3D08` | 892 | rings C | no |
| `0x3D08..0x406C` | 868 | quad C | no |

Every span has complete direct control flow, one terminal return and no
unresolved indirect transfer. The entry's direct call graph reaches the
first six functions, including sheets and webs. Spokes, rings and quad are retained
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

The entry also initializes three 416-byte `ModelVariantWebNarrow` records at
`context + 0..0x4E0`, with two 4x6 `SVECTOR` grids at `0/0xC0`, color at
`0x180`, and scale at `0x194`. The webs helper's timing read is the first
sheet size at `context + 0xFD8 + 0x88 = 0x1060`, not a fourth web field.
Thirty-four web-related entry/helper anchors and 27 freshly target-compiled
layout constants independently establish this accessed view and the
canonical SDK layouts, including the 32-bit `PSXLONG`.

All twelve legal archive selections retain their actual positive commands.
Selected descriptors are 60-byte records at
`module + 0x4168 + (command % 1000) * 60`, within the retained suffix.
Both contexts' measured direct extent of at least `0x1978` bytes is
disjoint from the selected primary, secondary and variant load spans.
This minimum extent does not prove allocation capacity or global isolation.

## Exact matching and preservation

The [attempt ledger](french-model-variant421-attempts.csv) records eight
terminal canonical-wrapper matches from the initial integration, followed
by two terminal webs matches. The initial four accepted bodies compiled
unchanged with the authoritative French profile. Calibration scanned 2,362
distinct French secondary payloads; internal J26 rebasing was only a
discovery aid. Subsequent actual links reproduced all twelve unmasked
complete images with section-defined C function owners. The webs follow-up
recompiled its two final tracked wrappers and independently relinked all
twelve complete images before updating their existing registrations.

Production validation reproduces all 198 configured complete French images
plus the clean French resident. This family now contributes 60 C owners and
62,256 instruction bytes, with 48 assembly owners and 24 real header/suffix
owners. All 36 resident binding addresses remain unchanged and are checked
against fresh-resident section owners, including the webs helper's eight
callees; three resident caller owners are also rechecked.
The existing `0x80089928` binding is renamed to the independently established
French SDK name `ratan2`, without adding an extra binding.
Across this family, 135,600 assembly bytes and 47,856 suffix bytes remain
untranslated or unclassified. All module manifest records and unrelated
accepted C bodies, headers, compiler profiles and regions are preserved.

Five inherited regression cases check archive slices, actual command words,
complete hashes, C source selection, wrapper fingerprints, real storage
extents, entry anchors and all nine function boundaries. They preserve the
entry-reachable sheets/webs versus retained-helper distinction without
changing the existing family fixtures. Two additional cases guard the SDK
binding, selected 60-byte descriptors and the measured context/load separation.

The webs follow-up adds 12 C instances and 16,704 instruction bytes without
adding images or function boundaries. Configured totals become 198 images,
838/1,377 matching C instances and 786,700 C instruction bytes. These are
configured inventory figures, not exhaustive runtime coverage or
seven-release completion. General progress snapshots remain separate.

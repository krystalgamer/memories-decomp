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
| `0x11D0..0x1860` | 1680 | ribbons C | yes |
| `0x1860..0x247C` | 3100 | generated assembly | yes |
| `0x247C..0x2C28` | 1964 | bands C | yes |
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

## Entry-called bands follow-up

The accepted US404 band body initially has the correct 1,964-byte extent
and 296-byte frame but differs at four next-column screen-coordinate loads,
at function-relative `+0x554/+0x560/+0x584/+0x590`. Using indexed
`band->sc[j + 1]` and `band->sb[j + 1]` expressions matches both French
slots. The tracked shared body selects only those four expressions with
`VERSION_FRENCH`; the original US expressions remain unchanged otherwise.
Two four-line canonical wrappers reproduce all twelve complete unmasked
images before metadata promotion. The ten original terminal ledger rows
remain byte-identical, followed by two original failures, two indexed
scratch experiments and two terminal canonical matches.

Independent evidence checks 35 target-compiled constants and 132 instruction
anchors across every image. The entry initializes one 456-byte
`ModelVariantBand` at context `0xE10..0xFD8`. Its three nine-point `SVECTOR`
rows begin at `0/0x48/0x90`; packed outputs at `0xD8/0xFC/0x120`,
colors at `0x144/0x168` and depths at `0x1A4`. The nine `PSXLONG` flags
are stack storage at frame `+0xC8`, not a field or inferred extension of
the band. The `POLY_GT4` packet is at context `+0x1778`; eight adjacent
point pairs each produce two quad submissions. Negative depths are
clamped to zero, with the low sixteen bits passed to sorting.

The actual commands `587000..587005` select the existing 60-byte records
at `module + 0x4168`. Timing pairs at `+0x24/+0x28` and `+0x2C/+0x30`
have independently verified positive denominators; the radius word at
`+0x38` is 32 or 64. Mode, frame, descriptor, radius, path and phase
accesses remain at context `0x191C/0x1920/0x1938/0x1950/0x1954/0x1960`.
The direct context minimum remains `0x1978`, not an allocation-capacity
claim. The entry graph reaches bands, sheets and webs, while spokes,
rings and quad remain retained code without a direct entry-call path.

All twelve band helper callees, all 36 resident binding addresses and
three resident caller owners were checked independently. Existing
bindings `0x800866F8/0x80087898` receive the established French SDK names
`rcos/RotTransPers3`, without introducing addresses or source-local
declarations. The 3,988-byte suffix per image remains unclassified.

The initial independent branch started at accepted
`ca8e1590e49dd6452b12a9f8d9c3ac3363989588`, not pending French422 sheets.
The addition is twelve C instances / 23,568 bytes: initial configured totals were
252 images, 1,018/1,581 C instances and 1,057,796 bytes. Expected family
ownership becomes 72 C owners / 85,824 bytes, 36 assembly owners /
112,032 bytes and 24 real header/suffix owners / 47,904 bytes.
Full French and US production acceptance passed: all 263 US images, the
clean US resident, all 252 French images and the clean French resident
match exactly. Fresh production ELF/object evidence verifies all 72
French421 C owners, preserving the sixty previous C owners and all
twelve US404 band C owners with their unchanged original profiles.
The 201 French, 112 Spanish, sixteen progress and five US-toolchain
regressions pass without skips, alongside G32 and repository policy gates.

An ordinary merge of accepted
`a1520c70d8fd0b8a78d4bbb5ccc2c331f48aafe4` retains the subsequently
maintainer-merged French422 sheets. Final configured totals are 252 images,
1,022/1,581 C instances and 1,062,820 bytes, adding only these twelve bands
to all 1,010 accepted C instances. Reconciled acceptance reproduces all
252 complete French images and the clean resident, with 202 French,
112 Spanish, sixteen progress and five US-toolchain regressions passing
without skips. Fresh production ELF/object evidence preserves all twenty
accepted Family422 C owners / 22,416 bytes alongside the 72 French421 C
owners / 85,824 bytes. Fifty-five of the original 57 authored files remain
byte-identical; only this note and the progress fixture change.
The verified US sources/profiles and original exact artifacts are unchanged
by this accepted-sheet merge; this does not claim a second US rebuild.

## Entry-called ribbons follow-up

The unchanged accepted US404 `variant404_ribbons.c` body matches both
French slots at `0x8013C1D0/0x8017C1D0`: 1,680 bytes and a 360-byte frame.
Two three-line wrappers only rename its symbol and include the existing
body and types. No regional macro, expression change, new declaration,
shared-header change or compiler-profile change is needed. Both final
wrappers independently relink all twelve complete unmasked images before
promotion; the sixteen historical band and earlier-helper ledger rows
remain byte-identical, followed by two terminal ribbon matches.

Thirty-six freshly target-compiled layout constants and 177 entry/helper
instruction anchors establish eight 108-byte `ModelVariantRibbonShort`
records at `context + 0xAB0..0xE10`, ending at the accepted band's base.
Each has two `SVECTOR` points at `0/0x20`, packed screens at `0x10/0x30`,
angles at `0x18`, widths at `0x38`, depths at `0x5C`, and **unsigned**
halfword offsets at `0x64/0x68`. The opaque `0x40..0x5C` bytes remain
opaque. The helper constructs its accessed point fields; this evidence
does not claim that the entry initializes the ribbon records.

The two points use radii `0x28/0x96`; mode word `0x191C` low bit chooses
the far point's Z of `0xA0/0xC0`. Four initial `ratan2` calls are retained,
including two whose results are unused. Slot halfword `0x1974` selects
turn negation. Translation uses words `0x189C/0x18A0/0x18A4`, direction
`0x18F0/0x18F4/0x18F8` and distance `0x1954`. Uniform scale is the first
152-byte sheet's size at `0xFD8 + 0x88 = 0x1060`, not ribbon storage.

The projection status grid is `PSXLONG status[8][2]`: 64 stack bytes at
frame `0xD0..0x110`. Projection `p` is at `0x110`, and the separate
offset-point projection flag is at `0x114`. The latter is not substituted
for the retained status grid. The 28-byte `POLY_G3` packet at context
`0x1738` produces **one triangle per ribbon**. Vertices zero and two use
RGB `0x80/0x80/0x80`; vertex one uses `0x40/0x60/0xFF`. Both depth and
stored status must be nonnegative. Negative depths are skipped, not
clamped. The existing custom emitter at resident `0x8004D5B8` receives
the low sixteen depth bits and fourth argument one.

The entry's positive-phase branch calls the ribbon helper at `0x1044`
and passes the original context at `0x1048`. All six actual command words
`587000..587005`, read independently from their stage-specific record
command locations, select a descriptor whose unsigned halfword `+0x18`
is one. Only when signed context halfword `0x194C` plus one equals that
threshold does the helper advance angle word `0x195C` by word `0x1928`
shifted left five. This is not the band's growth/fade timing calculation;
the ribbon helper contains no such division.

All eleven ribbon callees, 36 existing resident binding addresses and
three resident caller owners were checked against the resident ELF and
retail bytes. The existing `0x80087868` binding receives the established
French SDK name `RotTransPers`, without adding a binding or changing an
address. The `0x1978` direct context minimum still does not establish
allocation capacity; the full `0x406C..0x5000` suffix stays unclassified.

This independent branch starts at accepted
`fc40a96bcb6264279ef0da189eb3ac2ade9b370c`, not pending French445 bands.
It adds twelve C instances / 20,160 bytes, giving configured totals of
252 images, 1,120/1,581 matching C instances and 1,243,644 instruction
bytes. Verified family ownership is 84 C owners / 105,984 bytes,
24 assembly owners / 91,872 bytes, and 24 real header/suffix owners /
47,904 bytes, preserving all 72 prior C owners / 85,824 bytes.
Ribbon production acceptance passed: all 252 complete French overlays and
the clean French resident match exactly. Actual defining objects and linked
ELFs verify all 84 family C owners and preserve all 208 accepted Family435
C owners / 273,416 bytes, 24 Family422 owners / 27,872 bytes and 56
Family439 owners / 77,224 bytes. The 206 French, 112 Spanish and 21
progress/toolchain regressions pass without skips, alongside repository
policy and G32 checks. The shared US source, header, original profiles and
other regional registrations remain unchanged; no new US rebuild is claimed.

An ordinary merge of verified maintainer-accepted
`0ffe220bf7c8b9fd61fc8f8f05ceb6963df9dbe2` preserves French445 bands.
Ribbon reconciled acceptance passed: fresh builds reproduce all 252
French images and the clean resident, and actual object/ELF evidence
additionally preserves all 72 accepted Family445 C owners / 84,912 bytes.
The 84 Family421 and accepted Family435/422/439 owners remain exact.
All 207 French, 112 Spanish and 21 progress/toolchain regressions pass
without skips, alongside metadata/G32 checks. Final configured totals
are 1,132/1,581 C instances and 1,266,540 bytes across 252 images.
Fifty-three of the original 55 authored files remain byte-identical;
only this note and the aggregate progress fixture change in reconciliation.

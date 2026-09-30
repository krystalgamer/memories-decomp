# French MODEL headers 414 and 564

These 24 secondary-handler archive instances reuse the unchanged accepted
`src/overlays/model_variant/variant397_{sheets,webs,spokes,rings,quad}.c` bodies.
Ten three-line wrappers rename their symbols to measured French addresses.
Every matching entry uses `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81.
The original US compiler profile and shared C/header bodies are unchanged.
US header 397 is structural provenance, not a French header identity.

## Loader and instance evidence

The matched MODEL loader and phase callback read 276 sectors per compact
record. Stages 7/8 occupy record sectors 180/190 and stages 9/10 occupy
200/210. Each image occupies ten 2,048-byte sectors and loads at
`0x8013B000` or `0x8017B000`.

| Stages | Models |
|---|---|
| 7/8 | 2, 20, 87, 108, 138, 193, 573 |
| 9/10 | 152, 168, 170, 388, 427 |

The [instance ledger](french-model-variant414-instances.csv) records all
compact indices, slices, positive metadata commands and complete hashes.
These 24 independently checked archive instances contain 22 distinct complete
images. Whole-image equality, not a shared code prefix, establishes duplicates.
The existing resident secondary dispatchers call the selected buffer at `+4`
with its stored context and initial command or the update argument `-1`.

## Boundaries, ownership and reachability

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1228` | 4644 | generated assembly | yes |
| `0x1228..0x1990` | 1896 | generated assembly | yes |
| `0x1990..0x1E74` | 1252 | sheets C | yes |
| `0x1E74..0x23E4` | 1392 | webs C | yes |
| `0x23E4..0x26EC` | 776 | spokes C | no |
| `0x26EC..0x2A68` | 892 | rings C | no |
| `0x2A68..0x2DCC` | 868 | quad C | no |

Every function span has complete direct control flow, one terminal return and
no unresolved indirect transfer. The entry's direct call graph reaches the
first four functions, including sheets and webs. The other three matching bodies are
retained module-local code, not demonstrated additional runtime call paths.
Execution frequency and exhaustive runtime coverage are not established.

Ownership is supported by the entry's initialization of the exact records
used by these helpers. It captures `a0 -> s2 -> s6`, forms sheet/ring/spoke/quad
bases at `context + 0x5FC/0x72C/0xA8C/0xCCC`, advances sheet records by 152
bytes and the other records by 144 bytes, bounds the spoke loop at four,
and clears the rotation/state word at `context + 0xF74`. Twelve instruction
anchors were independently checked in all distinct images.

Seventy-one target-compiled layout constants cover the accepted local
`ModelVariantSheet`, quad, ring and spoke records and canonical SDK structures,
including both `POLY_GT4` and `POLY_G4`. These verify accessed offsets and
record extents, not the context's allocation bound.

The entry also initializes three 416-byte `ModelVariantWebNarrow` records
at `context + 0..0x4E0`. Each has two 4x6 `SVECTOR` grids at `0/0xC0`,
color at `0x180`, and scale at `0x194`. The webs helper reads the first
sheet's size through `context + 0x5FC + 0x88 = 0x684`; that timing read is
not a field in a fourth web. Thirty-one web-related entry/helper anchors
and 27 freshly target-compiled constants independently verify this view,
including SDK structures and the 32-bit `PSXLONG`.

All 24 legal archive selections retain their measured positive commands.
The selected descriptors are 72-byte records at
`module + 0x2EC8 + (command % 1000) * 72`, within the retained suffix.
The measured direct context extent is at least `0xF8C` bytes. Both contexts
are disjoint from the selected primary, secondary and variant load spans.
Neither this minimum extent nor the local web view proves allocation
capacity or global isolation.

Each image retains a real four-byte header owner and an 8,756-byte suffix
owner at `0x2DCC..0x5000`. That suffix is explicitly unclassified; using a data
segment to preserve its bytes does not prove it contains no code. No
linker-only storage alias substitutes for either owner.

## Exact matching and preservation

The [attempt ledger](french-model-variant414-attempts.csv) records eight
terminal canonical-wrapper matches from the initial integration, followed
by two terminal webs matches. No function-body or flag variation was
needed. Initial calibration compiled four accepted local US sources with the
authoritative French profile and scanned 2,362 distinct secondary payloads.
Internal J26 rebasing served only candidate discovery. Actual subsequent
links, unmasked complete-image comparisons and section-defined function owners
established the result before integration. The webs follow-up recompiled its
two final tracked wrappers and independently relinked all 24 complete
images before updating their existing registrations.

Production validation reproduces all 198 configured French images and the
complete French resident executable. This family now contributes 120 C owners
and 124,320 instruction bytes, with 48 assembly owners and 48 real
header/suffix owners. All 36 existing resident binding addresses are
preserved and independently checked against fresh-resident section owners.
The existing `0x80089928` binding is named `ratan2`, as independently
established by accepted French SDK bindings; no extra binding is introduced.
The webs helper's eight callees and three resident caller owners are also
rechecked. Across the family, 156,960 assembly bytes and 210,144 suffix bytes
remain untranslated or unclassified. All module manifest records and every
unrelated accepted C body, header, profile and region remain unchanged.

The shared regression fixture tests both families without duplicate test
discovery. It checks loader selections and commands, full hashes, source
selection, canonical wrappers, storage extents, entry anchors, all function
boundaries and the sheets/webs-versus-retained reachability distinction.
It also guards the resident binding name, descriptor stride and selection,
timing-read anchors, measured direct context extent and load separation.

The webs follow-up adds 24 C instances and 33,408 instruction bytes without
adding images or function boundaries. Configured totals become 198 images,
826/1,377 matching C instances and 769,996 C instruction bytes. These are
inventory figures, not exhaustive runtime coverage or seven-release
completion. General progress snapshots remain separate.

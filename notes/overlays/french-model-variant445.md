# French MODEL headers 445 and 595

These twelve secondary-handler images reuse the unchanged accepted
`src/overlays/model_variant/variant428_{sheets,webs,spokes,rings,quad}.c` bodies
and directly select the accepted Spanish445 band sources. Ten three-line
French wrappers rename the original shared helpers; the Spanish band body
and its existing slot-one wrapper already define the measured French
symbols. All use the existing `gcc_2_8_1_g0_split` profile,
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
| `0x1034..0x17A8` | 1908 | bands C | yes |
| `0x17A8..0x1C8C` | 1252 | sheets C | yes |
| `0x1C8C..0x21F0` | 1380 | webs C | yes |
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

## Original integration

The [attempt ledger](french-model-variant445-attempts.csv) records eight
original terminal canonical-wrapper matches. All four original bodies compiled
unchanged with the authoritative French profile. Calibration searched 2,362 distinct French
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

The original cutoff totals were 116 images, 554/889 matching C instances and
457,908 C instruction bytes. These figures are not exhaustive runtime
coverage or seven-release completion. General progress snapshots remain
separate.

## Entry-called narrow webs

The unchanged accepted `variant428_webs.c` now matches all twelve
`0x1C8C..0x21F0` helpers with a 288-byte frame. Two additional terminal
canonical-wrapper records add 12 C instances / 16,560 instruction bytes.
The helper is directly entry-called, unlike the retained spokes/rings/quad.

Entry initializes three 416-byte narrow-web records at context
`0x5D0..0xAB0`. Each has two 4-by-6 `SVECTOR` grids at record offsets
`0/0xC0`, RGB at `0x180`, scale at `0x194`, and a separately cleared
word at `0x198`. Element, row and record advances are 8, 48 and 416 bytes;
the six/four/three loop bounds agree independently.

The timing pointer targets the **first of two 152-byte sheets** at
`0xC78..0xDA8`, reading its `+0x88` size field. The saved-pointer
reload, zero store at that field, two-record bound and 152-byte advances
distinguish it from the 296-byte timing record used by Family465.
Phase zero subtracts `step * 0xE0`; the helper sorts the `GsGLINE`
at context `0x1514` for **nonnegative** depth and flags. Step and phase
are at `0x1588` and `0x15BC`.

Actual commands `611000..611004` select 52-byte descriptors at module
`0x3690 + (command % 1000) * 52`, stored in context `0x1590`.
The shift/add sequence establishes the stride independently. The direct
context minimum `0x15D0` is separated from measured loader ranges but is
not allocation capacity or a global-isolation proof.

Sixty-one focused retail anchors and 30 freshly target-compiled C/SDK
constants establish the accessed views. All 36 resident binding addresses,
eight helper callees and three loader/controller owners have actual
resident ELF evidence. Existing addresses `0x80087898`/`0x80089928`
are named `RotTransPers3`/`ratan2`; no binding address is added.

Initial acceptance used `da5118957` and reproduced all 198 configured French
images. After the maintainer accepted Family415/439, the unpublished batch
was refreshed once onto accepted `af5859e680`, without stacking the pending
Family424 batch. It preserves every one of the 222 accepted French
registrations, the other C selections, shared bodies, declarations and
compiler profiles. Family ownership becomes
60 C / 62,016 C bytes, 36 assembly / 102,528 assembly bytes, and
24 raw owners. Clean acceptance reproduced all 222 complete French images
and the French resident, with actual production C owners and fresh layouts.
Configured cutoff totals are 902/1,521 C instances and 866,724 C bytes.
Seven family regressions include the new view, descriptor and binding
checks; French variant and 16 progress regressions and repository
policy gates pass. Remaining assembly and unclassified suffixes are not promoted.

## Entry-called bands from the accepted Spanish implementation

The unchanged `src/overlays/spanish_model_variant/variant445_bands.c`
and its existing slot-one wrapper independently reproduce both French
1,908-byte functions with 296-byte frames and all twelve complete
canonical scratch images. Their symbols already match the French load
addresses, so the manifests directly reuse those two sources. No new
wrapper, regional conditional, body, header, type or profile is needed.
The ten previous ledger rows remain byte-identical, followed by two
canonical matches. This adds twelve C instances / 22,896 bytes.

Thirty-five freshly compiled layout constants and 151 retail instruction
anchors establish one 456-byte `ModelVariantBand` at context
`0xAB0..0xC78`. Three nine-point `SVECTOR` rows occupy record
`0/0x48/0x90`; their screens are at `0xD8/0xFC/0x120`, colors at
`0x144/0x168`, and depths at `0x1A4`. Entry initializes all nine color
entries and advances by `0x1C8`. Existing opaque bytes stay opaque.
The nine four-byte projection flags instead occupy stack `0xC8..0xEC`;
the projection `p` output is at `0xF0`. They are not record fields.

The helper projects nine points and draws two `POLY_GT4` halves for
each of eight segments through context packet `0x1418`. Negative
depths are clamped to zero, projection flags are cleared, and the
subsequent nonnegative-depth tests and low-sixteen-bit sorting remain
unchanged. The accepted Spanish next-column indexing is reused as-is.
Mode word `0x157C` chooses radius factors 48/56; angle uses signed
halfwords `0x1562/0x1560`. Translation words are `0x153C/0x1540/0x1544`,
and direction words are `0x1550/0x1554/0x1558`.

Phase `0x15BC` grows distance word `0x15B4` to `0x400`, then advances
to phase two. The later fade reduces radius halfword `0x15B0` from
`0x1000` to zero and advances to phase three. The preserved arithmetic
uses unsigned timing division. All actual stage-specific commands
independently select the following descriptor words `+0x20/+0x24`
and `+0x28/+0x2C`; every selected denominator is positive.

| Command | Growth start/end | Fade start/end |
|---|---|---|
| 611000 | 24/32 | 94/116 |
| 611001 | 148/160 | 360/390 |
| 611002 | 48/64 | 240/300 |
| 611003 | 20/28 | 120/140 |
| 611004 | 90/98 | 160/170 |

Entry calls at `+0xECC`, passing its original context at `+0xED0`,
only on the positive-phase path. The direct context extent remains
`0x15D0`, not allocation capacity, and `0x3594..0x5000` remains
unclassified. Twelve helper callees, all 36 resident binding addresses
and three loader/controller owners are independently verified.
The existing address `0x800866F8` is normalized to `rcos` in the
family bindings and twelve symbol files. Its 160-byte SDK interval
is byte-identical to the accepted Spanish target, and the same French
name/address pair is already established in Family435. No address is
added or changed.

This branch starts independently from accepted
`aac97ef48488cde0e875b47dfb41bba1a190a4af`, excluding pending
French435 spiral work. A standalone French regression adds eighty
band-specific anchors and five exact timing records without changing
the inherited generic anchors. Spanish445 retains its six accepted
helpers, direct source selection and reachable-helper set.

Band production acceptance passed all 252 complete French overlays and
the clean French resident. Actual defining objects and linked ELFs prove
72 Family445 C owners / 84,912 bytes, retaining all sixty previous C
owners / 62,016 bytes. The 24 remaining assembly owners occupy 79,632
bytes; 24 raw header/suffix owners occupy 81,216 bytes. Fresh production
also preserves all 24 accepted Family422 C owners / 27,872 bytes and
all 56 Family439 C owners / 77,224 bytes, and verifies all recorded
resident bindings and callers.

All 205 French, 112 Spanish and 21 progress/toolchain regressions pass
without skips, alongside metadata and G32 checks. The independent scope
is 54 paths, with no C/header or US/Spanish profile changes. No fresh US
rebuild is claimed for this unchanged-source reuse. Fixed-cutoff totals
are 1,094/1,581 C instances / 1,177,740 bytes, excluding French435
spiral work and making no exhaustive coverage claim.

## Band reconciliation with accepted French435 spiral

The independently verified band checkpoint
`58842e19e274360fb57a2bbdbeeae5506a1b5739` ordinarily merged accepted
`fc40a96bcb6264279ef0da189eb3ac2ade9b370c` after verifying the
maintainer's spiral squash: all 86 authored paths were byte-identical
and all twelve exact-head checks succeeded. Only aggregate progress
conflicted; no pending branch was stacked.

Band reconciled acceptance passed all 252 complete French overlays and
the clean French resident again, with 206 French, 112 Spanish and
21 progress/toolchain regressions and metadata/G32 checks. Fresh defining
objects and linked ELFs preserve all 208 accepted Family435 C owners /
273,416 bytes, 24 Family422 owners / 27,872 bytes and 56 Family439
owners / 77,224 bytes, alongside the 72 Family445 owners / 84,912 bytes.
All source bodies and US/Spanish metadata remain unchanged relative to
the accepted cutoff; no new US rebuild is claimed.

Final cutoff totals are 1,120/1,581 C instances / 1,246,380 bytes.
The authored scope remains 54 paths, with 52 original files byte-identical;
only this note and the aggregate progress fixture changed in reconciliation.

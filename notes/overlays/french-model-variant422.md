# French MODEL headers 422 and 572

Four distinct secondary images reuse the unchanged accepted
`src/overlays/model_variant/variant405_{veils,bands,sheets,webs,spokes,rings,quad}.c` bodies.
Fourteen three-line wrappers select their functions for independently measured
French addresses. The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1
and MASPSX 2.81. Shared bodies, headers, G32 annotations and US profiles
are unchanged; US header 405 establishes provenance, not French identity.

## Loader and boundaries

Models 1 and 550 (compact records 1 and 500) use these images at stages 7/8.
The matched loader selects record sectors 180/190 from 276-sector records;
ten 2,048-byte sectors load at `0x8013B000`/`0x8017B000`. The resident
controller calls image `+4` with context and initial command or update `-1`.
The [instance ledger](french-model-variant422-instances.csv) records actual
nonnegative commands, indices, slices and complete hashes. Other stages and
models are excluded.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1128` | 4388 | generated assembly | yes |
| `0x1128..0x1620` | 1272 | generated assembly | yes |
| `0x1620..0x1CFC` | 1756 | veils C | yes |
| `0x1CFC..0x2408` | 1804 | bands C | no |
| `0x2408..0x28F0` | 1256 | sheets C | no |
| `0x28F0..0x2E44` | 1364 | webs C | no |
| `0x2E44..0x3154` | 784 | spokes C | no |
| `0x3154..0x34D0` | 892 | rings C | no |
| `0x34D0..0x3834` | 868 | quad C | no |

Strict control-flow walks cover each entire span with one terminal return
and no unresolved indirect transfer. Entry reaches only the first three
functions. Veils are entry-called; the other six C helpers are retained
module-local code, not proven additional runtime paths. Each image's four-byte header and 6,092-byte
suffix at `0x3834..0x5000` have real storage owners. The suffix remains
unclassified; declaring raw storage does not prove it contains no code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. Its saved pointers describe one 456-byte
`ModelVariantBand` at context `+0x12B4`, two 152-byte sheets at `+0x147C`,
six 144-byte rings at `+0x15AC`, four 144-byte spoke records at `+0x190C`,
and one 144-byte quad at `+0x1B4C`. Adjacent extents agree exactly. The
band is not header435's 492-byte padded record.

Forty-four checked entry anchors include pointer saves/reloads, loop
initialization, increments, bounds and strides. Band counter `a1` starts
zero, increments once and repeats only while nonpositive. Its two color
rows begin at 324/360, with nine four-byte entries. The quad loop likewise
initializes one record, while ring/spoke bounds are six/four. These are
accessed-view observations, not a complete context allocation proof.

Ninety-five target-compiled constants check the accepted local band,
sheet, ring, spoke and quad records and SDK layouts: `SVECTOR`, `VECTOR`,
`MATRIX`, stored-pointer-bearing `GsCOORDINATE2`, `GsGLINE`, `POLY_G4`,
`POLY_GT4`, and target primitive widths. Band size is 456; point rows begin
at 0/72/144, screen rows at 216/252/288, and nine depths at 420..452.

## Exact matching and preservation

The [attempt ledger](french-model-variant422-attempts.csv) originally recorded eight
terminal canonical-wrapper matches. All four original accepted bodies were freshly
compiled against the current local header. Rebasing located candidates
only; actual canonical links reproduced all four unmasked complete images.
Thirty-six independent resident bindings include `RotTransPers3` from
the accepted French Exodia manifest, rather than a guessed alias.

Production validation of the original combined
[422/442 batch](french-model-variant442.md) preserved all 144 then-accepted
French registrations and reproduced all 152 complete images plus the clean
French resident. That integration contributed 16 sized C owners, 17,392 C bytes,
20 assembly owners, eight raw owners, 36 freshly verified resident callee
owners and 95 recompiled layouts. Its 40,144 assembly bytes and 24,368
unclassified suffix bytes remain untranslated.

Five inherited regressions check actual archive slices/commands/hashes,
canonical source selection and fingerprints, all nine control-flow spans,
44 entry anchors, and raw storage extents. The two-family addition totals
32 C instances and 34,784 bytes: 152 configured images, 668/1105 matching C
instances and 602,540 C instruction bytes. These counts do not establish
exhaustive runtime coverage or seven-release completion. General report
snapshots remain separate.

## Retained sheets follow-up

The newly accepted US405 sheet body also exactly reproduces French
`+0x2408..+0x28F0`: 1,256 instruction bytes and a 272-byte frame in both
slots. The body, headers and compiler profiles are unchanged. Two additional
canonical-wrapper ledger records follow the eight unchanged original rows.
All four complete scratch images match without masking and have real sized
sheet C owners. This adds four C instances and 5,024 bytes, not new images
or a new demonstrated runtime path.

Thirty freshly target-compiled constants and 107 instruction anchors verify
the sheet/SDK layouts and their uses. The two 152-byte sheet views occupy
`context + 0x147C..0x15AC`, with four four-point `SVECTOR` rows at
`0/0x20/0x40/0x60`, outer color at `0x80`, inner color at `0x84` and size
at `0x88`. The helper uses the `POLY_GT4` packet at context `+0x1C84`,
draws four quads per sheet and sorts only positive depths, passing their
low sixteen bits. The measured view does not assign unaccessed padding.

Mode, frame and frame-step reads are at `+0x1DB4/+0x1DB8/+0x1DC0`;
the descriptor pointer is at `+0x1DC8`. Size, path and phase reads are at
`+0x1E10/+0x1E14/+0x1E1C`. All four legal raw commands are `588000`;
their selected index zero addresses the 48-byte descriptor at module
`+0x386C`, whose timing words at `+0x1C/+0x20` are `60/120`.
The positive denominator is independently checked against the retail
archive, not inferred from the reused C.

All 36 resident binding addresses and names remain unchanged. Nine sheet
callees and three resident caller owners are independently verified against
the exact resident. The entry-call graph still reaches only `+4`, `+0x1128`
and `+0x1620`; the sheets remain retained, non-entry-reachable code. The minimum
direct context extent `0x1E38` is checked for separation from selected load
spans, not claimed as allocation capacity or complete runtime coverage.
The suffix at `+0x3834..+0x5000` remains unclassified.

Fresh production gates reproduce all 252 complete French images and the clean
French resident without masking. Actual linked objects and ELFs prove
20 C owners / 22,416 bytes, preserving all sixteen prior owners;
sixteen assembly owners / 35,120 bytes and eight real header/suffix owners /
24,384 bytes. At the initial accepted baseline, configured totals became
252 images, 978/1,581 C instances
and 983,188 C instruction bytes. US/Spanish bodies, profiles, bindings and
inventories are unchanged. All 200 French, 112 Spanish, 16 progress and five
US-toolchain regressions pass without skips, alongside G32, metadata,
external-attempts, basic-types, declaration and note-policy checks.
General progress snapshots remain separate.

The ordinary reconciliation onto accepted
`095a121978dd1e2e23d4395f78c4ab9676d51516` preserves the accepted French440
sheet/web integration. Combined configured totals are 252 images,
986/1,581 C instances and 993,748 bytes. Fresh production gates reproduce all
252 complete French images and the clean resident, explicitly preserving
all twenty accepted Family440 C owners and their 20,736 bytes. All 201 French,
112 Spanish, 16 progress and five US-toolchain regressions and policy checks
pass. Both sheet wrappers, the shared body/header/profile, all ten
terminal ledger rows and the measured view/descriptor evidence are unchanged.

The subsequent ordinary merge of accepted
`ca8e1590e49dd6452b12a9f8d9c3ac3363989588` also retains the accepted
Family414 bands. Final configured totals are 252 images, 1,010/1,581 C
instances and 1,039,252 bytes, adding only these four sheets to all 1,006
accepted C instances. Fresh production gates reproduce all 252 complete
French images and the clean resident, with 201 French, 112 Spanish,
sixteen progress and five US-toolchain regressions passing without skips.
Fresh linked ELF/object evidence preserves all 144 accepted Family414 C
owners / 169,824 bytes and twenty Family440 C owners / 20,736 bytes,
alongside this family's twenty C owners / 22,416 bytes. Sixteen of the
original eighteen authored files remain byte-identical; only this note and
the progress fixture change. Shared US/Spanish sources, profiles and
inventories are unchanged relative to that accepted cutoff; this does not
claim a fresh US rebuild.

## Retained webs follow-up

The unchanged accepted US405 web body reproduces `+0x28F0..+0x2E44`:
1,364 instruction bytes and a 288-byte frame in each slot. Four complete
canonical scratch images match without masking, with sized defining C
objects and linked ELF symbols. Two terminal ledger records follow the ten
unchanged accepted records. This adds four C instances and 5,456 bytes,
not new images or demonstrated runtime paths. Shared source, header,
compiler profile and all 36 binding names/addresses remain unchanged.

Twenty-four freshly compiled layout constants and 155 retail instruction
anchors verify three 416-byte narrow webs at context `+0xDD4..+0x12B4`,
ending at the accepted band view. Each contains two four-by-six `SVECTOR`
grids at `0/0xC0`, 48-byte rows, color at `0x180`, scale at `0x194` and
the initialized done field at `0x198`. Entry initializes scale to
`0x1000 + index * 0x1000 / 3` and clears done. These observations do not
assign unaccessed padding or prove a complete context allocation.

The helper retains four `ratan2` calls and uses the line packet at
`+0x1D4C`. Projection outputs are stack `p/flag` at `0xD0/0xD4`; only
strictly positive depth gates sorting, whose argument is truncated to
sixteen bits. Unlike Family341, the flag is unused. Phase at `+0x1E1C`
selects translation words `+0x1D74/+0x1D78/+0x1D7C` or signed halfwords
`+0x1D80/+0x1D82/+0x1D84`.

Phase zero fades over `0x800..0x1000`. Its unsigned timing division
subtracts descriptor word `+0x1C` from time at `+0x1DB8`, then divides
by descriptor `+0x20 - +0x1C`; all four retail commands select the
previously verified `60/120` timing pair. Phase one uses black at scale
`0x1000` and resets scales to negative index offsets. Later phases fade
over `0x1800..0x2000` and advance by `context[+0x1DC0] << 8`.
Phases at least four clamp at `0x2000`; phase four can advance to five
on the final web when the local done flag remains set. Other wrapping
clears that local flag. This is not a write to the web's done field.

Eight helper callees, all 36 resident binding owners and three resident
caller owners are checked against the exact resident. Entry still reaches
only `+4`, `+0x1128` and `+0x1620`; webs remain retained. The direct context
minimum stays `0x1E38`, not capacity, and the suffix remains unclassified.
Production acceptance passed for all 252 complete French images and the
clean resident. Actual defining objects and linked ELFs prove 24 Family422
C owners / 27,872 bytes, preserving all twenty prior owners / 22,416 bytes.
Twelve assembly owners / 29,664 bytes and eight raw owners / 24,384 bytes
retain the remaining image contents. All 204 French, 112 Spanish, sixteen
progress and five US-toolchain regressions pass without skips, alongside
external-attempts, basic-types, metadata, note, G32 and declaration checks.

The independent base is accepted
`154f085e522027d621f39f5ed8d3410645d40a1f`, excluding unpublished Family439.
Fixed-cutoff totals become 252 images, 1,054/1,581 C instances and
1,109,820 C instruction bytes. US/Spanish bodies, profiles and inventories
are unchanged; this does not claim a fresh US rebuild. General report
snapshots remain separate, and this does not establish French completion.

Reconciled acceptance passed after ordinarily merging accepted French439
`28bd694898b41b037bdf0d10cf3ed458b76b0f49`. All 252 complete French images
and the clean resident match again. Fresh defining-object/ELF evidence also
preserves all 56 accepted Family439 C owners / 77,224 bytes, alongside this
family's 24 owners and all twenty prior Family422 owners.

Combined totals are 252 images, 1,082/1,581 C instances and 1,154,844 bytes.
All 204 French, 112 Spanish, sixteen progress and five US-toolchain
regressions and the policy gates pass without skips. Sixteen original
authored files remain byte-identical; only this note and the aggregate
progress fixture change. Shared sources, headers, profiles and other
regional inventories are unchanged relative to that accepted cutoff.

## Entry-called veils follow-up

The unchanged accepted US405 veil body/header reproduces
`+0x1620..+0x1CFC`: 1,756 instruction bytes and a 312-byte frame in both
slots. The first calibration and canonical wrappers reproduce all four
unmasked complete images. Combined scratch links preserve all 24 accepted
C owners / 27,872 bytes and add four veils / 7,024 bytes, yielding 28
genuine defining-object and linked-ELF C owners / 34,896 bytes.
The twelve accepted ledger rows remain byte-identical, followed by two
exact calibration rows and two canonical terminals. Shared sources,
headers and compiler profiles are unchanged.

Forty-four freshly target-compiled constants and 114 literal retail
anchors independently verify five `0x1A0` veil records at context
`+0..+0x820`. Three seventeen-point `SVECTOR` rows start at
`0/0x88/0x110`, with scale/count at `0x198/0x19C`. Entry initializes
all five rows, clears each count, and staggers scale using signed division
by five and successive accumulator decrements of `0x800`. These extents
end at the separately measured ring-strip view; they do not establish
unaccessed storage ownership or total context allocation.

Entry `+0xFA4`, with its delay slot passing the original context, calls
veils after unsigned clock `+0x1DB8` reaches descriptor `+0x1C`.
All four commands are `588000`, selecting the 48-byte descriptor at
module `+0x386C`; its `+0x1C/+0x20/+0x24` values are `60/120/360`.
The accessed context minimum remains `0x1E38`, not allocation capacity.

Each positive-scale record regenerates seventeen points per row, using
angles `index << 8` and radial factors `224/320/416`. A single `POLY_GT4`
at context `+0x1CB8..+0x1CEC` draws sixteen strips. A/C projection
selects texture coordinates and screen-dependent texture pages, followed
by A/B projection for final geometry. The active-buffer call and two
unused-result `ratan2` calls are retained. Semi-transparency is enabled,
texture shading is enabled, inner corners use white and outer corners
use RGB `0/128/255`. Signed nonnegative depth and flag gate sorting;
depth is narrowed to sixteen bits.

Scale below `0x800` advances by `context[+0x1DC0] * 40`. Crossing
`0x800` wraps before clock 360; afterwards it clamps and marks the
record's count. Such a crossing can change phase zero to two once clock
120 is reached. All five counts are multiplied; on the fifth record,
product one and phase two advance phase to five.

The accepted body's unused 24-byte stack reservation is preserved, not
assigned a recovered semantic owner or original array type. Retail
anchors independently place the coordinate at stack `0x80..0xD0`,
dark RGB at `0xE8..0xEA`, light RGB at `0xF0..0xF2`, and projection
outputs at `0xF8/0xFC`. No direct stack memory access enters the
intervening `0xD0..0xE8` interval.

All 36 resident binding addresses, fourteen helper callees and three
resident caller owners are independently verified. Five already-established
French SDK names replace address aliases without changing their addresses:
`GsGetActiveBuff`, `GetTPage`, `SetPolyGT4`, `SetSemiTrans` and `SetShadeTex`.
The remaining entry and ring-strip functions stay assembly, and
`+0x3834..+0x5000` remains unclassified.

The independent accepted base is
`8744bcd9d36319b5980a96e26b7b8799e24eaab5`, excluding pending French337
ribbons. Fixed-cutoff configured totals become 252 images,
1,180/1,581 matching C instances and 1,329,580 C instruction bytes.
General report snapshots remain separate; this does not establish
French campaign completion.

Final production acceptance reproduces all 252 complete French images and
the clean French resident. Selected input objects and final linked ELFs
verify the 28 C owners / 34,896 bytes, eight assembly owners / 22,640
bytes, and eight raw header/suffix owners / 24,384 bytes. All 239 French,
142 Spanish and 22 focused progress/toolchain regressions pass without
skips, alongside metadata, external-attempts, basic-types, G32,
declaration and note-policy gates. Shared US/Spanish source and headers
remain byte-identical to the accepted base; this does not claim a fresh
US rebuild.

# French MODEL headers 422 and 572

Four distinct secondary images reuse the unchanged accepted
`src/overlays/model_variant/variant405_{bands,sheets,spokes,rings,quad}.c` bodies.
Ten three-line wrappers rename their functions for independently measured
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
| `0x1620..0x1CFC` | 1756 | generated assembly | yes |
| `0x1CFC..0x2408` | 1804 | bands C | no |
| `0x2408..0x28F0` | 1256 | sheets C | no |
| `0x28F0..0x2E44` | 1364 | generated assembly | no |
| `0x2E44..0x3154` | 784 | spokes C | no |
| `0x3154..0x34D0` | 892 | rings C | no |
| `0x34D0..0x3834` | 868 | quad C | no |

Strict control-flow walks cover each entire span with one terminal return
and no unresolved indirect transfer. Entry reaches only the first three
functions. All five C helpers are retained module-local code, not proven
additional runtime paths. Each image's four-byte header and 6,092-byte
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

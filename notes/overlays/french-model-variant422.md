# French MODEL headers 422 and 572

Four distinct secondary images reuse the unchanged accepted
`src/overlays/model_variant/variant405_{bands,spokes,rings,quad}.c` bodies.
Eight three-line wrappers rename their functions for independently measured
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
| `0x2408..0x28F0` | 1256 | generated assembly | no |
| `0x28F0..0x2E44` | 1364 | generated assembly | no |
| `0x2E44..0x3154` | 784 | spokes C | no |
| `0x3154..0x34D0` | 892 | rings C | no |
| `0x34D0..0x3834` | 868 | quad C | no |

Strict control-flow walks cover each entire span with one terminal return
and no unresolved indirect transfer. Entry reaches only the first three
functions. All four C helpers are retained module-local code, not proven
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

The [attempt ledger](french-model-variant422-attempts.csv) records eight
terminal canonical-wrapper matches. All four accepted bodies were freshly
compiled against the current local header. Rebasing located candidates
only; actual canonical links reproduced all four unmasked complete images.
Thirty-six independent resident bindings include `RotTransPers3` from
the accepted French Exodia manifest, rather than a guessed alias.

Production validation of the combined
[422/442 batch](french-model-variant442.md) preserves all 144 accepted
French registrations and reproduces all 152 complete images plus the clean
French resident. This family contributes 16 sized C owners, 17,392 C bytes,
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

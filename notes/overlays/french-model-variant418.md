# French MODEL headers 418 and 568

Two distinct images for model410 reuse the unchanged accepted
`src/overlays/model_variant/variant401_{spokes,rings,quad}.c` bodies through
six three-line wrappers. The existing `gcc_2_8_1_g0_split` profile uses
GCC 2.8.1 and MASPSX 2.81. Shared bodies, headers, G32 annotations and US
profiles are unchanged. US source family401 is provenance, not French
identity; it is unrelated to the older French model401/header432 renderer.

## Loader and boundaries

Model410 is compact record360. Stages9/10 select record sectors200/210
from 276-sector records; each ten-sector image loads at
`0x8013B000`/`0x8017B000`. The matched resident controller dispatches `+4`
with context and initial command or update `-1`. The
[instance ledger](french-model-variant418-instances.csv) records actual
nonnegative commands, archive slices and complete hashes. Other stages and
models are excluded.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0xFE0` | 4060 | generated assembly | yes |
| `0xFE0..0x1760` | 1920 | generated assembly | yes |
| `0x1760..0x1C9C` | 1340 | generated assembly | yes |
| `0x1C9C..0x2204` | 1384 | generated assembly | yes |
| `0x2204..0x272C` | 1320 | generated assembly | yes |
| `0x272C..0x2A3C` | 784 | spokes C | no |
| `0x2A3C..0x2DB8` | 892 | rings C | no |
| `0x2DB8..0x311C` | 868 | quad C | no |

Strict walks cover every instruction in each span with one terminal return
and no unresolved indirect transfer. Entry reaches the first five functions;
all three C helpers remain retained module-local code with no demonstrated
entry execution path. Real storage owners preserve each four-byte header
and 7,908-byte suffix at `0x311C..0x5000`. That suffix remains unclassified,
not established non-code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. Saved context pointers describe two
152-byte sheets at `+0x11DC`, six 144-byte rings at `+0x130C`, four
144-byte spoke records at `+0x166C`, and one 144-byte quad at `+0x18AC`.
Adjacent extents agree exactly. The spokes helper consumes the first
sheet's size at record `+0x88`, which entry clears independently.

Forty-three entry anchors cover the saved pointer bases, reloads, advances,
initialization counters and loop bounds. Quad counter `s7` starts zero,
increments and repeats only while nonpositive: one record. Sheet count
is two, ring count six and spoke count four. These accessed views do not
establish whole-context allocation or additional runtime paths.

Seventy-one target-compiled constants independently verify local
`ModelVariantQuad`, `ModelVariantRing`, `ModelVariantSpokeRing` and
`ModelVariantSheet` layouts, plus `SVECTOR`, `VECTOR`, `MATRIX`,
stored-pointer-bearing `GsCOORDINATE2`, `GsGLINE`, `POLY_G4`, `POLY_GT4`
and four-byte target `long`/`s32`.

## Exact matching and preservation

The [attempt ledger](french-model-variant418-attempts.csv) records six
terminal canonical-wrapper matches. Current local source/header fingerprints
were checked before target compilation. Rebasing located candidates only;
actual canonical links reproduce both unmasked complete images. The
slot-zero rings symbol already equals the US source symbol; its self-renaming
macro preserves the uniform, independently verified wrapper form.

Production validation of the combined
[418/433 batch](french-model-variant433.md) preserves all 152 accepted
French registrations and reproduces all 158 complete images plus the clean
French resident. This family adds six sized C owners and 5,088 C bytes,
ten assembly owners, four raw owners, 36 independently verified fresh-resident
callee owners and 71 recompiled layouts. Its 20,048 assembly bytes and
15,816 unclassified suffix bytes remain untranslated.

Five inherited regressions cover actual archive slices/commands/hashes,
source selection and fingerprints, real storage extents, all eight
control-flow spans and 43 entry anchors. The combined addition is 18 C
instances and 15,264 bytes: 158 configured images, 686/1153 matching C
instances and 617,804 C bytes. These figures are not exhaustive runtime
coverage or seven-release completion; general progress snapshots stay separate.

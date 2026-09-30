# French MODEL headers 433 and 583

Four distinct images for models180/440 reuse the unchanged accepted
`src/overlays/model_variant/variant416_{spokes,rings,quad}.c` bodies through
six three-line French wrappers. The authoritative `gcc_2_8_1_g0_split`
profile uses GCC 2.8.1 and MASPSX 2.81. Shared bodies, headers, G32
annotations and US compiler profiles remain unchanged.

## Loader and boundaries

Models180/440 map to compact records180/390. Stages7/8 select record
sectors180/190 from 276-sector records. Each ten-sector image loads at
`0x8013B000`/`0x8017B000`; the matched resident controller calls `+4`
with context and initial command or update `-1`. The
[instance ledger](french-model-variant433-instances.csv) records actual
nonnegative commands, slices and four distinct complete hashes. Other
stages and models are not covered.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1050` | 4172 | generated assembly | yes |
| `0x1050..0x1810` | 1984 | generated assembly | yes |
| `0x1810..0x1C64` | 1108 | generated assembly | yes |
| `0x1C64..0x21CC` | 1384 | generated assembly | yes |
| `0x21CC..0x26C8` | 1276 | generated assembly | yes |
| `0x26C8..0x29D8` | 784 | spokes C | no |
| `0x29D8..0x2D54` | 892 | rings C | no |
| `0x2D54..0x30B8` | 868 | quad C | no |

Complete direct control-flow walks cover all eight spans, each with one
terminal return and no unresolved indirect transfer. Entry reaches the first
five functions. The three C helpers remain retained module-local code, not
demonstrated entry execution paths. Every four-byte header and 8,008-byte
suffix at `0x30B8..0x5000` has a real storage owner. The suffix remains
explicitly unclassified, not proven free of code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. Three 152-byte sheets start at context
`+0x10B0`, followed by six 144-byte rings at `+0x1278`, four 144-byte
spoke records at `+0x15D8`, and one 144-byte quad at `+0x1818`.
Adjacent extents agree exactly. Entry clears the first sheet's `+0x88`
size field consumed by the spokes helper.

Forty-three instruction anchors independently confirm saved pointers,
reloads, strides, initialized counters and bounds. The quad counter starts
zero and repeats only while nonpositive after increment; sheets use bound
three, rings six and spokes four. The initialization sequence shares the
companion418 form at a measured `+0x24` instruction offset, with different
context bases and sheet count. Its regression fixture reuses that structure
but verifies all resulting words in all four real French images.

Seventy-one target-compiled local/SDK layout constants cover the same
accessed views as the [418 evidence](french-model-variant418.md), including
stored-pointer-bearing `GsCOORDINATE2` and four-byte target `long`. They are
recompiled independently for this family. Accessed views do not prove total
context allocation or additional execution paths.

## Exact matching and preservation

The [attempt ledger](french-model-variant433-attempts.csv) records six
terminal canonical-wrapper matches after fresh current-header compilation.
Candidate rebasing is not acceptance evidence: actual canonical links
reproduce all four unmasked complete images with sized section-defined C
owners.

The combined418/433 production gate preserves all 152 previous French
registrations, reproduces all 158 images and matches the clean French
resident. This family contributes twelve C owners and 10,176 bytes,
twenty assembly owners, eight raw owners, 36 fresh-resident callee owners
and 71 recompiled layouts. The 39,696 assembly bytes and 32,032 unclassified
suffix bytes remain untranslated.

Five inherited regressions check archive slices, commands, complete hashes,
canonical sources/fingerprints, storage extents, eight control-flow spans and
43 entry anchors. The two-family batch adds six images, 18 C instances
and 15,264 bytes. Configured totals become 158 images, 686/1153 C instances
and 617,804 C instruction bytes; these are not exhaustive runtime coverage
or seven-release completion. General report snapshots remain separate.

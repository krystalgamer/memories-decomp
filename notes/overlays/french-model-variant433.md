# French MODEL headers 433 and 583

Four distinct images for models180/440 reuse the unchanged accepted
`src/overlays/model_variant/variant416_{webs,spokes,rings,quad}.c` bodies through
eight three-line French wrappers. The authoritative `gcc_2_8_1_g0_split`
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
| `0x1C64..0x21CC` | 1384 | webs C | yes |
| `0x21CC..0x26C8` | 1276 | generated assembly | yes |
| `0x26C8..0x29D8` | 784 | spokes C | no |
| `0x29D8..0x2D54` | 892 | rings C | no |
| `0x2D54..0x30B8` | 868 | quad C | no |

Complete direct control-flow walks cover all eight spans, each with one
terminal return and no unresolved indirect transfer. Entry reaches the first
five functions. Spokes, rings and quad remain retained module-local code, not
demonstrated entry execution paths; webs is entry-called. Every four-byte header and 8,008-byte
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

## Original integration

The [attempt ledger](french-model-variant433-attempts.csv) initially recorded
six terminal canonical-wrapper matches after fresh current-header compilation.
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

## Entry-called phased webs

The newly accepted unchanged US Family416 webs body reproduces the French
helper's 1,384 bytes and 288-byte frame at both load addresses. Two additional
canonical wrappers reproduce all four complete images, adding four C
instances / 5,536 bytes. Existing shared types, source expressions and
compiler profiles are unchanged.

Entry initializes three 416-byte narrow webs at context `0xA80..0xF60`.
Each has two 4-by-6 SVECTOR grids at `0/0xC0`, color at `0x180` and scale
at `0x194`. Element/row/record advances of 8/48/416 and the six/four/three
bounds are independently verified. The helper projects four point arguments
with `RotTransPers4` and sorts the `GsGLINE` at context `0x19E4` for
nonnegative depth and flags.

Timing comes from the **selected 48-byte descriptor**, not a sheet or a
web-local timing record. Entry constructs module `0x31B4 + index * 48`
and stores its pointer at context `0x1A60`. Commands `599000/599001`
select descriptor 0/1, whose start/end words at `+0x1C/+0x20` are
`0/26` for model440 and `0/76` for model180. Both measured denominators
are positive. Phase zero uses unsigned progress division and one-third
web staggering; phase one stores negative `i * 8192 / 3` scales. Later
phases advance by `step << 8`, preserving wrapping, clamping and completion
conditions. Frame, step and phase are at `0x1A50/0x1A58/0x1A84`.

Sixty-five focused retail anchors and 24 target-compiled local/SDK constants
establish the accessed views. All four physical slices/commands, 36 resident
callee addresses, eight helper callees and three loader/controller owners
have independent evidence. The existing `0x80089928` binding is named
`ratan2`; no address is added. The direct context minimum `0x1A98` is
separated from measured loader ranges, not asserted to be allocation
capacity or global lifetime isolation.

This independent batch starts from accepted `1caacd139`, which supplied
the shared US body, without stacking the pending petal, Family445 webs or
Family402 strip batches. All 222 existing French module records are
preserved. Production object selections and sized ELF definitions confirm
16 C / 15,712 C bytes, 16 assembly / 34,160 assembly bytes, and eight raw
owners / 32,048 bytes. All 222 complete French images and the clean resident
passed unmasked exact matching, with fresh layouts and resident ownership
verification. All 130 French variant, eight Spanish Family433 and 16 progress
regressions passed without skips, along with metadata, basic-types,
external-attempts and G32 gates. Configured totals are 894/1,521 C instances
and 855,700 C bytes.

The Spanish fixture inherits this family's structural checks, so it
explicitly retains its original three-helper selection and empty
entry-reachable C set. Its unchanged legal images independently satisfy
the new raw anchors and descriptor checks; no Spanish source, inventory,
binding or compiler profile is promoted or modified. All remaining French
assembly functions and unclassified suffixes stay untranslated.

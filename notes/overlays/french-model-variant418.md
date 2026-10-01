# French MODEL headers 418 and 568

Two distinct images for model410 reuse the unchanged accepted
`src/overlays/model_variant/variant401_{spokes,rings,quad}.c` bodies through
six three-line wrappers. The existing `gcc_2_8_1_g0_split` profile uses
GCC 2.8.1 and MASPSX 2.81. Those three shared bodies, headers, G32 annotations and US
profiles are unchanged. US source family401 is provenance, not French
identity; it is unrelated to the older French model401/header432 renderer.
Entry-called sheets now use a standalone French body, and entry-called
webs reuse US Family416 with a measured compile-time context-tail offset.

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
| `0x1760..0x1C9C` | 1340 | sheets C | yes |
| `0x1C9C..0x2204` | 1384 | webs C | yes |
| `0x2204..0x272C` | 1320 | generated assembly | yes |
| `0x272C..0x2A3C` | 784 | spokes C | no |
| `0x2A3C..0x2DB8` | 892 | rings C | no |
| `0x2DB8..0x311C` | 868 | quad C | no |

Strict walks cover every instruction in each span with one terminal return
and no unresolved indirect transfer. Entry reaches the first five functions;
the three retained spoke/ring/quad helpers have no demonstrated entry
execution path. Sheets and webs are entry-called. Real storage owners preserve each four-byte header
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

## Entry-called sheets and phased webs

The first independently measured candidates reproduce both 1,340-byte
sheets with 264-byte frames and both 1,384-byte webs with 288-byte frames.
Both complete unmasked images match. Current canonical links freshly select
the two new helpers and three retained C objects in each image: ten actual
C owners / 10,536 bytes, preserving all six old owners / 5,088 bytes.
The addition is four C instances / 5,448 bytes. Six assembly owners /
14,600 bytes and four raw owners / 15,824 bytes remain untranslated.
The 7,908-byte suffixes remain explicitly unclassified.

### Single companion record and two sheets

One **636-byte** companion record occupies context `0xF60..0x11DC`.
Entry starts its outer counter at zero, increments it once and repeats
only while nonpositive, advancing by `0x27C`. An inner nine-iteration loop
initializes consecutive signed words at `+0x1D4..+0x1F4`. These are not two
separate companion records. A pointer-only view avoids guessing the rest
of the record. Two 152-byte sheets follow at `0x11DC..0x130C`.

The first sheet reads companion `+0x1F4`; the second reads `+0x1D4`.
Negative interpolation factors clamp to zero, with **no upper clamp**.
Both translate from context words `0x1AA0/4/8` plus the corresponding
`0x1AB4/8/C` deltas times the factor divided by `0x400`, preserving signed
rounding. Only the second sheet applies odd-frame `size / 8` bias.
Four quads per sheet reuse `POLY_GT4` at `0x19B0`, with the measured
matrix sequence, inner/outer colors, depth `*8/10`, nonnegative clipping
tests and the SDK's low-16-bit ordering index.

After rendering, the first sheet sets scale `0x800` and clears it when
phase is at least three. The second uses unsigned selected-descriptor
growth during phase zero, clamps to `0x1000` and enters phase one.
The subsequent signed phase-less-than-two branch fixes scale `0x1000`;
phase two grows by `step << 12` to `0x8000`, and phase three shrinks by
`step << 8`, clamping at zero and entering phase four. An already-zero
scale does not take the latter transition. Frame parity, animation frame,
step, descriptor pointer and phase are at
`0x1AE0/0x1AE4/0x1AEC/0x1AF4/0x1B18`.

### Shared web body and preserved default users

Three 416-byte narrow webs occupy `0xA80..0xF60`. Both 4-by-6 SVECTOR
grids, color and scale reuse the existing local `ModelVariantWebNarrow`.
The measured web control flow matches accepted US416 structure; only the
context tail is `0x94` later. `VERSION_FRENCH` selects this constant without
changing the arithmetic or phase control flow of `variant416_webs.c`.
French433 deliberately uses the default layout, as do the US416 users. The shared header and all
existing compiler-profile names and flags remain unchanged.

Fresh normal production builds reproduce all four default-layout US416
images and all four default-layout French433 images, checking every one
of their 32 existing C owners in selected objects and sized final ELF
sections. The US web functions remain 1,388 bytes under their unchanged
legacy profile; French web functions remain 1,384 bytes under
`gcc_2_8_1_g0_split`. No US inventory or profile is promoted or altered.

The ledger retains six historical terminal rows, two explicit-offset web
calibrations and two shared-body calibrations, followed by four new
canonical terminal matches. All calibrations have zero word differences;
the intermediate `text_exact` rows also reproduced both complete scratch
images but are not the selected canonical sources.

### Descriptor, callers and validation

Actual command `584002` selects the third 48-byte descriptor at module
`0x3218 + 2 * 48 = 0x3278`. Both images contain timing words
`30/110/120` at `+0x1C/+0x20/+0x24`, so the measured growth denominator
is 80. Entry calls sheets at `0xE3C`, then webs at `0xE44`, passing the
original context in both delay slots. Their shared gate is unsigned frame
at least descriptor `+0x1C`; it is not the French433 sheet gate.

Forty-nine freshly target-compiled constants and 210 retail anchors,
plus the three counter-initialization checks, establish the accessed
views. The eleven distinct helper callees, 36 resident binding addresses
and three resident caller owners are independently verified. The existing
`0x80089928` alias becomes `ratan2`, without changing its address.
Direct context extent `0x1B2C` is an accessed minimum, not allocation
capacity or a lifetime-isolation claim.

The original 43 common initialization anchors remain a separate baseline
for French433's shifted initializer checks. New Family418 anchors do not
propagate into that different image layout. Spanish418 keeps its original
three C helpers and empty entry-reachable C set; its legal images satisfy
the inherited raw-layout and descriptor checks. Spanish433 is also preserved.

This independent batch starts from accepted `b4799e7a`, excluding the
then-pending French433 sheet branch. The focused 49-regression gate passes.
Configured totals become 1,146/1,581 French C instances / 1,282,148 bytes.
Family418 production acceptance passed: all 252 complete French images and
the clean French resident match without masking. Fresh selected objects and
sized defining ELF symbols prove ten C owners / 10,536 bytes, preserving
all six prior C owners / 5,088 bytes. Six assembly owners / 14,600 bytes
and four real raw owners / 15,824 bytes remain; neither suffix is classified
as non-code. The four new C instances contribute 5,448 bytes.

All 214 French, 115 Spanish and 21 progress/toolchain regressions pass
without skips, along with repository policies. The source-fingerprinted
eight-image default-user proof retains all 32 existing C owners.
Accepted families 421, 431, 433, 476, 422, 435, 439, 445 and 465 retain
their actual C owners. All US and Spanish configuration, shared headers
and existing compiler profiles remain unchanged.

The ordinary reconciliation of accepted French433 sheets at
`18c08573e011b7bbb70548c4e45411792945cf2f` retains their four additional
C owners. Combined configured totals are 1,150/1,581 French C instances
and 1,286,580 bytes. The accepted French433 sheet fixture keeps its own
two-companion layout while using the isolated common initializer baseline.
Fresh combined production acceptance passed: all 252 complete French
images and the clean resident match, with 215 French, 116 Spanish and
21 progress/toolchain regressions and all repository policies passing
without skips. The final linked images preserve all twenty accepted
French433 C owners / 20,144 bytes, all ten Family418 C owners, and all
36 current default-layout US416/French433 C owners. Eighteen of the
21 original authored paths remain byte-identical to the independent
checkpoint; only this note, the progress fixture and the reconciled
French433 fixture differ. General progress snapshots remain separate.

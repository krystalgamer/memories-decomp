# French MODEL headers 442 and 592

Four distinct secondary images reuse the unchanged accepted
`src/overlays/model_variant/variant425_{bands,spokes,rings,quad}.c` bodies
through eight three-line French symbol-renaming wrappers. The existing
`gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 pipeline is authoritative.
Shared bodies, headers, G32 annotations and US profiles remain unchanged.
US header 425 is source provenance, not French byte-identity evidence.

## Loader and boundaries

Models 259 and 630 (compact records 259 and 580) use stages 7/8. Record
sectors 180/190 in 276-sector compact records select ten-sector images at
`0x8013B000`/`0x8017B000`. The resident controller calls image `+4` with
context and initial command or update `-1`. The
[instance ledger](french-model-variant442-instances.csv) records actual
nonnegative commands, slices and four distinct complete hashes. Other
stages and models are not covered.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1174` | 4464 | generated assembly | yes |
| `0x1174..0x1560` | 1004 | generated assembly | yes |
| `0x1560..0x1F2C` | 2508 | generated assembly | yes |
| `0x1F2C..0x2924` | 2552 | generated assembly | yes |
| `0x2924..0x2CE8` | 964 | generated assembly | yes |
| `0x2CE8..0x3334` | 1612 | generated assembly | no |
| `0x3334..0x3A40` | 1804 | bands C | no |
| `0x3A40..0x3D50` | 784 | spokes C | no |
| `0x3D50..0x40CC` | 892 | rings C | no |
| `0x40CC..0x4430` | 868 | quad C | no |

Strict walks cover all ten functions with one terminal return per span and
no unresolved indirect transfer. Entry reaches only the first five
functions. All four C helpers remain retained module-local code without
a demonstrated entry execution path. Real storage owners preserve each
four-byte header and 3,024-byte suffix at `0x4430..0x5000`; the suffix is
explicitly unclassified, not established non-code.

## Accessed layouts and ownership

Entry captures `a0 -> s3 -> s6`. A single 456-byte `ModelVariantBand` begins
at context `+0x1CA0`, followed by one 152-byte sheet at `+0x1E68`, six
144-byte rings at `+0x1F00`, four 144-byte spoke records at `+0x2260`,
and one 144-byte quad at `+0x24A0`. Adjacent extents agree exactly. This is
the 456-byte band, not header435's 492-byte padded form.

Forty-four entry anchors check saved pointers, initialization counters,
increments, bounds, strides and cleared rotation. The band counter starts
zero and repeats only while nonpositive after its increment, establishing
one record. Its color rows at 324/360 each contain nine four-byte entries.
The quad loop also initializes one record; rings/spokes use bounds six/four.
These are accessed views, not whole-context allocation or extra execution
path evidence.

Ninety-five local layout constants are independently target-compiled for
this family. They cover band/sheet/ring/spoke/quad records and the SDK views
listed in the [companion422 evidence](french-model-variant422.md), including
four-byte target `long` and stored-pointer-bearing `GsCOORDINATE2`. The band
has point rows at 0/72/144, screen rows at 216/252/288 and depths at 420..452.

## Exact matching and preservation

The [attempt ledger](french-model-variant442-attempts.csv) records eight
terminal canonical-wrapper matches after fresh current-header compilation.
Rebased candidate searches are not the acceptance gate: all four real
canonical links reproduce unmasked complete images. Thirty-six resident
bindings are independently checked; the named `RotTransPers3` address comes
from the accepted French Exodia manifest.

The combined422/442 production gate preserves all 144 previously accepted
registrations, reproduces all 152 French images and matches the clean
French resident. This family adds 16 C owners and 17,392 bytes, alongside
24 assembly owners, eight raw owners, 36 fresh-resident callee owners and
95 recompiled layouts. The 52,416 assembly bytes and 12,096 unclassified
suffix bytes remain untranslated.

Five inherited regressions check actual archive slices, commands, complete
hashes, canonical sources/fingerprints, raw extents, all ten control-flow
spans and 44 entry anchors. Together the two families add eight images,
32 C instances and 34,784 C bytes. Configured totals become 152 images,
668/1105 matching C instances and 602,540 C bytes, without claiming
exhaustive runtime coverage or seven-release completion. General progress
snapshots are separate.

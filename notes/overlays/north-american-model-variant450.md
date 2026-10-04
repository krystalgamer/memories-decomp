# North American MODEL450 quad and line helpers

North American model 174 stages 9 and 10 contain two distinct 20,480-byte
images at sectors 48224 and 48234. Their seven closed function extents are
`4..DDC`, `DDC..1770`, `1770..1E7C`, `1E7C..2754`, `2754..2D20`,
`2D20..30A8`, and `30A8..38DC`. The four-byte header and complete
`38DC..5000` suffix retain raw owners; the suffix remains unclassified.

The accepted Spanish quad and line bodies compile to the North American
instruction streams when selected with the existing `gcc_2_7_2_cdk_g0`
profile. Region-specific wrappers only rename the functions:

| Offset | Bytes | Matching instances | Selected body |
| --- | ---: | ---: | --- |
| `0x2754..0x2D20` | 1,484 | 2 | `spanish_model_variant/variant450_quads.c` |
| `0x2D20..0x30A8` | 904 | 2 | `spanish_model_variant/variant450_lines.c` |

The four resulting objects and linked symbol extents match independently.
Both complete images then reproduce their retail SHA-256 values. This adds
four matching C instances / 4,776 instruction bytes while retaining ten
functions / 24,328 bytes as generated assembly and 11,856 bytes as raw data.
The [attempt ledger](north-american-model-variant450-attempts.csv) records
the object and production checks; the
[instance table](north-american-model-variant450-instances.csv) records the
loader slices and complete-image hashes.

The two helpers import twelve distinct resident functions. Each binding was
recovered by pairing the C-object relocation with the corresponding retail
JAL, and every destination is an actual North American resident function
start. The shared `model_variant_linker_symbols.txt` already contains those
canonical bindings; the per-image Splat symbol files retain the exact local
function boundaries and regional addresses.

This is exact ownership evidence, not a claim that the North American and PAL
executables share a compiler globally. The five remaining function ordinals
still require independent recovery. Reusing accepted bodies also does not
extend the bounded structure, allocation, lifetime, geometry, or reachability
claims documented by the Spanish recoveries.

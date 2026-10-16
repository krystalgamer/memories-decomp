# North American MODEL450 helpers

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
executables share a compiler globally. The three remaining function ordinals
still require independent recovery. Reusing accepted bodies also does not
extend the bounded structure, allocation, lifetime, geometry, or reachability
claims documented by the Spanish recoveries.

## Ribbons and bands

The `DDC..1770` ribbons and `1770..1E7C` bands helpers are written for the
North American instruction streams. They are the French header-450 ribbons and
bands (`french_model_variant/variant450_ribbons.c` and `variant450_bands.c`)
with the retail register allocation put back, so they are separate sources
with their own per-header state types in `variant433_ribbons.h` and
`variant433_bands.h` rather than wrappers. `gcc_2_7_2_cdk_g0` compiles the
French bodies 28 and 32 bytes too large, and with a different register
allocation.

What the retail allocation needs, in both helpers:

| Source form | Effect |
| --- | --- |
| `off` is assigned in both arms of the `j == 8` test, with a dead `0` in the `j == 8` arm and `i * 9` in the other | The global allocator keeps the sign-extended band number in a saved register and the projection address spills, as in retail. A single assignment lets the loop pass hoist `i * 36` out of the inner loop. |
| `[8]` instead of `[j]` throughout the `j == 8` arm | `j` is known to be 8 there, and the compiler otherwise hoists a second base register for it. |
| `points[j + 1]` and `screen[j + 1]` plain in the ribbons' projection call, with `(&x[j])[1]` elsewhere | The `+ 8` is folded into the displacement only where retail folds it. |
| The first quad corner read through `(u8 *)screen + j * 4` | Index first, as the retail `addu` has it. |

The linked 0x994 and 0x70C extents match, and both complete images reproduce
their retail SHA-256 values. This adds four matching C instances / 8,512
instruction bytes. Six functions / 15,816 bytes remain as generated assembly:
the `4..DDC` entry, `1E7C..2754` and `30A8..38DC`. The raw data is unchanged.

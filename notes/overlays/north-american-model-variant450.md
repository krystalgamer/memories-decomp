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
instruction bytes.

## Rings

The `1E7C..2754` rings helper (`variant433_rings.c`) has no PAL counterpart
yet: the French header-450 function at the same position is still assembly.
It is written for the North American stream and grows sixteen nine-point
rings. Each is projected with RotTransPers4 and drawn as quads from the
projected edges. What the retail code needs:

| Source form | Effect |
| --- | --- |
| Screen `y` read as `(packed >> 16)` from the 32-bit screen word | Gives retail's `lh` with one instruction per read. A named `s32` read also gives `lh` but adds two instructions to the inner loop each, and the loop pass then no longer hoists `i * 36` out of the quad loop (its movable cost test passes with exactly 92 instructions). |
| Ring points stored through `(SVECTOR *)(j * 8 + (s32)ring)` | Index first in the address `addu`, as retail has it. |
| `ot` declared before `i`, and `ang2` after it | The spilled `ot` and `ang2` take the `796` and `808` stack slots in retail's order. |
| `ring->amount[j] = limit;` with `limit` set once to `0x400`, written a second time at the top of the then-arm | The constant is rematerialized into the reload register as retail does. Reload's post-pass deletes the repeated store, but the register copy it made for it stays and fills the `j == 8` branch's delay slot, as in retail. |

The linked 0x8D8 extent matches, and both complete images reproduce their
retail SHA-256 values. This adds two matching C instances / 4,528 instruction
bytes.

## Coils

The `30A8..38DC` coils helper (`variant433_coils.c`) is the French header-450
coils (`french_model_variant/variant450_coils.c`) with the retail allocation
put back. It uses that file's state types through `variant433_coils.h`.
`gcc_2_7_2_cdk_g0` compiles the French body 14 instructions too large. What
the retail code needs:

| Source form | Effect |
| --- | --- |
| No `previous` pointer: the last segment reads `&coil->screen[15]` | The compiler keeps no second walking pointer, and the projection addresses hang off the `edge_screen[16]` base register as in retail. |
| `[16]` instead of `[j]` throughout the `j == 16` arm | The same constant-index rule as the bands. |
| `(&coil->screen[j])[1]` for the neighbour in `dx`/`dy` and in the quad corners, the first corner through `(u8 *)coil->screen + j * 4` | The `+ 4` is folded into the displacement, with the index first as in the bands. |
| `work->rotation[2] += 0;` after `work->rotation[1] += 32;` | Dead. Reload's post-pass deletes the store, but the retail code's load of `rotation[2]` in front of the matrix setup stays. |

The linked 0x834 extent matches, and both complete images reproduce their
retail SHA-256 values. This adds two matching C instances / 4,200 instruction
bytes. Two functions / 7,088 bytes remain as generated assembly: the `4..DDC`
entry of each image. The raw data is unchanged.

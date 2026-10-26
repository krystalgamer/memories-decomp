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
executables share a compiler globally. Reusing accepted bodies also does not
extend the bounded structure, allocation, lifetime, geometry, or reachability
claims documented by the Spanish recoveries. The sections below record the
later recoveries of the other five functions.

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
bytes. The raw data is unchanged.

## Entry

The `4..DDC` entry (`variant433_entry.c`, 886 instructions) is written for the
North American stream. Built with `gcc_2_8_1_g0_split` and the PAL palette
x = 640, it is also the French header-450 entry
(`french_model_variant/variant450_entry.c`). On the
setup command it fills six fans, six ribbons, three bands, sixteen rings, seven
sheets and two streamers from the state at the work pointer, whose block arrays
are contiguous (`fans[6]` at `0x0`, `ribbons[6]` at `0x270`, `bands[3]` at
`0xF00`, `rings[16]` at `0x1518`, `sheets[7]` at `0x3598`, `streamers[2]` at
`0x39F8`, packets at `0x40E8`), reads its config (at `0xC4 + 0x38` in the data
block, 0x24 bytes per command, trigger at `+ 0xC`) and four image descriptors,
and builds the packets. Otherwise it tracks the part with the header-337 camera
code and runs the four draw helpers above the config's trigger frame. What the
retail code needs:

| Source form | Effect |
| --- | --- |
| One `packed` variable for all four texture words, with the first flats' tpage and clut extracted into locals before the fourth call | The four results share one stack slot, and the two extractions stay in front of the slot's second store. |
| The fan loop's counter is also the sheets loop's counter (`n`), and the sheets' inner index is a separate local (`k`) | The counter is spilled to one slot in both loops; a counter that is also the sheets' inner index has enough references to keep `s5`, and the fan address giv spills instead. |
| Fan fields written through `fan`, with `color.b` after the first three zero words and the `+ 0x64` word last; no tail pointer | The loop pass reduces the field addresses to one giv biased at the last store, `fan + 100` in `s4`, with the `+ 0x0` store left on the biv. The `255` store follows three word stores in retail; written before them, or with another word last, the function differs. |
| The extra packets' tpage held in `angle` | A fresh local is allocated before the shared `63` and takes `s2`, which costs `triangle` its register and adds a stack slot. |
| The ribbons loop as `for (i = 0, angle = 0, angle2 = 0; i < 6; angle += 1300, angle2 += 800)` with `i++; ribbon++;` ending the body | The post-reload scheduler fills load-delay slots with the free increments in source order. |
| Ribbon phases as `-(j << 9) / 8 + i * -512` | The inner loop hoists `i * -512` as a temporary (shift and negate), the outer loop reduces it to a giv initialized after the hoisted constants and copies it into `a0`; `- (i << 9)` hoists only the shift, which is not worth a giv. |
| Bands: `outer = i << 9` before the amount loop, and `dx = i << 8` in the odd arm of the heading | Two shifts of `i` in separate arms are identical givs and get combined and reduced; a value with other assignments is not a giv, and the lone remaining one is not worth reducing. |
| Rings: `-(j << 7) - (i << 9)` | The giv's initial value is emitted after the hoisted `i << 9`. |
| Sheets: `outer[0..2]`, `inner[2]`, `inner[0..1]`, `field_98`, `field_9C`, `size`, `field_8C`, `field_90`, in that order | The fourth `255` is written to `inner[2]`, not `outer[3]`, and the address giv is biased at `field_90`. |
| `work->config->trigger <= work->frame` | The trigger is loaded first. |
| The two `MATRIX` locals are never used | They hold the frame at 216 bytes. |

The linked 0xDD8 extents match, and both complete images reproduce their
retail SHA-256 values. This adds two matching C instances / 7,088 instruction
bytes; every function of both images is now C. The raw data is unchanged.

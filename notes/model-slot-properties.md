# Model slot property interfaces

The six-function `model_slot_properties.c` group remains one contiguous
`gcc_2_8_1_g0` object, from resident `0x800593D0` through `0x8005969C`.
The common NA/JP source and European wrapper keep their original definition
order, types, layouts, native addresses and profiles. These names describe
local SDK-backed behavior, not a new property representation.

## Part vertex transformation

`Model_TransformPartVertex` is the former `func_800593D0`. It selects a
slot, part and vertex, then calls `PushMatrix`, `GsGetLwUnit`,
`GsSetLsMatrix`, `RotTrans` and `PopMatrix`. The SDK declarations establish
the `SVECTOR` input, `VECTOR` output, `MATRIX` scratch and `PSXLONG` flags.
The function returns the first flag word. The name does not claim screen
projection or an observed gameplay coordinate system.

The matching MODEL418 and French MODEL338/393 callers copy the output
components into matrix translation. All eleven native overlay import
bindings follow the same identity; French and Spanish imports still use
`0x8005C4D8`. The two existing French control files change that identifier
only, not values, stubs, assertions or execution paths.

Thirty historical overlay caller bodies remain byte-for-byte unchanged.
Three explicit identifier bridges in `model_slot_properties.h` let their
frozen numeric spellings emit the canonical resident symbols without
rewriting terminal attempt fingerprints or shared-source dependency
ledgers. These are preprocessor spellings, not extra linker aliases,
declarations, functions or storage. Their existing include paths stay.

The part lookup deliberately remains an integer sum. A typed indexed
`GsUNIT` expression folds the extra eight bytes into a load and loses an
instruction required by retail. There is no new bounds check, result
interpretation, declaration adapter or compiler flag.

## Slot scale quad

`Model_GetSlotScale` (`0x8005949C`) returns the `render_scale` interior
`ModelSlotS32Quad` through `gModel_Slot0Scale` and the original slot stride.
`Model_DrawSlot` passes that same member as a `VECTOR` view to
`ScaleMatrix` or `Model_UpdateCoordinateUnitMatrix`; this establishes its
scale role without guessing a getter caller or UI context.

The getter has no static caller. A runtime trace under #418 (interpreter
CPU, 2,448 control hits) saw no call while visiting the main menu, the
Library including a monster's 3-D view, and Build Deck; a later trace of
two 3-D battles did not watch it. Later watch traces also saw no call on the
idle title screen, in the Library 3-D view, or on the duel board; see
[`trace-uncalled-functions.md`](trace-uncalled-functions.md).

`Model_SetSlotScale` (`0x800594C0`) retains the whole four-word copy for a
nonnull source. Null writes `MODEL_FIXED_ONE` to offsets eight, four and
zero, in that order, and leaves the fourth word untouched. Neither API is
retyped to `VECTOR`, and the alias is not replaced with typed indexing.

The unreferenced-helper trace still seeks the getter's actual callers.
Its numeric `slot-quad func_8005949C` runtime label, breakpoint, address
formula, observations and existing controls remain unchanged. Naming the
statically proven scale view is not evidence that a call was observed.

## Cached coordinate-origin dimensions

`ModelSlot.field_DC8` is now `cached_dimensions`, retaining `u16[4]` at
+0xDC8. It is distinct from the adjacent tint bytes and is not a tight
mesh AABB, radius vector or universally current model size.

After the original part-mask early return, `Model_DrawSlot` updates it
only for slots below two with a nonnull render-parent coordinate. Its
second unit walk excludes the root and render-parent coordinates, keeps
the original primitive-list tests and uses eligible `workm.t` origins,
not mesh vertices. The signed 16-bit min/max cursors and their initial
sentinels remain, including the behavior when no origin qualifies.

The first three raw spans are max-minus-min along X/Y/Z. BEFORE adding
the 0x32 margin, the renderer compares raw X then raw Z against the
signed view of word three and stores through its original u16 type.
That word is separate horizontal history; Y and the margin are absent.
No nonnegative or monotonic invariant survives arbitrary narrowing,
signed observations or resets.

For each first-three span, the renderer adds 0x32 and either stores it
directly when the old signed value is zero or moves the existing value
toward it by at most 0x14. Preserve each test, signed read, per-axis
update order and original u16 truncation. Do not replace the loop with
an exact bound, clamp empty inputs or call it a per-frame timer.
The part-mask, slot and coordinate gates can leave these values stale.

`Model_InitSlot` clears all four words before its null-HMD return.
`Model_SeekSlotAnimation` clears ONLY word three after its existing
sequence work, retaining the smoothed X/Y/Z values. View-separation
setup reads all four through signed halfword views, divides by two and
applies its own minimum; its fourth-word use remains distinct.

`Model_GetSlotDimensionsWithMinimums` copies the original eight bytes,
then floors each axis against a nonzero CF8 threshold times sixteen.
`Model_GetSlotDimensionsWithOverrides` instead REPLACES an axis for a
nonzero threshold, even if the replacement is smaller. Both recompute
the returned maximum from X/Y/Z in their existing order, rather than
exporting the stored horizontal-history word unchanged. Preserve the
different typed and alignment-one block-copy views, pointer ABIs,
signed comparisons and policies; do not unify the two helpers.

The paired +0xDB0 member is now `render_scale`, still the four-word
`ModelSlotS32Quad`. Its existing VECTOR views apply only to the original
selected render-parent children and animation/mask/primitive paths.
The scale and cached dimensions are related draw inputs/derived state,
not reciprocal units or a promise that every coordinate was scaled.
No SDK type, generic quad submember, function group, native alias,
profile, fixture or runtime capture changes.

## Tint descriptor and ambient triplet

`Model_GetSlotTintDescriptor` (`0x80059520`) keeps its raw `u8 *` return.
It chooses `tint` or `tint + 4` only when the original remainder
of byte seven modulo six is nonzero and byte three is zero. The renderer
uses these target/transitional color-and-mode blocks, model primary
overlays copy a `CVECTOR` view, and buffer-space accounting reads byte
three, treating mode five specially. The result is not merely an RGB
triplet, a new allocation or a normalized color.

`Model_SetSlotAmbient` (`0x800595C8`) writes the three `ambient_rgb_scale` words
consumed directly by `GsSetAmbient` in `Model_DrawSlot` and
`Model_DrawBackground`. Each signed input is independently clamped to
`[-4096, 12288]` and stored before the next component is processed.
Mode four remains untouched; otherwise three equal `2048` values select
mode zero, and any other triplet selects mode three. The existing negative
one triplet also participates in draw-skip gates. This does not turn the
setter into a visibility API or change that sentinel behavior.

All nine descriptor and 116 ambient overlay imports keep their original
native addresses, including declarations unused by a particular matching
entry. Caller casts, argument values and sequencing remain unchanged.
The ambient identity does not imply that an overlay's locally named tint,
level or brightness field should be renamed or retyped.

The per-image Splat symbol maps must agree with those linker imports,
including unmatched assembly fallbacks. Exactly 1009 existing maps
canonicalize 201 transform and 1009 ambient absolute function labels;
every address, type/absolute attribute, local symbol, size and ordering is
unchanged. Updating only shared linker imports leaves fallback assembly
calling the old labels and fails the existing binding-coverage controls.

## Native resident identities

| API | NA | Japan | UK | Continental Europe |
|---|---|---|---|---|
| `Model_TransformPartVertex` | `0x800593D0` | `0x8005C200` | `0x8005D894` | `0x8005C4D8` |
| `Model_GetSlotScale` | `0x8005949C` | `0x8005C2CC` | `0x8005D960` | `0x8005C5A4` |
| `Model_SetSlotScale` | `0x800594C0` | `0x8005C2F0` | `0x8005D984` | `0x8005C5C8` |
| `Model_GetSlotTintDescriptor` | `0x80059520` | `0x8005C350` | `0x8005D9E4` | `0x8005C628` |
| `Model_SetSlotAmbient` | `0x800595C8` | `0x8005C3F8` | `0x8005DA8C` | `0x8005C6D0` |

All seven resident manifests retain the same six-function group and G0
profile, including the already named `Model_SetSlotTintTarget` between the
descriptor getter and ambient setter. The later member names `render_scale`
and `cached_dimensions` preserve their representations; the native interior
data alias and generic quad submembers remain unchanged.

# Model slot property interfaces

The six-function `model_slot_support.c` group remains one contiguous
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

## Cached slot positions and four-halfword copy

`ModelSlot.field_DD0` is now `cached_position`, still `s16[4]` at
+0xDD0. `Model_CopySlotPosition` is the former `Model_CopySlotU16Values`
at resident 0x80058FB0. Its ABI is still `(s32, u16 *)`: it copies ALL
FOUR halfwords in 0/1/2/3 order, preserving their bit patterns. This is
not a three-component copy or permission to retype storage as SVECTOR,
clear its fourth word, normalize signedness or change caller extents.

Before the HMD test, `Model_InitSlot` writes X=0, Y=-350, word three=-350
and Z=450/-450/0 for slot zero/one/other, in the original store order.
`Model_ResetSlot` still clears the entire record independently.

After the part-mask early return, `Model_DrawSlot` refreshes the quad
only for slots below two with a render-parent coordinate. Its FIRST
unit-origin pass averages workm.t X/Y/Z from nonnull coordinates with
nonnull eligible primitive lists. Unlike the later dimension pass, it
does NOT exclude root or render-parent units. A zero count falls back
to the render-parent workm.t; no mesh vertices or tight-box center is
involved. Original sums, division order and s16 stores remain.

Signed CF8 bytes four/five/six provide offsets multiplied by sixteen.
X/Z use Model_MirrorOffset: add for index <=0 and subtract for index >0;
Y adds directly. The separate second pass averages Y only for eligible
direct children of the selected render parent, excluding root and parent,
and falls back to the parent's workm.t Y when empty. Its count is taken
BEFORE the primitive-list content test used by the dimension extrema.
Word three remains `(y - 700) / 3 + signed_CF8_byte5 * 16`, with original
integer division and narrowing. It is not a homogeneous coordinate or
padding, and no universal camera-target interpretation is asserted.

Camera orbit setup, endpoint capture/following, eye/target distances and
view separation use this position chain. Animation-sound spatialization
also reads its X/Z without changing any cue gate. Effect placement uses its XYZ
through existing local SDK views; several handlers replace Y with zero.
The Library copies all eight bytes to its original literal work arenas
and reads XYZ through its existing local SVECTOR view. Those consumer
views do not redefine the shared storage or establish freshness, range,
ownership, readiness, units, exact model origin or geometric-centroid
invariants. Existing gates can leave the cache stale.

The gModel_awSlot0Position interior alias and independent byte/halfword
stride constructions remain unchanged. The new canonical header keeps
one explicit old-name preprocessor bridge, and the old include path
forwards to it. Frozen overlay C bodies, terminal fingerprints and all
attempt/dependency ledgers remain byte-for-byte untouched. Native import
tables use only the canonical name at their original addresses.
Existing fixture edits are identifiers/include spellings only; every
oracle, fourth-halfword guard, assertion, mutation, scenario and numeric
expectation remains unchanged. No fixture or cloud tests ran locally.

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

## Coordinate construction and vertex-transform local views

The two complete camera-relative builder and part-vertex transformer
definitions now name only their existing local views. view_angles and
packed_view_angles remain TWO eight-byte SVECTORs: zero the first, set
its yaw-derived Y and pitch-derived Z, then copy the WHOLE record.
Keep pad bytes, full-turn subtraction/narrowing and repeated selected
slot root-coordinate angle reads. No slot-zero orientation substitution.

view_rotation remains MATRIX. RotMatrixZXY on the destination, RotMatrix_gte
on the packed angles and MulMatrix retain their order and original operands.
Keep destination flag clear, Z/Y/X translation clears and LAST parent
publication. No common Euler convention, new camera transform, pad
initialization policy or parent-lifetime repair is inferred.

The vertex helper retains separate slot, part_unit, primitive_words and
vertices views. part_unit STILL uses the signed INTEGER address sum with
part+1 and sizeof(GsUNIT), not a direct array subscript. primitive_words
retains the original primtop[1] load and vertices the subsequent word2
pointer load, both BEFORE the SDK matrix-stack calls. No bounds, primitive
schema, vertex count or null-pointer guarantee follows.

local_to_world is the existing MATRIX populated from entries+part with
the original GsCOORDUNIT stride, DISTINCT from the units[part+1] selection.
Keep PushMatrix/GetLw/SetLs/RotTrans/PopMatrix order and vertex indexing.
transform_flags remains PSXLONG[2], and the return is still its FIRST
element. The MATRIX and flags are not packed, widened or replaced with
new structs. No screen-projection, clipping or gameplay-space claim.

Only scoped bare locals in these TWO complete definitions and ONE matching
identifier in the existing integer-sum comment change. Arguments, member
tokens, types, constants, expressions, scopes and order remain. All other
source bytes, headers, SDK declarations, callers, native imports, profiles
and data stay. The current manifest selects the WHOLE twenty-function
gcc_2_8_1_g8_split support group; the earlier six-function/G0 wording above
records pre-grouping evidence, not a new profile selection by this pass.

The existing extent witness extracts ONLY the unchanged four-halfword
copier. The background owner witness still compiles the WHOLE source
and checks its unchanged projection-header include and owners. French
338/393 bindings and all controls/oracles/assertions/flags remain exact.
No fixture was imported or executed locally and no new harness is added.

## Signed metadata offsets in the position cache

The metadata prefix now has a FOURTH union arm, position_offsets:
signed bytes x/y/z at relative +4/+5/+6 (slot +0xCFC/+0xCFD/+0xCFE).
Its explicit padding keeps this view twelve bytes. The original byte,
comparison-value and threshold arms retain their exact order and bounds;
bytes remains FIRST, including for any positional aggregate initialization.
No storage, larger union, alignment, packed attribute or new SDK type is added.

These are consumer-backed cached-position offsets, not a recovered asset
producer schema or proof of a mesh center, world units or current bounds.
After the original slot/parent/part-mask gates, the first origin walk
averages eligible workm translations or uses its existing parent fallback.
X/Z still call Model_MirrorOffset with the selected slot index and the
signed metadata byte multiplied by 0x10. Y still adds its signed byte
times 0x10 directly. Keep each s16 cache store and X/Y/Z evaluation order.

The second walk still has DIFFERENT root/parent/content/count gates and
its own Y average/fallback. cached_position[3] remains
(y - 700) / 3 + signed_Y_offset * 0x10, preserving division, its SECOND
Y-byte read and original narrowing. It is not padding, homogeneous W,
an exact center or an always-fresh camera target.

Only FOUR existing blk[4/5/6] reads in the COMPLETE draw definition acquire
qualified position_offsets fields through the existing blk pointer.
blk stays s8*, initialized at the same metadata address and kept live
over the same scope. The new fields stay s8: negative byte values do not
become unsigned thresholds. Multipliers, casts elsewhere, helper calls,
mixed-role scratch bindings and every other source byte remain unchanged.

The prefix's existing size guard and new member/arm layout guard preserve
the twelve-byte view and three byte offsets/widths. The separate 28-byte
metadata/word-copy views, full CF8 tail, alias relocation and SDK layouts
are not retyped. Existing minimum/override queries keep their unsigned
threshold views and distinct policies. All fixtures, oracles, mutations,
scenarios, numeric expectations and flags remain byte-for-byte unchanged;
none was imported/executed and no new behavioral harness is introduced.


## Dimension floors and coordinate/orbit argument views

Three COMPLETE definitions in model_slot_control.c name their existing
dimension-floor locals and coordinate/orbit definition parameters. Public
headers/prototypes, including parameter labels, remain byte-identical.
No new field, helper, layout, validation, supported domain or caller is
introduced. The cached-metric producer and inline matrix helpers stay
unchanged; no retired gameplay witness or new behavioral harness is added.

Model_GetSlotDimensionsWithMinimums distinguishes dimension_slot from its
metadata_prefix and the separate minimum_x/minimum_y/minimum_z block locals.
These are THREE lexical bindings, not a shared or hoisted scratch value.
Preserve the complete eight-byte copy, u16 storage versus signed output
view, nonzero byte tests, shift by four, signed comparisons, narrowing and
X/Y/Z order. The copied fourth word is still replaced by the original
nonnegative maximum computation; no new cache-freshness or mesh-bound claim.
The separate override helper and all frozen overlay bodies stay unchanged.

Model_BuildViewRelativeCoordinateUnit names entry_index, local_position
and output_coordinate. entry_index is still mutable: ONLY the original
strict entry_count < entry_index test selects root_coordinate_index.
No equality, negative-index or range guard is added. Preserve the integer
80-byte entry stride, GsGetLwUnit/GsSetLsMatrix/RotTransSV sequence, existing
SVECTOR/MATRIX objects, byte-backed flag scratch and every SDK boundary.
Translation still uses the selected slot while rotation repeatedly reads
slot ZERO through the existing volatile views. Do not conflate the slots,
claim an anatomical entry, normalize orientation or cache the root reads.
Keep declaration/stack order, inline helpers, Z/Y/X translation clears and
the output super pointer to the AUTOMATIC translation_parent. Its legacy
lifetime-sensitive behavior is preserved, not repaired or made owning.

Model_StepViewOrbit names direction_scale and yaw/pitch/distance deltas.
direction_scale remains BOTH the positive-versus-nonpositive pivot choice
and the FULL signed position multiplier, not a Boolean or normalized sign.
Preserve the interior reference-point alias, triple arithmetic and all
reused trigonometric/angle/horizontal-or-X bindings. The distance pretest
still uses a signed halfword, while the subsequent update reloads the
unsigned halfword and narrows on store; there is no universal clamp.
Masked angle-update gates remain separate from RAW nonzero recomputation
gates. Preserve signed divisions/products, angle reduction and X/Y/Z stores.
The guarded unused fifth CALL-SITE argument contract remains unchanged;
the definition still reads exactly its original four parameters.

All three whole definitions invert byte-for-byte under scoped bare
identifiers. All member tokens, types, declarations/order/lifetimes,
constants, operators, labels, calls, macros, existing comments and other
source bytes remain. The full grouped objects keep their other functions,
switch/private data, named profiles, storage and object order.

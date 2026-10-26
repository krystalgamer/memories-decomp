# Model Shared Structures

This phase is limited to the shared model storage rooted at `D_800F2C40` and
the handler registry at `D_800F5918`. `src/game/model.h` records only locally
verified widths, offsets, strides, and extents. Offset-based names remain
where semantics, signedness, array bounds, or pointee layouts are uncertain.
GMS and the Unchiga decompilation/recompilation inventories were used as
corroborating evidence, not as sources of guessed reference types.

## Model-slot directional lights

The three records at slot offsets `0xD70`, `0xD80` and `0xD90` are now
`ModelSlot.flat_lights[3]`. The evidence is the complete local producer and
consumer chain, not resemblance to a vector or a reference declaration:

| Original member | Canonical member | Preserved representation |
| --- | --- | --- |
| `ModelSlot.field_D70` | `flat_lights` | three `0x10`-byte records at `+0xD70` |
| `ModelSlotLightEntry.field_00` | `direction_x` | `s32`, `+0` |
| `ModelSlotLightEntry.field_04` | `direction_y` | `s32`, `+4` |
| `ModelSlotLightEntry.field_08` | `direction_z` | `s32`, `+8` |
| `ModelSlotLightEntry.field_0C` | `rgb` | `u8[3]`, `+0xC` |

`Model_InitLightTriplet` seeds directions `(3000, 5000, 2000)`,
`(-3000, 5000, -2000)` and `(0, -5000, 0)`. Its first two RGB triplets
are `(128, 128, 128)` and its third is `(64, 64, 64)`.
`Model_DrawSlot` and `Model_DrawBackground` pass these exact records to
`GsSetFlatLight(0..2)`. The local SDK declaration names the corresponding
words `vx/vy/vz` and bytes `r/g/b`; compile-time checks retain their offsets
and sizes without importing the SDK header chain into `model.h`.

These are directional-light inputs, not point positions, Euler angles or
unit vectors. No coordinate-space, normalization, channel-clamping or
universal key/fill/rim assignment is inferred. The words stay signed, RGB
stays a three-byte array and `pad_0F` remains an uninterpreted byte.

`Model_GetFlatLight` retains its legacy byte-pointer return, unsigned
light-index fallback to zero for indices at least three, unchecked model
index and separately relocated `gModel_aSlot0FlatLights` interior alias
at `0x800F39B0`. It is not rewritten as a direct slot-member address.

The Library card-view path obtains lights two and zero through that
accessor. Its nine direction-word stores now use the same canonical names.
Its three packed color stores remain full `s32` writes at `+0xC`, `+0x1C`
and `+0x2C`, including each record's padding byte; replacing them with RGB
byte stores would change behavior. All computations, values, writes,
SDK-call order, render gates, native bindings and grouped owners remain
unchanged. Historical code-generation experiments below retain their
original offset spellings.

## `D_800F2C40`: three `0xE20`-byte slots

`ModelSlot` has size `0xE20`, and `D_800F2C40` is declared as three entries.
The stride is independently established by exact matching C and target
instructions in multiple functions:

- `Model_GetSlotAnimationSpeed`, `Model_GetSlotAnimationIndex`, and
  `Model_GetSlotAnimationFrame` all compile the index
  as `((index * 8 - index) * 16 + index) * 32`, which is `index * 0xE20`,
  before accessing three different late-slot fields.
- `Model_GetCurrentDataEntry`, `Model_CopySlotU16Values`, `func_80059284`,
  `Model_SetSlotTintTarget`, and `Model_SetSlotShadowEnabled` independently
  produce the same stride
  while accessing unrelated pointers, arrays, and bytes.
- `func_80057E20`, `func_80059000`,
  `Model_BuildCameraRelativeCoordinateUnit`, `Model_TransformPartVertex`, and
  `Model_SetSlotAmbient` provide further exact target sequences with the same
  multiplication. GMS describes the base as `dword_800F2C40[904 * index]`,
  independently giving `904 * 4 == 0xE20`.
- Unchiga's same-address decompilation sources consistently use `0xE20`;
  its recompilation seed inventory identifies the same function family.
  Local recompilation and full-executable matching remain decisive.

The extent is also locally bounded. Linker aliases `gModel_aSlot0FlatLights`,
`gModel_Slot0Scale`, and `gModel_awSlot0Position` are respectively `+0xD70`, `+0xDB0`, and
`+0xDD0` from the base, matching fields inside slot zero.
`gModel_bAuxiliaryActive` retains native label `D_800F569F` at
`gModel_aSlots + 2 * 0xE20 + 0xE1F`: slot 2's `is_active`, also the last
byte of the three-slot run. All seven actual providers agree with this
relation; `gModel_CameraCoordinateUnit` follows immediately. The interior
aliases retain their original native declarations and independent relocations.

Verified shared fields and partial arrays are:

| Offset | Shared member | Exact local evidence |
|---:|---|---|
| `0x000` | `hmd_units[60]`, `0x8`-byte `GsUNIT` views | `Model_InitSlot` fills the SDK `coord` and `primtop` views; `Model_TransformPartVertex` indexes `(arg1 + 1) * 8`; `Model_RunSlotHandlers` advances by 8 and reads `primitive_data` at `+4` |
| `0x1E0` | `animation_sequences[58]`, `ModelSlotPart *` (a `GsSEQ`) | `Model_SeekSlotAnimation` and `Model_SetSlotPartSpeeds` advance pointers by 4, bounded at runtime by `animation_sequence_count`; `Model_ResetSlotAnimationTables` bounds the array at 58 by filling the key table that pairs with it at a `0x74` stride |
| `0x2C8` | `animation_command_indices[10][58]`, `u16` | `Model_ResetSlotAnimationTables` fills it with `0xFFFF` at a `0x74` stride over ten rows; `Model_BuildSlotAnimationTables` indexes it `[row][part]`; `Model_ControlSlotAnimation` reaches it as `0x2C8 + current * 116 + part * 2` |
| `0x750` | `animation_duration_rows[10]`, `ModelSlotRow` | `Model_ResetSlotAnimationTables` zeroes the 58 halfwords at `0x750 + row * 0x76` and the halfword at `0x7C4 + row * 0x76` in one loop iteration, which is what groups them into one `0x76`-byte record; `Model_BuildSlotAnimationTables` leaves that halfword holding the largest of the 58, and `Model_GetSlotAnimationLength` reads it as `current_animation_row * 118` |
| `0xBEC` | `extended_animation_part_mask[8]`, part bitfield | `Model_ResetSlotAnimationTables` sets bit `part % 8` of byte `part / 8`; `Model_ControlSlotAnimation` reads it back the same way; `Model_LoadSlot` reserves 20 rather than 12 snapshot bytes for qualified flagged parts |
| `0xBF4` | `queued_animation_row` | end-of-animation request compared with its latch before seeking |
| `0xBF5` | `current_animation_row` | `Model_GetSlotAnimationIndex` returns it; `Model_GetSlotAnimationLength` uses it to select an animation row, while `Model_SeekSlotAnimation` stores an animation row after seeking each part to it |
| `0xBF6` | `queued_animation_row_latch` | acknowledged queue value; can be invalidated or copied without a seek |
| `0xBF8` | `sound_entries[64]` | `model_slot_control.c` clears 64 four-byte records; `Model_PlaySlotAnimationSounds` reads each record as `{animation_row, sound_id, flags}` |
| `0xCF8` | `field_CF8`, `0x1C`-byte mixed block | `func_80057E20` and `func_80059000` read threshold bytes `+7`, `+8`, and `+9`; `Model_StartBattleSlotAction` indexes the two halfwords at `+0xC`; four setup/transfer paths reset the signed words at `+0x10`, `+0x14`, and `+0x18` |
| `0xD14` | `field_D14` | 80-byte entry selection in `Model_GetSlotDataEntry`, `Model_GetCurrentDataEntry`, and `Model_TransformPartVertex` |
| `0xD18` | `root_coordinate` | `Model_BuildCameraRelativeCoordinateUnit` repeatedly reads pointee halfwords `+0x44`, `+0x46`, and `+0x48` |
| `0xD1C` | `render_parent_coordinate` | selected parent/fallback for child scaling and world-position bounds; not the parent of every rendered unit |
| `0xD70` | `flat_lights[3]`, `GsF_LIGHT`-shaped | `Model_InitLightTriplet` writes three `0x10`-byte records; `Model_DrawSlot` and `Model_DrawBackground` pass `+0xD70`, `+0xD80` and `+0xD90` to `GsSetFlatLight(0)`, `(1)` and `(2)`; `Model_GetFlatLight` returns one of them |
| `0xDA0` | `ambient_rgb_scale[3]` | three adjacent clamped `s32` writes in `Model_SetSlotAmbient` |
| `0xDB0` | `render_scale` | four-word copy/reset block in `Model_SetSlotScale` |
| `0xDC0` | `tint[8]` | fade tint: target colour/mode writes in `Model_SetSlotTintTarget` and selection in `Model_GetSlotTintDescriptor`; live 3-D battle values below (formerly `field_DC0`) |
| `0xDC8` | `cached_dimensions[4]` | exact eight-byte copies in `func_80057E20` and `func_80059000`; element 3 is cleared by `Model_SeekSlotAnimation` |
| `0xDD0` | `field_DD0[4]` | four adjacent `u16` reads in `Model_CopySlotU16Values` |
| `0xDDC` | `animation_parameter_base`, `u8 *G32` | command-halfword word offsets address live words saved before a row and restored afterward |
| `0xDE0` | `animation_snapshot_buffer`, `u8 *G32` | contiguous three/five-word snapshots; imported or hmd+size fallback, followed by cache work span |
| `0xDE8` | `primary_module_data_address`, `s32` | independently selected primary data word passed to the primary module at base +4 |
| `0xDEC` | `variant_module_data_address`, `s32` | independently selected variant data word passed to variant/presentation handlers |
| `0xDF8` | `model_id` | first property in `Model_SetSlotProperties`; mirrored to `gModel_awSlotModelIds[index]` for all three slots when non-negative |
| `0xDFA` | `field_DFA` | second property in `Model_SetSlotProperties` for slots 0 and 1; negative leaves unchanged |
| `0xDFC` | `field_DFC` | third property in `Model_SetSlotProperties` for slots 0 and 1; negative leaves unchanged |
| `0xDFE` | `guardian_star_selection` | fourth property in `Model_SetSlotProperties`; when non-negative, normalized to a boolean and mirrored to `gModelDebug_abSlotAfFlags[index]` |
| `0xDFF` | `defense_position` | fifth property in `Model_SetSlotProperties`; when non-negative, normalized to a boolean and mirrored to `gModelDebug_abSlotGfFlags[index]` |
| `0xE00` | model-data size, `u16` | `Model_HasInsufficientBufferSpace` subtracts this value from the remaining model-data bank capacity |
| `0xE06` | `animation_position` | 12.4 fixed-point animation frame returned as an integer by `Model_GetSlotAnimationFrame`; write/read in `Model_SeekSlotAnimation` |
| `0xE08` | `animation_sound_position` | separate wrapping command-branch sound cursor; original volatile diagnostic reload retained |
| `0xE0D` | `playback_speed` | `Model_GetSlotAnimationSpeed` reads it and `Model_SetSlotPlaybackSpeed` writes it |
| `0xE11` | `draw_mode_request` | `Model_DrawSlotUnitsOnly`, `func_80059284`, and `Model_SetSlotAmbient` |
| `0xE12` | `shadow_enabled` | shadow-render enable read/conditional write in `Model_SetSlotShadowEnabled`; `Model_DrawSlot` guards its projected ground-fan path with the same byte |
| `0xE13` | `stone_texture_state` | STONE reaction seeds 0x80/0xFF; qualifying draws grayscale, remap handlers, finalize at 0xFE and clear |
| `0xE14` | `load_step` | sentinel test in `func_80058DD8` |
| `0xE16`-`0xE17` | `playback_state`, `entry_count` | raw playback mode and coordinate entry bound; original notification and request comparisons retained |
| `0xE18` | `root_coordinate_index` | parentless coordinate scan result, narrowed to u8; strict out-of-count request fallback |
| `0xE19` | `render_parent_coordinate_index` | initialization selection/replacement index; can remain stale when its pointer becomes null |
| `0xE1A` | `hmd_unit_count` | head-entry count in `Model_RunSlotHandlers` |
| `0xE1B` | `animation_sequence_count` | pointer-array count in `Model_SeekSlotAnimation` and `Model_SetSlotPartSpeeds` |
| `0xE1D` | `skip_sound_setup`, `u8` | raw packed-slot 0x80 option skips sequence/sample setup and clears three CF8 commands; not a mute/active state |
| `0xE1E` | `animation_sound_enabled`, `u8` | independent nonzero animation-cue gate; cleared before HMD test, conditionally armed at load step 11 |
| `0xE1F` | `is_active` | status tests in `func_80058DD8` and `Model_RunSlotHandlers` |

`hmd_units` retains the existing `MODEL_SLOT_UNIT_COUNT` (60) declaration,
occupying `0x1E0` bytes before `animation_sequences`. The initializer retains
its byte-count capture and clamp to 60; no new bound or guard is introduced.

`tint` (formerly `field_DC0`) reads statically as four sub-fields:

| Bytes | Static role |
|---|---|
| 0-2 | target RGB: `Model_SetSlotTintTarget` arguments and the scene controllers' ±2 fade steps |
| 3 | blend selector: non-zero sets semi-transparency in `Model_DrawSlot`; 1-4 select draw-mode rate `byte - 1`, 5 uses rate 1 (additive) plus the second-pass CLUT word |
| 4-6 | current RGB: `Model_DrawSlot` steps it toward 0-2 while byte 7 % 6 is non-zero and byte 3 is 0 |
| 7 | fade/one-shot flag: `Model_InitSlot` stores 5 for flag `0x80`; 6 makes `Model_UpdateSlotControl` queue an endpoint |

### Live values in 3-D battles

The `model_slot_dc0_watch` trace (result in #7948) covered three Free Duel
battles plus one non-battle scene, with 784 `Model_SetSlotTintTarget` calls
and 8,849 decoded stores. Slot 0 was the player's monster, slot 1 the
opponent's. Slot 2's frame-polled snapshots were always `808080:00`; its
only observed changes were inside `Model_ResetSlot`, where
`Model_FillWords` clears RGB to `00` and the following stores restore
`80` within the same call.

Bytes 0-3 are the monster's fade. Byte 3 only ever held 0 or 5:

| Moment on screen | Writer | Bytes 0-2 | Byte 3 |
|---|---|---|---|
| battle set-up, before either monster appears | `Model_SetSlotTintTarget`, both slots | `00` | 0→5 |
| player monster fades in among the purple flames | `Model_UpdateBurstEffect` (ra `0x8006F0C4`), slot 0 | 0→124 in steps of 4, 63 frames, then 128 | 5, then 0 |
| opponent appears | `Model_UpdateAnimatedBattle` (`0x8004EED8`) | 128 at once | 0 |
| destroyed monster, with light beams under it | `Model_UpdateAnimatedBattle` (`0x8004F7C0`) switches to 5, then `Model_UpdateSparkEffect` (`0x8007019C`) | 128→0 in steps of 2, 125 frames | 5 |
| surviving monster fades out at the end | `Model_UpdateAnimatedBattle` (`0x8004FB0C`, `0x8004FC5C`, `0x8004FD68`) switches to 5, then its own stores | 128→0 in steps of 2, 129 frames | 5 |
| scene ends | `Model_ResetSlot`, through `Model_FillWords` | 128 | 0 |

So byte 3 = 5 is the additive fade mode. In those mode-5 fades bytes 0-2
act as the monster's brightness: 0 is invisible and 128 is normal. This is
scoped to mode 5: `Model_DrawSlot` skips a zero-RGB slot only when
`draw_mode_request != 1` and byte 3 is at least 2, so zero RGB in mode 0
is not by itself a hidden model. Byte 3 = 0 with 128 is the
opaque, untinted model. The destroyed-monster fade matched slot 0 in #3
(Dragon Piper lost) and slot 1 in #2 (Armored Zombie lost). A MODEL.MRG
variant overlay (return `0x8013C960`) also set slot 1 to mode 0 at 128
twice; which variant is not identified.

Byte 7 never became non-zero: all 4,762 `Model_DrawSlot` stores to it
wrote 0 over 0, so the bytes 4-6 ramp branch never ran in these
battles. Bytes 4-6 stayed 0, written only by `Model_InitSlot` and
`Model_ResetSlot`. Their roles and byte-3 values 1-4 remain unobserved.

`animation_sequences` and the run at `0x750` are no longer in that position. The reset
in `Model_ResetSlotAnimationTables` walks both to fixed bounds -- ten rows and 58 parts, from
its `i < 0xA` and `j < 0x3A` loops and their `0x74` and `0x76` strides -- and
those bounds tile the record exactly from `0x1E0` to `0xBEC`, so the header
declares `MODEL_SLOT_PART_COUNT` and `MODEL_SLOT_ROW_COUNT` rather than
guessing.

Each `animation_sequences` entry points at libhmd's `GsSEQ`, the per-part animation
sequence. `Model_LinkSlotAnimationUnit` passes the table to `GsLinkAnim` as `GsSEQ **`, and
the game touches the fields the way libhmd defines them. `Model_LoadSlot`
rearms a part with `ti = start`, `sid = start_sid`, `ii = aframe = 0xFFFF`,
`rframe = 0` and `speed = 0x10`. `Model_BuildSlotAnimationTables` reads `start` as the part's
first command key and stamps `start_sid`. `Model_SeekSlotPartAnimation` seeks a part by
writing `rframe`, `tframe`, `ci` and `ti`. `model.h` mirrors the layout as
`ModelSlotPart` so it does not need the libhmd chain, and
`model_slot_updates.c` asserts that the mirror and `GsSEQ` agree field by
field.

The reset, table builder and subsequent command-row linker are named and
traced through their producer/consumer chain in
`notes/model-animation-tables.md`.

### Render scale and cached coordinate-origin dimensions

`ModelSlot.field_DB0` is now `render_scale`, retaining the four signed
words of `ModelSlotS32Quad` at +0xDB0. The original scale APIs and
`gModel_Slot0Scale` alias retain their pointer/stride construction;
nonnull setter copies all four words, null resets only the first three
in the original order. No SDK VECTOR retyping or fourth-word clear.

`ModelSlot.field_DC8` is now `cached_dimensions`, still `u16[4]` at
+0xDC8. First-three entries are padded/smoothed eligible coordinate-origin
spans, while entry three separately retains raw horizontal history.
Initialization clears all four; seek clears only entry three. Keep
signed observations, u16 truncation, empty-pass sentinels, stale-state
gates and the distinct minimum-versus-replacement getter policies.
No exact mesh bound, positive/monotonic range or frame/readiness
invariant is introduced. See [the full scale/cache protocol](model-slot-properties.md#cached-coordinate-origin-dimensions).

### Root and selected render-parent coordinates

The paired pointer/index roles are now explicit:

| Original | Canonical | Unchanged representation |
|---|---|---|
| `field_D18` | `root_coordinate` | stored `struct _GsCOORDUNIT *G32`, +0xD18 |
| `field_D1C` | `render_parent_coordinate` | stored `struct _GsCOORDUNIT *G32`, +0xD1C |
| `field_E18` | `root_coordinate_index` | `u8`, +0xE18 |
| `field_E19` | `render_parent_coordinate_index` | `u8`, +0xE19 |

`Model_InitSlot` clears all four before its null-HMD return. With a
mapped coordinate run, it scans to the first entry whose SDK `super`
is null, stores the original scan count through the root index byte
and computes the root pointer from the original integer count. Keep
that distinction: narrowing is not an invitation to recompute the
pointer from the stored byte, clamp the scan or add a root/count guard.

The initializer asks `Model_FindBranchingChildCoord` for a child that
itself has a child. It stores the returned index through the original
byte conversion, falls back to the root index under the existing
comparison, and initially selects that coordinate as render parent.
Its retry scan keeps the exact non-null coordinate/list checks and
primitive-header test while looking for a unit parented by the current
coordinate. It can descend another branching child before selecting
the current coordinate's parent. A parent equal to the root retains
the earlier selection. A different parent replaces the pointer, but
only a non-null replacement updates the index. Thus null pointers,
retained fallback selections and stale indices remain possible;
no pointer/index round-trip, unique subtree or readiness invariant
is implied. All loop forms, sentinels, limits and partial writes survive.

`Model_SetSlotRootTransform`, debug setup and credits-monster setup
write root translation/yaw and retain their original matrix-refresh
calls, including calls outside a pointer guard. The two view builders
read root rotations; the view-relative builder still repeatedly reads
slot ZERO through its volatile view, not the selected slot. Background
height reads slot 2's root local Y before its active check; the default
shadow plane uses that field in its original draw path.

The renderer tests `unit->super == render_parent_coordinate` for its
existing child scaling path and uses this selected coordinate's world
translation as a fallback in the centroid/height passes. It excludes
both coordinate pointers from the second bounds pass. The name does
not make this the direct parent of every rendered unit, the model's
centroid or an anatomical bone. Keep all part masks, live-list tests,
cached/world matrix choices, averaging, clamps and draw order.

`Model_GetSlotDataEntry` and `Model_BuildViewRelativeCoordinateUnit`
retain the strictly-greater-than-count fallback to the root index;
equality and negative requests are not newly rejected.
`Model_GetCurrentDataEntry` keeps its integer-address return and
0x50-byte stride. No SDK layout, pointer type, G32 marker, allocation,
stable-parent lifetime, API or validation behavior changes.

### Model IDs, guardian-star selection and defense posture

The property producer establishes three meanings independently of the
debug AF/GF abbreviations:

| Original | Canonical | Unchanged representation |
| --- | --- | --- |
| `ModelSlot.field_DF8` | `model_id` | `u16`, +0xDF8 |
| `ModelSlot.field_DFE` | `guardian_star_selection` | `u8`, +0xDFE |
| `ModelSlot.field_DFF` | `defense_position` | `u8`, +0xDFF |
| `AnimatedBattleModelProperties.field_07` | `guardian_star_selection` | `u8`, +0x07 |
| `AnimatedBattleModelProperties.field_06` | `defense_position` | `u8`, +0x06 |

`DuelScene_UpdateBattle` case 5 copies saved record flag bit 9 to both
guardian-star selections. That is the same 0x200 bit
`DuelCard_GetGuardianStar` and `Duel_CalcGuardianStarBonus` use to choose
the second, rather than first, guardian-star nibble. It is a selection
between two stars, not a star ID or a newly checked array bound.
The attacker receives defense position zero; the defender receives
saved bit 11, the 0x800 position bit. `Duel_CompareBattleStrength`
independently selects defense rather than attack with that bit.
`Main_RunAnimatedBattle` passes these two input bytes in fourth/fifth
property order, retaining the original load order and model-id minus one.

`Model_SetSlotProperties` preserves every negative signed argument.
For a nonnegative model ID it retains the u16 store and, outside Japan,
the debug mirror. For nonnegative fourth/fifth properties it retains
`!= 0` and the AF/GF mirrors. No expansion of those abbreviations is
needed. Slot 2 still consumes only the model/terrain property and keeps
the same lookup refresh and unconditional scene-mode write.

`Model_LoadMonsterMerge` reuses the stored ID for a negative model
argument; otherwise it stores the original argument BEFORE validating
or compacting it for the MRG request. Ordinary, special-battle and
auxiliary IDs remain distinct; no valid-ID, one-based-card-ID, range or
mirror-equality invariant is introduced.
Its accepted ordinary-model branch retains the conditional property
writes and copies the guardian-star selection to transfer position.
That same selection indexes the variant command and camera/duration
metadata and selects animation row `selection + 3`, including its
sound-command paths. Defense position selects the opponent's comparison
halfword and the existing presentation reactions. The CF8 tail view's
relative `field_106`/`field_107` aliases denote these same bytes; their
names, raw-address reads and offset constants remain unchanged.

Reset still writes the slot index into guardian-star selection, including
2 for the auxiliary slot, and zero into defense position. Only the
nonnegative property setters normalize values; no universal Boolean,
duelist-only record, live-model or readiness guarantee is inferred.
Repeated selector loads, volatile diagnostics, callback order, random
calls, fallback branches and all source groups/profiles remain intact.

The two middle properties stay `field_DFA`/`field_DFC`, and the input
halfwords stay `field_02`/`field_04`. The battle producer deliberately
supplies zero/one comparison overrides to replay the already determined
result, not the cards' calculated attack/defense totals. Their 0xFFFF
reset sentinels, signed-argument preservation and conditional CF8 copies
are not changed or promoted to guessed stat types.
The unrelated Spanish `ModelVariant408EntryState.field_DF8` remains unchanged.

### Sound-setup option and animation-sound gate

`ModelSlot.field_E1D` and `ModelSlot.field_E1E` now have the separate
roles `skip_sound_setup` and `animation_sound_enabled`, retaining both
raw bytes and exact offsets. The credits request's packed 0x80 option
skips voice shutdown, sequence-sector RAM write, SPU sample upload and
sequence-bank registration; it also resets three copied CF8 commands
and does not arm the cue gate at step 11. Existing voices are not muted.
Phase 14's mode 3 is authoritatively SPU audio, not the image mode 2.
The cue gate remains independent of active/load state and follows the
original active and tint tests. Null-HMD/nonnull-HMD resets, conditional
writes, all copied metadata, transfer masks/order and byte narrowing
remain intact; no Boolean inverse, silence or readiness guarantee.
See [the complete audio load/cue protocol](model-slot-lifecycle.md#sound-setup-and-animation-cue-gates).

### Variadic slot-property update

Matching `Model_SetSlotProperties` (`0x80053248`) takes a
slot index followed by signed 32-bit properties. A negative value leaves its
corresponding field unchanged. The arguments themselves must still be supplied:
five properties for slots 0 and 1, or one for slot 2. The
first property writes `model_id` for any of the three slots and mirrors it to
`gModel_awSlotModelIds[index]`. Slots 0 and 1 consume four more properties for
`field_DFA`, `field_DFC`, `guardian_star_selection`, and `defense_position`; the final two are
normalized to zero or one and mirrored to `gModelDebug_abSlotAfFlags[index]` and
`gModelDebug_abSlotGfFlags[index]`.

Slot 2 consumes only the first property, then refreshes `D_8009AF88` from the
`0xB2`-byte table selected by `gModel_awStageSlotModelId[0]`. Every call finishes by setting
`gModel_bSceneMode` to `15`. These writes establish field widths, slot bounds, and
mirror relationships. The preceding producer trace establishes the model ID,
guardian-star selection and defense posture; the middle comparison words
and the purpose of the final value `15` remain unclassified. The array supports indices 0 through 2;
the function does not validate its index, and its `idx >= 2` arm is not evidence
for additional slots.

Two retail callers corroborate the contract independently of the setter:

- `Main_RunAnimatedBattle` calls slots 0 and 1 at `0x8002D22C` and `0x8002D254`,
  supplying three signed halfwords and two bytes from successive eight-byte
  records, with the first halfword decremented. Its call at `0x8002D264`
  supplies just slot 2 and `gDuel_bTerrain` (`0x8009B364`).
- `ModelDebug_UpdateController` calls slots 0 and 1 at `0x80054050` and
  `0x80054074`, restoring the first and last two properties from their mirrors
  while passing `-1` for the second and third. Its call at `0x80054080`
  supplies just slot 2 and the third halfword mirror, `D_8009B48C`.

This supports the high-confidence name `Model_SetSlotProperties` without
assigning meanings from the debug abbreviations alone.
`main_modes.h` records the two eight-byte inputs as
`AnimatedBattleModelProperties`, now sharing the confirmed choice names
while retaining mechanical names for the two comparison halfwords.

The animated-battle calls belong to the ordinary initialization branch,
not every mode tick: `D_8009B26C & 0x40` gates initialization, and a first
halfword of `0x309` in `gMain_aAnimatedBattleModels` selects another path instead of the
three property calls. The setter's writes, terrain input, and the wrapper's
mode/audio changes rule out a read-only two-combatant contract; see
[the game description](research/the-game.md#59-the-3-d-battle-and-the-poly-mode).

The repeated values behind this path now have one owner in `model.h`.
`MODEL_SPECIAL_BATTLE_ID` names `0x309` across the animated-battle runner,
intro controller, model-control state machine, animation transition, and
Exodia candidate. `MODEL_DEFAULT_PROJECTION` names the canonical `0x12C`
projection installed by the resident, overworld, credits, and candidate
camera setup paths.

The ordinary monster MRG is indexed by card-derived model IDs ending at
`MODEL_MRG_LAST_ID`. Its compacted record stream omits two 50-ID ranges and
one single ID:

| Constant range | Numeric range | Effect |
|---|---:|---|
| `[MODEL_MRG_FIRST_GAP_START, MODEL_MRG_FIRST_GAP_END)` | `[0x12C, 0x15E)` | No model-MRG entry; later IDs subtract `MODEL_MRG_GAP_SIZE`. Both ranges are end-exclusive. |
| `[MODEL_MRG_SECOND_GAP_START, MODEL_MRG_SECOND_GAP_END)` | `[0x28A, 0x2BC)` | No model-MRG entry; later IDs subtract the same gap size. Both ranges are end-exclusive. |
| `MODEL_MRG_SINGLE_GAP_ID` | `0x2D0` | No record; the final ordinary ID is compacted by one. |

`Model_LoadMonsterMerge` and the random-model controller share these
boundaries, preventing their validity checks from drifting apart. The same
header owns the measured `0x114` model-MRG sector count, `0x74` auxiliary
sector count, `0xB2`-byte auxiliary lookup stride, and the special battle
file start sector/count used only for `MODEL_SPECIAL_BATTLE_ID`.

### Paired module data arguments

`ModelSlot.field_DE8` and `ModelSlot.field_DEC` are now
`primary_module_data_address` and `variant_module_data_address`.
Both remain signed `s32` address words at +0xDE8/+0xDEC, not pointer
members, callable entries, newly allocated contexts or a common record type.
The independently named words in `high_memory_addresses.h` and the complete
initializer/dispatcher/presentation chains establish the pairing.

`Model_InitSlot` first clears both words, then retains its original
`index < 2` selection of the four independently labeled data constants.
Normal slot 0 receives 0x80135000/0x80136000; slot 1 receives
0x80175000/0x80176000. This happens before the null-HMD early return,
so a cleared or inactive slot does not imply zero module arguments.
Do not add index, sign, lifetime or load-readiness guards.

`Model_UpdateSlotControl` captures the primary and variant addresses as
the original local byte-pointer views. The primary module is selected
separately and called at base +4 with the primary data and request -1;
its return remains ignored. The variant module receives the variant
data with the selected request modulo 1000 on the existing state-7
path, then with -1 on subsequent eligible updates. Keep callback-state
bracketing, state transitions, narrowed results and every missing-handler,
active-slot and special-model path. `Model_AdvanceSlotLoad` stage 9 uses
the same primary argument for the initial modulo-1000 request.

Presentation phases pass the variant address to their existing selected
effect handlers with unchanged requests, including 0, -1, -2 and 0x18E.
They do not turn the data word into the called address. French variant
notes independently trace the same fixed-RAM argument while retaining
their measured context views and non-overlap/lifetime caveats. No one
context layout, capacity, ownership or completion contract is inferred
for every module. The address-based fields of the unrelated
`Variant393EntryState` are intentionally unchanged despite coincident
numeric offsets.

See [high-memory-load-addresses.md](high-memory-load-addresses.md).
All source expressions, casts, signatures, absolute relocations, per-region
profiles and existing fixture layout expectations are unchanged.

### Live animation parameters and saved-word buffer

`ModelSlot.field_DDC` and `ModelSlot.field_DE0` are now
`animation_parameter_base` and `animation_snapshot_buffer`.
Both retain stored `u8 *G32` declarations at +0xDDC/+0xDE0.
These are live parameter and saved-word roles, not permanently one-way
source/destination pointers: saving and restoration reverse the copy.

`Model_ResetSlotAnimationTables` clears both through the original
`*(s32 *)&member` stores and preserves the byte-cursor imports. Enabled
kind-3 owner payload supplies the parameter base beside the command
list; enabled kind-2 payload supplies the snapshot buffer. The owner
walk, sentinels, enabled tests, part-mask reads and last-selected owner
behavior are unchanged. The adjacent kind-2 import at +0xDE4 stays
`field_DE4`: no independent consumer establishes its semantic role.

After non-null-HMD initialization, `Model_InitSlot` preserves an imported
snapshot pointer or supplies `hmd + size` only when it is null. The
null-HMD early return precedes table reset and this fallback, so it
does not promise cleared parameter/snapshot pointers. This is an
address selection, not a new allocation, proven extent or lifetime.

`Model_ControlSlotAnimation` captures both pointers before the active
gate. Each non-missing command-index entry uses the original first
command halfword times four as an offset from the parameter base.
Starting a row saves three words, or five for an extended-mask part,
into the contiguous snapshot cursor; missing 0xFFFF entries do not
advance that cursor. Restoring copies those saved words back to their
parameter offsets and clears the active row. The byte pointers, word
casts, copy direction, original bit arithmetic and per-loop word counts
are retained. Selector -1 still clears only the pending reset and
sequence indices; it does not restore or clear either buffer.

Keep every earlier lifecycle write, zero-duration return, replacement
flag, previous-row restoration, queue-latch update and same-row seek
path. No command bounds, non-null guard, common record layout or
memmove/overlap promise is inferred from `Model_CopyWords`.

`Model_LoadSlot` and staged-load case 4 reserve the original row-one
per-part span after the snapshot base: 12 or 20 bytes for each present
entry, using the extended-part mask. Only the original index<2 path
adds that span. Both still store the lit-color-cache base for every
slot, then derive its end from the existing halfword count. Do not
turn the row-one accounting into a validated snapshot capacity, guard
the third-slot store, change signed address arithmetic or clear extra
cache state during partial reset.

The unrelated French `Variant393EntryState` fields at coincident
offsets remain unchanged, including its source, header and fixture.
No guessed owner-block type, SDK declaration, G32 marker, profile,
source grouping, request order or existing test oracle is changed.

### Timed model-tint requests

Matching `func_80058938` and `Model_UpdateTintRequests` establish a ten-entry request
array at `D_800F2B50`, with `0x18` bytes per entry. The constructor scans from
entry zero and fills the first record whose low active bit is clear; if all ten
are active, it reaches the end without replacing one. It records one of model
slots 0 or 1, two four-byte colour values, an initial elapsed value of zero,
and a duration equal to twice its fifth argument. Optional variadic indices
become an eight-byte part-selection bitset. These are storage and control
relationships, not names for the remaining offset-based fields.

The shared model tick `Model_UpdateScene` calls `Model_UpdateTintRequests` after the ordinary
slot draw/update helpers. For each active request whose selected model slot is
active, the processor:

1. computes three colour bytes as linear integer interpolation from entry
   bytes `+0x10..+0x12` toward `+0x14..+0x16`, using elapsed `+0x0C` over
   duration `+0x0E`, while retaining byte `+0x13` as the fourth channel;
2. saves the slot colour at `tint`, `current_animation_row`, `animation_position`, and the
   affected per-part bytes, installs the request values, and redraws the slot;
3. clears the two draw-context globals, restores every saved model value, and
   restores the prior `Model_SetSlotShadowEnabled` state;
4. advances elapsed by `Model_GetFrameStep()` and clears the request's active bit
   once elapsed is at least duration.

An inactive selected model slot leaves the request and its elapsed value
unchanged. `Model_HasInsufficientBufferSpace` takes a different path: it skips
the temporary mutation and redraw but still advances elapsed and can expire the
request. The processor restores model state after each successful redraw, so
the matching code proves a temporary render override rather than a persistent
slot-colour assignment. Although animated battle reaches this processor through `Model_UpdateScene`,
the pipeline is shared; current matching callers do not justify calling the
queue battle-exclusive.

### Shadow gate and stone texture state

The raw bytes `ModelSlot.field_E12` and `ModelSlot.field_E13` are now
`shadow_enabled` and `stone_texture_state`, retaining `u8` at +0xE12
and +0xE13. These names follow the full setter/control/draw/presentation
chains, not a new runtime capture or a recovered card-status enumeration.

`Model_InitSlot` clears both. `Model_SetSlotShadowEnabled` returns the
old shadow byte and writes a nonnegative argument through the original
byte conversion; a negative argument only reads it. Do not normalize
nonzero values, clamp a large argument, change the public signature or
add an index guard. The renderer tests nonzero only for the projected
ground-shadow fan, not whole-slot visibility. Library/debug callers
enable it; the timed-tint pass saves it, disables it and restores it.
Keep all other draw gates, ground-height/clipping conditions and the
units-only early return.

The secondary-result 3 branch traces STONE and writes the opposing
slot's texture state to 0x80 or 0xFF along its existing reaction paths.
The presentation controller can resume 0xFF as 0x80 under its original
conditions, and later waits for zero. Zero means no pending texture
work; 0xFF holds it. This is not just a monotonic countdown or a
guaranteed once-per-frame timer: it advances on qualifying draw calls,
and the original early returns can leave it unchanged.

For an ordinary nonzero value other than 0xFF or 0xFE, drawing first
decrements the stored byte, then uses that value to select a two-by-eight
halfword rectangle. All sixteen BGR555 words still pass through
`Color_TintBgr555Pixel` with raw flags 6 and scale 0x1000, followed by
the original 0x7FFF mask. The local color constants and helper establish
that 6 is grayscale; no guessed palette type or alternate color formula
is introduced. Preserve the distinct source/destination rectangles,
European texture layout, GPU waits, transfers and store narrowing.

When that decrement reaches zero, the existing HMD primitive-data chains
are walked through `Model_RemapPrimitiveHandlers` with
`Model_MapToBasePrimitiveHandler`, and the byte becomes 0xFE. On the next
qualifying draw, 0xFE processes the two existing 256-by-four-word texture
bands, sets bit 0x8000 only on nonzero words, keeps the exact GPU call
order, holds playback through `Model_SetSlotPlaybackDirection(index, 0)`
and finally stores zero. Do not move finalization into the last grayscale
step or omit the original list/count/active checks.

No new enum, legal-value range, completion/readiness invariant, restoration
rule or elapsed-time behavior follows from the names. All source logic,
types, dimensions, sentinels, regions, packet behavior and existing fixture
oracles/mutation controls remain unchanged.

## HMD unit views and primitive-data chains

The loader, renderer, vertex transformer and primitive-handler walkers
share the slot's leading `GsUNIT`-compatible records:

| Original member | Canonical member | Preserved representation |
| --- | --- | --- |
| `ModelSlot.field_000` | `hmd_units` | `ModelSlotHeadEntry[60]`, `+0` |
| `ModelSlotHeadEntry.field_00` | `coordinate_address` | raw `u32`, `+0` |
| `ModelSlotHeadEntry.field_04` | `primitive_data` | `void *G32`, `+4` |

The authoritative local `GsUNIT` declaration places `coord` at `+0` and
`primtop` at `+4`. `Model_InitSlot` casts this exact array to `GsUNIT *`,
clears each processed `coord`, installs the input block pointer in
`primtop`, and scans its unit records. Its later link loop assigns
coordinate entries only over its existing indices `1 .. count-2`.
`Model_DrawSlot` hands the same record to `GsSortUnit`, obtains matrices
through its coordinate address, and traverses its primitive-data blocks.

`Model_RunSlotHandlers` walks that same pointer's linked blocks, retaining
all three header words and the `-1` terminator.
The palette-flash completion path performs the corresponding handler
remap. `Model_TransformPartVertex` retains its integer address sum for
`hmd_units[part+1]`, SDK view and subsequent vertex-data chase; direct
member indexing changes the measured instruction sequence.

`primitive_data` denotes the SDK `primtop` data, not a packet buffer,
drawn-polygon count or promise that all scanned blocks are polygon data.
The initializer also dispatches coordinate and animation unit records.
`coordinate_address` stays an integer word, not a newly typed pointer;
all existing SDK casts and the separately typed `void *G32` storage stay.
No pointee ownership, universal nonnull pairing or fully initialized
array invariant is inferred. Partial initialization, null-input early
return, stale records outside the processed range and every original
guard/order remain unchanged.

The record remains eight bytes and the array remains `0x1E0` bytes,
with explicit member-width/offset and array-extent checks in `model.h`.
No SDK declarations, primitive parsing, handler selection, coordinate
selection, counts, numeric constants or source/profile owners change.

## `D_800F56F0`: 32-byte reference-view record

The eight-word block at `D_800F56F0` has the exact target layout of Psy-Q's
`GsRVIEW2`: three 32-bit viewpoint coordinates, three 32-bit reference-point
coordinates, a 32-bit roll value, and a parent-coordinate pointer.
`Model_ResetSceneView` initializes all eight words and passes the block to
`GsSetRefView2`, while `Model_UpdateViewMetrics` copies exactly `0x20` bytes
when given an alternate view.

The matching `Model_UpdateViewMetrics` body also establishes the derived
camera values:

- `D_8009B478` first holds the XZ-plane distance between viewpoint and
  reference point, then is replaced by the full three-dimensional distance;
- `D_8009B47A` is `ratan2(delta_z, delta_x)`;
- `D_8009B47C` is `ratan2(delta_y, horizontal_distance)`;
- both angles are normalized into the 4096-unit turn range.

`Model_BuildCameraRelativeCoordinateUnit` consumes the two angles when
building a model rotation matrix. Matching `ViewState_ApplyViewpointOffset`
now uses
`GsRVIEW2` member accesses for a separate view record embedded at object
offset `+0x10`, with byte-identical code generation. Sources accessing the
shared `D_800F56F0` block retain local byte/word views until that migration
is checked independently.

The output transform in `Model_BuildCameraRelativeCoordinateUnit` uses the
imported `libgte.h` types: an eight-byte `SVECTOR` for each local angle
triplet and a 32-byte `MATRIX` at object offset `+0x04`. On the PSX ABI, the
matrix's translation vector starts at matrix offset `+0x14`, so `f4.t[0..2]`
occupies object offsets `+0x18`, `+0x1C`, and `+0x20`. The function clears
these components after `MulMatrix`, preserving the original reverse store
order. The object remains a game-specific local record; its rotation angles
and id are at `+0x44` and `+0x4C`, beyond the SDK matrix.

The current typed-migration snapshot has 25 pure-C users of `D_800F2C40`;
all include the shared header:
`func_80057E20`, `func_80058DD8`, `Model_GetSlotAnimationSpeed`,
`Model_GetSlotAnimationIndex`,
`Model_GetSlotAnimationFrame`, `Model_GetSlotAnimationLength`, `Model_GetSlotDataEntry`,
`Model_GetCurrentDataEntry`,
`Model_CopySlotU16Values`, `func_80059000`, `Model_DrawSlotUnitsOnly`,
`Model_InitLightTriplet`, `func_80059284`,
`Model_BuildCameraRelativeCoordinateUnit`, `Model_TransformPartVertex`,
`Model_SetSlotScale`, `Model_GetSlotTintDescriptor`, `Model_SetSlotTintTarget`, `Model_SetSlotAmbient`,
`Model_SetSlotPlaybackSpeed`, `Model_SeekSlotAnimation`, `Model_SetSlotShadowEnabled`,
`Model_RunSlotHandlers`,
`Model_SetSlotPartSpeeds`, and `Model_SetSlotProperties`.

## `gModel_aHandlerKeyRegistry` (`D_800F5918`): 80 handler registry entries

`ModelHandlerRegistryEntry` is an eight-byte pair: `handler_value` at `+0`
and `key` at `+4`. `Model_RegisterHandlerKey` and `Model_FindHandlerKey`
both advance by eight bytes for exactly 80 iterations. The target assembly,
GMS's paired `dword_800F5918`/`dword_800F591C` views, and Unchiga's
same-address functions agree on the order and count. The modeled extent is
`80 * 8 == 0x280`; `gAiScript_aMemory` (`0x800F5B98`) starts exactly at
`D_800F5918 + 0x280`.

All registry accessors use the shared entry type and
the typed extern. The two packed-ID dispatch maps used to keep a raw byte
walk over the same table, behind `MODEL_HANDLER_REGISTRY_CUSTOM_EXTERN`, on
the assumption that converting it would move their switch layout. Coalescing
them into `model_handler_registry.c` forced the question, because one
translation unit cannot hold both spellings, and the assumption did not hold:
`e->handler_value`/`e->key`/`e++` in place of `*(s32 *)p`/`*(s32 *)(p + 4)`/
`p += 8` builds byte-identically. The escape hatch had no other user and is
removed from `model.h` with it.

That the two maps open-code `Model_FindHandlerKey` exactly -- same
`GsU_00000000` sentinel, same eighty-entry scan, same `-1` on miss -- is also
what places them in this unit rather than beside the handlers they name.

### Observed contents and lookups

A `model_handler_families` trace (#418; interpreter CPU, setup confirmed by
19,498 control hits) followed one Free Duel against Villager2 with a Sogen
field change and two 3-D battles. Registrations came from two sites,
`0x800608A8` (the first at duel start in main mode `0x03`, the rest when
the battle scene loaded in mode `0x01`) and `0x80060B28`, which registered
key `0x01000000` many times. After the second battle the table held
fourteen pairs:

| Key | Group | Kind | Registered handler |
|---|---|---|---|
| `0x0002000D` | `0x0002` | `0x000D` | `0x80033DB0` (HMD fog-shaded textured triangle driver) |
| `0x00020015` | `0x0002` | `0x0015` | `0x80034830` (HMD fog-shaded textured quad driver) |
| `0x0000000D` | `0x0000` | `0x000D` | `0x8006151C` |
| `0x00000011` | `0x0000` | `0x0011` | `0x800617E0` |
| `0x00000015` | `0x0000` | `0x0015` | `0x80061A84` |
| `0x0030000D` | `0x0030` | `0x000D` | `0x80065BCC` |
| `0x01000000` | `0x0100` | `0x0000` | `0x80067220` |
| `0x0020000D` | `0x0020` | `0x000D` | `0x80064248` |
| `0x00200015` | `0x0020` | `0x0015` | `0x80064868` |
| `0x0120000D` | `0x0120` | `0x000D` | `0x8006875C` |
| `0x0130000D` | `0x0130` | `0x000D` | `0x8006930C` |
| `0x01200015` | `0x0120` | `0x0015` | `0x80068A00` |
| `0x0100000D` | `0x0100` | `0x000D` | `0x80067354` |
| `0x01000015` | `0x0100` | `0x0015` | `0x8006759C` |

These pairs tie the hand-written primitive drivers at `0x8006151C` through
`0x8006930C` to HMD primitive-type keys, which is evidence for naming them
by primitive type once the key bits are decoded. In the same run neither
dispatch map `func_8005FC1C` nor `func_8005FE44` was called once, and
`Model_FindHandlerKey` was not called either, so neither the duel board,
the field change nor the 3-D battle scenes reach them. Those two maps are
now `Model_MapToWireframePrimitiveHandler` and
`Model_MapToBasePrimitiveHandler`. Later watch traces found neither called
on the idle title screen, in the Library 3-D view of a monster, or on the
duel board ([`trace-uncalled-functions.md`](trace-uncalled-functions.md)).

## Migration snapshot and exact-code exceptions

`notes/global-usage.csv` is the authority for the current matching-C and
assembly users of `D_800F2C40`; those sets change whenever another function
is integrated and are not duplicated here.

Five additional matching-C functions include `model.h` but retain raw
model-base views:

- `Model_LinkSlotAnimationRows` uses `MODEL_SLOT_SIZE` while preserving explicit accesses
  across a large display-list construction path.
- `Model_ResetSlot` clears and initializes one `0xE20`-byte slot through
  explicit offsets while preserving its accepted compiler schedule.
- `Model_LoadSlot` uses `MODEL_SLOT_SIZE` while keeping the duel-side layout
  pass in raw offsets.
- `Model_SetSlotPlaybackDirection` uses shared `playback_speed` and `playback_state` members while
  selecting the signed velocity passed to `Model_SetSlotPartSpeeds`.
- `Model_HasInsufficientBufferSpace` includes `model.h` for
  `MODEL_SLOT_SIZE`, but retains a byte-array extern so its `+0xE00` size load
  keeps the accepted address construction.

`Model_UpdateCreditsMonsterSlot` now uses `ModelSlot` and `GsCOORDUNIT` for both model slots
and their placement units. Its repeated `root_coordinate` member expressions are
intentional: GCC 2.8.1 must reload the coordinate pointer before each rotation
and translation store to preserve the retail schedule. The case-10 staging
path in `file_transfer_steps.c` likewise reaches the sound-entry run,
`field_CF8` block, and `load_step` through the shared slot layout. The three
reset words inside `field_CF8` are now explicit signed members at relative
offsets `0x10`, `0x14`, and `0x18`; the whole-block copy retains its separate
`ModelSlotCF8BlockWords` view because its alignment and move width are
code-generation inputs.

The interior alias `D_800F3938` now has a separate
`ModelSlotCF8TailView`, rooted at slot offset `0xCF8` and extending through
the property bytes at relative offsets `0x106`/`0x107` (slot
`guardian_star_selection`/`defense_position`). Its first twelve bytes are one
`ModelSlotCF8Prefix` union: the byte arm preserves the threshold and
`field_0A` table consumers, while the value arm exposes the comparison
halfwords at `+0`/`+2` and the flag word spanning `+8..+0xB`.
`func_800559D4` keeps its signed address cursors, which are
allocation-sensitive, but reads their first and second halfwords through the
view instead of untyped dereferences. `func_8005A618` likewise uses the
prefix's `field_0A` table. Its selector load and the two selector loads in
`func_800559D4` retain byte-pointer address forms using the asserted
relative-offset constants: direct member syntax merges two independently
constructed addresses in `func_8005A618`, while it shortens
`func_800559D4` by eight bytes. Those expressions are exact-code constraints,
not alternate storage declarations.

Two typed pure-C functions retain raw local byte views:

- `Model_InitLightTriplet` keeps its offset writes. Replacing them with
  `field_D70[3]` member writes changed the exact instruction schedule, with
  the first mismatch at VRAM `0x8005914C`.
- `Model_TransformPartVertex` keeps its byte-pointer chase through the leading
  eight-byte entries and `field_D14`. Direct `ModelSlotHeadEntry` member
  syntax changed the resident text size.

Any matching-C report user not covered by the typed inventory or the
exceptions above has not yet adopted `model.h`; derive that changing set from
`notes/global-usage.csv`.

`func_80059000` retains its packed eight-byte local copy type because the
target uses `lwl`/`lwr` and `swl`/`swr`; the model base and source fields are
typed. These retained local views are code-generation constraints, not
competing declarations.

## Validation

Each migration batch was compared against the complete executable. The
final validation retained SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
with `MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`,
and `make check-global-usage` also passed.

## Regional native labels for scene and slot functions

The Japanese and European builds reach five shared model functions through
native labels declared in their owning headers, not through wrapper macros:

| Function | Region | Native label | Owner header |
|---|---|---|---|
| `Model_UpdateCreditsMonsterSlot` | Japan | `func_80053E40` | `model_scene_states.h` |
| `Model_StartBattleSlotAction` | Japan | `func_800547E0` | `model_scene_controllers.h` |
| `tent_Model_RebaseTexturedPrimitives` | PAL | `func_80051610` | `model_slot_row_tables.h` |
| `ModelTexture_LoadTim` | PAL | `func_8005CFD0` | `model_texture_upload.h` |
| `ModelTexture_PackPageClut` | PAL | `func_8005EE50` | `model_texture_upload.h` |

Each binding sits under the region's existing `VERSION_JAPAN` or
`VERSION_EUROPE` selector; other regions keep the plain declaration. The PAL
`model_init_slot.c` wrapper now defines `VERSION_EUROPE` so its rebase calls
see the binding; neither that source nor its headers otherwise test the
selector. Function bodies, symbol maps and profiles are unchanged.

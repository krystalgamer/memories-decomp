# Model Shared Structures

This phase is limited to the shared model storage rooted at `D_800F2C40` and
the handler registry at `D_800F5918`. `src/game/model.h` records only locally
verified widths, offsets, strides, and extents. Offset-based names remain
where semantics, signedness, array bounds, or pointee layouts are uncertain.
GMS and the Unchiga decompilation/recompilation inventories were used as
corroborating evidence, not as sources of guessed reference types.

## `D_800F2C40`: three `0xE20`-byte slots

`ModelSlot` has size `0xE20`, and `D_800F2C40` is declared as three entries.
The stride is independently established by exact matching C and target
instructions in multiple functions:

- `func_80058E3C`, `func_80058E68`, and `func_80058E94` all compile the index
  as `((index * 8 - index) * 16 + index) * 32`, which is `index * 0xE20`,
  before accessing three different late-slot fields.
- `Model_GetCurrentDataEntry`, `Model_CopySlotU16Values`, `func_80059284`,
  `func_80059590`, and `func_80059AA8` independently produce the same stride
  while accessing unrelated pointers, arrays, and bytes.
- `func_80057E20`, `func_80059000`,
  `Model_BuildCameraRelativeCoordinateUnit`, `func_800593D0`, and
  `func_800595C8` provide further exact target sequences with the same
  multiplication. GMS describes the base as `dword_800F2C40[904 * index]`,
  independently giving `904 * 4 == 0xE20`.
- Unchiga's same-address decompilation sources consistently use `0xE20`;
  its recompilation seed inventory identifies the same function family.
  Local recompilation and full-executable matching remain decisive.

The extent is also locally bounded. Linker aliases `D_800F39B0`,
`D_800F39F0`, and `D_800F3A10` are respectively `+0xD70`, `+0xDB0`, and
`+0xDD0` from the base, matching fields inside slot zero. `D_800F569F` is
`D_800F2C40 + 3 * 0xE20 - 1`, and `D_800F56A0` follows immediately. The
interior aliases retain their existing declarations and users.

Verified shared fields and partial arrays are:

| Offset | Shared member | Exact local evidence |
|---:|---|---|
| `0x000` | `field_000`, partial array of `0x8`-byte entries | `func_800593D0` indexes `(arg1 + 1) * 8`; `Model_RunSlotHandlers` advances by 8 and reads the pointer at `+4` |
| `0x1E0` | `field_1E0[58]`, `ModelSlotPart *` (a `GsSEQ`) | `func_800597C8` and `func_8005A468` advance pointers by 4, bounded at runtime by `field_E1B`; `func_8004D58C` bounds the array at 58 by filling the key table that pairs with it at a `0x74` stride |
| `0x2C8` | `field_2C8[10][58]`, `u16` | `func_8004D58C` fills it with `0xFFFF` at a `0x74` stride over ten rows; `func_8004D75C` indexes it `[row][part]`; `Model_ControlSlotAnimation` reaches it as `0x2C8 + current * 116 + part * 2` |
| `0x750` | `field_750[10]`, `ModelSlotRow` | `func_8004D58C` zeroes the 58 halfwords at `0x750 + row * 0x76` and the halfword at `0x7C4 + row * 0x76` in one loop iteration, which is what groups them into one `0x76`-byte record; `func_8004D75C` leaves that halfword holding the largest of the 58, and `func_80058EC0` reads it as `field_BF5 * 118` |
| `0xBEC` | `field_BEC[8]`, part bitfield | `func_8004D58C` sets bit `part % 8` of byte `part / 8`; `Model_ControlSlotAnimation` reads it back the same way; `func_80056250` widens a card for the parts it flags |
| `0xBF5` | `field_BF5` | direct reads in `func_80058E68`, `func_80058EC0`, and `func_800597C8` |
| `0xBF8` | `sound_entries[64]` | `model_slot_setup.c` clears 64 four-byte records; `func_8005106C` reads each record as `{frame, id, flags}` |
| `0xCF8` | `field_CF8`, `0x1C`-byte mixed block | `func_80057E20` and `func_80059000` read threshold bytes `+7`, `+8`, and `+9`; `func_80050F24` indexes the two halfwords at `+0xC`; four setup/transfer paths reset the signed words at `+0x10`, `+0x14`, and `+0x18` |
| `0xD14` | `field_D14` | 80-byte entry selection in `Model_GetSlotDataEntry`, `Model_GetCurrentDataEntry`, and `func_800593D0` |
| `0xD18` | `field_D18` | `Model_BuildCameraRelativeCoordinateUnit` repeatedly reads pointee halfwords `+0x44`, `+0x46`, and `+0x48` |
| `0xD70` | `field_D70[3]`, `GsF_LIGHT`-shaped | `Model_InitLightTriplet` writes three `0x10`-byte records; `func_800540B4` and `func_8004DE24` pass `+0xD70`, `+0xD80` and `+0xD90` to `GsSetFlatLight(0)`, `(1)` and `(2)`; `Model_GetFlatLight` returns one of them |
| `0xDA0` | `field_DA0[3]` | three adjacent clamped `s32` writes in `func_800595C8` |
| `0xDB0` | `field_DB0` | four-word copy/reset block in `func_800594C0` |
| `0xDC0` | `field_DC0[8]` | byte writes in `func_80059590` and selection in `func_80059520` |
| `0xDC8` | `field_DC8[4]` | exact eight-byte copies in `func_80057E20` and `func_80059000`; element 3 is cleared by `func_800597C8` |
| `0xDD0` | `field_DD0[4]` | four adjacent `u16` reads in `Model_CopySlotU16Values` |
| `0xDF8` | `field_DF8` | first property in `Model_SetSlotProperties`; mirrored to `D_8009B488[index]` for all three slots when non-negative |
| `0xDFA` | `field_DFA` | second property in `Model_SetSlotProperties` for slots 0 and 1; negative leaves unchanged |
| `0xDFC` | `field_DFC` | third property in `Model_SetSlotProperties` for slots 0 and 1; negative leaves unchanged |
| `0xDFE` | `field_DFE` | fourth property in `Model_SetSlotProperties`; when non-negative, normalized to a boolean and mirrored to `D_8009B48E[index]` |
| `0xDFF` | `field_DFF` | fifth property in `Model_SetSlotProperties`; when non-negative, normalized to a boolean and mirrored to `D_8009B490[index]` |
| `0xE00` | model-data size, `u16` | `Model_HasInsufficientBufferSpace` subtracts this value from the remaining model-data bank capacity |
| `0xE06` | `field_E06` | shifted read in `func_80058E94`; write/read in `func_800597C8` |
| `0xE0D` | `field_E0D` | `Model_GetSlotAnimationSpeed` reads it and `func_8005969C` writes it |
| `0xE11` | `field_E11` | `func_800590DC`, `func_80059284`, and `func_800595C8` |
| `0xE12` | `field_E12` | read/conditional write in `func_80059AA8` |
| `0xE14` | `field_E14` | sentinel test in `func_80058DD8` |
| `0xE16`-`0xE18` | byte fields | notification test in `func_8005969C`; clamp/index fields in `Model_GetSlotDataEntry` and `Model_GetCurrentDataEntry` |
| `0xE1A` | `field_E1A` | head-entry count in `Model_RunSlotHandlers` |
| `0xE1B` | `field_E1B` | pointer-array count in `func_800597C8` and `func_8005A468` |
| `0xE1F` | `field_E1F` | status tests in `func_80058DD8` and `Model_RunSlotHandlers` |

`field_000` is still a one-element declaration: it expresses verified element
layout and stride only, because the runtime count establishes it is an array
without proving its static bound.

`field_1E0` and the run at `0x750` are no longer in that position. The reset
in `func_8004D58C` walks both to fixed bounds -- ten rows and 58 parts, from
its `i < 0xA` and `j < 0x3A` loops and their `0x74` and `0x76` strides -- and
those bounds tile the record exactly from `0x1E0` to `0xBEC`, so the header
declares `MODEL_SLOT_PART_COUNT` and `MODEL_SLOT_ROW_COUNT` rather than
guessing.

Each `field_1E0` entry points at libhmd's `GsSEQ`, the per-part animation
sequence. `func_8005C6A0` passes the table to `GsLinkAnim` as `GsSEQ **`, and
the game touches the fields the way libhmd defines them. `func_80056250`
rearms a part with `ti = start`, `sid = start_sid`, `ii = aframe = 0xFFFF`,
`rframe = 0` and `speed = 0x10`. `func_8004D75C` reads `start` as the part's
first command key and stamps `start_sid`. `func_8004DC38` seeks a part by
writing `rframe`, `tframe`, `ci` and `ti`. `model.h` mirrors the layout as
`ModelSlotPart` so it does not need the libhmd chain, and
`model_slot_updates.c` asserts that the mirror and `GsSEQ` agree field by
field.

### Variadic slot-property update

Matching `Model_SetSlotProperties` (`0x80053248`) takes a
slot index followed by signed 32-bit properties. A negative value leaves its
corresponding field unchanged. The arguments themselves must still be supplied:
five properties for slots 0 and 1, or one for slot 2. The
first property writes `field_DF8` for any of the three slots and mirrors it to
`D_8009B488[index]`. Slots 0 and 1 consume four more properties for
`field_DFA`, `field_DFC`, `field_DFE`, and `field_DFF`; the final two are
normalized to zero or one and mirrored to `D_8009B48E[index]` and
`D_8009B490[index]`.

Slot 2 consumes only the first property, then refreshes `D_8009AF88` from the
`0xB2`-byte table selected by `D_800F5678[0]`. Every call finishes by setting
`D_8009AF94` to `15`. These writes establish field widths, slot bounds, and
mirror relationships, but not the user-facing meaning of the five properties
or the purpose of the final value `15`. The array supports indices 0 through 2;
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
assigning speculative meanings to the individual offset-named fields.
`main_modes.h` records the two eight-byte inputs as
`AnimatedBattleModelProperties`, retaining mechanical names for the four
properties whose meanings are still unknown.

The animated-battle calls belong to the ordinary initialization branch,
not every mode tick: `D_8009B26C & 0x40` gates initialization, and a first
halfword of `0x309` in `D_800EF658` selects another path instead of the
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

### Timed model-tint requests

Matching `func_80058938` and `func_800528AC` establish a ten-entry request
array at `D_800F2B50`, with `0x18` bytes per entry. The constructor scans from
entry zero and fills the first record whose low active bit is clear; if all ten
are active, it reaches the end without replacing one. It records one of model
slots 0 or 1, two four-byte colour values, an initial elapsed value of zero,
and a duration equal to twice its fifth argument. Optional variadic indices
become an eight-byte part-selection bitset. These are storage and control
relationships, not names for the remaining offset-based fields.

The shared model tick `Model_UpdateScene` calls `func_800528AC` after the ordinary
slot draw/update helpers. For each active request whose selected model slot is
active, the processor:

1. computes three colour bytes as linear integer interpolation from entry
   bytes `+0x10..+0x12` toward `+0x14..+0x16`, using elapsed `+0x0C` over
   duration `+0x0E`, while retaining byte `+0x13` as the fourth channel;
2. saves the slot colour at `field_DC0`, `field_BF5`, `field_E06`, and the
   affected per-part bytes, installs the request values, and redraws the slot;
3. clears the two draw-context globals, restores every saved model value, and
   restores the prior `func_80059AA8` state;
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

## `D_800F56F0`: 32-byte reference-view record

The eight-word block at `D_800F56F0` has the exact target layout of Psy-Q's
`GsRVIEW2`: three 32-bit viewpoint coordinates, three 32-bit reference-point
coordinates, a 32-bit roll value, and a parent-coordinate pointer.
`func_800530C4` initializes all eight words and passes the block to
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
`func_80057E20`, `func_80058DD8`, `Model_GetSlotAnimationSpeed`, `func_80058E68`,
`func_80058E94`, `func_80058EC0`, `Model_GetSlotDataEntry`,
`Model_GetCurrentDataEntry`,
`Model_CopySlotU16Values`, `func_80059000`, `func_800590DC`,
`Model_InitLightTriplet`, `func_80059284`,
`Model_BuildCameraRelativeCoordinateUnit`, `func_800593D0`,
`func_800594C0`, `func_80059520`, `func_80059590`, `func_800595C8`,
`func_8005969C`, `func_800597C8`, `func_80059AA8`, `Model_RunSlotHandlers`,
`func_8005A468`, and `Model_SetSlotProperties`.

## `D_800F5918`: 80 handler registry entries

`ModelHandlerRegistryEntry` is an eight-byte pair: `handler_value` at `+0`
and `key` at `+4`. `Model_RegisterHandlerKey` and `Model_FindHandlerKey`
both advance by eight bytes for exactly 80 iterations. The target assembly,
GMS's paired `dword_800F5918`/`dword_800F591C` views, and Unchiga's
same-address functions agree on the order and count. The modeled extent is
`80 * 8 == 0x280`; `gAiScript_aMemory` (`0x800F5B98`) starts exactly at
`D_800F5918 + 0x280`.

All five functions that reach the registry now use the shared entry type and
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

## Migration snapshot and exact-code exceptions

`notes/global-usage.csv` is the authority for the current matching-C and
assembly users of `D_800F2C40`; those sets change whenever another function
is integrated and are not duplicated here.

Five additional matching-C functions include `model.h` but retain raw
model-base views:

- `func_8004D914` uses `MODEL_SLOT_SIZE` while preserving explicit accesses
  across a large display-list construction path.
- `func_8005611C` clears and initializes one `0xE20`-byte slot through
  explicit offsets while preserving its accepted compiler schedule.
- `func_80056250` uses `MODEL_SLOT_SIZE` while keeping the duel-side layout
  pass in raw offsets.
- `func_80059700` uses shared `field_E0D` and `field_E16` members while
  selecting the signed velocity passed to `func_8005A468`.
- `Model_HasInsufficientBufferSpace` includes `model.h` for
  `MODEL_SLOT_SIZE`, but retains a byte-array extern so its `+0xE00` size load
  keeps the accepted address construction.

`func_80050584` now uses `ModelSlot` and `GsCOORDUNIT` for both model slots
and their placement units. Its repeated `field_D18` member expressions are
intentional: GCC 2.8.1 must reload the coordinate pointer before each rotation
and translation store to preserve the retail schedule. The case-10 staging
path in `file_transfer_steps.c` likewise reaches the sound-entry run,
`field_CF8` block, and `field_E14` through the shared slot layout. The three
reset words inside `field_CF8` are now explicit signed members at relative
offsets `0x10`, `0x14`, and `0x18`; the whole-block copy retains its separate
`ModelSlotCF8BlockWords` view because its alignment and move width are
code-generation inputs.

The interior alias `D_800F3938` now has a separate
`ModelSlotCF8TailView`, rooted at slot offset `0xCF8` and extending through
the property bytes at relative offsets `0x106`/`0x107` (slot
`field_DFE`/`field_DFF`). Its first twelve bytes are one
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
- `func_800593D0` keeps its byte-pointer chase through the leading
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

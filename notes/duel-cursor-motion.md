# Shared display object for cursor motion

`DuelSelectionRecord.cursor_object` points to the canonical `DisplayObject`,
rather than the separate `DuelFieldCursorObject` prefix view. The cursor display-object
constructor in `duel_field_display_objects.c` allocates that same pool
object, and `DuelCursor_ProcessGridDirection` only accesses its fixed members: it does not
allocate, copy by the private type's size, or advance a private-type array.

| Former member | Offset | Canonical member |
| --- | --- | --- |
| `x` | `0x28` | `position.h.field_28` |
| `y` | `0x2A` | `position.h.field_2A` |
| `target_x` | `0x2C` | `field_2C.h.field_2C` |
| `target_y` | `0x2E` | `field_2C.h.field_2E` |
| `step_x` | `0x36` | `field_34.h.field_36` |
| `step_y` | `0x3A` | `field_38.h.field_3A` |
| `steps` | `0x60` | `field_60` |
| `moving` | `0x6C` | `field_6C` |

The removed view was `0x6E` bytes because its last byte was followed by
halfword alignment. That was not the pool allocation size: the canonical
slot remains `0x70`, as checked by `display_object.h`. All eight member
offsets and widths agree; local assertions retain the motion offset checks.

Current and target coordinates are signed reads in this consumer. Explicit
`s16` casts preserve that interpretation where the canonical union exposes
an unsigned halfword. The step and countdown assignments retain their low
halfword stores, and the moving latch remains a byte. These meanings belong
to the cursor-motion path, not every object kind that reuses the fields.

The former padded `GridCursor` view is replaced by `DuelSelectionRecord` in
the direction step and held-input forwarder. The field-display constructor
stores the cursor object at +4 in the same backing record; its +0x0F/+0x10
coordinates are the selection record's column and row, and the status
wrappers return its byte at +0x19. Neither helper allocates a record or
advances a private-view array. Seven local offset assertions and the
canonical record's existing 0x1C size assertion retain the layout checks.

| Former cursor member | Offset | Selection record member |
| --- | --- | --- |
| `object` | `0x04` | `cursor_object` |
| `col` / `row` | `0x0F` / `0x10` | `col` / `row` |
| `min_row` / `max_row` | `0x11` / `0x12` | `(s8)field_11` / `(s8)field_12` |
| `page` | `0x18` | `field_18` |
| `flags` | `0x19` | `status` |

The bounds remain signed reads, including bytes 0x80 through 0xFF. Their
storage and the page selector keep their existing neutral canonical names;
the cursor consumer's roles are not asserted for every selection phase.
The status bits and wrapper return widths are unchanged. The narrower
`DuelCursorStatus` wrapper view remains intentional.

In particular, the ambient direction
arm in `func_80023D08.h` and the ambient-source call to `func_8002348C` are
not normalized. The ownership witness checks that the stored pointer is
`DisplayObject *` and that both one-argument and two-argument declaration
arms still accept only their measured function-pointer signatures.

## Signed field-display coordinate reads

The field text builder (`0x80023144`) and transition stepper (`0x800235C0`)
read the canonical object's +0x28/+0x2A position pair and +0x30/+0x32
output pair as signed halfwords. Their ten reads use explicit `s16` value
casts rather than rebuilding signed pointers to those already-typed members.
This preserves negative coordinates, including stored halfwords 0x8000
through 0xFFFF, without changing the shared unsigned storage or assigning
the coordinate interpretation to other object kinds.

The casts occur before the existing 8.8-fraction assembly, interpolation
arguments and text-position offsets. Fraction bytes, arithmetic order,
halfword stores, completion snaps and status handling are unchanged.

## Active selection-record pointer

`D_8009B1B4` stores a `DuelSelectionRecord *`, not a pointer to the narrower
pick-cursor view. Every resident assignment chooses a full record in the
selection table: a side base or one of its +0x1C/+0x38/+0x54 aliases, using
the existing side stride. No writer publishes a standalone 0x1A cursor.
The phase-entry hand store at +8 and the battle/Exodia cursor-object access
at +4 therefore use the canonical members without bridge casts.

Both phase-entry hand-pointer stores now use that same canonical member.
The removed `DuelSelectionSideView` only described the prefix through +8;
no code allocated, advanced or copied records using its prefix size. The
canonical hand-at-+8 and 0x1C record-size assertions remain in force.

The global remains a four-byte gp-relative pointer. The table aliases,
operand order and side-index calculations remain distinct to preserve their
relocations. `DuelCardPickCursor` still has its checked 0x1A size and
separate role at the pick-cursor API; that call uses an explicit view.
The field-display consumer also retains its phase-specific view. These are
intentional member/ABI views, not alternate declarations of the global.

## Grid-direction handlers

`DuelCursor_ProcessGridDirection` (`0x80023D08`) starts a bounded grid move
from direction bits 0/1, publishing the cursor target, velocity, duration and
movement requests. The frame integrator remains `0x800235C0`. The helper
also completes a settling move: status bit 0x80 makes it wait for the global
movement request to clear, then refresh the field text, clear status bits
0xC0 and play sound 6. That arm precedes the negative-direction check.

`DuelCursor_ProcessHeldGridDirection` (`0x80023FBC`) derives that argument
from held input, not newly pressed input. With a stable pad word, the
RIGHT/DOWN/LEFT/UP tests make UP win over LEFT, then DOWN, then RIGHT; no
direction produces -1. Each test retains its own volatile read, so this is
not a promise of a single input snapshot or a combined diagonal direction.
Both helpers return void; the existing status wrappers remain separate.

The address-based `FUNC_80023D08_AMBIENT_DIRECTION_ARG` switch and the
no-expression settling call are unchanged. Naming does not normalize either
measured argument boundary.

## Shared view defaults

`ViewState_ResetDefaults` (`0x80017130`) applies the measured default
values to the shared view prefix and reference view, sets the projection
and calls `ViewState_ApplyOrbit`. Its two observed resident calls,
`0x800178C8` and `0x80017970`, bracket reference sampling.

The name deliberately uses the shared ViewState subsystem. It does not
assign the defaults to one particular screen, restore arbitrary caller
state or imply that unknown bytes beyond the measured record are cleared.
The original second `s32 *` base into the view remains: replacing its
three target stores with direct members loses a retail instruction.
All pointer identities, store order, SDK calls and source/profile owners
are preserved.

## Four-channel duel view tween

`Duel_StartViewTween` (`0x80022D94`) prepares the resident duel transition
stepper's distance, orbit-angle, heading and reference-target-Z channels.
Eight observed resident calls come from field actions, trap presentation,
turn switch, field-display transitions and cursor movement. The last
channel writes `GsRVIEW2.vrz`; it is not view rotation.

The setup still captures the first three current values as signed
halfwords and target Z as a full word, computes the same signed 16.16
deltas, biases starts by `0x8000` and stores the same halfword targets and
remaining-frame count. The stepper increments those accumulators, applies
orbit and snaps to each saved target on the last frame. Setup itself does
not advance the view.

`gDuel_wViewTweenFramesRemaining` (originally `D_8009B204`, North
American `0x8009B204`) is that scalar counter. Init clears it, setup
narrows the unchanged full-word input into the halfword, and the stepper
uses the existing unsigned decrement and zero test. The signed/default
and unsigned/initializer declaration arms, private
`D_8009B204_UNSIGNED` selector, GP addressing and zero/wrap behavior are
retained. The unrelated French SDK label at the same numeric address is
untouched.

The distance channel keeps three separate scalar cells:
`gDuel_wViewTweenDistanceTarget` is the signed halfword snap target,
`gDuel_dwViewTweenDistanceAccumulator` is the signed 16.16 value with the
existing half-unit bias, and `gDuel_dwViewTweenDistanceStep` is its signed
16.16 delta. Their scattered addresses, widths, GP accesses and all
shift/division/update/snap operations remain unchanged; they are not
combined into an invented state object.

The orbit-angle channel likewise retains a signed halfword target,
`gDuel_wViewTweenOrbitAngleTarget`, and separate signed 16.16 cells
`gDuel_dwViewTweenOrbitAngleAccumulator` and
`gDuel_dwViewTweenOrbitAngleStep`. They feed the existing `field_04`
channel without declaring that shared field to be pitch in every context.
The linear delta, half-unit bias, arithmetic shift and final snap remain
unchanged; no angle wrapping or validation is introduced.

The heading channel's signed target,
`gDuel_wViewTweenHeadingTarget`, and signed 16.16
`gDuel_dwViewTweenHeadingAccumulator`/`gDuel_dwViewTweenHeadingStep`
feed the existing heading field with the same unwrapped arithmetic.
Japanese producer/stepper wrappers preserve their explicit localized
target and step bindings; only those symbol names change, not their
addresses. No shared heading type, caller arguments, bias or snap changes.

The reference-Z channel retains
`gDuel_wViewTweenReferenceZTarget` as a signed halfword and
`gDuel_dwViewTweenReferenceZAccumulator` /
`gDuel_dwViewTweenReferenceZStep` as separate signed 16.16 words.
The current `GsRVIEW2.vrz` and setup argument remain full words even
though the saved snap target is narrowed. Its arithmetic, bias, GP access
and final sign-extended target assignment are unchanged; this is not the
view rotation channel or a new wider target type.

Semantic parameter/local names and fixed-width word aliases do not narrow
the inputs, add frame validation or wrap heading deltas. Every operation
and store remains in its original order. The shared ViewState members
retain their existing names because changing them would require overlay
edits outside the current main-game scope.

## Ground-projection reference initialization

`Duel_InitGroundProjectionReference` (`0x800178BC`) is called from
`Duel_InitScene` at resident JAL `0x80017B38`. It resets the shared view
defaults, selects the fixed reference view values and side-0 angle, then
projects `(1000, 0, 1000)`. The stored halfwords `D_8009B200/D_8009B202`
are the projected X/Y values minus the 160/108 screen center.

The name describes that observed initialization only. No tracked C reader
of the stored pair has been identified, so their downstream purpose is not
called a scale, velocity or cursor step. The neutral view-field names and
output-global names remain unchanged.

`ViewState_ResetDefaults` runs before and after the sample. It resets defaults; the
pair of calls does not save and restore the caller's arbitrary prior view.
Scene initialization applies the active side's angle afterward. All
scratchpad/GTE order, unsigned packed X, signed Y intermediate, geometry
offset reset, named RTPS profile and address-based source owners remain
unchanged.

## Field-grid ground coordinates

`gDuel_aFieldGridGroundPositions` (originally `D_80090800`, North American
`0x80090800`) supplies the ground-plane X/Z pair for each side's 20 field
cells, using the same perspective and cell index as the live-record map.
Its 160-byte `DuelFieldPosition[2][20]` initializer is unchanged.

These are inputs to projection, not screen pixels.
`Duel_CreateFieldCursorObject` copies the pair into object position
`+0x28/+0x2A` and installs `DisplayObject_ProjectGroundPosition`.
The callback constructs `(position28, 0, position2A)`, performs RTPS and
writes the separately biased screen output at `+0x30/+0x32`. The sprite's
screen-space rendering flag does not make those stored inputs pixel
coordinates. Cursor movement interpolates those same input components.

Field-effect setup independently copies the pair to request offsets
`+0x00/+0x04` with `+0x02` zero. The shared pair type retains its generic
`x`/`y` member names; the second member carries ground Z for this table.
Signed storage, explicit unsigned-halfword views, byte-pointer walks and
integer-added addressing all remain intact. The separate thirty-card
position table and the unrelated French SDK label at numeric `0x80090800`
are untouched.

## Field-grid record map

`gDuel_abFieldGridCardRecordIndices` (originally `D_800907D8`, North
American `0x800907D8`) maps each side's 20 visible field cells to live
`DuelCardRecord` indices. The initialized 40-byte table presents the other
side's ten field records in reverse order, then the acting side's ten
records in normal order. Row/column indexing agrees with the neighboring
position table; it is not the separate five-slot hand map.

The cursor, effect, AI and field-action readers use the selected byte to
reach the live record. Scene initialization and turn switch retain a
pointer to the selected 20-byte perspective. All selection arithmetic,
byte reads, side strides and stored-pointer behavior stay unchanged.

The declaration keeps its existing three arms: unsized flat readers,
explicit 2-D readers and two asm-bound views. The private
`D_800907D8_2d`/`D_800907D8_flat` C identifiers and the
`DUEL_FIELD_GRID_ALIASES`/`DUEL_FIELD_GRID_2D` switches stay in place;
their asm target follows the semantic name.
Japanese wrappers preserve their first-declaration binding to the
localized target, now `gJapanese_DuelFieldGridCardRecordIndices` at
`0x80090688`. Its unrelated result table at numeric `0x800907D8`, and the
French SDK label `D_800907D8_French`, are not this grid and remain untouched.

## Field-placement selection entry

`Duel_BeginFieldPlacementSelection` (`0x8001B938`) starts the destination
selection used by `DuelScene_UpdateHandActions`. The two direct resident
calls, at `0x8001C498` and `0x8001CDEC`, enter it from the AI placement path
and interactive placement path respectively.

It selects record 2 of the active side's selection table, initializes
cursor state, and sets the row interval to `[2, 3)` by default. During
hand selection, a magic-or-later type with nonzero facing, or a trap,
instead selects `[3, 4)`. The direction handler's signed lower-bound and
exclusive upper-bound checks independently establish those interval
semantics. The helper starts on that row and prefers the first column
whose live record is unoccupied. If every column is occupied, it retains
the previous column; no fallback or empty-slot guarantee is added.

The final `D_8009B162 = 3` requests the selection-display transition.
The controller waits for that request to clear, drives the cursor and
only later consumes its row/column as the placement destination. The
helper neither commits the play nor constructs the placed card.

Its address-based source/header paths remain the owners of the existing
two-function group. The second function, `Duel_ApplyAiHandSelection`,
applies script picks to the hand as described in
[the hand-display notes](duel-hand-display-slots.md#applying-ai-hand-picks).
Neither function's order, compiler profile or grouped extent changes.

## Selection display-parent slot

The canonical selection record's first word is a stored `DisplayObject *`,
with the neutral name `field_00` retained. This is not inferred from the
initializer's zero store: the field-display transition creates a display
object, stores it at +0, slides it and later releases/clears it.
`DuelSelection_LinkDisplayObject` reads the same slot as `position_base`
when installing child callbacks.

`DuelSelection_UpdateLinkedObject` now reads that pointer through
`DuelSelectionRecord`, rather than a second padded `DuelSelectionDisplayRecord`
view. It follows the parent's output position, retires the callback when the
parent's `field_6C` latch is zero, and releases the child when the parent slot
is null. The signed record selector, 0x1C record stride, side offset, byte-base
arithmetic and these branches are unchanged. The canonical offset-zero and
existing 0x1C size assertions cover the removed view's layout contract.

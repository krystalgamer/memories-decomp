# Field information, selection records and linked hand objects

This contract joins the five functions in
[`duel_field_display_objects.c`](../src/game/duel_field_display_objects.c)
with the two
[`parent-link helpers`](../src/game/display_parent_links.c) and the existing
[`linked-object callback`](../src/game/duel_card_turn_animations.c).
All claims describe the matching C and its real producer/caller chain, not a
new runtime capture.

## One actual selection record

The field display, placement and hand-scene callers already select whole
`DuelSelectionRecord` elements from `gDuel_abSelectionSides` or its measured interior
aliases. `Duel_UpdateDisplayTransition` receives the same object through
`gDuel_pActiveSelectionRecord`. The former `DuelFieldDisplaySource` and
`DisplayParent` duplicated this object with different member names; they
were not separate allocations.

The shared record remains `0x1C` bytes, four records per side. Its existing
panel pointer at `+0x00`, cursor pointer at `+0x04`, hand-cache pointer at
`+0x08` and halfword at `+0x0C` keep their widths and offsets. The panel and
cursor pointers remain distinct; nothing assumes they point to the same
display object.

| Offset | Canonical member | Field-display use |
|---|---|---|
| `0x0F` | `col` | Signed grid column. |
| `0x10` | `row` | Signed grid row and existing Swords-side selection. |
| `0x13` | `field_13` | Existing resource-variant selector; no broader rename is inferred. |
| `0x14` | `field_14` | Existing text-box channel selector. |
| `0x16` | `text_y_offset` | Signed byte added to the panel's screen Y; transition entry writes `0x24` or `7`. This was live data, not padding. |
| `0x17` | `record_index` | Ordinal set by `Duel_InitSelectionRecords`; selects cursor resources/styles and tags linked objects for callback lookup. |
| `0x18` | `field_18` | Existing mode byte, retained without a new interpretation. |

The initializer still writes the original partial set of fields; it does not
gain a text-offset, padding or whole-record clear. Existing size/offset
contracts are retained and extended to the signed coordinates, text offset
and ordinal. Hand-context `field_15` semantics and the other selection records
are not changed.

## The linked entries are the hand cache

This reuse is established by ownership, not equal-sized layouts:

- The only nonnull writes to a selection record's `+0x08` pointer publish
  `gDuelHand_aSlots`; initialization writes null there.
- The transition coordinator reads that same record, positions/releases the
  first and second pointers of each entry, and passes the same parent to the
  linking helpers.
- `DuelSelection_UpdateLinkedObject` reconstructs its parent from the stored
  ordinal using the actual `0x1C` record and side strides, then reads that
  record's panel pointer.

The old `DisplayLinkEntry` is therefore replaced by the actual `DuelHandSlot`,
not by a speculative merge of unrelated twelve-byte objects. The primary
object and `order_marker` retain their `DisplayObject *G32` storage and
`0x0C` slot stride.

`DuelSelection_LinkDisplayObjects` keeps its null-cache condition and the
exact order: attach the current cursor, clear its stored handle, then attach
the primary and marker objects of each of five slots. The marker cursor is
now the proper `DisplayObject *G32 *` derived from `&entry->order_marker`;
no whole-record cast at an interior pointer is needed. `entry++, slot++`
remains in the matched order.

When requested, the two slot handles are cleared after linking. This is not
an immediate destruction or a new lifetime guarantee. The callback keeps its
original behavior: follow the panel while it exists, stop updating once the
panel settles, or release the object when the stored panel pointer is null.
No parent-side capture, null guard, allocation rule or order-byte reset is
added.

## Field-info APIs

| NA address | API | Exact role |
|---|---|---|
| `0x80023090` | `DuelField_GetGuardianStarDisplayCode` | Looks up the two selection records' signed grid coordinates and returns `4`, `1` or `6` for a zero, negative or positive guardian-star bonus. Not a damage or battle-strength result. |
| `0x80023144` | `DuelField_CreateCardInfoTextBox` | Stages live-card visibility/ID, guardian star, effective stats and the existing Swords presentation, then creates the mode-specific text box. |
| `0x8002348C` | `DuelField_RefreshCursorCardInfo` | Maps the current signed grid position to a live card-record index and calls the builder. |

The builder's integer argument is a `gDuel_aCardRecords` index, not a card ID,
AI snapshot slot or selection-record ordinal. Its empty/hidden-card behavior,
own-side reveal override, guardian-star choice, signed Swords clamp, style
selection and unsigned display-depth byte interpretation remain unchanged.
The comparison retains its first typed byte-offset calculation before the
record-base load and the second record's independent table alias.

The settling-move path still supplies no argument expression: it calls the
refresh through a `void (*)(void)` cast. `DuelCursor_ProcessGridDirection`
deliberately leaves the actual record in `$a0`; no synthesized argument or
new movement step is introduced.

The coordinator retains both empty `do/while` scheduling barriers, request
entry order, view-tween accumulators, replay-object loops, linked panel slide,
text movement, pointer releases, pending-model gate and busy result. This
typing/naming change is not a transition-state or animation rewrite.

## Native bindings and acceptance

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Guardian-star display code | `0x80023090` | `0x80022ED8` | `0x80023008` | `0x8002314C` |
| Card-info text-box builder | `0x80023144` | `0x80022F8C` | `0x800230BC` | `0x80023200` |
| Current-grid refresh | `0x8002348C` | `0x800232D4` | `0x80023404` | `0x80023548` |

Japan's builder previously used a native address-name macro. The semantic
name binds directly to `0x80022F8C`; only the obsolete two wrapper macros are
removed. All native addresses, sizes, row order, ownership, notes, compiler
profiles and source grouping are retained. The `gcc_2_8_1_g8` parent-link
unit remains separate from the `gcc_2_8_1_g8_split` field-display unit.

The canonical-type stage reproduces all sixteen complete compiler/MASPSX
artifacts for eight field/parent/caller units raw-identically. The named stage
compares those complete artifacts after reversing only three function names;
no instructions, directives or relocation structure are filtered. Full clean
executable matching remains the acceptance gate.

The existing declaration-removal compiler probe retains the real canonical
data header when withholding the field API header. Its cases, expected
implicit-callee result, diagnostics and compiler controls are unchanged.
No new harness, fixture, scenario or source-string test is introduced;
regional/cloud tests remain PR validation, not local runs.

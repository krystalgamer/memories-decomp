# Name-entry type ownership

The name-entry headers now import their game types from `src/ygo_types.h`
instead of defining private layouts. The screen's C bodies, callback
signatures, function order, compiler profiles and data are unchanged.
In particular, this does not alter the pinned keyboard function slated for
separate reclassification under #3859.

## The selection frame is one object

`NameEntry_Init` allocates one object, writes its position and extents,
installs `NameEntry_DrawSelectionFrame` at `+0x4C`, and publishes the same
pointer as `D_8016D404`. The drawing callback receives that object, while
`NameEntry_UpdateKeyboard` moves and resizes it through `D_8016D404`.
`NameEntry_SpawnGlyphSprite` also reads its position for the cursor case.

The former drawing and tween views agree where they overlap:

| Offset | Drawing evidence | Tween/setup evidence | Central member |
|---|---|---|---|
| `0x14` | Signed ordering-table priority, narrowed only at submission | Previously padding | `priority` |
| `0x30`, `0x32` | Signed frame XY | Initialized and moved as signed XY | `x`, `y` |
| `0x36`, `0x38` | Previously padding | Signed 8.8 movement deltas | `stepX`, `stepY` |
| `0x3C` | Unsigned width | Resized for the finish control | `width` |
| `0x3E` | Unsigned height | Initialized through the allocation's byte view | `height` |
| `0x5E` | Previously outside the drawing prefix | Extra width, 20 for the wide control | `widthBonus` |
| `0x60` | Previously outside the drawing prefix | Eight-update movement timer | `timer` |

`SelectionFrame` now contains this union of observed fields.
`NameEntrySelectionFrameView` is an alias of it, preserving the drawing
interface's spelling without preserving a second struct definition.
The alias's `sizeof` is consequently `0x62`, not the former drawing-only
prefix's `0x40`. No source indexes arrays of the drawing view, allocates it,
copies it by value, or uses its size; its only function interface is the
registered pointer callback. All accessed offsets, signedness and alignment
are unchanged and asserted.

This remains an observed prefix, **not** the complete display-object
allocation. Padding is an unobserved range, not a declaration that the bytes
are unused. The `+0x4C` callback remains inside such a range in this view.

## Glyph sprites and dialog panels stay distinct

The other two views are centralized unchanged, not merged into the frame
or generalized across every display-object kind.

`GlyphSprite` is the constructor/pulse contract.
`NameEntry_SpawnGlyphSprite` obtains the object from the display allocator,
stores the source `DuelEffectEntry *` at `+0x4C`, the text-box slot at `+0x67`,
and the sequence at `+0x6A`. `NameEntry_UpdateGlyphPulse` uses that source
pointer to park the source glyph off-screen, saves its X at `+0x5A`, counts
the pulse through `+0x60`, and restores X when the effect ends. Its
`+0x44/+0x46` are scale during the pulse, but the transfer/caret callbacks
use those offsets as destination XY. The installed callback selects the
meaning; the central type does not turn one interpretation into a universal
display-object field name.

`DialogCaret` is the separate sliding panel created by
`NameEntry_UpdateDialog`, stored in the text-box record's `field_2C`, and
positioned through the interpolation helpers. Its `slide` at `+0x60` is
signed remaining travel, not the selection frame's eight-update timer.
The same view's `status` at `+0x6C` is also used when the dialog controller
finds a completed transfer sprite by tag. Its size (`0x6E`) and the glyph
view's size (`0x6C`) describe their observed prefixes only.

`NameEntryGlyphUpdate` keeps the existing one-argument `u8 *` callback
contract. It moves to the central type file without changing the callback
definitions or installers.

## The source glyph and starter-pool dependencies

Moving `GlyphSprite` centrally also requires its actual source-node type
there. `DuelEffectEntry` moves from `game/duel_effect.h` without changing a
field. The resident compactor copies whole `0x1C`-byte entries with aligned
word loads/stores; the existing size, four-byte alignment and field-offset
assertions move with it. `TextBox_GetGlyphAt` constrains the signed XY pair,
glyph code and active flag that name-entry consumes. The declaration and
APIs for the separate `DuelEffectChannel` stay in `duel_effect.h`.

`NameEntryStarterDeckPool` likewise moves unchanged. Its first halfword is
the draw count; the following `CARD_COUNT` halfwords are weights; 18 trailing
bytes preserve the existing `0x5B8` size. Its reader still uses the measured
halfword cursor and only scans `STARTER_DECK_WEIGHT_SCAN_COUNT` weights.
The seven-pool count, eight-pointer bound and extern declarations remain in
`name_entry_starter_deck.h`. `ygo_types.h` imports the existing, independent
card-constants header so the array bound is not duplicated as a numeric
literal.

The screen's four interface headers therefore contain declarations and
imports, not record definitions. All seven public type names involved in
this contract have one central definition or alias, and the resident
source-node layout retains all its original assertions.

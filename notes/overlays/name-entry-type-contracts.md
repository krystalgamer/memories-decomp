# Name-entry type ownership

The type-ownership change moved the name-entry layouts into `src/ygo_types.h`
without changing the screen's C bodies, callback signatures, function order,
compiler profiles or data. That change did not remove the keyboard's register
bindings. Its separate binding-free implementation now lives in
[`name_entry_runtime.c`](../../src/overlays/password/name_entry_runtime.c)
and retains these same layout contracts.

## The selection frame is one object

`NameEntry_Init` allocates one object, writes its position and extents,
installs `NameEntry_DrawSelectionFrame` at `+0x4C`, and publishes the same
pointer as `gNameEntry_pSelectionFrame`. The drawing callback receives that object, while
`NameEntry_UpdateKeyboard` moves and resizes it through `gNameEntry_pSelectionFrame`.
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

## PAL source-node coordinates remain local, not clone state

PAL EuropeanDuelEffectEntry local_x/local_y name its original u16
field_0C/field_0E at physical +0x02/+0x04. TextBox_GetGlyphAt retains its
explicit (s16) casts and both equality tests. NameEntry_UpdateGlyphPulse
still saves the SOURCE node's X, stores 0x400 and restores the saved value
at the same frame boundary; neither source ownership nor clone scale/
tag/release timing changes. NameEntry_SpawnGlyphSprite retains signed
coordinate casts before adding channel rect_x/rect_y and all PAL atlas
metrics. The shared NA/Japanese x_0C/y_0E spellings, original NA pre-null
code read and unchecked allocation are not changed or generalized.

## Glyph lookup starts from the channel's entry beginning

entry_begin is the original stored entry_head_24 pointer at channel
+0x24. TextBox_GetGlyphAt retains its existing regional channel selection:
shared NA/PAL indexing and the compact Japanese stride followed by the
same shared prefix cast. It still walks local node pointers to the first
inactive flag, not to entry_write_cursor or entry_range_count.

All entry strides, signed equality comparisons and PAL local_x/local_y
casts remain. No new bounds, null guard, pointer ownership, source-node
lifetime or clone behavior is introduced; the query does not move the
channel's entry_begin pointer. No fixture or harness changes are needed.

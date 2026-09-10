# Password Overlay

This directory contains matching source from the password-screen
runtime module.

Verified boundaries:

| Item | Range |
|---|---|
| WA package | sectors `7983-8069` |
| Executable phase | sectors `8054-8069`, `0x7800` bytes |
| Runtime image range | `0x80168000-0x8016F800` |

The [name-entry package](../name_entry/README.md) contains a closely related
`0x7800` phase at sectors `7968-7983`. The two chunks share code but differ in
their final data sector, so they must not be treated as one interchangeable
module image.

Module-scoped symbol evidence is kept in
[`notes/research/Unchiga_Symbols/modules/password.txt`](../../../notes/research/Unchiga_Symbols/modules/password.txt).

The verified executable phase is tracked in
[`config/slus_01411/overlays.json`](../../../config/slus_01411/overlays.json).
`make overlays` extracts it to `tmp/overlays/password/module.bin`, and
`make verify-overlays` checks the archive and payload hashes.

Its accepted build layout is
[`config/slus_01411/overlays/password.yaml`](../../../config/slus_01411/overlays/password.yaml).
`make match-overlays` splits, assembles, links, and verifies the complete
module byte-for-byte. Keep candidate sources, objects, and diffs under `tmp/`
until a function passes this overlay-specific exact-match process. Do not add
this module to the resident `config/slus_01411/matching_c.json`.

## Password-shop lifecycle translation unit

[`shop.c`](shop.c) is the whole password shop screen: ten functions in
executable order from `Password_RefreshDigitDisplay` at `0x80169C30` through
`Password_UpdateShopScreen` at `0x8016A37C`. One
`gcc_2_8_1_g0_split` C subsegment at module offset `0x1C30` covers the full
`0xD00`-byte text extent through `0x8016A930`; the same source owns its rodata
block at module `+0x7C`.

The first three functions rebuild the entered password, publish the starchip
balance, and create the modal messages used by the lifecycle. All three
create fixed text slots through `func_80035BE4`, and the refreshers and
flagged message path drive them through `func_80039A14`.

The merged source keeps two same-symbol views of `D_801D5608`. The starchip
renderer writes its first word through a scalar alias; the purchase dialog
writes the price and card ID through the existing `Pair` view. Combining the
files does not make those two uses one type.

`Main_RunPasswordMenu` waits for the package through its resident loader,
calls `Password_InitShopScreen` (`0x8016A080`) once, then calls
`Password_UpdateShopScreen` (`0x8016A37C`) each tick. Neither entry returns
a completion code. Cancellation writes the saved previous mode to
`D_8009B26C`; the updater does not itself request a disk save or explicitly
tear down every object.

The initializer calls all three text helpers, while the updater calls the
digit refresh after edits, the starchip refresh during payment, and the
message creator throughout its state machine. The same two entry points also
share the cursor helpers, preview recreation, and password lookup that follow
the text functions in this unit.

The initializer clears eight decimal digit bytes, index and shop state,
rebuilds the digit/starchip displays and creates message 226. It configures
the cached preview record, a display object, one digit cursor and **four**
kind-specific cursor decorations, not one decoration per digit.
`func_80029574(0)` only clears the cached record's two display pointers; it
does not load a background. The halfwords `320,256,512,240` feed texture
configuration, not proven background dimensions, and the separate selectors
`2,3` are not established pixel widths.

The contiguous cursor update, decoration, and target helpers open the shop
translation unit in executable order. They jointly use the selected digit
index and the published cursor widget, matching their initialization and
update call sites in the same source.

The cursor's provisional `(256,120)` is replaced during initialization by
its target `(163,99)` for index zero. `Password_UpdateDigitCursor`
(`0x80169E20`) increments byte `+0x22` on every callback, including idle
ones. Movement bit `0x40` additionally enables an eight-update signed-8.8
tween from current `+0x30/+0x32` to target `+0x18/+0x1A`; completion snaps
XY and clears bits `0xC0`, leaving the callback installed. No rotation axis
or real-time duration is inferred from the phase byte.

### Function order and boundary

| Address | Function | Callers |
|---|---|---|
| `0x80169C30` | `Password_RefreshDigitDisplay` | init and update |
| `0x80169D10` | `Password_RefreshStarchipDisplay` | init and update |
| `0x80169D84` | `Password_CreateMessageBox` | init and update |
| `0x80169E20` | `Password_UpdateDigitCursor` | installed by init |
| `0x80169F38` | `Password_UpdateDigitCursorDecoration` | four init-installed callbacks |
| `0x8016A00C` | `Password_SetDigitCursorTarget` | init and update |
| `0x8016A02C` | `Password_RecreateCardPreview` | 2, both inside |
| `0x8016A080` | `Password_InitShopScreen` | 1, resident |
| `0x8016A304` | `Password_LookupCardID` | 1, inside |
| `0x8016A37C` | `Password_UpdateShopScreen` | 1, resident |

As everywhere in this module, one compiler profile covers the whole overlay,
so a profile change does not mark this boundary. The two externally reached
functions are the pair `main_run_frontend_menus.c` calls: init once and update
per tick. Every other function in the table is called or installed by that
pair.

`NameEntry_BuildStarterDeck` follows this run in the image and is **not** part
of it. Its caller is [`name_entry_main.c`](name_entry_main.c), not the shop,
so the run stops at `0x8016A930` on the call graph rather than at a gap.

One unit removes all source-boundary ambiguity around
`gPassword_pDigitCursorWidget`: the cursor helpers, initializer and updater
now share `PasswordCursorView` in one source.
While checking that, the paragraph in `shop.h` describing the disagreement
turned out to contradict the header it sits in - the declaration it says is
absent was added above it at some point and the paragraph was never updated.
That is corrected here.

### Preview recreation and shared interfaces

`Password_RecreateCardPreview` (`0x8016A02C`) releases both cached slot-0
display pointers, recreates the composite preview from that slot's existing
card/resource state, sets control-object Y to 30 and phase to `0x80`, sets
flag `0x4`, and publishes `D_8016D4D8`.
Its callers' `1` and selected-card arguments do **not** select the preview.
The shared declaration explicitly retains an ignored `s32` parameter to
preserve their emitted setup; the body still gets the card through cached
slot 0. In particular, the initializer's literal `1` does not prove that the
initial preview is card 1.

[`shop.h`](shop.h) is included by definitions, installers and the resident
caller. It provides the two entrypoints, the preview contract and a common
one-argument byte-pointer type for the cursor and decoration callbacks.
`PasswordCardPreviewView` describes only the control prefix: flags at `+8`,
phase at `+0x21`, and the raw halfword Y at `+0x32`. It does not describe the
whole composite allocation or assign a rotation axis. The phase offset is
`0x21`, not `0x33`; the old `f33` field name used a decimal offset.
Conditional resident imports live in `c_symbols.ld`; the function definitions
remain in this overlay, not the resident inventory.

### Shop state machine

The updater services text **before** testing its gates. It then waits while
the cursor is moving or `(D_800EB12C & 0x2008) != 0x2000`; the latter reads
the slot-0 text flags for completion and pending acknowledgment. The low five
bits of `D_8016D424` choose the following states. Its `0x8000` and `0x4000`
subflags are state-local bookkeeping, not shared campaign-flag modifiers.

| State | Work and transition |
|---:|---|
| 0 | Edit digits or leave the screen; successful nonzero lookup enters 1 |
| 1 | Request cached slot-0 card resources, wait, recreate and reveal preview; phase wrap enters 2 in the same invocation |
| 2 | Publish price/card formatting values, show used-password or price-dependent dialog, then award or enter 4 |
| 3 | Incrementally subtract price from RAM starchips; zero remaining enters 4 |
| 4 | Advance preview hide phase, recreate message 226 and return to 0 without clearing digits or index |

State 0 gives held horizontal input priority, with Right winning over Left.
Movement clamps at indices 0 and 7; an out-of-range request returns without
sound or lower-priority input handling. Next comes repeat/new-press vertical
input, with Up winning over Down and decimal digits wrapping 0-9. Newly
pressed Circle then cancels, and newly pressed Cross finally attempts lookup.
Square alone and Start are not confirmation here. These use three distinct
volatile streams: held `D_8009B3A4`, repeat `D_8009B394`, and newly pressed
`D_8009B398`.

State 1 first requests the selected card into cached slot 0. It waits for
`((D_8009B0F4 & 0x2000030) | D_8009B134) == 0` before recreating the
preview. Later eligible updates add 8 to preview phase `+0x21`; wrapping
from the initialized `0x80` to zero takes sixteen such updates. State 4
similarly advances from zero until the phase is negative as `s8`. These are
eligible-update counts, not wall-clock timings.

### Award, dialogs and payment are separate

State 2 tests shared flag `0x400 + card`. A previously used password displays
message 229 and proceeds to hiding without an award. Otherwise the handler
chooses message 228 when balance is below price, or 227 when sufficient.
Both paths delegate the next decision to the dialog's signed choice value;
the award branch does not repeat the affordability comparison. Exact captions
and message-specific enabled choices remain unverified by the pending human
trace, so this body alone does not establish how message 228 prevents choice
zero.

Choice zero sets the password-used flag and calls `Duel_AwardCard` **before**
payment begins in state 3. The award helper caps ordinary chest quantity at
250 and records the card in the recent-card list; this updater has no separate
chest-cap refusal. Payment uses the first word of the card's eight-byte
cost/password record and subtracts from both unsigned remaining cost and
`gLibrary_dwStarchips` (`SaveDataState.starchips` at `+0x5E0`, address
`0x801D07E0`).

For current remaining cost `c`, the deduction starts at 1. Successive
thresholds replace it with `c/10` at 10, `c/20` at 100, `c/30` at 1000,
and `c/40` at 10000; the largest applicable threshold wins. Integer division
is used and a zero step is replaced by 1. The starchip display is refreshed
on every payment update, including the last. There is no confirmation/cancel
poll or disk-save request in this state.

The literal zero-price case would subtract 1 from unsigned zero remaining
cost and from the balance. No reachable zero-price record was established
here; that arithmetic is preserved, not silently repaired or claimed as a
runtime reproduction. Allocation and cursor/preview dereferences likewise
retain their existing unchecked behavior.

## Name-entry finalization translation unit

`name_entry_main.c` keeps `NameEntry_BuildStarterDeck` (`0x8016A930`)
immediately before its only caller, `NameEntry_Main` (`0x8016AA6C`). After
name acceptance, the main routine builds the weighted starter deck, hashes the
completed player-name bytes, and stamps a nonzero save identifier.

The generator consumes the checked `NameEntryStarterDeckPool` layout through
the null-terminated `gNameEntry_apStarterDeckPools` table. Seven records at
`gNameEntry_aStarterDeckPools` contain one draw count, 722 weights and 18
padding bytes each; see
[`notes/starter-deck-pools.md`](../../../notes/starter-deck-pools.md).

Both functions use `gcc_2_8_1_g0_split`. One C subsegment at module offset
`0x2930` covers the complete `0x208`-byte range through `0x8016AB38`; the
unmatched module text begins there. The source also retains
`NameEntry_Main`'s rodata at module `+0x90`.

## Name-entry lifecycle translation unit

`name_entry_runtime.c` contains the complete matched name-entry pipeline:
fourteen functions in executable order from `NameEntry_BuildKeyboardTextBox`
at `0x80168138` through `NameEntry_PollCompletion` at `0x80169C08`. One
`gcc_2_8_1_g0_split` C subsegment at module offset `0x138` covers the full
`0x1AF8`-byte text extent through `0x80169C30`, and the same source owns the
glyph atlas rodata at module offset `0x4`.

The two external doors are the lifecycle pair: `NameEntry_Init` builds the
screen once, while `NameEntry_PollCompletion` advances it each frame and
reports acceptance. Everything between them is connected by stores and calls:
initialization installs the selection-frame callback, the keyboard and dialog
handlers create glyph sprites and install their update callbacks, and those
callbacks share the same glyph lookup and `D_8016D400` state block.

The run deliberately starts after `func_801680B4`: adjacency and profile
agree, but no name-entry caller or shared state ownership is proven for that
generic display-object constructor. It stops before
`Password_RefreshDigitDisplay`, where the password-shop text pipeline begins.

## Name-entry selection-frame packets

`NameEntry_Init` installs
[`NameEntry_DrawSelectionFrame`](name_entry_runtime.c)
(`0x801681A0`) at callback offset `+0x4C` of the selection cursor stored in
`D_8016D404`. The initializer sets its position to `(22, 24)` and its
dimensions to `16 x 16`; keyboard movement later updates that cursor's
position and width. This establishes a name-entry selection-frame role,
not a general password-screen or input handler.

The matching drawing routine uses the actual Psy-Q line records rather than
byte-offset views of its scratchpad packets:

| Scratchpad address | SDK record | Source payload words | Use |
|---|---|---:|---|
| `0x1F800000` | `LINE_F3` | 5 | Four three-vertex corner brackets |
| `0x1F800040` | `LINE_G2` | 4 | Four callouts to `x=0/320` or `y=0/192` |

The lengths exclude each packet's tag word; the records occupy 24 and 20
bytes respectively. `setLineF3` also supplies the `0x55555555` polyline
terminator. The corner color and first callout color are green; the second
callout color is black. These are input packet colors, not a claim about
opaque final pixels: `func_8005B260` copies the packet and applies draw-mode
and semi-transparency handling.

The caller uses that helper's `u32 *`, `GsOT *`, `s32`, `s32` interface.
Its signed priority load and explicit `u16` conversion at submission retain
the target's access and mask placement. Packed color-word writes and vertex
store order remain unchanged.

[`name_entry_frame.h`](name_entry_frame.h) declares the callback once and
is included by both its definition and `NameEntry_Init`. The shared
`NameEntrySelectionFrameView` names only the proven drawing prefix:
signed priority at `+0x14`, signed x/y at `+0x30/+0x32`, and unsigned
width/height at `+0x3C/+0x3E`. Padding and field widths are unchanged; this
does not claim that the cursor allocation ends at `+0x40`. The installer
retains its existing object writes and callback-slot assignment rather
than pretending that the callback takes no arguments.

## Name-entry glyph effects

Within `name_entry_runtime.c`, the sprite factory and five update callbacks
form the per-sprite effect layer at `0x80168708..0x8016909C`. Their glyph
atlas table is the module's first rodata block at `0x4`.

| Address | Function |
|---|---|
| `0x80168708` | `NameEntry_UpdateGlyphPulse` |
| `0x80168808` | `NameEntry_UpdateGlyphFragment` |
| `0x801688AC` | `NameEntry_UpdateGlyphShatter` |
| `0x801689B4` | `NameEntry_UpdateCaretTween` |
| `0x80168AB4` | `NameEntry_UpdateGlyphTransfer` |
| `0x80168CDC` | `NameEntry_SpawnGlyphSprite` |

The definitions stay in executable order, which is not call order.

The grouping rests on stores and calls rather than on adjacency:

- `NameEntry_UpdateGlyphShatter` writes `NameEntry_UpdateGlyphFragment` into
  each shard's update slot at `+0x24`.
- `NameEntry_UpdateGlyphTransfer` calls `NameEntry_SpawnGlyphSprite` and
  writes `NameEntry_UpdateGlyphShatter` into the sprite it gets back.
- `NameEntry_SpawnGlyphSprite` stores the source glyph node at `+0x4C`, and
  `NameEntry_UpdateGlyphPulse` is the only reader of that field.
- `NameEntry_UpdateCaretTween` and `NameEntry_UpdateGlyphTransfer` open with
  the same eight-line prologue: latch `+0x6C` bit 7, then divide the distance
  from `+0x30/+0x32` to `+0x44/+0x46` by the `+0x60` frame count into the
  `+0x36/+0x38` step.

**The two private object views are now one.** The earlier grouping stopped at
the shatter pair and said so: absorbing the glyph pulse would have meant
reconciling its private `Obj` against the one in the sprite factory rather
than concatenating text. In one translation unit that reconciliation is
forced, and it turns out to be free. The two views agreed everywhere they
overlapped — the pulse's `sourceGlyph` at `+0x4C` is the factory's `f76`, the
field it writes the glyph node into — so they collapse to a single
`GlyphSprite` with no byte moving. The same is true of `func_80040510`, which
the merged sources declared three times with three different first parameter
types (`void *`, `u8 *`, and a private `Obj *`); one `void *` declaration
serves all three call sites.

`+0x44/+0x46` is the one place the views genuinely disagree, and the disagreement
is real rather than an error: `NameEntry_UpdateGlyphPulse` writes the pair as
the sprite's x and y scale, while the two tween callbacks read it as the
destination they are sliding to. The callback installed at `+0x24` decides
which. `GlyphSprite` names it for the scale use and the tween callbacks keep
reaching it by offset, because nothing yet distinguishes the two at the type
level; a union here would assert a relationship that has not been shown.

## Name-entry keyboard and dialog runtime

The last four functions in `name_entry_runtime.c` are the per-frame update
path: the completion entry point, the dialog state machine behind it, the
keyboard handler that machine drives, and the caret helper both of them use.
They occupy `0x8016909C..0x80169C30`.

| Address | Function |
|---|---|
| `0x8016909C` | `NameEntry_AdjustLength` |
| `0x8016913C` | `NameEntry_UpdateKeyboard` |
| `0x80169734` | `NameEntry_UpdateDialog` |
| `0x80169C08` | `NameEntry_PollCompletion` |

The definitions stay in executable order, which is the reverse of the call
order.

### The runtime tail is a closed call chain

Every module in this overlay is a single `gcc_2_8_1_g0_split` run end to end,
so a profile change cannot mark this boundary and no such claim is made here.
What marks it is that the run is closed under calls in one direction and has
exactly one door:

| Function | Callers | Where they are |
|---|---|---|
| `NameEntry_AdjustLength` | 3 | all inside this run |
| `NameEntry_UpdateKeyboard` | 1 | `NameEntry_UpdateDialog`, inside |
| `NameEntry_UpdateDialog` | 1 | `NameEntry_PollCompletion`, inside |
| `NameEntry_PollCompletion` | 2 | both **outside** the run |

`NameEntry_PollCompletion` is the runtime tail's public surface, and its own
body is two statements: run the dialog for one frame, then report bit `0x10`.
The three functions behind it have no caller anywhere else in the tree.
`NameEntry_AdjustLength` is reached only from the keyboard handler's caret
controls and from the dialog's arrival handling; the same unified source also
contains the caret callback it installs.

The two callers of `NameEntry_PollCompletion` are
[`name_entry_main.c`](name_entry_main.c) in this overlay and the resident
`main_run_frontend_menus.c`; both take it through
[`name_entry_keyboard.h`](name_entry_keyboard.h) or their own extern, and
neither reaches past it.

### What one unit forced, and what it did not

**The pad-spelling arms are now the unit's, not a file's.** The keyboard
handler needs `GINPUT_PAD1_HELD_IS_VOLATILE`,
`GINPUT_PAD1_REPEAT_IS_VOLATILE` and `GINPUT_PAD1_PRESSED_IS_VOLATILE`,
because it re-reads the held halfword on five paths and gcc otherwise commons
them into one register. `NameEntry_UpdateDialog` reads
`gInput_wPad1Pressed` exactly once and previously took the plain scalar. One
translation unit gets one arm, and the volatile arm is the one that had to
win. It is free for the dialog: volatile forces re-reads, and a single read
is a single read under either spelling. The module still matches
byte-for-byte, which is the proof rather than the argument.

**The two private object views stay two, and that is the finding.** Unlike
the glyph-effect merge, where two views turned out to describe one object,
these describe genuinely different ones: `SelectionFrame` is the keyboard
cursor in `D_8016D404`, while `DialogCaret` is the panel the dialog reaches
through the text box's `field_2C` and by tag from `func_80042B40(6)`. They
are not collapsed.

What the merge did force is a numbering collision. The two files spelled
their offsets in different bases — the keyboard's `f30` and the dialog's
`f48` are both offset `0x30`, and `f60` against `f96` are both `0x60`. Two
such structs in one file could be misread as sharing a scheme, so both now
name their fields by role with the hex offset in a comment.

`SelectionFrame` agrees with `NameEntrySelectionFrameView` in
[`name_entry_frame.h`](name_entry_frame.h) everywhere the two overlap —
signed x/y at `+0x30/+0x32` and unsigned width at `+0x3C` — and extends it
with the tween fields at `+0x36/+0x38`, the width bonus at `+0x5E` and the
timer at `+0x60`. The drawing callback and keyboard now live in the same
source but retain two named views: `NameEntrySelectionFrameView` is the proven
drawing prefix, while `SelectionFrame` extends it with the tween fields.
Keeping both makes each function's evidence explicit without claiming the
allocation ends at either view. `D_8016D404` remains declared through the
superset in [`name_entry_state.h`](name_entry_state.h).

## Glyph-encoding constants and the three meanings of `0xF0`

The password overlay's two text scanners now use the shared names from
`src/game/text_constants.h` instead of raw bytes:

| Site | Was | Now |
|---|---|---|
| `Password_RefreshDigitDisplay` glyph test | `glyph >= 0xF0` | `TEXT_SINGLE_BYTE_GLYPH_LIMIT` |
| `Password_RefreshDigitDisplay` string end | `*out = 0xFF` | `TEXT_STRING_TERMINATOR` |
| `NameEntry_UpdateDialog` scan skip | `*p >= 0xF0` | `TEXT_SINGLE_BYTE_GLYPH_LIMIT` |
| `NameEntry_UpdateDialog` saved end byte | `term = 0xFF` | `TEXT_STRING_TERMINATOR` |
| `NameEntry_UpdateDialog` end test | `c != 0xFF` | `TEXT_STRING_TERMINATOR` |

Both sites implement the same two-byte glyph rule the resident builder
`func_80038148` already spells out with these names: a code at or above the
limit occupies two bytes, so the digit display emits a high and low byte for
it and the dialog scanner steps an extra byte past it.

`0xF0` is **not** a single concept in this module, and the remaining
occurrences are deliberately left as numbers:

- `NameEntry_UpdateGlyphFragment` tests `*(s16 *)(object + 0x32) >= 0xF0`.
  That field is the fragment's Y position, so this is the 240-pixel screen
  bottom — an offscreen test, not a glyph limit.
- `NameEntry_Init` calls
  `func_80035BE4(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0)`, where the
  same literal appears twice with two further meanings: `textOffset + 0xF0`
  is a text-ID base, while the trailing `0x140, 0xF0` are the 320x240 box
  extents.

Three unrelated meanings share the value `0xF0` in these files. Only the
glyph-encoding uses are named here, because only those are the concept that
`TEXT_SINGLE_BYTE_GLYPH_LIMIT` describes.

## Keyboard input and glyph effects

`NameEntry_BuildKeyboardTextBox` (`0x80168138`) prepares text slot 1 for
the keyboard. It configures that slot's entry partition, creates the box at
`(22,24,320,240)`, sets its glyph-cell dimensions to `20 x 18`, then calls
the existing synchronous builder until `TEXT_BOX_FLAG_DONE` is set.
Its argument is an integer text selector: the text ID is `0xF0 + textOffset`,
not a pointer into RAM. `NameEntry_Init` passes 2, selecting text 242.
The header shared by both sides now declares the actual `void` return;
other selectors' captions and reachability remain unassigned.

The nearby factory at `0x801680B4` remains `func_801680B4`. Its X/Y
parameters are established by the configurator's stores at `+0x30/+0x32`,
and it allocates a list/type-2 object with fixed selectors `0,0,0` and values
`0x17,0x101`, then sets flag `0x8`. No current C caller or fixed graphic
identity was established. That absence is not evidence of global dead code,
and the numeric configuration alone is not a reason to invent a screen role.

The matching functions distinguish keyboard input from the outer dialog and
from the selection-frame drawing callback:

| Address | Function | Role |
|---|---|---|
| `0x8016913C` | `NameEntry_UpdateKeyboard` | Navigation, caret controls, character insertion and completion request |
| `0x80168CDC` | `NameEntry_SpawnGlyphSprite` | Allocate and configure a glyph copy; does not choose its update callback |
| `0x80168AB4` | `NameEntry_UpdateGlyphTransfer` | Move an inserted character to the name slot, with trail and arrival signaling |
| `0x801688AC` | `NameEntry_UpdateGlyphShatter` | Spawn fragments from the previous displayed glyph, then retire the clone |
| `0x80168708` | `NameEntry_UpdateGlyphPulse` | Hide/restore the source glyph's X while uniformly scaling its copy |
| `0x80168808` | `NameEntry_UpdateGlyphFragment` | Randomized motion, damping, downward acceleration and offscreen retirement |

`NameEntry_UpdateDialog` calls the keyboard handler only when its dialog and
effect gates allow input. The handler first advances an active selection-frame
tween: width moves by two pixels toward the requested width, XY advances in
signed 8.8 fixed point, and the timer decrements. A still-active timer returns
early; expiration snaps the rectangle, clears global tween bit `0x4000` and
continues processing input in that same invocation.

Held directions at `D_8009B3A4[0]` take priority. Horizontal movement wraps
over 15 columns; ordinary vertical movement wraps over nine rows. The
right-hand control block instead uses the row-transition table. Newly pressed
Start at `D_8009B398[0] & 0x800` moves selection through the same navigation
path to the finish control; it does not immediately accept the name.
Confirmation uses `D_8009B394[0] & 0xC0`, the repeat/new-press stream.
Circle in that stream moves the caret left without erasing a backing word.

The signed `9 x 15` table at `0x8016AB38` is navigation metadata, not a
character-code table. Negative entries redirect the column; some wrap to
column zero rather than merely skipping a continuation cell. Descriptor zero
restores the remembered row. Descriptor `4` selects caret controls on row 4;
`0x46` selects a wider finish control on row 6. Start's provisional `(14,8)`
resolves to `(11,6)`. The second table at `0x8016ABC0` redirects right-block
vertical movement between those rows. These descriptions use code actions,
not newly inferred UI captions.

### Name words, caret index and encoded bytes are different

`gSaveData_aPlayerNameSjis` is a twelve-byte field containing six `u16`
SJIS code words. `D_8016D42C` is the caret/insertion **word index**,
with observed movement bounds 0-5. The corresponding quantities are:

| Quantity | Calculation |
|---|---|
| Backing-word byte offset | `2 * index` |
| Box-3 glyph lookup, local X | `16 * index` |
| Transfer destination, screen X | `112 + 16 * index` |
| Caret destination, screen X | `107 + 16 * index` |

The actual inserted code comes from a rendered box-1 glyph node, not from the
navigation descriptor. The keyboard writes one backing word and sets global
bit `0x80` while its transfer effect runs. Box 3 still holds the previous
displayed text until arrival, allowing the old glyph to be copied for shatter.
The outer dialog then consumes arrival, re-encodes the name, rebuilds box 3
and requests caret advancement.

`Text_SjisToGlyphCodes` converts up to six source words into a separate
variable-width byte stream: glyph indices below `0xF0` use one byte, larger
ones use an `0xF0`-prefixed pair, and the stream ends in `0xFF`.
`TextBox_GetGlyphAt` searches `0x1C`-stride glyph nodes by exact local-pixel
coordinates, not by a byte offset or character ordinal.

The keyboard can write slot 5 and does not append a `u16` terminator to the
source. Its caret helper moves an index; it neither deletes words nor maintains
a terminator. These bodies therefore do **not** establish the older
five-character-plus-terminator claim or an observed UI maximum. Finish request
bit `0x40` is separate from pending-character bit `0x80`: the outer dialog
processes completion and trims the encoded display stream, not the SJIS array.
Likewise, `D_8016D408` is an incrementing per-copy `u8` tag, not a name-byte
count.

### Effect lifetimes and partial views

The pulse's source pointer at sprite `+0x4C` points to a **text glyph node**.
It saves that node's signed local X at `+0x0C`, writes `0x400` to hide it,
and applies the same `4096 - step * 341` scale to both components of the
copy. After the twelfth update it restores the source X; a later invocation
entered with frame 12 releases the copy. This is not a color save/restore,
and restoration and release do not happen on the same callback.

Transfer sprites have tag 6 and normally start with an eight-update timer.
Their trail copies begin at RGB `0x606060`; `6` is the fade decrement per
update, not a six-update lifetime. Arrival sets object bit `0x40`, snaps to
the target XY and arms a two-update retirement timer. The outer dialog can
consume and release the arrived sprite before that local retirement finishes.
An existing box-3 glyph is cloned with tag 5 for shattering.

Shatter attempts sixteen `4 x 4` fragments on its first callback, skipping
individual allocation failures. The source clone survives that callback and
is released on the next one; missing fragments are not retried. Each tag-3
fragment initializes X velocity to `Rand_GetInterval(512) - 256` and Y
velocity to `-Rand_GetInterval(384)`. In signed 8.8 units, every update
then damps X toward zero by 8, increases Y by 64 capped at 2048, integrates
XY and retires at signed Y of at least 240. Those updates also occur on the
initial callback; there is no fixed lifetime or X clipping.

[`name_entry_keyboard.h`](name_entry_keyboard.h) supplies one declaration
per function and a truthful one-argument byte-pointer callback type, included
by definitions and installers. The glyph nodes this screen walks are the
resident `DuelEffectEntry` records of `D_800EB288`, so the header consumes
that type from [`duel_effect.h`](../../game/duel_effect.h) instead of
describing the node again: `code_00` is the SJIS code and `x_0C`/`y_0E` the
signed local position this screen searches. The text-box records the screen
builds are `DuelEffectChannel`, from the same header. Display-object
prefixes remain local, and their `+0x44/+0x46` fields retain role-dependent
meanings: transfer targets versus pulse scales.
The update callback at `+0x24` is not the two-argument selection-frame draw
callback at `+0x4C` in the other object type.

The constructor still reads the glyph code **before** its null test and does
not check allocation success. Its apparent fallback configures a `16 x 16`
sprite with UV `(128,128)`, not a `128 x 128` sprite; it is not a proven
null-safe C path. The constructor's thirty-`s32` atlas table still occupies
module `+0x04..+0x7C`, with the renamed C source owning both text and rodata.
Existing resident helper/dispatcher argument-register idioms are not
normalized by this interface change.

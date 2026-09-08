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

## Display-refresh translation unit

`refresh_displays.c` keeps the eight-digit password renderer next to the
starchip-balance renderer. Both rebuild fixed text slots through
`func_8003B6AC`, `func_80035BE4`, and `func_80039A14`.

The definitions remain in executable order: `Password_RefreshDigitDisplay`
occupies `0x80169C30..0x80169D10`, followed by
`Password_RefreshStarchipDisplay` through `0x80169D84`. Both use
`gcc_2_8_1_g0_split`, and one C subsegment at module offset `0x1C30` covers
the complete contiguous `0x154`-byte text range.

## Password-shop lifecycle

`Main_RunPasswordMenu` waits for the package through its resident loader,
calls `Password_InitShopScreen` (`0x8016A080`) once, then calls
`Password_UpdateShopScreen` (`0x8016A37C`) each tick. Neither entry returns
a completion code. Cancellation writes the saved previous mode to
`D_8009B26C`; the updater does not itself request a disk save or explicitly
tear down every object.

The initializer clears eight decimal digit bytes, index and shop state,
rebuilds the digit/starchip displays and creates message 226. It configures
the cached preview record, a display object, one digit cursor and **four**
kind-specific cursor decorations, not one decoration per digit.
`func_80029574(0)` only clears the cached record's two display pointers; it
does not load a background. The halfwords `320,256,512,240` feed texture
configuration, not proven background dimensions, and the separate selectors
`2,3` are not established pixel widths.

The cursor's provisional `(256,120)` is replaced during initialization by
its target `(163,99)` for index zero. `Password_UpdateDigitCursor`
(`0x80169E20`) increments byte `+0x22` on every callback, including idle
ones. Movement bit `0x40` additionally enables an eight-update signed-8.8
tween from current `+0x30/+0x32` to target `+0x18/+0x1A`; completion snaps
XY and clears bits `0xC0`, leaving the callback installed. No rotation axis
or real-time duration is inferred from the phase byte.

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
`gLibrary_dwStarchips` (`D_801D0000[504]`, address `0x801D07E0`).

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

## Name-entry setup translation unit

`name_entry_setup.c` keeps `NameEntry_Init` next to the two routines it
installs and drives: the keyboard text-box builder it calls during setup, and
the selection-frame drawing callback it registers at `+0x4C`. Grouping them
records the install relationship that `NameEntry_Init` establishes by its own
stores, rather than an inference from adjacency.

The definitions remain in executable order, which is not call order:
`NameEntry_BuildKeyboardTextBox` occupies `0x80168138..0x801681A0`,
`NameEntry_DrawSelectionFrame` runs through `0x801683EC`, and
`NameEntry_Init` closes the unit at `0x8016868C`. All three use
`gcc_2_8_1_g0_split`, and their shared manifest source and one C subsegment
at module offset `0x138` cover the complete contiguous `0x554`-byte text
range, ending exactly where `TextBox_GetGlyphAt` begins.

This is a grouping of the name-entry setup path only. It makes no claim about
the original author's translation-unit boundaries, and it is not the whole
name-entry screen: the per-frame glyph pulse at `0x80168708` and the routines
after it remain separate units. A later wider grouping would have to absorb
this entire three-function object rather than one member of it.

## Name-entry selection-frame packets

`NameEntry_Init` installs
[`NameEntry_DrawSelectionFrame`](name_entry_setup.c)
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

## Name-entry glyph-fragment translation unit

`name_entry_glyph_fragments.c` keeps the glyph shatter effect next to the
per-piece fragment updater it installs. `NameEntry_UpdateGlyphShatter`
allocates a 4x4 grid of quarter-size pieces from the shared display-object
pool, configures each from the source glyph's position, palette pair and
clut/tpage words, marks them mode `3`, and writes
`NameEntry_UpdateGlyphFragment` into each piece's update slot at `+0x24`.
The callback relationship is established by that store, not inferred from
adjacency.

The definitions remain in executable order, which is again not call order:
`NameEntry_UpdateGlyphFragment` occupies `0x80168808..0x801688AC` and
`NameEntry_UpdateGlyphShatter` runs through `0x801689B4`. Both use
`gcc_2_8_1_g0_split`, and their shared manifest source and one C subsegment
at module offset `0x808` cover the complete contiguous `0x1AC`-byte text
range, ending exactly where `NameEntry_UpdateCaretTween` begins.

This unit covers the shatter lifecycle only. The preceding glyph pulse at
`0x80168708` is deliberately left out: it is cohesive with these two, but it
declares `func_8004036C` through its own private object view rather than the
`void *` these files use, so absorbing it would require changing that view
rather than concatenating text. That reconciliation is a separate question
from this grouping.

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
by definitions and installers. The shared `NameEntryGlyphNodeView` describes
only the leading `0x10` bytes of a `0x1C`-stride node: SJIS code at zero
and signed local X/Y at `+0x0C/+0x0E`. It is not a complete array-element
layout. Display-object prefixes remain local, and their `+0x44/+0x46`
fields retain role-dependent meanings: transfer targets versus pulse scales.
The update callback at `+0x24` is not the two-argument selection-frame draw
callback at `+0x4C` in the other object type.

The constructor still reads the glyph code **before** its null test and does
not check allocation success. Its apparent fallback configures a `16 x 16`
sprite with UV `(128,128)`, not a `128 x 128` sprite; it is not a proven
null-safe C path. The constructor's thirty-`s32` atlas table still occupies
module `+0x04..+0x7C`, with the renamed C source owning both text and rodata.
Existing resident helper/dispatcher argument-register idioms are not
normalized by this interface change.

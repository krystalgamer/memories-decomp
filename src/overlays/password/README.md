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

## Name-entry selection-frame packets

`NameEntry_Init` installs
[`NameEntry_DrawSelectionFrame`](name_entry_draw_selection_frame.c)
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

## Keyboard input and glyph effects

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

# Text control-byte contract

## Scope

`TextBox_BuildStep` interprets the byte streams behind text-box records.
Bytes below `0xF0` are printable glyph codes. Bytes `0xF0..0xFF` dispatch
through the sixteen function pointers beginning at `gText_apfnPrimaryCommands`.

In North America, this alias is entry 27 within the 47-entry secondary command
table `D_80090EAC`. This fixes the mapping without inferring it from text
alignment: F0 selects entry 27, F1 entry 28, through FF entry 42.

The shared current-code, disposition and choice-layout callback contracts,
including the different European primary-table offset, are documented in
[text-command-state.md](text-command-state.md).

The text-selected image/tween states and their separate scene-script
dispatch table are documented in
[text-script-bridge.md](text-script-bridge.md), including their distinct
operand formats and optional completion wait.

The complete secondary table now uses
`void (*)(DuelEffectChannel *)`, matching the record that
`TextBox_BuildStep` and `Text_DispatchSecondaryCommand` pass through it.
Control handlers that operate directly on the shared channel expose that
parameter type. Entries retaining narrower raw or command-specific views use
explicit table casts; those casts document alternate source views without
weakening the dispatch contract back to `u8 *`.

## Primary control table

| Byte | Following data | Matching handler | Effect |
|---:|---|---|---|
| `F0-F5` | `u8 low` | `Text_ExtendGlyphCode` | Forms glyph code `((opcode - F0) << 8) | low` and returns it to the normal glyph renderer |
| `F6` | `u8 effect_id, u8 flags` | `Text_HandleDisplayEffectCommand` | Starts, adjusts, waits on, or stops one of the three display-effect records |
| `F7` | `u8 state` | `Text_SetStateFromStream` | Writes the text channel's state byte and yields |
| `F8` | `u8 secondary_opcode`, then handler-defined operands | `Text_DispatchSecondaryCommand` | Dispatches `D_80090EAC[secondary_opcode]`; F8 itself imposes no fixed operand count |
| `F9` | `u16 flag_command`; non-write form also has `u16 target` | `Text_HandleCampaignFlagCommand` | Writes a card/story flag, or conditionally replaces the current stream's low 16-bit offset |
| `FA` | none | `Text_StartPageWait` | Enters text state 4 and yields for page-advance input |
| `FB` | control byte, optional enabled-mask byte, or selected `u16` jump entry | `Text_HandleChoiceCommand` | Starts a choice layout or jumps through its selected target |
| `FC` | `u16 offset` | `Text_PushStreamOffset` | Stores the offset in the next stream slot and increments stream depth |
| `FD` | `u16 offset` | `Text_SetCursorOffset` | Replaces the current stream cursor's low 16-bit offset |
| `FE` | none | `Text_NewLine` | Advances the line, resets horizontal position, handles page overflow, and notifies choice-line counting |
| `FF` | none | `Text_EndStream` | Pops one stream; marks the text complete only when the depth becomes negative |

All `u16` values are little-endian because `TextStream_ReadU16LE` reads
`current[0] | current[1] << 8` and advances the current stream by two bytes.

## F8 is a secondary-command escape

`Text_DispatchSecondaryCommand` consumes exactly one byte: the index into
`D_80090EAC`. The selected secondary handler then consumes its own operands.
The complete table contains 47 entries and covers text formatting, string
insertion, sound, fades, campaign-duel setup, display objects, and other
effect commands.

Retail campaign string 1350 contains different F8 lengths:

```text
F8 10 03 80 87
F8 15 A0
F8 0E 20 70
F8 16 FF
```

Opcode `0x10` selects `DuelEffect_PlaySoundCommand`, while opcode `0x15`
selects the fade command. The different byte counts are therefore expected;
describing F8 as `F8 xx yy` or as a position/style command is incorrect.

The file, BGM-fade and CD-sound callback predicates and polling phases are
documented in [text-media-waits.md](text-media-waits.md). The CD poll budget
is not a count of restarted playback requests.

The F6-selected record continuation, wait/transition callbacks and partial
release contract are documented in
[text-display-effect-states.md](text-display-effect-states.md).

## Campaign-flag command

F9 first reads one `u16 flag_command`.

- If `CAMPAIGN_FLAG_COMMAND_WRITE` is set, it updates the requested
  card/story flag and returns. Total operands: two bytes.
- Otherwise it reads a second `u16 target`. If `Campaign_TestStoryFlag`
  satisfies the requested normal/inverted predicate, the handler replaces the
  low 16 bits of the current stream cursor with `target`. Total operands: four
  bytes.

Retail string 1350 contains:

```text
F9 6E 40 FD 6E 44
```

This decodes as flag command `0x406E` and target `0x446E`, directly confirming
the two-word conditional form.

## Page wait and resume

FA has no operand. `Text_StartPageWait` sets state 4 and yields.

State-table entry 4 (`Text_WaitForPageAdvance`) performs the actual wait:

1. on first entry it sets the state latch, initializes a ten-frame
   Square-held countdown, and creates the shared choice/page cursor with
   `Dialog_OpenChoice`;
2. it waits for Cross/Square press, or for the Square-held countdown to reach
   zero;
3. it plays sound effect `0x0B`, switches to state 2, destroys the cursor, and
   clears its pointer.

State-table entry 2 (`Text_CompletePageAdvance`) asks the existing glyph/effect entries
to clear, waits until no active entry remains, resets line and cursor fields,
and returns the text channel to state zero. Parsing then resumes at the byte
after FA.

This establishes the code path behind the observed blinking “press X” arrow
and page advance without naming the two shared state callbacks more narrowly
than their full table roles.

The raw entry-control setter, slot-3 clear handoff and shared signed-byte
hold countdown are detailed in [text-page-transition.md](text-page-transition.md).

## Choice setup and selected jump

FB always consumes a control byte.

When bit `0x80` is clear, it starts a choice:

- low three bits set the number of choice lines;
- high nibble sets choice style/state flags;
- if bit `0x08` is set, one additional byte supplies the enabled-choice mask
  and input-mode flag;
- it clears existing glyph markers, installs
  `Text_TryCompleteChoiceLayout` as the pending newline callback, and raises
  the choice-active text flag.

Text rendering continues through the choice labels. Each FE calls
`Text_NewLine`, which invokes the pending callback. Once the line counter
reaches the choice count, the callback enters dialog state 1 and clears the
choice-layout flag so normal choice input takes over.

When bit `0x80` is set, FB is the selected branch. It advances past
`gDialog_bChoice * 2`, reads the selected little-endian `u16`, and replaces
the current stream offset. The jump table therefore contains one two-byte
target per choice; its total encoded length belongs to the earlier choice
count, not to FB itself.

String 1350 demonstrates both forms:

```text
FB 02
...
FB 80 DE 15 ...
```

The first starts a two-choice layout. The second selects one target from the
following 16-bit table; `DE 15` is target `0x15DE` for choice zero.

## Nested streams and completion

FC reads a little-endian offset, writes it into the low half of the next
stream slot while preserving that slot's high half, and increments the signed
stream-depth byte. String 1350 contains `FC 5A 12`, which pushes offset
`0x125A`.

FD reads an offset and replaces the low half of the current stream cursor.
The same string contains `FD 8B 12`, which jumps to `0x128B`.

FF decrements stream depth:

- if the result remains nonnegative, `TextBox_BuildStep` immediately resumes
  parsing from the previous stream;
- if the result is negative, it sets the text-box done flag.

FF is therefore both a nested-stream return and the final terminator. Calling
it simply “end” is accurate only for the outermost stream.

## Regional choice-command setup and input handoff

The Japanese inventory's old `func_800382A8` label at `0x800382A8`
(`0x124` bytes) identifies the already-canonical `Text_HandleChoiceCommand`
definition in the actual Japanese `text_control_commands.c` wrapper. It is
not a new function, ABI or native alias. The North American definition is at
`0x80038BF0`; the European definition is at `0x800389F0` (`0x80038BF0`
in the other PAL images). Source ownership, grouped order and named
`gcc_2_8_1_g8_split` profiles remain unchanged. FB is established by the NA
initialized table; this inventory alignment does not assign a regional opcode.

The command, options byte and published control byte have distinct domains:

- `TEXT_CHOICE_COMMAND_COUNT_MASK` (`7`) publishes the raw three-bit count.
  It is neither validated nor clamped to the four enabled-mask bits; zero
  and counts above four retain their original behavior.
- `TEXT_CHOICE_COMMAND_HAS_OPTIONS` (`0x08`) consumes an options byte
  **before** testing `TEXT_CHOICE_COMMAND_BRANCH` (`0x80`). This also applies
  when both bits are set. Without the options byte, the local defaults to
  `TEXT_CHOICE_OPTIONS_ENABLED_MASK` (`0x0F`).
- During setup, the options low nibble becomes the enabled mask.
  `TEXT_CHOICE_OPTIONS_INPUT_FLAG` (`0x80`) sets the shared input byte to
  exactly `1`; it is not the command's selected-branch bit and is not the
  later `DIALOG_CHOICE_INPUT_CANCELLED` status, despite sharing its value.
  No new input producer or external-input protocol is inferred.
- `DIALOG_CHOICE_CONTROL_MASK` (`0xF0`) publishes the control high nibble.
  `DIALOG_CHOICE_CONTROL_LAYOUT_MASK` (`0x30`) remains a raw masked value,
  not a shifted enum or Boolean. `DuelEffect_UpdateObjectLayout` uses it
  directly in Japan/PAL and with the existing row-specific arithmetic in
  NA. `Text_TryCompleteChoiceLayout` changes the count to two when those
  bits are nonzero, only after comparing the rendered line count with the
  originally published count and clearing the pending callback.
- `DIALOG_CHOICE_CONTROL_RETAIN_HIGHLIGHT` (`0x40`) leaves the current
  highlight object stored at channel `+0x30` after an enabled choice is
  accepted. State is still written to zero. With the bit clear, the object
  is released, its pointer cleared and state set to three. This bit does
  not bypass disabled-choice rejection or describe eventual caller cleanup.

Setup still publishes disposition before consuming operands, resets selection,
input and line state in the original order, installs the same volatile-channel
layout callback and sets channel bit `0x1000`. That channel bit has other text
uses; it is not renamed as an exclusively choice-owned flag. NA/Japan clear
both occupancy value and matching marker; PAL omits only the occupancy clear.
The branch path still skips the signed selected index times two and calls the
existing regional cursor-offset helper. No new bounds, input normalization,
table entries, layout fields or constructor/release behavior are introduced.

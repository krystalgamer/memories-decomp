# Text control-byte contract

## Scope

`TextBox_BuildStep` interprets the byte streams behind text-box records.
Bytes below `0xF0` are printable glyph codes. Bytes `0xF0..0xFF` dispatch
through the sixteen function pointers beginning at `D_80090F18`.

`D_80090F18` is the address of entry 27 within the 47-entry secondary command
table `D_80090EAC`. This fixes the mapping without inferring it from text
alignment: F0 selects entry 27, F1 entry 28, through FF entry 42.

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

All `u16` values are little-endian because `func_80036D3C` reads
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

State-table entry 4 (`func_800375A4`) performs the actual wait:

1. on first entry it sets the state latch, initializes a ten-frame
   Square-held countdown, and creates the shared choice/page cursor with
   `Dialog_OpenChoice`;
2. it waits for Cross/Square press, or for the Square-held countdown to reach
   zero;
3. it plays sound effect `0x0B`, switches to state 2, destroys the cursor, and
   clears its pointer.

State-table entry 2 (`func_8003741C`) asks the existing glyph/effect entries
to clear, waits until no active entry remains, resets line and cursor fields,
and returns the text channel to state zero. Parsing then resumes at the byte
after FA.

This establishes the code path behind the observed blinking “press X” arrow
and page advance without naming the two shared state callbacks more narrowly
than their full table roles.

## Choice setup and selected jump

FB always consumes a control byte.

When bit `0x80` is clear, it starts a choice:

- low three bits set the number of choice lines;
- high nibble sets choice style/state flags;
- if bit `0x08` is set, one additional byte supplies the enabled-choice mask
  and input-mode flag;
- it clears existing glyph markers, installs `func_80037CE0` as the pending
  newline callback, and raises the choice-active text flag.

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

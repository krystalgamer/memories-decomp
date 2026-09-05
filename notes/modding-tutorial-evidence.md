# Modding Tutorial Evidence

This note records claims recovered from the community modding tutorials
attached to issue #368. The tutorials describe edits from a hex-editor
perspective, so each claim is mapped back to resident code or data before it
is accepted.

Confidence levels:

- **Confirmed**: the retail bytes, disassembly, and independently recovered
  symbols or behavior agree.
- **High**: the code and tutorial agree, but part of the surrounding semantic
  interpretation is not independently named.
- **Tentative**: the patch effect is plausible, but its target or broader
  behavior still needs corroboration.

## Initial duel life points

**Tutorial:** `Alterar LP - Por Jael Rivera.txt`

The tutorial identifies SLUS file offset `0x7DD0`, where the retail halfword
`40 1F` represents decimal 8000 in little-endian order. It suggests replacing
that halfword with another 16-bit value, for example `28 23` for decimal 9000.

The offset maps to VRAM `0x800175D0`:

```text
0x80010000 + (0x7DD0 - 0x800) = 0x800175D0
```

At that address, `func_800175A0` contains:

```mips
addiu $v0, $zero, 0x1F40
sh    $v0, 0x2($sp)
sh    $v0, 0x0($sp)
```

The function uses these two stack halfwords while initializing the two
`0x20`-byte duel-side records rooted at `D_800E9FF0`. For each side it:

- clears the displayed-life-point field at record offset `+0x12`;
- writes the selected initial value at offsets `+0x14` and `+0x16`;
- clears the remaining per-side display state.

The existing RAM-map-backed symbols identify:

| Record | Displayed LP (`+0x12`) | Authoritative LP (`+0x14`) | Adjacent value (`+0x16`) |
|---|---:|---:|---:|
| Player | `gDuel_wPlayerLifePointDisplay` (`0x800EA002`) | `gDuel_wPlayerLifePoint` (`0x800EA004`) | `0x800EA006` |
| Opponent | `gDuel_wOpponentLifePointDisplay` (`0x800EA022`) | `gDuel_wOpponentLifePoint` (`0x800EA024`) | `0x800EA026` |

Therefore the patch does not directly initialize the displayed LP to 8000.
It initializes the authoritative LP and the adjacent LP value, while the
displayed counter begins at zero and is updated afterward. The exact role of
the `+0x16` halfword is not yet independently named; its initialization to the
same value strongly suggests an LP display target or previous-value field.

The constant is used only when `gDuel_bOpponentID >= 0`, the ordinary duel
path. When the opponent ID is negative, `func_800175A0` instead initializes
the two values from `D_8009B234` and `D_8009B236`, so changing `0x7DD0` does
not affect that alternate path.

**Confidence:**

- **Confirmed** that file offset `0x7DD0` is the 8000 immediate in
  `func_800175A0`.
- **Confirmed** that changing it changes the initial authoritative player and
  opponent LP values on the ordinary duel path.
- **High** that the adjacent `+0x16` fields are LP animation targets or
  previous values; their exact role remains unnamed.

For a value representable by one 16-bit halfword, the tutorial's byte-edit
method preserves the instruction opcode: decimal 9000 is hexadecimal
`0x2328`, so the complete instruction changes from `40 1F 02 24` to
`28 23 02 24`.

## Millennium Eye opponent-card display

**Tutorial:** `Olho do Milenio.txt`

The tutorial identifies SLUS offset `0x8864`, whose retail bytes are
`FF 00 02 24`. The offset maps to `0x80018064`, inside the exact matching C
for `func_80018004`:

```text
0x80010000 + (0x8864 - 0x800) = 0x80018064
```

The instruction is:

```mips
addiu $v0, $zero, 0xFF
```

It supplies the final `field_67` value when the current duel-card display
state byte at `+0x1F` is negative. The matching source first selects the
card's normal display-data index plus one, then overrides that selection with
`0xFF` for the negative mode:

```c
if (D_8009B1C8->field_1F != 0) {
    object->flags |= 0x2000;
    result->field_67 = object->child->field_04 + 1;
    if (D_8009B1C8->field_1F < 0)
        result->field_67 = 0xFF;
}
```

`func_8003AAE4` later passes `field_67` to `func_8003A1EC`, which uses it as
the resource index for image lookup and display-object setup. Replacing the
immediate byte with `0x00` or `0x01` therefore changes the selected opponent
card image resource; it does not alter the duel's underlying card data.

`func_80017DB4` contains the same negative-mode `field_67 = 0xFF` override
for another card-object update path. The tutorial's one-byte edit does not
change that site, so static evidence alone does not prove that every
opponent-card object keeps the replacement index through all later updates.

The tutorial reports that value `0` exposes the opponent's card faces and
value `1` keeps the cards hidden while showing their order numbers. The
static call chain confirms that these values select different image
resources, but the exact appearance of those resource indices has not yet
been independently verified against extracted art or a runtime trace.

**Confidence:**

- **Confirmed** that `0x8864` is the low immediate byte of the negative-mode
  `field_67 = 0xFF` assignment in `func_80018004`.
- **Confirmed** that `field_67` becomes an image resource index and does not
  modify card identity or duel state.
- **Tentative** that resource indices `0` and `1` have exactly the two visual
  meanings reported by the tutorial; runtime or asset-level corroboration is
  still required.

## Dialogue pointer redirects

**Tutorial:** `Alterar Dialogo.txt`

The tutorial uses SLUS file offset `0x1A1C3B` as an example dialogue target
and derives bytes `3B 14` for an `FB 80` redirect. The result is correct, but
the reliable conversion is to remove the PS-X EXE header before encoding the
loaded address:

```text
loaded address = 0x80010000 + (0x1A1C3B - 0x800) = 0x801B143B
stored target  = 0x143B
little endian  = 3B 14
```

`FB 80` belongs to the text stream, not the campaign script opcode table.
The handler at `0x80038BF0` advances the text cursor by
`gDialog_bChoice * 2`, then calls `Text_SetCursorOffset` (`0x80038BA8`).
Exact matching C for `Text_SetCursorOffset` reads the selected 16-bit value
and replaces only the low halfword of the current text pointer:

```c
*p = (*p & 0xFFFF0000) | (v & 0xFFFF);
```

The redirect target must therefore remain in the cursor's current 64 KiB
bank. A real multi-choice block needs one little-endian target halfword per
possible choice. A single target works only when the selected choice index is
known to be zero, so the tutorial's four-byte `FB 80 xx xx` recipe is not a
general unconditional jump.

**Confidence:**

- **Confirmed** that file offset `0x1A1C3B` maps to loaded address
  `0x801B143B` and target bytes `3B 14`.
- **Confirmed** that `FB 80` indexes a table of 16-bit targets by the current
  choice and preserves the pointer's upper 16 bits.
- **Tentative** that inserting only one target after an arbitrary page wait is
  safe; that depends on the active choice state and the source text bank.

## Player name in the duel-result label

**Tutorial:** `Nome no Final.txt`

The tutorial gives runtime address `0x801B0514` and replacement bytes
`FC 5A 12`. That address maps back to SLUS file offset `0x1A0D14`:

```text
file offset = 0x800 + (0x801B0514 - 0x80010000) = 0x1A0D14
```

The retail bytes there begin `24 21 35 FF`. The recovered glyph mapping decodes
`24 21 35` as `YOU`, followed by the `FF` string terminator. This is the
player-side label in the duel-result text bank; the adjacent string contains
the `COM` label.

In this text format, `FC lo hi` inserts another string from the current text
bank. The same `FC 5A 12` sequence is used by the name-confirmation dialog,
where it inserts the string at `0x801B125A`. Matching C in
`SaveData_ApplyRuntimeState` fills that buffer from the saved player name
through `Text_SjisToGlyphCodes`.

Replacing only the three `YOU` glyph bytes with `FC 5A 12` therefore keeps
the existing `FF` terminator and makes the result label render the current
player name. It does not rename the save data or patch a general ending or
credits routine, despite the tutorial's broad title.

**Confidence:**

- **Confirmed** that `0x801B0514` contains the `YOU` glyph sequence and maps
  to SLUS offset `0x1A0D14`.
- **Confirmed** that `FC 5A 12` inserts the runtime player-name string at
  `0x801B125A`.
- **Confirmed** that the patched bytes belong to the player-side duel-result
  label rather than executable code.

## Skip the opening Heishin text segment

**Tutorial:** `Remover Heishin.txt`

The tutorial replaces three bytes at SLUS offset `0x1A1A7A` with
`FD 1C 13`. The file offset maps to loaded address `0x801B127A`, the start of
global text string 1350. Its retail bytes begin `F7 05 00`; the patch replaces
that first control with an `FD` text-stream jump.

The `FD` dispatch-table entry is `Text_SetCursorOffset` (`0x80038BA8`). Its
exact matching C reads a little-endian 16-bit target and replaces only the low
halfword of the current text pointer. The patched target is therefore:

```text
FD 1C 13 -> low halfword 0x131C -> 0x801B131C
```

The destination remains in the same `0x801Bxxxx` text bank and begins with
valid text controls (`FE F7 0A ...`). Relative to the byte after the inserted
three-byte jump, the patch skips `0x9F` bytes of the original text stream.

This is not a change to the campaign bytecode interpreter, opponent data, or
the sound driver. Any skipped Heishin, music, or title presentation is encoded
inside the bypassed text-stream region. The static data establishes the
redirect exactly, but the complete visible sequence after the jump still
needs a runtime trace.

**Confidence:**

- **Confirmed** that `0x1A1A7A` maps to `0x801B127A` and begins with retail
  bytes `F7 05 00`.
- **Confirmed** that `FD 1C 13` redirects the text cursor to `0x801B131C`
  while preserving its current 64 KiB bank.
- **High** that this removes the opening Heishin segment described by the
  tutorial; the jump span is proven, but the resulting presentation has not
  been replayed locally.

## Campaign duel records and music selection

**Tutorials:**

- `Trocar Musica.txt`
- `Heishin Campanha.txt`

The tutorials identify nine-byte records embedded in the global text bank.
The Heishin record appears verbatim at SLUS offsets `0x1A5964` and
`0x1A5C93`:

```text
F8 0D 08 74 75 06 30 72 FF
```

`F8` dispatches through the 27-entry extended text-control table, where
subcommand `0x0D` resolves to exact matching C function
`Text_StartCampaignDuel`.
That function consumes the record in this order:

| Bytes | Meaning | Heishin value |
|---|---|---:|
| `F8 0D` | Extended text-control command and subcommand | `0x0D` |
| next byte | Opponent ID | `8` |
| next byte | Post-duel continuation selected for result index `0` | `0x74` |
| next byte | Post-duel continuation selected for result index `1` | `0x75` |
| next byte | Duel terrain | `6` (Dark) |
| next two bytes | Sound command, little endian | `0x7230` |
| `FF` | End of the containing text string | |

After storing those fields, the handler selects duel mode. `Main_RunDuel`
later indexes the two continuation values with `D_8009B362 * 2`. The
tutorial identifies index `0` as the win path and index `1` as the loss path;
the code independently confirms that they are separate post-duel
continuations.

The explicit `0x7230` value is later passed to `func_8003FF08`, the resident
sound-command wrapper. It must be edited as one little-endian halfword, not
as two independent music bytes. The tutorial's labels for the `0x71D0`-
`0x72C0` values are behavior reports; static code confirms the command values
but not every audible track name.

`Text_StartCampaignDuel` also chooses a second sound value from the opponent ID:
`0x7280` for IDs `9`-`16`, `0x7290` for ID `17`, `0x72B0` for ID `38`, and
`0x72A0` otherwise. Changing the record's explicit sound command therefore
does not replace every opponent-dependent sound selection made for the duel.

The campaign tutorial changes the first Heishin record from continuation pair
`74 75` to `75 61`. This leaves the opponent, terrain, and sound command
unchanged and redirects only the two post-duel paths. Because the same retail
record occurs twice, editing `0x1A5964` does not globally modify every
Heishin encounter.

**Confidence:**

- **Confirmed** that `F8 0D` dispatches to `Text_StartCampaignDuel` and that
  the record fields have the layout shown above.
- **Confirmed** that `30 72` is the little-endian sound command `0x7230` and
  that the two Heishin records occur at the listed SLUS offsets.
- **High** that `0x74` and `0x75` are respectively the win and loss
  continuations; the selection mechanism is exact, while the outcome labels
  come from the tutorial's observed behavior.
- **Tentative** for music names that have not been corroborated by an audio
  trace.

## Pharaoh Atem final-duel payload

**Tutorial:** `Pharaoh Atem no Ultimo Duelo.txt`

The tutorial writes a `0x642`-byte payload at SLUS offset `0x1B01BE`. That
range maps to `0x801BF9BE-0x801BFFFF` and ends exactly at file offset
`0x1B0800`, where the `0x801C0000` global string-offset table begins. The
payload therefore does not overwrite the offset table, but its destination is
not empty padding: the retail range contains 339 nonzero bytes through
`0x1B034D`. Applying the tutorial replaces existing tail text data and clears
the rest of the bank.

At payload offset `+0x94` (SLUS `0x1B0252`) is this campaign duel record:

```text
F8 0D 27 02 61 00 40 72 FF
```

Using the verified `F8 0D` layout, it selects opponent ID `0x27` (decimal
`39`), post-duel continuation IDs `0x02` and `0x61`, Normal terrain, and sound
command `0x7240`. Existing game research identifies opponent slot 39 as Duel
Master K. The payload does not add a new opponent identity or alter that
opponent's deck, model, portrait, or name; presenting the slot as Pharaoh Atem
depends on other data changes.

The large zero-filled tail is significant. The payload is sized to the
remaining text-bank space rather than just the encoded dialogue and duel
record, so it should be treated as a full replacement of that region, not as
a short insertion that can be combined blindly with other text mods.

**Confidence:**

- **Confirmed** that the payload exactly fills `0x801BF9BE-0x801BFFFF`
  without crossing into the string-offset table.
- **Confirmed** that its embedded duel record launches opponent slot 39 with
  Normal terrain and sound command `0x7240`.
- **Confirmed** that the destination overwrites existing nonzero retail text
  data.
- **Tentative** that the complete patch produces the tutorial's claimed
  Pharaoh Atem presentation without additional modified assets or opponent
  data.

## Attack-trigger trap thresholds

**Tutorial:** `Efeito trap.txt`

The tutorial identifies six consecutive bytes beginning at SLUS offset
`0x8B724`. These are initialized data rather than instruction immediates and
map to resident addresses `0x8009AF24`-`0x8009AF29`:

| SLUS offset | Resident address | Retail byte | ATK threshold | Trap card |
|---:|---:|---:|---:|---|
| `0x8B724` | `0x8009AF24` | `5` | `500` | House of Adhesive Tape (`681`) |
| `0x8B725` | `0x8009AF25` | `10` | `1000` | Eatgaboon (`682`) |
| `0x8B726` | `0x8009AF26` | `15` | `1500` | Bear Trap (`683`) |
| `0x8B727` | `0x8009AF27` | `20` | `2000` | Invisible Wire (`684`) |
| `0x8B728` | `0x8009AF28` | `30` | `3000` | Acid Trap Hole (`685`) |
| `0x8B729` | `0x8009AF29` | `255` | `25500` | Widespread Ruin (`686`) |

The trap-selection path in `func_8001F0D0` scans indices `5` through `0`.
For each available trap it loads the corresponding byte from
`D_8009AF24`, multiplies it by 100 using shifts and additions, and compares
that product with the attacking monster's calculated ATK. The comparison
accepts equal values, matching the documented "ATK less than or equal to"
effects.

After selecting an index, the function adds `0x2A9` (decimal `681`) to
produce the trap card ID. This independently fixes the table order to the
first six trap cards. The retail value `255` gives Widespread Ruin an
effective `25500` threshold, which is why it behaves as the unrestricted
attack-trigger trap under normal duel stats.

Each edit changes a table byte directly. For example, a desired threshold of
`5000` uses decimal byte value `50`, encoded as hexadecimal `0x32`; writing
decimal `50` as hexadecimal `0x50` would instead select `80 * 100 = 8000`.
Values above `25500` cannot be represented by this one-byte table.

**Confidence:**

- **Confirmed** that `0x8B724`-`0x8B729` are the six threshold bytes consumed
  by `func_8001F0D0`.
- **Confirmed** that each value is multiplied by 100 and compared inclusively
  with calculated ATK.
- **Confirmed** that indices `0`-`5` map to card IDs `681`-`686` in the trap
  order shown above.

## Guardian Star bonus magnitude

**Tutorial:** `Guardian Star V2 - Por Rafael Ferreira.txt`

The tutorial identifies three little-endian `500` immediates in the SLUS:

| SLUS offset | VRAM | Retail bytes | Instruction role |
|---:|---:|---|---|
| `0x10770` | `0x8001FF70` | `F4 01 42 28` | Test whether the displayed modifier is below `500` |
| `0x10784` | `0x8001FF84` | `F4 01 02 24` | Clamp the displayed modifier to `500` |
| `0x1D3D0` | `0x8002CBD0` | `F4 01 02 24` | Return the positive Guardian Star matchup modifier |

The first two sites are in `func_8001F55C`. That path advances a signed
display value by `16` per update, clears its active flag once the value
reaches the limit, clamps it to `500`, and writes the result to two display
fields. Both immediates must change together to preserve the same stop and
clamp value.

The third site is the positive return in the exact matching
`Duel_CalcGuardianStarMatchup`. Guardian Star IDs `1`-`6` form one advantage
cycle and IDs `7`-`10` form another. Adjacent values in one direction return
`500`, adjacent values in the opposite direction return `-500`, and all
other pairs return zero.

The tutorial changes only the positive `500` return. The negative immediate
is a separate instruction at SLUS offset `0x1D3E4`, with retail bytes
`0C FE 02 24` for `-500`. A patch that changes the three listed positive
values but leaves `0x1D3E4` untouched makes favorable and unfavorable
matchups asymmetric.

The tutorial's replacement table correctly treats the value as a 16-bit
little-endian integer: for example, decimal `1000` is hexadecimal `0x03E8`
and is written as bytes `E8 03`.

**Confidence:**

- **Confirmed** that the first two offsets are the display stop/clamp pair.
- **Confirmed** that `0x1D3D0` and `0x1D3E4` are the positive and negative
  matchup returns.
- **Confirmed** that changing only the tutorial's three offsets leaves the
  negative matchup modifier at `-500`.

## Spellbinding Circle and Shadow Spell reductions

**Tutorial:** `Shadow Spell e Spellbinding Circle.txt`

The tutorial replaces two paired signed immediates in `func_80025D30`:

| Spell | Role | SLUS offset | Retail bytes/value | Tutorial bytes/value |
|---|---|---:|---|---|
| Spellbinding Circle | Active-card modifier | `0x16660` | `0C FE` / `-500` | `44 FD` / `-700` |
| Spellbinding Circle | Effect-record modifier | `0x1666C` | `0C FE` / `-500` | `44 FD` / `-700` |
| Shadow Spell | Active-card modifier | `0x16680` | `18 FC` / `-1000` | `F1 D8` / `-9999` |
| Shadow Spell | Effect-record modifier | `0x16688` | `18 FC` / `-1000` | `F1 D8` / `-9999` |

The function selects the Spellbinding Circle branch when the current effect
card ID is `0x15D` (decimal `349`). It subtracts `500` from the target
`DuelCardRecord.stat_modifier` at offset `+0x12`, then stores the same
`-500` value in the paired effect record. The alternate branch performs the
same two writes with `-1000`, matching Shadow Spell's stronger two-level
reduction documented by the card behavior research.

The first value in each pair changes the duel state used by card-stat
calculation. The second keeps the associated effect/display record aligned
with that reduction. Editing only one offset can therefore make the applied
modifier and its effect state disagree.

The tutorial values are signed 16-bit little-endian integers:

```text
-700  = 0xFD44 -> 44 FD
-9999 = 0xD8F1 -> F1 D8
```

Both replacements fit in the signed halfword written by the retail code.

**Confidence:**

- **Confirmed** that the retail pairs are `-500` and `-1000`.
- **Confirmed** that the first write updates the active card's
  `stat_modifier`.
- **High** that the second write is the visual/effect-state counterpart; its
  record is populated from the card's display object immediately before the
  paired modifier write, but its complete layout remains unnamed.

## Force Exodia wins to S-TEC

**Tutorial:** `Exodia Sempre S-Tec - Por Wladmir Ghost.txt`

The tutorial changes the same signed end-reason value at three code sites:

| SLUS offset | VRAM | Retail instruction | Role |
|---:|---:|---|---|
| `0x8BD0` | `0x800183D0` | `addiu $v0, $zero, 40` | Recognize the Exodia end reason in the summon sequence |
| `0x9BDC` | `0x800193DC` | `addiu $v1, $zero, 40` | Write the winner's Exodia end-reason byte |
| `0x11DF4` | `0x800215F4` | `addiu $v0, $zero, 40` | Recognize Exodia while building the duel result/rank display |

Retail uses end reason `+40` for Exodia. The writer stores only its low byte,
and both readers load that field as a signed byte. Replacing each immediate's
little-endian bytes `28 00` with `81 FF` changes the shared value to `-127`
while keeping all three producer/consumer sites synchronized.

`Duel_CalcRankScore` starts each score at `50`, adds the signed end-reason
byte, then adds the ten statistic-table adjustments. Existing exhaustive
rank research gives a retail maximum of `+139`; because an Exodia reason of
`+40` contributes to that maximum, the other terms can contribute at most
`49`:

```text
patched maximum = 50 - 127 + 49 = -28
```

A score of `9` or lower maps to S-TEC, so the patched Exodia result remains
S-TEC regardless of the other tracked duel statistics. Updating the two
comparison sites as well as the writer preserves the Exodia-specific summon
and result-display branches despite changing the score contribution.

**Confidence:**

- **Confirmed** that all three retail immediates are the same signed
  end-reason value.
- **Confirmed** that `81 FF` encodes `-127` and is stored/read as a signed
  byte.
- **Confirmed** from the score bounds that the patched value always produces
  S-TEC.
- **High** that both comparison sites preserve the complete Exodia-specific
  presentation path; their branches are clear in resident assembly, while
  the surrounding state machines remain only partially named.

## Library 3D-card crash workaround

**Tutorial:** `Tutorial Bug do Library e Bug 3D.txt`

The tutorial writes `5C 02 3C` beginning at SLUS offset `0x1B5ED`. The byte
immediately before that range is zero, so the complete instruction at
`0x1B5EC` changes from:

```text
00 00 00 00    nop
00 5C 02 3C    lui $v0, 0x5C00
```

That instruction maps to `0x8002ADEC` in `func_8002ACA4`, which is reached
from the Library menu update path. Retail code has just loaded the selected
card's packed word from `gDuel_adwCardStats[card_id - 1]`. It then extracts
bits `26`-`30` and calls `Model_LoadMonsterMerge` only when the value is less
than `CARD_TYPE_MAGIC` (`20`):

```text
type = (gDuel_adwCardStats[card_id - 1] >> 26) & 0x1F
if (type < 20)
    Model_LoadMonsterMerge(...)
```

The inserted `lui` overwrites the loaded word with `0x5C000000`. Its type
field is `23`, the `CARD_TYPE_EQUIP` value, so the comparison fails and the
monster-model load is skipped. The patch therefore avoids the reported crash
by suppressing this 3D setup path; it does not repair the card-name pointer
described in the first half of the tutorial. It also applies at the shared
decision point rather than only to the malformed card.

**Confidence:**

- **Confirmed** that the three-byte edit completes the instruction
  `lui $v0, 0x5C00` at `0x8002ADEC`.
- **Confirmed** that the replacement produces type value `23` and bypasses
  the `Model_LoadMonsterMerge` call.
- **High** that bypassing this call is the complete reason the tutorial's
  reported Library crash disappears; the failure itself has not been
  reproduced with the external card editor.

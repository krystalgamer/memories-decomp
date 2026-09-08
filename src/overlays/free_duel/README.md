# Free Duel Overlay

This directory contains matching source from the Free Duel runtime
module.

Verified boundaries:

| Item | Range |
|---|---|
| WA package | sectors `7816-7903` |
| Executable phase | sectors `7898-7903`, `0x2800` bytes |
| Runtime image range | `0x80168000-0x8016A800` |

The package also contains phases loaded elsewhere; the five-sector executable
phase is not a complete inner-file manifest. Module-scoped symbol evidence is
kept in
[`notes/research/Unchiga_Symbols/modules/free_duel.txt`](../../../notes/research/Unchiga_Symbols/modules/free_duel.txt).

The verified executable phase is tracked in
[`config/slus_01411/overlays.json`](../../../config/slus_01411/overlays.json).
`make overlays` extracts it to `tmp/overlays/free_duel/module.bin`, and
`make verify-overlays` checks the archive and payload hashes.

Its accepted build layout is
[`config/slus_01411/overlays/free_duel.yaml`](../../../config/slus_01411/overlays/free_duel.yaml).
The adjacent `_functions.csv` inventory tracks per-function status, and
`free_duel_matching_c.json` maps accepted source/profile pairs.
`make match-overlays` remains the complete module's exact-byte gate.

## Committed and target cursor coordinates

The cursor has two separate signed-byte coordinate pairs:

| Address | Symbol | Role |
|---|---|---|
| `0x8009B366` | `gFreeDuel_bCursorColumn` | Committed grid column |
| `0x8009B367` | `gFreeDuel_bCursorRow` | Committed grid row |
| `0x8009B36C` | `gFreeDuel_bTargetColumn` | Requested column, clamped to `0..4` |
| `0x8009B36D` | `gFreeDuel_bTargetRow` | Requested row, clamped to `0..7` |

`FreeDuel_UpdateScreen` changes the target pair on directional input.
`FreeDuel_UpdateCursorTween` compares it with the committed pair, performs
the eight-step movement, and copies the target into the committed pair on
completion. Confirmation and duel-record updates use the committed position.
`FreeDuel_PlaceCursor` retains its existing mixed read of target column and
committed row when forming the record index; naming must not collapse the
two storage locations merely because they agree after a completed tween.

The committed names retain their live-trace evidence. The target names are
corroborated by the matching input/tween/init bodies and recorded with high
confidence in
[`notes/semantic-symbol-map.csv`](../../../notes/semantic-symbol-map.csv).
The module imports these resident byte addresses through
[`free_duel_linker_symbols.txt`](../../../config/slus_01411/overlays/free_duel_linker_symbols.txt).

## Screen-runtime translation unit

`screen_runtime.c` keeps the overlay entry tick next to the screen update it
calls and the cursor tween that update drives. They share the
committed/target coordinate pairs, the cursor widget `gFreeDuel_pCursorWidget`
and the screen-state flags.

`FreeDuel_Entry` is the per-frame tick: it advances the shared RNG, calls
`FreeDuel_UpdateScreen`, drives the cursor widget's own scale pulse through a
triangle wave over `D_8009B0CC & 0x7F`, then calls `FreeDuel_UpdateSparkle`.
That last callee stays in `sparkle_runtime.c`; this unit does not absorb it.

The definitions remain in executable order, which here is not call order:
`FreeDuel_UpdateCursorTween` occupies `0x80168A9C..0x80168C7C`,
`FreeDuel_UpdateScreen` runs through `0x80168FB4`, and `FreeDuel_Entry` closes
the unit at `0x80169030`. All three use `gcc_2_8_1_g0_split`, and their shared
manifest source and one C subsegment at module offset `0xA9C` cover the
complete contiguous `0x594`-byte text range, with no data or rodata
contribution. Data still begins at module `+0x1030`. Preserve that order and
complete extent when editing the group.

## Sparkle-runtime translation unit

`sparkle_runtime.c` keeps the sparkle-pool allocator next to the updater that
releases each completed object and clears its pool slot. Both functions
reverse-scan the same 16-entry `gFreeDuel_apSparklePool`.

The definitions remain in executable order: `FreeDuel_GetSparkleSlot`
occupies `0x8016899C..0x801689D4`, followed by `FreeDuel_UpdateSparkle`
through `0x80168A9C`. Both use `gcc_2_8_1_g0_split`, and their shared
manifest source and one C subsegment at module offset `0x99C` cover the
complete contiguous `0x100`-byte text range.

## Input publication and button meanings

Normal browsing uses `gInput_wPad1Held` for directional movement and
`gInput_wPad1Pressed` for Cancel and Confirm. The named masks come from
[`input.h`](../../game/input.h); the code-derived controls and their
precedence are recorded in
[`the-game.md`](../../../notes/research/the-game.md#8-free-duel).

Held directions take precedence over button actions, and Cancel is tested
before Confirm. The accesses remain separate volatile reads, not a
timer-repeat sample or a cached once-per-call value. The names preserve the
existing access pattern, dialog handling and cursor-movement gates.

Keep unaccepted candidate sources, objects, and diffs under `tmp/`. Do not add
this module to the resident `config/slus_01411/matching_c.json`.

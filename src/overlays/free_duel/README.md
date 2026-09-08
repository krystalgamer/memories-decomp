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

## Screen-update translation unit

`update_screen.c` keeps the cursor tween next to the screen update that calls
it. They share the committed/target coordinate pairs, cursor widget and
screen-state flags.

The definitions remain in executable order: `FreeDuel_UpdateCursorTween`
occupies `0x80168A9C..0x80168C7C`, followed by `FreeDuel_UpdateScreen` through
`0x80168FB4`. Both use `gcc_2_8_1_g0_split`. Their individual manifest entries
cover the complete `0x518`-byte text section, with no data or rodata
contribution. One C subsegment selects the group at module offset `0xA9C`.
Preserve that order and complete extent when editing the group.

## Sparkle-runtime translation unit

`sparkle_runtime.c` keeps the sparkle-pool allocator next to the updater that
releases each completed object and clears its pool slot. Both functions
reverse-scan the same 16-entry `gFreeDuel_apSparklePool`.

The definitions remain in executable order: `FreeDuel_GetSparkleSlot`
occupies `0x8016899C..0x801689D4`, followed by `FreeDuel_UpdateSparkle`
through `0x80168A9C`. Both use `gcc_2_8_1_g0_split`, and their shared
manifest source and one C subsegment at module offset `0x99C` cover the
complete contiguous `0x100`-byte text range.

## Duelist portrait record sizes

`FreeDuel_Init` walks the transferred portrait arena with two named strides
rather than raw literals:

| Constant | Value | Meaning |
|---|---|---|
| `FREE_DUEL_PORTRAIT_IMAGE_SIZE` | `2304` (`0x900`) | Offset from a record's start to its CLUT |
| `FREE_DUEL_PORTRAIT_RECORD_SIZE` | `2432` (`0x980`) | Stride from one record to the next |

Both values are backed by [`notes/mrg-files.md`](../../../notes/mrg-files.md),
which records that this package supplies 40 portrait records of `0x980` bytes
each, and that each record holds a `48 x 48` 8-bit image of `0x900` bytes
followed by its palette. The `LoadImage2` pair in the upload loop uses exactly
that split: the image at the record base, the CLUT at `+0x900`.

The neighbouring literals in that loop are deliberately **left as numbers**.
The `5`-iteration outer loops and the `25`/`15` split are VRAM band
dimensions, not the logical grid: the first rectangle band holds 25 portraits
and the second holds the remaining 15. `5` there coincides in value with
`FREE_DUEL_GRID_COLUMN_COUNT` but does not mean it, and `8` never appears,
so substituting the grid constants would assert a relationship the code does
not establish.

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

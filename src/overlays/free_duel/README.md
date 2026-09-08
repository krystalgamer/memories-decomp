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
and the second holds the remaining 15.

The positive evidence is the rectangle setup itself. `img.x` is pinned to
`128` for the first band and `256` for the second, `img.y` is
`row * 48 + 256`, and each cell is `24` wide by `48` tall. Those are VRAM
texture-page coordinates and a half-width 4bpp storage layout, so the `5`
counts rows within a texture page rather than grid columns.

The grid's row count is **not** absent from the loop, and an argument from its
absence would be wrong: the second band stops on
`count >= FREE_DUEL_GRID_ENTRY_COUNT`, which is defined as
`FREE_DUEL_GRID_COLUMN_COUNT * FREE_DUEL_GRID_ROW_COUNT`. The row count is
therefore present transitively and is exactly what terminates the upload,
which also explains the split arithmetically: `25 + 15 = 40 = 5 x 8`, the two
bands together uploading precisely one grid's worth.

So the split is band capacity while the total it sums to is the grid. Both are
real facts about different things, which is why the grid constants do not
belong on the band bounds.

## Display-object flag bits

The renderable and screen-space bits in the `u16` flag word at object offset
`+8` now use the shared names from `src/game/display_object_layout.h`:

| Site | Was | Now |
|---|---|---|
| `FreeDuel_Init` grid tiles | `obj->attr &= ~8` | `~DISPLAY_OBJECT_FLAG_SCREEN_SPACE` |
| `FreeDuel_Init` scroll banners | `obj->attr \|= 8` | `DISPLAY_OBJECT_FLAG_SCREEN_SPACE` |
| `FreeDuel_Init` hidden cursor | `obj->attr &= ~0x40` | `~DISPLAY_OBJECT_FLAG_RENDERABLE` |
| `FreeDuel_UpdateScreen` cursor reveal | `\|= 0x40` | `DISPLAY_OBJECT_FLAG_RENDERABLE` |

`attr` in this module's local `Obj` view sits at `+8` — after a four-byte pad
and the separate 32-bit word at `+4` — so it is the same field the resident
renderers test through `*(u16 *)(e + 8)`.

Two neighbouring literals are deliberately **left as numbers**:

- `obj->attr |= 0x28` combines `0x8` with a `0x20` bit that
  `display_object_layout.h` does not define. Naming only half of a composite
  would imply the rest is understood, so the whole value stays raw.
- The 32-bit `obj->flags` writes (`0x1000000`, `0x8000000`, `0x50000000`) are
  a **different field at `+4`**, not the flag word, and none of the display
  object flag constants apply to them.

## The opponent grid scrolls vertically

The forty grid entries are laid out in one coordinate space taller than the
screen, and the module scrolls a viewport over it rather than paging.

`FreeDuel_Init` places every entry at `X = (index % FREE_DUEL_GRID_COLUMN_COUNT)
* 56 + 20` and `Y = (index / FREE_DUEL_GRID_COLUMN_COUNT) * 52 + 40`, so rows
0 through 7 occupy `Y` 40 to 404 — well past the 240-line display. The entries
are created with `DISPLAY_OBJECT_FLAG_SCREEN_SPACE` cleared, so they are world
space and move with the viewport.

`FreeDuel_UpdateScrollbar` drives that viewport from the cursor. It keeps the
cursor's offset within the visible window between `0x28` and `0x90`, pushing
`gGraphics_sViewportY` up or down whenever the cursor leaves that band, and it
positions the scrollbar thumb with

```c
gFreeDuel_pThumbWidget->y = (cursor->y - 0x28) * 72 / 364 + 7;
```

The `364` in that expression is exactly `7 * 52` — the row pitch times
`FREE_DUEL_GRID_ROW_COUNT - 1` — which is the full scroll travel of an
eight-row grid whose first row sits at `Y = 40`. `gGraphics_sViewportY` is
zeroed by `FreeDuel_Init`, so the screen opens at the top of the grid.

The scroll numbers are **not** named. `0x28`, `0x90`, `72` and `7` are
widget-layout values whose relationship is established only by this one
expression, and `364`, while derivable as `7 * 52`, is written as a literal
rather than as that product. Recording the geometry is useful; asserting a
formula the code does not spell out would not be.

### The placement split at 25 is a texture band, not a visible page

The two placement loops split at index 25, which looks like a visible-page
count and is not. Together they place all forty objects: the second loop takes
its `Y` from the **absolute** index `k`, continuing into rows 5 to 7 of the
same unbroken space. What changes at 25 is the texture-page selector — `18`
for the first band, `20` for the second — with the texture V coordinate
restarting. That is the same VRAM band boundary as the upload loops above,
for the same reason.

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

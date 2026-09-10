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

## Opponent-selection lifecycle translation unit

`screen_runtime.c` is the Free Duel overlay's complete text section: nine
functions in executable order from `FreeDuel_UpdateScrollbar` at `0x80168004`
through `FreeDuel_Entry` at `0x80168FB4`. One
`gcc_2_8_1_g0_split` C subsegment at module offset `0x4` covers the full
`0x102C`-byte range through `0x80169030`; data begins immediately afterward at
module `+0x1030`.

The former cursor-layout, initialization, and per-frame source boundaries did
not mark object or ownership boundaries:

- `FreeDuel_Init` calls both cursor-layout helpers, creates the cursor with
  `FreeDuel_SpawnSparkle`, and initializes the shared screen flags, committed
  and target coordinates, viewport, and sparkle pool.
- `FreeDuel_UpdateCursorTween` calls the same placement helper and sparkle
  constructor, while `FreeDuel_UpdateScreen` drives the tween and scrollbar.
- `FreeDuel_Entry` drives the screen update, cursor pulse, and sparkle-pool
  updater each frame.

`gFreeDuel_pCursorWidget` and `gFreeDuel_pThumbWidget` are declared once, as
byte pointers, in `free_duel.h`; `FreeDuel_UpdateScrollbar` casts them to the
verified `FreeDuelWidget` `x`/`y` prefix at the use, `FreeDuel_UpdateScreen`
casts the cursor when it calls `FreeDuel_PlaceCursor`, and initialization and
runtime code keep their offset-based byte accesses. `D_800EB0F8` keeps a byte
alias for `FreeDuel_PlaceCursor` beside the typed `DuelEffectChannel`
declaration used by the runtime.

The sparkle-pool allocator and updater remain at `0x8016899C` and
`0x801689D4`. Both reverse-scan the same 16-entry
`gFreeDuel_apSparklePool`, and their direct callers are now visibly in the
same source: `FreeDuel_UpdateCursorTween` takes the pool slot and
`FreeDuel_Entry` runs the updater.

One unit settles `FreeDuel_GetSparkleSlot`'s return type, which the caller
declared `u8 **` and the definition spells `void **`. The definition wins and
the single call site takes a `void **` local.

`func_8004036C` keeps the local `void (void)` declaration the sparkle updater
carried, and the reason for it: its two calls pass no argument at all, so
`display_object_api.h`'s typed `void func_8004036C(void *)` would make the
compiler set up an argument the retail image does not. Nothing else in this
unit's include set declares that symbol, so the local spelling is still the
only one in scope - worth stating, because it is the kind of thing a merge
can quietly break.

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
- The 32-bit `obj->flags` writes are a **different field at `+4`**, not the
  flag word, so no display object flag constant applies to them. They are
  libgs `GsSPRITE` attribute bits, and `libgs.h` names most of them:
  `0x8000000` is `GsROTOFF` and `0x50000000` is `GsALON | GsAONE`, both now
  spelled that way. `0x1000000` is bit 24, which `libgs.h` does not name, so
  it stays raw.

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
((FreeDuelWidget *)gFreeDuel_pThumbWidget)->y =
    (cursor->y - 0x28) * 72 / 364 + 7;
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

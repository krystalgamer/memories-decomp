# Overlay named-constant audit

This records a systematic sweep of the five overlay modules for magic numbers
that could adopt existing named constants from `src/game/*.h`.

Both halves are recorded. The negative results are the more useful half:
several substitutions look correct by value and are wrong by meaning, and
without a written record the next sweep will rediscover and possibly apply
them.

## Method

Collect every simple numeric `#define` in `src/game/*.h` whose value is at
least `16` — smaller values collide with ordinary arithmetic too often to be
evidence of anything — then locate raw literals of those values across
`src/overlays/*/*.c`. Inspect each site's surrounding code rather than
trusting the value match.

A value match is a *lead*, never a conclusion. The constant must also name the
concept the site is using.

## Applied

| Site | Constant | Backing |
|---|---|---|
| `FreeDuel_Init` record walk (4 literals) | `FREE_DUEL_PORTRAIT_IMAGE_SIZE`, `FREE_DUEL_PORTRAIT_RECORD_SIZE` | [`../mrg-files.md`](../mrg-files.md): 40 portrait records of `0x980`, each a `48 x 48` 8-bit image of `0x900` plus palette |
| `Password_RefreshDigitDisplay`, `NameEntry_UpdateDialog` (5 literals) | `TEXT_SINGLE_BYTE_GLYPH_LIMIT`, `TEXT_STRING_TERMINATOR` | Resident `func_80038148` already spells the same two-byte glyph rule with these names |

## Negative results

### `0xF0` means three different things in the password module alone

Besides the glyph limit, it is the 240-pixel screen bottom in
`NameEntry_UpdateGlyphFragment`'s offscreen test on the Y field at `+0x32`,
and inside a single `NameEntry_Init` call it appears twice more as *both* a
text-ID base and a 240-pixel box extent. A value-driven rewrite would have
been wrong at three of five sites.

### Same value, different concept, in the Free Duel upload loop

In `FreeDuel_Init` the outer loop bound `5` coincides with
`FREE_DUEL_GRID_COLUMN_COUNT` but is a VRAM rectangle band dimension.

The positive evidence is in the rectangle setup itself: `img.x` is pinned to
`128` for the first band and `256` for the second, `img.y` is
`row * 48 + 256`, and each cell is `24` wide by `48` tall. Those are VRAM
texture-page coordinates and a half-width 4bpp storage layout, not screen
positions or grid indices. The `5` counts rows *within a texture page*.

Note that the grid's row count is **not** absent from this loop, and an
argument from its absence would be wrong. The second band terminates on
`count >= FREE_DUEL_GRID_ENTRY_COUNT`, and that constant is defined as
`FREE_DUEL_GRID_COLUMN_COUNT * FREE_DUEL_GRID_ROW_COUNT` — so the row count is
present transitively and is exactly what stops the upload. It also explains
the partition arithmetically: `25 + 15 = 40 = 5 x 8`, the two bands between
them uploading precisely one grid's worth of portraits.

So the `25`/`15` split is band capacity, while the total it sums to is the
grid. Both facts are real; they are simply about different things.

### `0x404040` is a domain mismatch, not a missing constant

The value appears in `name_entry_runtime.c` and `free_duel/screen_runtime.c`,
and in resident `func_80060B38.c`. The only constant with that value is
`DUEL_DISPLAY_COLOR_DIMMED` in [`../../src/game/duel_display.h`](../../src/game/duel_display.h).

All three sites write a colour field — two through `*(u32 *)&obj->r` and
`*(u32 *)&sp->r`, packing an RGB triple, and the third through
`*(s32 *)(obj + 0xC)`, the offset documented as the colour word. So these are
positively display-layer colour writes.

That is precisely why the duel constant does not fit.
`duel_draw_status_numbers.c` documents `0x404040` as the *dim* member of a
shade pair whose *active* sibling is `0x808080`, selected per side by
`D_8009B1D5`. The overlay sites take part in no such pairing: a name-entry
object and a sparkle are not one side of a two-sided status readout. Adopting
the name would import a shade-family relationship that does not exist here.

If the shared grey is worth naming, it needs a neutral display-layer constant
rather than the duel one. That is a design decision, not a mechanical
substitution, and it is deliberately left open.

### The common bit values are unusable by value alone

Each of `0x10`, `0x20`, `0x40`, `0x80`, `0x100`, `0x1000` and `0x8000` matches
between four and twelve distinct constant names spanning input, display-object
flags, sound sequencing, memory card and campaign flags. `0x40` alone is a
candidate for `PAD_BUTTON_CROSS`, `DISPLAY_OBJECT_FLAG_RENDERABLE`,
`SD_SECONDARY_PAN_CENTER` and five others.

These require per-site semantic evidence. A sweep cannot resolve them, and
guessing would inflate the apparent level of understanding while actually
reducing it.

## Remaining scope

The mechanically safe overlay substitutions are exhausted. What remains is
genuine semantic work, in rough value order:

1. **A decision** on whether a neutral shared constant for the `0x404040` grey
   is wanted, and where it should live. All three sites are display-layer
   colour writes, so a constant is defensible; the duel one is not the right
   one to reuse.
2. The composite `0x28` writes remain raw. The `0x20` bit's **provenance and
   consumer are now both established**, but the consumer is unmatched
   assembly, so a name would rest on disassembly rather than build-verified C.

   An earlier version of this entry also listed `0x48` as blocked. That was
   wrong: `0x48` is `0x40 | 0x08`, both of which are named, and it contains no
   `0x20` at all. Its single site is now written with those two constants.
   Only `0x28` — which is `0x20 | 0x08` — depends on the open bit.

   `func_80040468` — the configurator reached through `func_800404CC` and
   `func_800428A8` — clears `0x20` from the flag word at `+8` and then sets it
   again only when the texture argument has bit `0x8000`:

   ```c
   flags = *(u16 *)(object + 8) & 0xFFDF;
   *(u16 *)(object + 8) = flags;
   if (texture & 0x8000) {
       *(u16 *)(object + 8) = flags | 0x20;
   }
   ```

   So the bit is texture-derived. **Its consumer is now identified too**, in
   generated assembly rather than in tracked C, which is why an earlier pass
   over `src/` alone concluded there was no reader. No tracked renderer tests
   it: `display_object_list_renderers.c` and `func_80040588.c` check only
   `DISPLAY_OBJECT_RENDERABLE_MASK`, `DISPLAY_OBJECT_FLAG_SCREEN_SPACE` and
   `DISPLAY_OBJECT_FLAG_CLIP_TEST`.

   The single reader is `func_8004158C`, the unmatched `0x700`-byte sprite
   builder called from `func_80040814.c`. At `0x800416F0` it loads the flag
   word and branches on the bit. The packet's texture coordinates have just
   been initialised from the object's `+0x40` and `+0x42` — the tpage and clut
   halves the configurator wrote from the same `texture` argument. When the
   bit is set, it adds a further cell offset taken from the byte at `+3` of
   the record pointed to by object `+0x4C`:

   - the low nibble, shifted left by 4, is added to the U coordinate;
   - the high nibble is added to the V coordinate.

   So the bit selects whether that extra per-cell `(U, V)` displacement is
   applied on top of the base texture coordinates. It is one packed nibble
   pair, not two independent fields.

   Worth recording alongside that: the overlay `|= 0x28` writes are **not**
   redundant with the configurator, they override it. In `FreeDuel_Init` the
   preceding `func_800428A8(obj, 0, 0, 0, 0, 1, 16, 0, D_801AF000)` passes
   texture `0`, so the configurator has just *cleared* `0x20`; the overlay
   then forces it back on. Those callers are therefore opting into the cell
   offset for objects whose texture argument did not request it.

   A name is now supportable on producer-and-consumer evidence, which is the
   standard the rest of this note applies. It is deliberately **not** minted
   here, for two reasons worth stating rather than glossing: the consumer is
   still unmatched assembly, so the reading rests on disassembly rather than
   on compiled C; and naming the bit means touching the remaining composite
   `0x28` sites across two overlay modules, which is a source change and
   belongs in its own reviewable PR rather than in a note.

   What the bit is **not**: it is not a visibility or draw-order control, and
   it does not select a texture page. The page and clut come from `+0x40` and
   `+0x42` regardless; this only shifts the coordinates within them.

### Closed since this note was written

- **Display-object flag bits** — done. The renderable (`0x40`) and
  screen-space (`0x8`) bits at object offset `+8` are named against
  `display_object_layout.h`. `free_duel/screen_runtime.c` formerly declared
  `u32 flags; u16 attr;` after a four-byte pad, so naming by member name
  rather than offset would have been backwards. It now uses the shared
  `DisplayObject`: `attribute` is the word at `+4` and `flags` the halfword
  at `+8`, preserving the same accesses.
- **Screen-extent literals `0x140`/`0xF0`** — closed as *not* nameable. All
  eight call sites of the text-box builder `TextBox_Create` (`0x80035BE4`)
  take `(slot, textId, x, y, w, h)`, and the trailing pair is a per-call box
  extent (`0xA0 x 0x40`, `96 x 16`, `100 x 100`, `288 x 16`, and so on).
  Only one of the eight is `320 x 240`, so naming that one occurrence after
  the display mode would assert a dependency the builder does not have.
- **The "third `25`" — traced, and it is *not* a visible-page extent.** An
  earlier draft of this note recorded it as grid-shaped evidence of a
  five-row visible page. That was wrong, and the correction matters because it
  is the same reasoning-by-coincidence this note exists to prevent.

  The two placement loops after `done:` together place **all forty** objects,
  not twenty-five. The first runs `i = 0..24` at `Y = (i / COLUMN_COUNT) * 52
  + 40`, and the second runs `k = 25..39` at `Y = (k / COLUMN_COUNT) * 52 +
  40` — using the *absolute* index for Y, so it continues into rows 5, 6 and
  7 of one unbroken coordinate space. What actually changes at `25` is the
  texture-page selector, `18` in the first loop and `20` in the second, with
  the texture V coordinate restarting at `i / COLUMN_COUNT`. That is the same
  VRAM band boundary as the upload loops, for the same reason.

  So the third `25` is not a second meaning after all — it is the first
  meaning again.

  Vertical scrolling is nevertheless real, on independent evidence:
  `FreeDuel_UpdateScrollbar` drives `gGraphics_sViewportY` from the cursor,
  holding it between `+0x28` and `+0x90` of the viewport, and positions the
  thumb by `(cursor->y - 0x28) * 72 / 364 + 7`. That `364` is exactly
  `7 * 52`, the pitch between rows times `FREE_DUEL_GRID_ROW_COUNT - 1`, which
  is the full travel of an eight-row grid whose first row sits at `Y = 40`.
  The grid objects are placed with the screen-space flag cleared, so they move
  with that viewport.

  No constant was minted from this. The scroll geometry is consistent and
  well evidenced, but `0x28`, `0x90`, `72` and `7` are widget-layout numbers
  whose relationship to each other is not established by anything except this
  one expression.

# Checkerboard background records

`Checkerboard_DrawBackground` (`0x8003D334`, `src/game/mem_card.c`)
tiles the screen with alternating sprite cells. It has no caller, so its two
records in `src/game/mem_card.h` are described only by this
function. Every member it touches now has a name. The renames are spelling
only: offsets, widths, signedness and the `uv` union's layout are unchanged,
and every image still matches byte for byte.

## `CheckerboardSprite`

The scratchpad record at `0x1F800000` is handed to `GsSortFastSprite` as a
`GsSPRITE *` for every cell, and its 0x18 bytes are the first 0x18 bytes of
that libgs type. The members take `libgs.h`'s names, and `SpritePrim`'s names
for the packed pairs.

| Offset | Old | New | Evidence | Confidence | Uses |
|---|---|---|---|---|---|
| `0x00` | `field_00` | `attribute` | `GsSPRITE.attribute`; loaded from the state's `attribute` | high | 1 |
| `0x04` | `field_04` | `x` | `GsSPRITE.x`; starts at 0 or 32 and steps by the 64-pixel cell width | high | 6 |
| `0x06` | `field_06` | `y` | `GsSPRITE.y`; set to the row's y, which steps by the 32-pixel cell height | high | 1 |
| `0x08` | `field_08` | `wh` | `GsSPRITE.w`/`h` stored as one word (`0x200020`, 32 by 32) | high | 1 |
| `0x0C` | `field_0C` | `tpage` | `GsSPRITE.tpage` (`0xB`) | high | 1 |
| `0x0E` | union `field_0E`, byte `bytes.field_0E` | union `uv`, bytes `bytes.u`/`bytes.v` | `GsSPRITE.u`/`v`; the word store sets both and the per-row stores pick `u` (`0x30` or `0x50`) | high | 3 |
| `0x10` | `field_10` | `cx` | `GsSPRITE.cx`; `CHECKERBOARD_CLUT_X` or 16 past it for the second cell colour | high | 3 |
| `0x12` | `field_12` | `cy` | `GsSPRITE.cy` (`0xFC`) | high | 1 |
| `0x14` | `field_14` | `rgb` | `GsSPRITE.r`/`g`/`b` stored as one word; loaded from the state's `rgb` | high | 1 |

## `CheckerboardState`

The header used to describe these as scroll offsets and a phase. They aren't:

| Offset | Old | New | Evidence | Confidence | Uses |
|---|---|---|---|---|---|
| `0x04` | `field_04` | `attribute` | Copied into the sprite's `attribute` word | high | 1 |
| `0x0C` | `field_0C` | `rgb` | Copied into the sprite's `rgb` word | high | 1 |
| `0x14` | `field_14` | `ot_depth` | Passed, masked to 16 bits, as `GsSortFastSprite`'s ordering-table priority | high | 1 |

All three sit at the offsets `DisplayObject` uses for `attribute`, `rgb` and
`ot_depth`, which fits a display object being passed as the state. With no
caller that stays an observation, so the record keeps its own type.

The function's two locals that held the copied words are renamed to match:
`field_04` and `field_0C` became `attribute` and `rgb`.

`DuelStatusDigitPacket` in `src/ygo_types.h` is the same `GsSPRITE` prefix,
used by `Duel_DrawLifePointsAndDeckCounts`. It is left for a separate change.
Its only user, `duel_scene.c`, edits the same lines as an open
pull request, and `ygo_types.h` puts every overlay image in scope for
verification.

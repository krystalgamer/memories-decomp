# Duel status digit packet

`DuelStatusDigitPacket` (`src/ygo_types.h`) is the scratchpad record at
`0x1F800320` that `Duel_DrawLifePointsAndDeckCounts` fills before each of its
four `Duel_DrawDecimalDigits` calls. It passes the record as a `GsSPRITE *`.
`Duel_DrawDecimalDigits` writes `u` from each digit, sorts the sprite with
`GsSortFastSprite`, and then adds 8 to `x`. The record's 0x18 bytes are the
first 0x18 bytes of `GsSPRITE`, so its members take `libgs.h`'s names. The
packed pairs take `SpritePrim`'s names, as `CheckerboardSprite` does.

| Offset | Old | New | Evidence | Confidence | Uses |
|---|---|---|---|---|---|
| `0x00` | `field_00` | `attribute` | `GsSPRITE.attribute`; set to `0x09000000` | high | 1 |
| `0x04` | `field_04` | `x` | `GsSPRITE.x`; the anchor object's x plus an offset, then advanced by `Duel_DrawDecimalDigits` | high | 4 |
| `0x06` | `field_06` | `y` | `GsSPRITE.y`; the anchor object's y plus an offset | high | 4 |
| `0x08` | `field_08` | `wh` | `GsSPRITE.w`/`h` stored as one word (`0x80008`, 8 by 8 digit cells) | high | 1 |
| `0x0C` | `field_0C` | `tpage` | `GsSPRITE.tpage` (`0x1E`) | high | 1 |
| `0x0E` | `field_0E` | `uv` | `GsSPRITE.u`/`v` stored as one halfword (`0x5800`: v = 0x58); `Duel_DrawDecimalDigits` rewrites the `u` byte per digit | high | 1 |
| `0x10` | `field_10` | `cxcy` | `GsSPRITE.cx`/`cy` stored as one word (`DUEL_SPRITE_CLUT_CXCY`) | high | 1 |
| `0x14` | `field_14` | `rgb` | `GsSPRITE.r`/`g`/`b` stored as one word: `DUEL_DISPLAY_COLOR_NORMAL` for the active side, `DUEL_DISPLAY_COLOR_DIMMED` for the other | high | 4 |

The rename is spelling only. Every member keeps its width and offset, and
`x`/`y` stay unsigned. The size assertion is unchanged. The offset
assertion is renamed for `rgb`.

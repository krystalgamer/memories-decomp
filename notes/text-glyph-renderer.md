# North American text-box glyph-entry renderer

`TextBox_DrawEntries` (formerly `func_80035E20`) is the resident draw callback
installed by `TextBox_RefreshDisplayObjects` (formerly `func_800391E4`)
in a display object's `field_4C`. Its North American branch selects the
object's text-box channel through `D_800EB0F8[obj->field_67]` and reads that
channel's `entry_head_24`, already declared `DuelEffectEntry *`.
The loop walks the same `0x1C` entries as `DuelEffect_AppendEntry` and
`DuelEffect_ProcessEntries`; it does not introduce another record layout.

## Observed entry fields

The renderer previously addressed entries through a byte cursor biased by
`+0x16`, alongside a separate halfword cursor for the Shift-JIS code. The
typed accessors recover the existing entry from that cursor.

| Entry offset | Old cursor access | Renderer interpretation |
| --- | --- | --- |
| `0x00` | separate `*code` | Shift-JIS glyph code |
| `0x04..0x07` | `p[-0x12]..p[-0xF]` | Four corner intensities copied into the GT4's RGB components |
| `0x08..0x0A` | `p[-0xE]..p[-0xC]` | Three rotation bytes, each scaled by `0x10` for `RotMatrixZYX_gte` |
| `0x0C`, `0x0E` | halfwords at `p-0xA`, `p-8` | Local x/y added to the display-object position |
| `0x10` | `p[-6]` | Glyph/icon selector on the flagged icon paths |
| `0x11` | `p[-5]` | Active marker and glyph-kind flags |
| `0x14` | `p[-2]` | Quad x expansion and quarter-sized y displacement |
| `0x15` | `p[-1]` | Render mode: sprite, rotating FT4, or two-pass GT4 |
| `0x16` | `p[0]` | Palette selection on ordinary glyph and small-icon paths |
| `0x17` | `p[1]` | Special wide-icon variant on the flagged icon path |
| `0x18` | `p[2]` | Font-size/layout selector on the ordinary glyph path |

These are interpretations within this renderer, not universal semantic
names for all effect entries. In particular, `0x14` is no longer called
padding: this reader uses it, but its shared name stays `field_14`.
The four-byte words at `0x04` and `0x08` retain their existing types and
alignment; byte views of the named words preserve the renderer's individual
byte loads without changing the compactor's aligned whole-entry copies.
Compile-time assertions cover the newly exposed member offsets.

The coordinates remain unsigned reads here. Although the shared fields are
`s16`, explicit `u16` casts retain the original `lhu` operations. Other
consumers' signed readings are not changed.

## Exact-match constraint

A local `DuelEffectEntry *entry = (DuelEffectEntry *)(p - 0x16)` at the
loop head, with the same named accesses, produced an executable four bytes
shorter than retail under the unchanged
`gcc_2_8_1_g8_split_no_strength_reduce` profile. It was rejected.
Expanding the typed view at each access instead preserves the original
biased byte cursor, the separately cached glyph-code cursor, and the
complete North American executable SHA-256:
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
No new profile, inline assembly, register pin, or data alias is involved.

The Japanese and European renderer branches retain their distinct record
strides and offsets unchanged. This conversion applies only to the
North American `0x1C` entry walk.

The shared display-object creation, outline positioning and callback ownership
contract is documented in [text-box-display.md](text-box-display.md).

The producer's regional return, partial-write and occupancy-query contracts
are in [duel-effect-entry-emission.md](duel-effect-entry-emission.md).

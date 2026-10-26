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

## Palette indices and range-persistent layout selectors

palette_index names the shared entry's original u8 field_16 at +0x16.
The complete NA producer/consumer chain copies channel color_index_54
in the extracted-symbol and ordinary-glyph append paths. The renderer
uses the byte as a palette index, not RGB or a ready-made CLUT word:
the small-icon path selects CLUT X as index*16+0x290, while ordinary
glyphs use CLUT Y index+0xE8. The shaded-quad path retains its original
packed CLUT expression and separate icon override. No new mask, clamp,
palette table or unified CLUT coordinate formula is introduced.

The direct 0x80 icon append path does NOT gain a palette store. Its
wide-icon variant at +0x17 and renderer-specific palettes stay distinct.
Every partial write, rejection point and tail/terminator publication
remains. Japan retains its existing shared-prefix +0x16 store for the
extracted-symbol path and the renderer's raw palette byte for that path;
normal Japanese glyph color is baked by the rasterizer instead. The
Japanese marker-entry type's pad_16 remains unsplit. Europe keeps its
existing color store through pad_14[0] at physical +0x14 and raw palette
loads; its adjacent raw +0x15 blend intensity remains separate.
Neither regional record gains an invented +0x16 palette declaration.

layout_selector names the NA entry's original u8 field_18 at +0x18.
DuelEffect_SetEntryRangeLayout (0x8003B6AC) writes the supplied value
with its original byte narrowing across the ENTIRE half-open channel
range, including inactive/future entries. It retains the empty-range
guard, original boundary loads and all unchecked index arithmetic.
DuelEffect_ResetEntryMarkers clears it only in the original NA branch;
JP/PAL records have no corresponding +0x18 member or new clear.
Append and channel initialization do NOT overwrite the preconfigured
selector. It is not the channel's independent Text_SetFontMode command.

All eight renderer comparisons still use the FULL byte. A value of one
selects the 16x16 glyph cells, lookup/fallback choices, nudges and wide
quad geometry; a value of two retains its original baseline displacement.
DuelEffect_UpdateObjectLayout alone uses selector%10 for choice-widget
row/layout selection. The real NA card-move path supplies 0xB through
the original func_8003B6AC compatibility call: eleven is NOT normalized
to one for glyph rendering. Name-entry keyboard construction supplies
one; existing dialog/debug/library/shop callers and every numeric value
remain unchanged. No global font-size enum or decimal encoding is added.

The debug campaign editor's existing diagnostic prints the same raw
selector after its value-two range configuration; neither call changes.
Six whole C units change only seventeen qualified non-comment member
identifiers and three existing comment identifiers. The whole shared
header changes two declarations, two existing assertions' member/label
identifiers and the existing glyph-kind caveat; the choice-layout header
updates one existing comment identifier. All widths, offsets,
sizes, alignment, qualifiers, G32 and numeric assertions remain. Other
DisplayObject, selection and cursor field_18 owners stay unchanged.
There are zero fixture edits and no compatibility-label/call rewrites.

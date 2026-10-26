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

## Glyph indices, occupancy release and wide-icon variants

glyph_index names the shared entry's original u8 field_10 at +0x10.
Its meaning follows the existing glyph-kind flags, not a new global
character encoding. The direct 0x80 append path narrows the input into
this byte, while the 0x100 path extracts bits 20..27. The NA ordinary
glyph path still stores zero and renders from the separate code word.
Japan's ordinary path stores the queried occupancy-slot index before
rasterization; the query still does not reserve that slot. Rasterization
publishes channel_index+1 to the occupancy table in the original order.
No bounds, tag rejection, allocation or publication behavior changes.

The Japanese renderer keeps its raw +0x10 UV reads. The Japanese
NameEntry_SpawnGlyphSprite reads the same glyph_index through the existing
shared prefix to derive its original 4-word-wide, 16-row VRAM rectangle,
then calls MoveImage and advances the existing copy-slot/sequence bytes.
This does not change the NA constructor's code-before-null read or its
unchecked allocation, nor Europe's distinct glyph byte and atlas metrics.
JapaneseDuelEffectEntry.pad_10 stays opaque; no regional record is split.

The legacy DuelEffectChannel entry-callback view at +0x10 uses the same
glyph_index spelling. DisplayObjectFade_ReleaseChannel still clears
occupancy[glyph_index] in NA/Japan, regardless of glyph kind, then clears
the existing occupied byte and requests compaction. Europe keeps its
occupancy-clear omission and physical+0x0F marker. No clamp, ownership
check, new reservation, lifetime change or real stream-pointer meaning
is introduced; unrelated DisplayObject/ScriptImage field_10 stays intact.

icon_variant names the shared entry's original u8 field_17 at +0x17.
Only the original NA direct-icon append path copies channel field_62.
Other glyph kinds gain no store, and Japan's trailing pad_16 is unsplit.
The NA renderer retains the FULL-byte >=0x14 and glyph_index<0x18 guard.
Variants 0x14/0x15/0x16/0x17 keep their exact U/CLUT-Y cases and default,
page 0x1E, 32x16 geometry, CLUT X 0x100 and attribute-bit update.
This is not palette_index, the glyph code, a Boolean or a closed enum:
values above the explicit cases still take the original default.

The selected-card command stages NA field_62 from non-monster card type
on its first-guardian path; its initial zero, opcode priority, color-only
early return, absent-second-star suppression and string expansion stay.
Choice/page completion keep their NA-only clears. Channel field_62 itself
remains address-named because Europe reads/writes a u16 output limit over
that byte and pad_63. No shared icon-variant name, new alias, union, padding
split, regional field, reset or expression rewrite is imposed on it.

Four whole C units change fourteen qualified member identifiers only.
The whole shared header changes three declarations, two existing offset
assertions' member/label identifiers and three existing comments. Widths,
offsets, alignment, qualifiers, G32, numeric assertions, raw regional
views, all other owner fields and all fixtures remain unchanged.

## Regional entry-local coordinate views

local_x/local_y name the original u16 field_0C/field_0E in the PAL
entry and legacy entry-callback prefix. The logical coordinates agree,
but their physical offsets do not:

| Existing view | Coordinate members | Physical offsets | Stored type |
| --- | --- | --- | --- |
| Shared NA entry / Japanese shared prefix | x_0C / y_0E, unchanged | +0x0C / +0x0E | s16 |
| Legacy DuelEffectChannel entry-callback prefix | local_x / local_y | +0x0C / +0x0E | u16 |
| EuropeanDuelEffectEntry | local_x / local_y | +0x02 / +0x04 | u16 |

The names describe glyph positions relative to their owning text display,
not the live channel's pen_x/pen_y, display-object world positions or
additional real stream fields. Append copies the pen into PAL coordinates
before its original rejection paths and retains the exact X-only glyph
metric adjustments and signed return values. The renderer adds its
display-object origin using the existing raw unsigned halfword reads;
its source, viewport handling, strides and byte-cursor biases do not change.

The PAL spiral callback saves these coordinates through its existing
halfword origin views, restores them on the same terminal path and keeps
the original rcos/rsin position updates. The delayed-rotation callback
reads the pair through the PAL or legacy prefix and retains its signed
shifts, byte delay narrowing and same-call first decrement. Neither
consumer gains a new coordinate conversion, type or callback signature.

PAL name-entry lookup keeps its (s16) casts and local-coordinate equality
tests. Glyph pulse still saves X, parks the SOURCE glyph at 0x400, and
restores it only at the original frame boundary. Sprite construction
still adds channel rect_x/rect_y to the explicitly signed local pair.
Shared x_0C/y_0E, Japanese padding, NA code-before-null/unchecked allocation
and all regional atlas, frame/tag and source-lifetime behavior remain.

Three whole C units change twenty-four qualified member identifiers only.
Two whole headers change four u16 declarations and one existing callback-
view comment; no type, offset, width, assertion, alignment, G32 or numeric
expression changes. Unrelated selection, view, packet and ritual fields
stay untouched, as do all fixtures. The remaining field_04/field_08/
field_14 storage is deliberately not given universal shade/rotation names:
handlers also use it for delay, saved origins, angle/radius and flags.

## Glyph code and entry-flag identities

glyph_code replaces the existing u16 code_00 at entry +0x00 in the
shared and PAL views. Ordinary NA glyphs and the password overlay establish
its Shift-JIS role; this does not make every icon or regional entry a
fully initialized character record. NA/Japan keep the original masked
WHOLE-word store through the entry base, including its retained high bit
and adjacent halfword. PAL keeps its original narrowing store of the
tagged input; its extracted glyph byte still uses pad_0D[1]. The Japanese
marker-only prefix is not split or given a new code member.

entry_flags is the original active/glyph-kind byte (flags_11), not a
replacement for glyph_index, palette_index, icon_variant or handler_state.
All bit tests and branch order stay literal, including the renderer's
0x80 active check, 0x60 kind test, 0x20 icon override and existing full
layout-selector comparisons. The biased renderer cursor and separately
cached raw code cursor remain unchanged, as do PAL/Japanese raw views.
The legacy entry callback's same flag byte gains the same spelling;
real channel stream pointers gain no entry-data interpretation.

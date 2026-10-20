# Text-box display refresh, outline and entry drawing

Three matching APIs cover the display-object family used by
[`TextBox_BuildStep`](../src/game/text_box_build_step.c) and
[`TextBox_SetPos`](../src/game/text_box_runtime.c). The channel owns the
object references; the shared fields do not acquire text-only names or types.

| API | Former NA identity | Role |
|---|---|---|
| `TextBox_RefreshDisplayObjects` | `func_800391E4` | Reuse/acquire the primary object, refresh geometry and depth, optionally replace the outline |
| `TextBox_UpdateOutline` | `func_80039140` | Position the existing six-point outline from the existing primary object |
| `TextBox_DrawEntries` | `func_80035E20` | Draw the channel's glyph/icon entries through the primary object's callback |

## Ownership and lifecycle

`TextBox_BuildStep` releases and nulls channel `field_30` and `field_2C`
before refreshing the display objects. It does not release or null
`field_28` at this point. The refresher only allocates a primary type-six
object when `field_28` is null; only that creation path installs the
two-argument `TextBox_DrawEntries` address at object `field_4C`, writes
the channel's `index_57` to object `field_67`, and sets its screen-space
flag and texture/CLUT fields.

Every refresh selects ordering table one, narrows channel `field_59` to
signed byte depth, and copies the four existing geometry halfwords into
the primary object. The width and height divided by two become its
half-extents. It does not reinstall the callback when reusing the object.

If the regional outline flag is set, an existing `field_2C` object is
released before acquiring a new type-four object. The Gouraud quad is
initialized, stored back, assigned the existing transparency attributes,
ordering table, `(s8)(field_59 - 1)` depth and six color words, then
positioned by `TextBox_UpdateOutline`. Neither helper adds allocation
failure handling, lifetime tracking or flag clearing. Repeated refreshes
with the outline bit set still replace the outline; they do not reuse it.

## Outline geometry and positioning

`TextBox_UpdateOutline` requires both object pointers to exist. It copies
the primary object's two half-extents, then writes six coordinate pairs
with the existing eight-pixel outside edges. Its middle lower pair uses
the primary half-height, while its bottom pair uses the full height plus
eight. The arithmetic remains unsigned halfword stores with wrapping;
neither the shape nor its assignment order is normalized.

`TextBox_SetPos` first updates the channel and primary coordinates. It
calls the outline helper only when the second object's type is four.
Other second-object types receive direct position stores. The third
object retains its independent type-four layout helper or bottom-right
positioning path. Thus `field_2C` is not globally an outline-only pointer.

## Regional behavior and entry drawing

| Region | Primary texture/CLUT pair | Outline flag | Entry stride |
|---|---|---|---|
| North America | `0x280`, `0xE8` | `0x20` | `0x1C` |
| Japan | `0x200`, `0xFF` | `0x20` | `0x18` |
| Europe | `0x280`, `0xC0` | `0x40` | `0x16` |

These object texture/CLUT fields are not text-box screen dimensions.
The existing regional wrapper constants and renderer profiles stay in
place. `TextBox_DrawEntries` selects the channel via object `field_67`
and traverses `entry_head_24` until the first inactive entry, producing
glyph/icon sprites, rotating textured quads or shaded polygon passes.
It draws entries, not the outline. Regional glyph layouts, modes, palettes
and packet scratchpad aliases remain independent. The detailed NA entry
interpretations and biased-cursor constraint remain in
[text-glyph-renderer.md](text-glyph-renderer.md).

## Native identities and acceptance

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Draw entries | `0x80035E20` | `0x80035B6C` | `0x800360AC` | `0x80036298` |
| Update outline | `0x80039140` | `0x800387F8` | `0x80038F48` | `0x80039148` |
| Refresh display objects | `0x800391E4` | `0x8003889C` | `0x80038FEC` | `0x800391EC` |

Japan's refresher was inventoried under its native `func_8003889C` name,
not the NA identity. The rename selects the exact native address; no
numeric-name inference or unrelated regional row is changed.

Definitions, prototypes, callers and callback installation use the same
three names. Existing source/header paths, include and declaration guards,
source grouping, compiler profiles, layouts, all other inventory columns
and initialized tables remain unchanged. Historical source/assembly paths
are retained. The complete accepted semantic ledger remains an unchanged
prefix, followed by three evidence/caveat rows.

Acceptance compares the complete renderer, control/layout, positioning and
builder compiler/MASPSX outputs after reversing only three function names.
No filename, instruction, directive or relocation filtering is needed.
The full clean retail executable match remains mandatory; no test,
fixture, diagnostic control, synthetic source or runtime capture changes.
Regional/cloud validation remains on the PR.

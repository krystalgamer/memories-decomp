# Scratchpad layout

The PlayStation data-cache scratchpad is 1 KB of fast RAM at
`0x1F800000..0x1F8003FF`. This note records how the game code divides it,
derived from every typed scratchpad reference in `src/game` and
`src/overlays`. Offsets below are relative to `0x1F800000`; widths are the
`sizeof` of the type stored there (Psy-Q: `SVECTOR` 8, `VECTOR` 16,
`MATRIX` 32, `CVECTOR` 4, `POLY_FT4` 40, `POLY_G4` 36, `POLY_GT4` 52,
`GsSPRITE` 36, `DIVPOLYGON4` 0x334).

Statements marked *inference* follow from the access pattern rather than
from any surviving allocation code or SDK documentation.

## Source convention

The convention is to reach the scratchpad through `SCRATCH_ADDR(off)` from
`src/game/scratchpad.h`, which expands to
`((void *)((u8 *)0x1F800000 + (off)))`. The use site supplies the real type
with a cast, for example `(SVECTOR *)SCRATCH_ADDR(0x300)`, so every
reference states both its offset and what lives there. New code should not
cast raw `0x1F80xxxx` integers; any such literal left in code is pending
conversion to this form. Comments may still quote absolute `0x1F80xxxx`
addresses. Psy-Q's `getScratchAddr` from
`src/psyq/libetc.h` is never used by game code.

## No allocator

There is no scratchpad allocator, arena or ownership record. Each routine
hard-codes absolute offsets, fills its records, hands them to the GTE or to
a Psy-Q packet builder, and is done with them before it returns. Two
conventions are visible instead:

- the low half is a private build area that any routine may overwrite;
- the top quarter has a fixed layout shared by contract between the
  display-object renderers and the code that feeds them.

## Low half, `0x000..0x1FF`: per-routine transient build area

Every user of this range treats offset 0 as the start of its own work
area. Nothing written here is read by a later call.

| Offset | Type | Users |
|---|---|---|
| `0x000` | `DIVPOLYGON4` | `DisplayObject_SubmitPacket` (DivideFT4 buffer) |
| `0x000` | `u32 *` work area | `Model_DrawSlot` (`GsSortUnit`), `Model_InitSlot` (`GsScanUnit`) |
| `0x000` | `POLY_GT4`; `0x038` `POLY_FT4`; `0x060` `SVECTOR`; `0x078` `MATRIX`; `0x0A0` `long`; `0x0C0`/`0x100`/`0x140` `DuelEffectTextSprite` | `TextBox_DrawEntries` |
| `0x000`, `0x040` | `u16` lookup tables | `DuelEffect_RasterizeGlyph` |
| `0x000` | `DuelCardRecord` copy | `DuelScene_UpdateCardMove` |
| `0x000` | `SpritePrim` (card art) | `Duel_DrawCardFrame` |
| `0x0C0`; `0x140`; `0x180` | colour word; `POLY_FT4`; `POLY_GT4` | `Duel_DrawFieldCards` |
| `0x000` | `GsGLINE` | `Library_DrawCardGrid` |
| `0x000`; `0x038..0x050`; `0x060`; `0x200` | `LINE_G3`; `SVECTOR` run; `long` flag pair; `SVECTOR` centre | `Library_DrawWireframeGlobe` |
| `0x000`; `0x010`; `0x020` | `s32` radius; `s32` scale; `POLY_G4` array | `DisplayObject_RenderSpotlightMask` |
| `0x000`; `0x020`, `0x060` | digit text bytes; two `GsSPRITE` | `BuildDeck_DrawCardListPage` |
| `0x000` | `CheckerboardSprite` | `Checkerboard_DrawBackground` |
| `0x000`; `0x040` | `LINE_F3`; `LINE_G2` | `NameEntry_DrawSelectionFrame` (password overlay) |

Offset 0 alone carries more than a dozen unrelated types, and `0x140` is
both a text-box sprite and a duel `POLY_FT4`.

## `0x280..0x2FF`: GTE and DivideFT4 inputs

| Offset | Type | Users |
|---|---|---|
| `0x280` | `u32` colour (passed as `CVECTOR *`) | `DisplayObject_SubmitPacket` |
| `0x290..0x29F` | four `u32` texture coordinates | `DisplayObject_SubmitPacket` |
| `0x2A0` | `LINE_F4` | `Library_DrawFadingCardOutline` |
| `0x2C0` | `MATRIX` | `Duel_RenderFieldCard3D` |
| `0x2D0` | `MATRIX` | `DisplayObject_ProjectRotationFacing`, `DisplayObject_ProjectPackedRotationFacing` |
| `0x2E0` | three `long` (otz, p, flag) | `DisplayObject_SubmitPacket` (`RotAverageNclip4`) |

`DisplayObject_SubmitPacket` places its `DIVPOLYGON4` at 0 and the
DivideFT4 colour and UV inputs at `0x280`. The Psy-Q declaration of
`DIVPOLYGON4` is 0x334 bytes, because its `cr[5]` recursion array has five
0x8C-byte levels after a 0x78-byte head. *Inference:* the layout only
works because DivideFT4 touches as many `cr[]` levels as the subdivision
count requires; with three levels the buffer ends at `0x21C`, below the
inputs, so the developers effectively reserved `0x000..0x27F` for the
subdivision buffer and `0x280..0x2FF` for its inputs and results.

## `0x300..0x3FF`: display-object fixed layout

This quarter behaves like an interface: the display-object renderers and
`DisplayObject_SubmitPacket` read fixed offsets, so every caller that
builds a primitive for them writes the same offsets.

| Offset | Type | Role and users |
|---|---|---|
| `0x300..0x31F` | four `SVECTOR` | Quad vertices v0..v3. `DisplayObject_SubmitPacket`; the rotation-facing projections fill `0x308..0x318`; `Duel_RenderFieldCard3D` and `Library_DrawFadingCardOutline` also build quads here. |
| `0x320` | `SpritePrim` / `GsSPRITE` / `DuelStatusDigitPacket` | Sprite primitive slot. `DisplayObject_RenderSpriteList`, `DisplayObject_RenderSpriteStrips`, `DisplayObject_RenderSpriteSheet`, `CardPreview_DrawDetailPanel`, `CardList_RenderDeckBoxStats`, `Library_DrawCardGrid`, `Duel_DrawCardFrame`, `Duel_DrawLifePointsAndDeckCounts`, `PalOptions_DrawWaveSprite`. |
| `0x344` | `POLY_FT4` / `POLY_G4` / `POLY_GT4` | Packet slot. All `DisplayObject_Render*` renderers, `CardPreview_DrawDetailPanel`, `Duel_DrawCardFrame`, `PalOptions_DrawWaveSprite`, `PalOptions_DrawScrollingStrips`. |
| `0x378` | `ClipState` / `SpriteSheetWork` | Clip and sprite-sheet work state. `DisplayObject_RenderSpriteList`, `DisplayObject_RenderSpriteStrips`, `DisplayObject_RenderSpriteSheet`. |
| `0x398` | `DisplayObjectPacketOrigin` / `struct ProjectionOut` | Projection result (screen origin plus subdivision count), which is also the tail of `ClipState`. Gouraud quad renderers, `CardPreview_DrawDetailPanel`, `Library_DrawFadingCardOutline`. |
| `0x3A4` | `SpriteSheetState` | `DisplayObject_RenderSpriteSheet`. |
| `0x3E0` | `SVECTOR` / `struct ProjectionOut` / `CardFrameScratch` | Ground-projection record. `DisplayObject_ProjectGroundPosition`, `Duel_InitGroundProjectionReference`, `Duel_ProjectCardSlotScreenX`, `Duel_ProjectCardSlotScreenPositions`, `Duel_DrawProjectedFieldCardFrame`, `Duel_DrawCardFrame`. |

The rest of the quarter is reused by two renderers that do not go through
the display-object path:

- `Model_DrawFogTexturedTriangles` and `Model_DrawFogTexturedQuads` build
  `LINE_G4`, `POLY_GT3` or `POLY_GT4` at `0x380`, keep `CVECTOR` colours at
  `0x3C0` and `0x3D0`, and GTE results at `0x3E0` and `0x3E8`;
- `Duel_RenderFieldCard3D` keeps a `VECTOR` scale at `0x320`, an up vector
  at `0x380`, a rotation at `0x3A0`, corner vectors at `0x3C0..0x3DF` and a
  depth word at `0x3E0`.

`Fade_DrawOverlay` builds its `FadeOverlayBox` (`GsBOXF`) at `0x3C0`.

## Shared offsets and why they are safe

Different subsystems store different types at the same offsets:
`0x320` (sprite primitive versus the field-card scale `VECTOR`), `0x380`
(fog packets versus a duel `SVECTOR`), `0x3C0` (fog colour, duel corner,
fade box), `0x3E0` (fog result, duel depth, ground-projection record,
card-frame record), and `0x2C0`/`0x2D0`, whose two `MATRIX` records
overlap at `0x2D0..0x2DF`. The whole low half is shared the same way.

This is safe because every use is confined to one call: a routine writes
the records, consumes them through the GTE or a packet builder, and keeps
any result it needs in ordinary RAM (for example the ground-projection
helpers store screen positions into `gDuel_aCardSlotScreenPositions` and
`gDuel_sGroundReferenceScreenX`). Rendering is single-threaded, and the
users of a shared offset never call one another while their records are
live. *Inference:* no interrupt handler uses the scratchpad, since no
callback or sound-driver source references it.

The display-object offsets are shared by contract rather than by
coincidence: callers fill `0x320` and `0x344` precisely because
`DisplayObject_SubmitPacket` and the renderers read them there.

## Extent

The highest offset referenced is `0x3E8` (an `s32` in
`Model_DrawFogTexturedQuads`), so the last byte game code is known to
touch is `0x3EB`; `CardFrameScratch` at `0x3E0` ends at the same byte.
*Inference:* `0x3EC..0x3FF` is unused by game code. The `0x200..0x27F` gap
holds only the wireframe globe's 8-byte `SVECTOR` centre at `0x200` and, at high
subdivision counts, by the DivideFT4 buffer.

## Related notes

- [Display-object runtime](display-object-runtime.md)
- [Ground-position projection](display-ground-projection.md)
- [Model fog driver state](model-fog-driver-state.md)
- [Fade transition state](fade-transition-state.md)

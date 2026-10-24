#include "../../types.h"
#include "display_quads.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "number_semantic_views.h"
#include "display_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_DrawQuadrantTileQuads(u8 *color, u16 size, s16 depth)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_1C;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_1E;
    setUV4(&packet->polygon, 128, 128, 191, 128, 128, 191, 191, 191);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    for (i = 0; i < 4; i++) {
        DuelEffect_InitQuadrantQuadVertices(packet->vertices, size, i);
        DuelEffect_ProjectSubmitFt4(&packet->polygon, packet->vertices, depth, 1);
    }
}

void DuelEffect_DrawTranslatedTileRect(u8 *color, SVECTOR *offset, u16 width, u16 height, u16 tile)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_20;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_22;
    setUV4(&packet->polygon, 128 + tile * 32, 192, 159 + tile * 32, 192,
           128 + tile * 32, 255, 159 + tile * 32, 255);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    setVector(&drawing.vertices[0], -width, -height, 0);
    setVector(&drawing.vertices[1], width, -height, 0);
    setVector(&drawing.vertices[2], -width, 0, 0);
    setVector(&drawing.vertices[3], width, 0, 0);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 1, 1);
}

void DuelEffect_DrawGradientConnectionBands(u8 *color, DVECTOR *first, DVECTOR *last, u16 width)
{
    POLY_G4 polygon;
    POLY_G4 *packet = &polygon;
    SVECTOR vertices[4];
    s32 p;
    s32 flag;
    s32 i;

    setPolyG4(packet);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, color[0], color[1], color[2]);
    setRGB2(packet, color[0], color[1], color[2]);
    setRGB3(packet, 0, 0, 0);
    for (i = 0; i < 2; i++) {
        setVector(&vertices[0], first->vx,
            first->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        setVector(&vertices[1], first->vx, first->vy, 0);
        setVector(&vertices[2], last->vx, last->vy, 0);
        setVector(&vertices[3], last->vx,
            last->vy + (s16)DuelEffect_PowS16(-1, i + 1) * width, 0);
        RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (flag >= 0) {
            DuelEffect_SubmitOffsetQuad(packet, 1);
        }
    }
}

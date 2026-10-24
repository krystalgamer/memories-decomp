#include "../../types.h"
#include "layered_drawing.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "layered_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_DrawLayeredTileQuad(u8 *color, u16 size, s16 depth, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page_at_00;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut_at_02;
    setUV4(&packet->polygon, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, depth, 1);
    setRGB0(&packet->polygon, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    DuelEffect_InitSquareVertices(drawing.vertices, size * 2);
    for (i = 0; i < 4; i++) {
        addVector(&drawing.vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, depth, 1);
}

void DuelEffect_DrawLayeredGradientTrapezoid(u8 *color, u16 *widths, u16 height, s16 depth)
{
    DuelEffectGradientQuad drawing;
    DuelEffectGradientQuad *record = &drawing;
    POLY_GT4 *packet = &record->polygon;
    s32 i;

    setPolyGT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_04;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_06;
    setUV4(packet, 128, 0, 159, 0, 128, 63, 159, 63);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    setRGB3(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    drawing.vertices[0].vx = -widths[0];
    drawing.vertices[0].vy = -height;
    drawing.vertices[0].vz = 0;
    drawing.vertices[1].vx = widths[0];
    drawing.vertices[1].vy = -height;
    drawing.vertices[1].vz = 0;
    drawing.vertices[2].vx = -widths[1];
    drawing.vertices[2].vy = 0;
    drawing.vertices[2].vz = 0;
    drawing.vertices[3].vx = widths[1];
    drawing.vertices[3].vy = 0;
    drawing.vertices[3].vz = 0;
    DuelEffect_ProjectSubmitGt4(packet, drawing.vertices, depth, 1);
    setRGB2(&drawing.polygon, color[0] >> 2, color[1] >> 2, color[2] >> 2);
    setRGB3(&drawing.polygon, color[0] >> 2, color[1] >> 2, color[2] >> 2);
    for (i = 0; i < 4; i++) {
        applyVector(&record->vertices[i], 2, 2, 0, *=);
    }
    DuelEffect_ProjectSubmitGt4(packet, drawing.vertices, depth, 1);
}

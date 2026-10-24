#include "../../types.h"
#include "textured_quads.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "texture_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_DrawFixedTextureQuad(u8 *color, u16 size, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page0;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut0;
    setUV4(&packet->polygon, 192, 128, 255, 128, 192, 191, 255, 191);
    setRGB0(&packet->polygon, color[0], color[1], color[2]);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 0, 1);
}

void DuelEffect_DrawAtlasColumnQuad(u16 size, u16 index, SVECTOR *offset)
{
    DuelEffectTexturedQuad drawing;
    DuelEffectTexturedQuad *packet = &drawing;
    s32 i;

    setPolyFT4(&packet->polygon);
    packet->polygon.tpage = gDuelEffect_TexturePageClutPairs.named.page1;
    packet->polygon.clut = gDuelEffect_TexturePageClutPairs.named.clut1;
    setUV4(&packet->polygon, index * 32, 0, index * 32 + 31, 0,
           index * 32, 31, index * 32 + 31, 31);
    setRGB0(&packet->polygon, 128, 128, 128);
    DuelEffect_InitSquareVertices(drawing.vertices, size);
    for (i = 0; i < 4; i++) {
        addVector(&packet->vertices[i], offset);
    }
    DuelEffect_ProjectSubmitFt4(&packet->polygon, drawing.vertices, 1, 1);
}

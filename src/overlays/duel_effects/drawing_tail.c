#include "../../types.h"
#include "drawing_tail.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "drawing_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_DrawFixedTileQuad(u8 *color)
{
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    SVECTOR vertices[4];

    setPolyFT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_08;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_0A;
    setUV4(packet, 0, 64, 63, 64, 0, 127, 63, 127);
    DuelEffect_InitSquareVertices(vertices, 64);
    setRGB0(packet, color[0] >> 1, color[1] >> 1, color[2] >> 1);
    DuelEffect_ProjectSubmitFt4(packet, vertices, 0, 1);
}

void DuelEffect_DrawRandomTileQuad(u8 *color, SVECTOR *vertices, s16 depth)
{
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    s32 u;
    s32 v;

    setPolyFT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_0C;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_0E;
    setRGB0(packet, color[0], color[1], color[2]);
    u = rand() % 2;
    v = rand() % 2;
    setUV4(packet, 160 + u * 32, v * 32, 191 + u * 32, v * 32,
           160 + u * 32, 31 + v * 32, 191 + u * 32, 31 + v * 32);
    DuelEffect_ProjectSubmitFt4(packet, vertices, depth, 1);
}

void DuelEffect_DrawTexturedRingStrip(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, s16 mode)
{
    POLY_GT4 polygon;
    POLY_GT4 *packet = &polygon;
    s32 p;
    s32 flag;
    s32 depth;
    s32 adjusted;
    s32 i;

    setPolyGT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_04;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_06;
    setUV4(packet, 128, 0, 159, 0, 128, 63, 159, 63);
    setRGB0(packet, 0, 0, 0);
    setRGB1(packet, 0, 0, 0);
    setRGB2(packet, color[0], color[1], color[2]);
    setRGB3(packet, color[0], color[1], color[2]);
    for (i = 0; i < 32; i++) {
        depth = RotAverage4(&first[i], &first[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (depth >= 0 && flag >= 0) {
            adjusted = depth + 1;
            if (mode == 0) {
                DuelEffect_SubmitOffsetGt4(packet, 1);
            } else {
                depth = adjusted - mode;
                if (depth < 0) {
                    depth = 0;
                }
                func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
        depth = RotAverage4(&last[i], &last[(i + 1) % 32],
            &middle[i], &middle[(i + 1) % 32],
            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
            (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
            (PSXLONG *)&p, (PSXLONG *)&flag);
        if (depth >= 0 && flag >= 0) {
            adjusted = depth + 1;
            if (mode == 0) {
                DuelEffect_SubmitOffsetGt4(packet, 1);
            } else {
                depth = adjusted - mode;
                if (depth < 0) {
                    depth = 0;
                }
                func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
    }
}

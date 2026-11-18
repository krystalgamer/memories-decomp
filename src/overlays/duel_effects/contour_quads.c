#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "effect_drawing.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "drawing_semantic_views.h"
#include "render_state_semantic_views.h"
#include "texture_state_semantic_views.h"

void DuelEffect_DrawQuadContour(CVECTOR *color, SVECTOR *inner, SVECTOR *outer, u16 bias, u16 flags)
{
    DisplayObjectG4Packet gradient;
    DisplayObjectG4Packet *edge = &gradient;
    POLY_FT4 polygon;
    POLY_FT4 *packet = &polygon;
    SVECTOR vertices[4];
    long p;
    long flag;
    s32 depth;
    s32 i;
    s32 j;

    setPolyG4(&edge->prim);
    setRGB0(&edge->prim, 0, 0, 0);
    setRGB1(&edge->prim, 0, 0, 0);
    setRGB2(&edge->prim, color->r >> 1, color->g >> 1, color->b >> 1);
    setRGB3(&gradient.prim, color->r >> 1, color->g >> 1, color->b >> 1);
    setSemiTrans(&edge->prim, 1);
    setPolyFT4(packet);
    packet->tpage = gDuelEffect_TexturePageClutPairs.named.page_at_00;
    packet->clut = gDuelEffect_TexturePageClutPairs.named.clut_at_02;
    setUV4(packet, 0, 0, 63, 0, 0, 63, 63, 63);
    setRGB0(packet, color->r, color->g, color->b);
    setSemiTrans(packet, 1);
    for (i = 0; i < 4; i++) {
        depth = RotAverage4(&outer[i], &outer[(i + 1) % 4],
            &inner[i], &inner[(i + 1) % 4],
            &edge->w.xy0, &edge->w.xy1,
            &edge->w.xy2, &edge->w.xy3,
            &p, &flag);
        if (flag >= 0) {
            depth++;
            if (bias == 0) {
                DuelEffect_SubmitOffsetQuad(&edge->prim, flags);
            } else {
                depth -= bias;
                Graphics_SubmitDrawModePacket(&edge->prim, gDuelEffect_pOrderingTable,
                    (u16)(depth >> 2), flags);
            }
        }
        if (bias == 0) {
            DuelEffect_InitSquareVertices(vertices, 8);
        } else {
            DuelEffect_InitSquareVertices(vertices, 16);
        }
        for (j = 0; j < 4; j++) {
            addVector(&vertices[j], &inner[i]);
        }
        DuelEffect_ProjectSubmitFt4(packet, vertices, (s16)bias, flags);
    }
}

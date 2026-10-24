#include "../../types.h"
#include "packet_helpers.h"
#include "packet_semantic_views.h"
#include "render_state_semantic_views.h"

void DuelEffect_SubmitOffsetQuad(void *primitive, u16 flags)
{
    POLY_FT4 *packet = primitive;

    packet->x0 += gDuelEffect_DrawOffset.vx;
    packet->x1 += gDuelEffect_DrawOffset.vx;
    packet->x2 += gDuelEffect_DrawOffset.vx;
    packet->x3 += gDuelEffect_DrawOffset.vx;
    packet->y0 += gDuelEffect_DrawOffset.vy;
    packet->y1 += gDuelEffect_DrawOffset.vy;
    packet->y2 += gDuelEffect_DrawOffset.vy;
    packet->y3 += gDuelEffect_DrawOffset.vy;
    func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(gDuelEffect_wBasePriority + 1), flags);
}

void DuelEffect_SubmitOffsetFt4(POLY_FT4 *packet, u16 mode)
{
    packet->x0 += gDuelEffect_DrawOffset.vx;
    packet->x1 += gDuelEffect_DrawOffset.vx;
    packet->x2 += gDuelEffect_DrawOffset.vx;
    packet->x3 += gDuelEffect_DrawOffset.vx;
    packet->y0 += gDuelEffect_DrawOffset.vy;
    packet->y1 += gDuelEffect_DrawOffset.vy;
    packet->y2 += gDuelEffect_DrawOffset.vy;
    packet->y3 += gDuelEffect_DrawOffset.vy;
    if (mode == 1) {
        func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
    } else {
        setSemiTrans(packet, 0);
        GsSortPoly(packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority);
    }
}

void DuelEffect_SubmitOffsetGt4(POLY_GT4 *packet, u16 mode)
{
    packet->x0 += gDuelEffect_DrawOffset.vx;
    packet->x1 += gDuelEffect_DrawOffset.vx;
    packet->x2 += gDuelEffect_DrawOffset.vx;
    packet->x3 += gDuelEffect_DrawOffset.vx;
    packet->y0 += gDuelEffect_DrawOffset.vy;
    packet->y1 += gDuelEffect_DrawOffset.vy;
    packet->y2 += gDuelEffect_DrawOffset.vy;
    packet->y3 += gDuelEffect_DrawOffset.vy;
    if (mode == 1) {
        func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, gDuelEffect_wBasePriority, 1);
    } else {
        setSemiTrans(packet, 0);
        GsSortPoly(packet, gDuelEffect_pOrderingTable, (u16)(gDuelEffect_wBasePriority + 1));
    }
}

void DuelEffect_InitBaseMatrix(MATRIX *matrix)
{
    matrix->m[0][0] = 4096;
    matrix->m[1][1] = 4096;
    matrix->m[2][2] = 4096;
    matrix->m[0][1] = matrix->m[0][2] = 0;
    matrix->m[1][0] = matrix->m[1][2] = 0;
    matrix->m[2][0] = matrix->m[2][1] = 0;
    matrix->t[0] = 0;
    matrix->t[1] = 0;
    matrix->t[2] = 300;
}

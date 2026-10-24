#include "../../types.h"
#include "packet_helpers.h"
#include "packet_semantic_views.h"

void DuelEffect_SubmitOffsetQuad(void *primitive, u16 flags)
{
    POLY_FT4 *packet = primitive;

    packet->x0 += D_8015B7F8.vx;
    packet->x1 += D_8015B7F8.vx;
    packet->x2 += D_8015B7F8.vx;
    packet->x3 += D_8015B7F8.vx;
    packet->y0 += D_8015B7F8.vy;
    packet->y1 += D_8015B7F8.vy;
    packet->y2 += D_8015B7F8.vy;
    packet->y3 += D_8015B7F8.vy;
    func_8005B260((u32 *)packet, D_8015B7F4, (u16)(D_8015B800 + 1), flags);
}

void DuelEffect_SubmitOffsetFt4(POLY_FT4 *packet, u16 mode)
{
    packet->x0 += D_8015B7F8.vx;
    packet->x1 += D_8015B7F8.vx;
    packet->x2 += D_8015B7F8.vx;
    packet->x3 += D_8015B7F8.vx;
    packet->y0 += D_8015B7F8.vy;
    packet->y1 += D_8015B7F8.vy;
    packet->y2 += D_8015B7F8.vy;
    packet->y3 += D_8015B7F8.vy;
    if (mode == 1) {
        func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);
    } else {
        setSemiTrans(packet, 0);
        GsSortPoly(packet, D_8015B7F4, D_8015B800);
    }
}

void DuelEffect_SubmitOffsetGt4(POLY_GT4 *packet, u16 mode)
{
    packet->x0 += D_8015B7F8.vx;
    packet->x1 += D_8015B7F8.vx;
    packet->x2 += D_8015B7F8.vx;
    packet->x3 += D_8015B7F8.vx;
    packet->y0 += D_8015B7F8.vy;
    packet->y1 += D_8015B7F8.vy;
    packet->y2 += D_8015B7F8.vy;
    packet->y3 += D_8015B7F8.vy;
    if (mode == 1) {
        func_8005B260((u32 *)packet, D_8015B7F4, D_8015B800, 1);
    } else {
        setSemiTrans(packet, 0);
        GsSortPoly(packet, D_8015B7F4, (u16)(D_8015B800 + 1));
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

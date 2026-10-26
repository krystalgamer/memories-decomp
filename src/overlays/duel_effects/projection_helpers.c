#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "packet_helpers.h"
#include "textured_quads.h"
#include "packet_semantic_views.h"
#include "render_state_semantic_views.h"
#include "utility_helpers.h"
#include "geometry_semantic_views.h"

void DuelEffect_ProjectSubmitFt4(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    s32 p;
    s32 flag;
    s32 depth;

    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
                        (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (bias == 0) {
            DuelEffect_SubmitOffsetFt4(packet, mode);
        } else {
            depth = depth - (bias - 1);
            if (depth < 0) {
                depth = 0;
            }
            if (mode == 0) {
                packet->code &= ~2;
                GsSortPoly(packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2));
            } else {
                func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
    }
}

void DuelEffect_ProjectSubmitGt4(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    s32 p;
    s32 flag;
    s32 depth;

    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                        (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3,
                        (PSXLONG *)&p, (PSXLONG *)&flag);
    if (flag >= 0) {
        if (bias == 0) {
            DuelEffect_SubmitOffsetGt4(packet, mode);
        } else {
            depth = depth - (bias - 1);
            if (depth < 0) {
                depth = 0;
            }
            func_8005B260((u32 *)packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
        }
    }
}

void DuelEffect_SetTransformMatrix(MATRIX *world, MATRIX *saved, SVECTOR *position,
                    SVECTOR *rotation, VECTOR *scale, u16 mode)
{
    MATRIX matrix;
    s32 flag;

    GsSetLsMatrix(world);
    RotTrans(position, (VECTOR *)&matrix.t[0], (PSXLONG *)&flag);
    RotMatrix(rotation, &matrix);
    if (mode == 2 || mode == 3) {
        MulMatrix2(world, &matrix);
    }
    if (mode == 1 || mode == 3) {
        DuelEffect_CopyMatrixComponents(&matrix, saved);
    }
    ScaleMatrix(&matrix, scale);
    GsSetLsMatrix(&matrix);
}

void DuelEffect_SetScaledLocalScreenMatrix(MATRIX *source, VECTOR *scale)
{
    MATRIX matrix;

    DuelEffect_CopyMatrixComponents(source, &matrix);
    ScaleMatrix(&matrix, scale);
    GsSetLsMatrix(&matrix);
}

void DuelEffect_CopyMatrixComponents(MATRIX *source, MATRIX *destination)
{
    s32 i;
    s32 j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            destination->m[i][j] = source->m[i][j];
        }
        destination->t[i] = source->t[i];
    }
}

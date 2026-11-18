#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "packet_helpers.h"
#include "textured_quads.h"
#include "packet_semantic_views.h"
#include "render_state_semantic_views.h"
#include "effect_geometry.h"
#include "geometry_semantic_views.h"

void DuelEffect_ProjectSubmitFt4(POLY_FT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    long p;
    long flag;
    s32 depth;

    /* The GTE stores each x/y pair as one long; callers pass the bare primitive. */
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (long *)&packet->x0, (long *)&packet->x1,
                        (long *)&packet->x2, (long *)&packet->x3,
                        &p, &flag);
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
                Graphics_SubmitDrawModePacket(packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
            }
        }
    }
}

void DuelEffect_ProjectSubmitGt4(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode)
{
    long p;
    long flag;
    s32 depth;

    /* The GTE stores each x/y pair as one long; callers pass the bare primitive. */
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (long *)&packet->x0, (long *)&packet->x1,
                        (long *)&packet->x2, (long *)&packet->x3,
                        &p, &flag);
    if (flag >= 0) {
        if (bias == 0) {
            DuelEffect_SubmitOffsetGt4(packet, mode);
        } else {
            depth = depth - (bias - 1);
            if (depth < 0) {
                depth = 0;
            }
            Graphics_SubmitDrawModePacket(packet, gDuelEffect_pOrderingTable, (u16)(depth >> 2), mode);
        }
    }
}

void DuelEffect_SetTransformMatrix(MATRIX *world, MATRIX *saved, SVECTOR *position,
                    SVECTOR *rotation, VECTOR *scale, u16 mode)
{
    MATRIX matrix;
    long flag;

    GsSetLsMatrix(world);
    /* RotTrans writes a VECTOR; MATRIX.t is its first three words. */
    RotTrans(position, (VECTOR *)&matrix.t[0], &flag);
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

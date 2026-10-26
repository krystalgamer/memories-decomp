#include "../../types.h"
#include "utility_helpers.h"
#include "geometry_semantic_views.h"

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

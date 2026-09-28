#include "../../types.h"
#include "utility_helpers.h"

void func_801514BC(MATRIX *source, VECTOR *scale)
{
    MATRIX matrix;

    func_801514F8(source, &matrix);
    ScaleMatrix(&matrix, scale);
    GsSetLsMatrix(&matrix);
}

void func_801514F8(MATRIX *source, MATRIX *destination)
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

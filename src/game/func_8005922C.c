#include "../types.h"
#include "../psyq/libgte.h"

void func_8005922C(u8 *object, void *source)
{
    RotMatrixYXZ_gte((SVECTOR *)(object + 0x44), (MATRIX *)(object + 4));
    if (source != 0) ScaleMatrix((MATRIX *)(object + 4), source);
    *(int *)object = 0;
}

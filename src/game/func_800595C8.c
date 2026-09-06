#include "../types.h"
#include "model.h"

void func_800595C8(s32 index, s32 x, s32 y, s32 z)
{
    ModelSlot *record = &D_800F2C40[index];

    x = x < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (x > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : x);
    record->field_DA0[0] = x;
    y = y < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (y > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : y);
    record->field_DA0[1] = y;
    z = z < MODEL_FIXED_NEGATIVE_ONE
            ? MODEL_FIXED_NEGATIVE_ONE
            : (z > MODEL_FIXED_THREE ? MODEL_FIXED_THREE : z);
    record->field_DA0[2] = z;
    if (record->field_E11 != 4) {
        if (x == MODEL_FIXED_HALF && y == x && z == y)
            record->field_E11 = 0;
        else
            record->field_E11 = 3;
    }
}

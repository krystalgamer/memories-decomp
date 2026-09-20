#include "../types.h"

#include "model.h"
#include "model_slot_queries.h"

s32 Model_GetSlotAnimationSpeed(s32 index)
{
    return D_800F2C40[index].field_E0D;
}

s32 Model_GetSlotAnimationIndex(s32 index)
{
    return D_800F2C40[index].field_BF5;
}

s32 Model_GetSlotAnimationFrame(s32 index)
{
    return D_800F2C40[index].field_E06 >> 4;
}

u16 func_80058EC0(s32 idx)
{
    u8 sub = D_800F2C40[idx].field_BF5;
    return D_800F2C40[idx].field_750[sub].max;
}

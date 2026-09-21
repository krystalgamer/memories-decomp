#include "../types.h"

#include "model.h"
#include "model_slot_queries.h"

#if !defined(VERSION_JAPAN) || \
    defined(VERSION_JAPAN_MODEL_GET_SLOT_ANIMATION_SPEED)
s32 Model_GetSlotAnimationSpeed(s32 index)
{
#ifdef VERSION_JAPAN
    return D_800F2C40[index].field_BF5;
#else
    return D_800F2C40[index].field_E0D;
#endif
}
#endif

#ifndef VERSION_JAPAN
s32 Model_GetSlotAnimationIndex(s32 index)
{
    return D_800F2C40[index].field_BF5;
}

s32 Model_GetSlotAnimationFrame(s32 index)
{
    return D_800F2C40[index].field_E06 >> 4;
}

u16 Model_GetSlotAnimationLength(s32 idx)
{
    u8 sub = D_800F2C40[idx].field_BF5;
    return D_800F2C40[idx].field_750[sub].max;
}
#endif

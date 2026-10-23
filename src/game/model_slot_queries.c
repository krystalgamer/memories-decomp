#include "../types.h"

#include "model.h"
#include "model_slot_queries.h"

s32 Model_GetSlotAnimationSpeed(s32 index)
{
    return gModel_aSlots[index].field_E0D;
}

s32 Model_GetSlotAnimationIndex(s32 index)
{
    return gModel_aSlots[index].field_BF5;
}

s32 Model_GetSlotAnimationFrame(s32 index)
{
    return gModel_aSlots[index].animation_position >> 4;
}

u16 Model_GetSlotAnimationLength(s32 idx)
{
    u8 sub = gModel_aSlots[idx].field_BF5;
    return gModel_aSlots[idx].animation_duration_rows[sub].max;
}

#include "../types.h"

#include "model.h"

void *func_80058F20(s32 index, s32 slot)
{
    ModelSlot *entry = &D_800F2C40[index];

    if (slot > entry->field_E17) {
        slot = entry->field_E18;
    }
    return entry->field_D14 + slot * 80;
}

u32 func_80058F74(s32 index)
{
    ModelSlot *entry = &D_800F2C40[index];

    return (u32)entry->field_D14 + entry->field_E18 * 80;
}

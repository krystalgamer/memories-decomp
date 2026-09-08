#include "../types.h"

#include "model.h"

void *func_80058F20(s32 index, s32 slot)
{
    ModelSlot *entry = &D_800F2C40[index];

    if (slot > entry->entry_count) {
        slot = entry->field_E18;
    }
    return entry->entries + slot * MODEL_SLOT_DATA_ENTRY_SIZE;
}

u32 func_80058F74(s32 index)
{
    ModelSlot *entry = &D_800F2C40[index];

    return (u32)entry->entries +
           entry->field_E18 * MODEL_SLOT_DATA_ENTRY_SIZE;
}

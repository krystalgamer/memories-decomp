#include "../types.h"
#include "model.h"
#include "func_80058DD8.h"

int func_80058DD8(int index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    if (entry->field_E14 != 0xFF) return 2;
    return entry->field_E1F != 0;
}

#include "../types.h"
#include "model.h"
#include "model_get_slot_readiness.h"

int Model_GetSlotReadiness(int index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    if (entry->field_E14 != 0xFF) return 2;
    return entry->field_E1F != 0;
}

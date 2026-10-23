#include "../types.h"
#include "model.h"
#include "model_get_slot_readiness.h"

int Model_GetSlotReadiness(int index)
{
    ModelSlot *entry = &gModel_aSlots[index];
    if (entry->load_step != 0xFF) return 2;
    return entry->is_active != 0;
}

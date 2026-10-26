#include "../types.h"
#include "model.h"
#include "model_set_slot_shadow_enabled.h"

int Model_SetSlotShadowEnabled(int index, int value)
{
    register ModelSlot *entry;
    register int old;

    {
        register ModelSlot *base = gModel_aSlots;

        entry = &((ModelSlot *)0)[index];

        entry = (ModelSlot *)((unsigned int)entry + (unsigned int)base);
    }

    old = entry->shadow_enabled;
    if (value >= 0) {
        entry->shadow_enabled = value;
    }
    return old;
}

#include "../types.h"
#include "func_8004DC38.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_slot_updates.h"

void func_8005969C(s32 index, s32 type)
{
    ModelSlot *slot = &D_800F2C40[index];

    if ((u32)(type - 4) >= 29) {
        type = 8;
    }
    slot->field_E0D = type;
    if (slot->field_E16 == 0x3E) {
        func_80059700(index, 1);
    }
}


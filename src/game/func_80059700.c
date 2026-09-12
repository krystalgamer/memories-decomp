#include "../types.h"
#include "../game/func_8004DC38.h"
#include "../game/model.h"
#include "../game/model_slot_state_updates.h"
#include "../game/model_slot_updates.h"

void func_80059700(s32 index, s32 sign)
{
    ModelSlot *slot;
    s32 state;
    /* The incoming value keeps a1 live before it becomes the output velocity. */
    s32 velocity = sign;
    s32 doubled;

    {
        ModelSlot *base = D_800F2C40;

        slot = base + index;
    }

    if (sign > 0)
        goto state_62;
    if (sign >= 0)
        goto state_35;
    slot->field_E16 = 60;
    goto update;
state_35:
    slot->field_E16 = 35;
    goto update;
state_62:
    slot->field_E16 = 62;

update:
    {
        ModelSlot *current = &D_800F2C40[index];

        doubled = current->field_E0D * 2;
        state = current->field_E16;
    }
    if (state == 60)
        goto negate;
    if (state < 61) {
        velocity = 0;
        if (state == 35)
            goto apply;
        return;
    } else {
        velocity = doubled;
        if (state == 62)
            goto apply;
        return;
    }

negate:
    velocity = -doubled;
apply:
    func_8005A468(index, velocity);
}


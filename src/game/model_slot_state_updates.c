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

void func_80059700(s32 index, s32 sign)
{
    ModelSlot *slot;
    s32 doubled_copy;
    s32 state;
    /* The incoming value keeps a1 live before it becomes the output velocity. */
    register s32 velocity asm("$5") = sign;
    register s32 doubled asm("$6");

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
    doubled_copy = doubled;
    velocity = doubled;
    if (state == 60)
        goto negate;
    if (state < 61) {
        velocity = 0;
        if (state == 35)
            goto apply;
        return;
    } else {
        velocity = doubled_copy;
        if (state == 62)
            goto apply;
        return;
    }

negate:
    velocity = -velocity;
apply:
    func_8005A468(index, velocity);
}

void func_800597C8(s32 idx, s32 flag, s32 val)
{
    s32 i = 0;
    ModelSlot *rec = &D_800F2C40[idx];
    s32 count;
    s32 arg3;

    count = rec->field_E1B;
    rec->field_E06 = val << 4;
    arg3 = rec->field_E06;

    if (count != 0) {
        do {
            s32 arg2 = rec->field_BF5;

            if (flag != 0) {
                ModelSlotPart *part = rec->field_1E0[i];
                arg2 = flag;
                part->sid = flag;
            }

            func_8004DC38(rec, i, arg2, arg3);
            count = rec->field_E1B;
            i++;
        } while (i < count);
    }

    if (flag != 0) {
        rec->field_BF5 = flag;
    }
    rec->field_DC8[3] = 0;
}

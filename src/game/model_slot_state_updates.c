#include "../types.h"
#include "model.h"

extern void func_8005A468(s32, s32);
extern s32 func_8004DC38(void *, s32, s32, s32);

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
                u8 *entry = rec->field_1E0[i];
                arg2 = flag;
                entry[0xC] = flag;
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

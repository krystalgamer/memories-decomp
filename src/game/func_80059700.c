#include "../types.h"

typedef struct {
    u8 pad_000[0xE0D];
    u8 type;
    u8 pad_E0E[0xE16 - 0xE0E];
    u8 state;
    u8 pad_E17[0xE20 - 0xE17];
} ModelSlot;

extern ModelSlot D_800F2C40[];
extern void func_8005A468(s32, s32);

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
    slot->state = 60;
    goto update;
state_35:
    slot->state = 35;
    goto update;
state_62:
    slot->state = 62;

update:
    {
        ModelSlot *current = &D_800F2C40[index];

        doubled = current->type * 2;
        state = current->state;
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

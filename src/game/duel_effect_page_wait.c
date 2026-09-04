#include "../types.h"
#include "duel_effect.h"

extern void func_800373C8(u8 *object, s32 field_15, s32 field_13);

void func_8003741C(u8 *object)
{
    u8 state = object[0x51];
    DuelEffectEntry *entry;

    if ((state & 0x80) == 0) {
        object[0x51] = state | 0x80;
        func_800373C8(object, 2, 0);
        return;
    }

    entry = &D_800EB288[*(u16 *)(object + 0x5C)];
    if (entry->flags_11 & 0x80) {
        return;
    }

    object[0x56] = 0;
    *(u16 *)(object + 0x38) = 0;
    *(u16 *)(object + 0x3A) = 0;
    object[0x51] = 0;
    object[0x62] = 0;
}

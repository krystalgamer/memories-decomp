#include "../types.h"
#include "duel_effect.h"

struct Obj {
    u8 pad[92];
    u16 field92;
    u16 field94;
};

/* Starting from record a0->field92, scans up to a0->field94 DuelEffectEntry
   entries; returns 1 on the first entry with field17&0x80 set and field19
   nonzero, 0 if field17's bit clears, the count runs out, or field94==0. */
int DuelEffect_HasActiveEntry(struct Obj *a0) {
    int v0;
    int count;
    u8 *v1;
    v0 = a0->field92;
    count = a0->field94;
    v1 = (u8 *)D_800EB288 + v0 * sizeof(DuelEffectEntry);
    if (count == 0) {
        goto ret_zero_a;
    }
    v1 = v1 + 19;
loop:
    v0 = v1[-2] & 0x80;
    if (v0 == 0) {
        return v0;
    }
    if (v1[0] != 0) {
        return 1;
    }
    count = count - 1;
    v1 = v1 + sizeof(DuelEffectEntry);
    if (count != 0) {
        goto loop;
    }
ret_zero_a:
    return 0;
}

/* Starting from record a0->field92, walks up to a0->field94 DuelEffectEntry
   entries; for each with field17&0x80 set, writes a1 to field19 and a2 to
   field21, stopping at the first entry with field17's bit clear or when the
   count runs out. */
void func_800373C8(struct Obj *a0, u8 a1, u8 a2) {
    int v0;
    int count;
    u8 *v1;

    v0 = a0->field92;
    count = a0->field94;
    v1 = (u8 *)D_800EB288 + v0 * sizeof(DuelEffectEntry);
    if (count == 0) {
        return;
    }
    v1 = v1 + 21;
loop:
    if ((v1[-4] & 0x80) == 0) {
        return;
    }
    count = count - 1;
    v1[-2] = a1;
    v1[0] = a2;
    v1 = v1 + sizeof(DuelEffectEntry);
    if (count != 0) {
        goto loop;
    }
}

void func_8003741C(u8 *object)
{
    u8 state = object[0x51];
    DuelEffectEntry *entry;

    if ((state & 0x80) == 0) {
        object[0x51] = state | 0x80;
        func_800373C8((struct Obj *)object, 2, 0);
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

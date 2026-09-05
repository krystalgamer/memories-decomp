#include "../types.h"

typedef struct {
    s32 field_00;
    s32 field_04;
    s32 field_08;
    s32 field_0C;
    u8 field_10;
    u8 flags_11;
    u8 field_12;
    u8 field_13;
    u8 field_14;
    u8 field_15;
    u8 field_16[2];
    u8 field_18;
    u8 field_19[3];
} DuelEffectCopyEntry;

typedef void (*DuelEffectEntryHandler)(DuelEffectCopyEntry *, u8 *);

extern DuelEffectCopyEntry D_800EB288[];
extern DuelEffectEntryHandler D_80090F58[];
extern s32 D_8009B330;

void DuelEffect_ProcessEntries(u8 *arg0)
{
    DuelEffectCopyEntry *p;
    DuelEffectCopyEntry *q;
    DuelEffectCopyEntry *s;
    DuelEffectCopyEntry *e;

    p = *(DuelEffectCopyEntry **)(arg0 + 0x24);
    D_8009B330 = 0;

    while (p->flags_11 & 0x80) {
        if (p->field_13 != 0) {
            D_80090F58[p->field_13 & 0x1F](p, arg0);
        }
        p++;
    }

    if (D_8009B330 != 0) {
        q = &D_800EB288[*(u16 *)(arg0 + 0x5C)];
        e = *(DuelEffectCopyEntry **)(arg0 + 0x20);
        s = q;
        while (s != e) {
            if (s->flags_11 & 0x80) {
                *q = *s;
                q++;
            }
            s++;
        }
        *(DuelEffectCopyEntry **)(arg0 + 0x20) = q;
        q->flags_11 = 0;
    }
}

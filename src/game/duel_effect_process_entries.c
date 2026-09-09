#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_process_entries.h"

extern void (*D_80090F58[])(DuelEffectEntry *, DuelEffectChannel *);

void DuelEffect_ProcessEntries(DuelEffectChannel *arg0)
{
    DuelEffectEntry *p;
    DuelEffectEntry *q;
    DuelEffectEntry *s;
    DuelEffectEntry *e;

    p = arg0->entry_head_24;
    D_8009B330 = 0;

    while (p->flags_11 & 0x80) {
        if (p->field_13 != 0) {
            D_80090F58[p->field_13 & 0x1F](p, arg0);
        }
        p++;
    }

    if (D_8009B330 != 0) {
        q = &D_800EB288[arg0->range_start_5C];
        e = arg0->entry_end_20;
        s = q;
        while (s != e) {
            if (s->flags_11 & 0x80) {
                *q = *s;
                q++;
            }
            s++;
        }
        arg0->entry_end_20 = q;
        q->flags_11 = 0;
    }
}

#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_process_entries.h"

#ifndef DUEL_EFFECT_PROCESS_ENTRY_TYPE
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef DUEL_EFFECT_PROCESS_RANGE_START
#define DUEL_EFFECT_PROCESS_RANGE_START(object) ((object)->range_start_5C)
#endif

void DuelEffect_ProcessEntries(DuelEffectChannel *arg0)
{
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *p;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *q;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *s;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *e;

    p = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)arg0->entry_head_24;
    gDuelEffect_nCompactionRequested = 0;

    while (p->flags_11 & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        if (p->handler_state != 0) {
            CALL32(void (*)(DuelEffectEntry *, DuelEffectChannel *),
                   gDuelEffect_EntryHandlers[p->handler_state & DUEL_EFFECT_ENTRY_HANDLER_INDEX_MASK])(
                (DuelEffectEntry *)p, arg0);
        }
        p++;
    }

    if (gDuelEffect_nCompactionRequested != 0) {
        q = &((DUEL_EFFECT_PROCESS_ENTRY_TYPE *)gDuelEffect_aEntries)[
            DUEL_EFFECT_PROCESS_RANGE_START(arg0)];
        e = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)arg0->entry_end_20;
        s = q;
        while (s != e) {
            if (s->flags_11 & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
                *q = *s;
                q++;
            }
            s++;
        }
        arg0->entry_end_20 = (DuelEffectEntry *)q;
        q->flags_11 = 0;
    }
}

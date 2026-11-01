#include "../types.h"
#include "duel_effect.h"

/* Zero-initialized storage this unit owns at 0x800EB288-0x800EF668, defined in
 * address order ahead of the headers that declare it (#2602). */
DuelEffectEntry tent_DuelEffectEntries[620];
u16 gMain_aAnimatedBattleModels[8];

#include "duel_effect_entry_state.h"
#include "duel_effect_process_entries.h"

#ifndef DUEL_EFFECT_PROCESS_ENTRY_TYPE
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE DuelEffectEntry
#endif
#ifndef DUEL_EFFECT_PROCESS_RANGE_START
#define DUEL_EFFECT_PROCESS_RANGE_START(object) ((object)->entry_range_start)
#endif

void DuelEffect_ProcessEntries(DuelEffectChannel *arg0)
{
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *p;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *q;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *s;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *e;

    p = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)arg0->entry_begin;
    gDuelEffect_nCompactionRequested = 0;

    while (p->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
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
        e = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)arg0->entry_write_cursor;
        s = q;
        while (s != e) {
            if (s->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
                *q = *s;
                q++;
            }
            s++;
        }
        arg0->entry_write_cursor = (DuelEffectEntry *)q;
        q->entry_flags = 0;
    }
}

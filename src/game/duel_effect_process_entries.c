#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "main_modes.h"

/* Zero-initialized storage this unit owns at 0x800EB288-0x800EF668 (#2602).
 * A regional wrapper selects its own entry record and count through
 * DUEL_EFFECT_ENTRY_STORAGE_TYPE/COUNT; the European build keeps the battle
 * models in text_box_runtime.c instead. */
DUEL_EFFECT_ENTRY_STORAGE_TYPE gDuelEffect_aEntries[DUEL_EFFECT_ENTRY_STORAGE_COUNT];
#ifndef DUEL_EFFECT_PROCESS_ENTRIES_NO_BATTLE_MODELS
AnimatedBattleModelProperties gMain_aAnimatedBattleModels[2];
#endif
#include "duel_effect_process_entries.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
s32 gDuelEffect_nCompactionRequested;

#ifndef DUEL_EFFECT_PROCESS_ENTRY_TYPE
#define DUEL_EFFECT_PROCESS_ENTRY_TYPE DuelEffectEntry
#endif

void DuelEffect_ProcessEntries(DuelEffectChannel *channel)
{
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *p;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *q;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *s;
    DUEL_EFFECT_PROCESS_ENTRY_TYPE *e;

    p = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_begin;
    gDuelEffect_nCompactionRequested = 0;

    while (p->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
        if (p->handler_state != 0) {
            gDuelEffect_EntryHandlers[p->handler_state & DUEL_EFFECT_ENTRY_HANDLER_INDEX_MASK](
                (DuelEffectEntry *)p, channel);
        }
        p++;
    }

    if (gDuelEffect_nCompactionRequested != 0) {
        q = &((DUEL_EFFECT_PROCESS_ENTRY_TYPE *)gDuelEffect_aEntries)[
            channel->entry_range_start];
        e = (DUEL_EFFECT_PROCESS_ENTRY_TYPE *)channel->entry_write_cursor;
        s = q;
        while (s != e) {
            if (s->entry_flags & DUEL_EFFECT_ENTRY_FLAG_ACTIVE) {
                *q = *s;
                q++;
            }
            s++;
        }
        channel->entry_write_cursor = (DuelEffectEntry *)q;
        q->entry_flags = 0;
    }
}

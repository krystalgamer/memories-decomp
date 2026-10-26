#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_set_entry_range_layout.h"

void DuelEffect_SetEntryRangeLayout(s32 arg0, s32 arg1)
{
    u16 *t = gDuelEffect_awEntryRangeBoundaries;
    s32 next = arg0 + 1;
    u16 *lo = &t[arg0];
    u16 *hi = &t[next];
    s32 i = *lo;

    if (i < *hi) {
        DuelEffectEntry *base = gDuelEffect_aEntries;
        DuelEffectEntry *p = &base[i];
        do {
            p->layout_selector = arg1;
            p++;
            i++;
        } while (i < *hi);
    }
}

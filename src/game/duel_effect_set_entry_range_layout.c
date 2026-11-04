#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_state.h"
#include "duel_effect_entry_ranges.h"
#include "duel_effect_set_entry_range_layout.h"

void DuelEffect_SetEntryRangeLayout(s32 range, s32 layout)
{
    u16 *t = gDuelEffect_awEntryRangeBoundaries;
    s32 next = range + 1;
    u16 *lo = &t[range];
    u16 *hi = &t[next];
    s32 i = *lo;

    if (i < *hi) {
        DuelEffectEntry *base = gDuelEffect_aEntries;
        DuelEffectEntry *p = &base[i];
        do {
            p->layout_selector = layout;
            p++;
            i++;
        } while (i < *hi);
    }
}

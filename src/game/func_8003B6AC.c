#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_ranges.h"
#include "func_8003B6AC.h"

void DuelEffect_SetEntryRangeLayout(s32 arg0, s32 arg1)
{
    u16 *t = gDuelEffect_awEntryRangeBoundaries;
    s32 next = arg0 + 1;
    u16 *lo = &t[arg0];
    u16 *hi = &t[next];
    s32 i = *lo;

    if (i < *hi) {
        DuelEffectEntry *base = D_800EB288;
        DuelEffectEntry *p = &base[i];
        do {
            p->field_18 = arg1;
            p++;
            i++;
        } while (i < *hi);
    }
}

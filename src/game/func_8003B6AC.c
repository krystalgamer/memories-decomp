#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_entry_ranges.h"
#include "func_8003B6AC.h"

void func_8003B6AC(s32 arg0, s32 arg1)
{
    u8 *t = (u8 *)gDuelEffect_awEntryRangeBoundaries;
    u8 *lo = t + arg0 * 2;
    u8 *hi = t + (arg0 + 1) * 2;
    s32 i = *(u16 *)lo;

    if (i < *(u16 *)hi) {
        DuelEffectEntry *base = D_800EB288;
        DuelEffectEntry *p = &base[i];
        do {
            p->field_18 = arg1;
            p++;
            i++;
        } while (i < *(u16 *)hi);
    }
}

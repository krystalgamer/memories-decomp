#include "../types.h"
#include "duel_card.h"

extern s32 Duel_CalcCardStats();
extern s32 Duel_CalcBattleDefense();
extern s32 Duel_CalcBattleAttack();

s32 func_8001EFD4(u8 *left, u8 *right)
{
    DuelCardRecord *a = &D_801A7AD8[left[106]];
    DuelCardRecord *b;
    s32 packed;
    s32 wanted;
    s32 actual;

    if (right == (u8 *)0)
        return Duel_CalcCardStats(a) & 65535;
    b = &D_801A7AD8[right[106]];
    packed = Duel_CalcCardStats(b);
    wanted = packed & 65535;
    if (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        wanted = (u32)packed >> 16;
    if (a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION)
        actual = Duel_CalcBattleDefense(a, b);
    else
        actual = Duel_CalcBattleAttack(a, b);
    if (actual != wanted)
        return actual - wanted;
    if ((a->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) ||
        (b->flags & DUEL_CARD_FLAG_DEFENSE_POSITION))
        return 0;
    return -1;
}

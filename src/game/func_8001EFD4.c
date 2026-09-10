#include "../types.h"
#include "duel_card.h"
#include "display_object.h"
#include "func_8001EFD4.h"

s32 func_8001EFD4(DisplayObject *left, DisplayObject *right)
{
    DuelCardRecord *a = &D_801A7AD8[left->field_6A];
    DuelCardRecord *b;
    s32 packed;
    s32 wanted;
    s32 actual;

    if (right == 0)
        return Duel_CalcCardStats(a) & 65535;
    b = &D_801A7AD8[right->field_6A];
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

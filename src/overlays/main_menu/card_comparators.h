#ifndef MEMORIES_DECOMP_MAIN_MENU_CARD_COMPARATORS_H
#define MEMORIES_DECOMP_MAIN_MENU_CARD_COMPARATORS_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "card_tables.h"

s32 MainMenu_CompareCardsByName(s16 *a, s16 *b);
s32 MainMenu_CompareCardsByMaxStat(s16 *a, s16 *b);
s32 MainMenu_CompareCardsByAttack(s16 *a, s16 *b);
s32 MainMenu_CompareCardsByDefense(s16 *a, s16 *b);
s32 MainMenu_CompareCardsByType(s16 *a, s16 *b);
s32 MainMenu_CompareCardsByCount(CardCountEntry *a, CardCountEntry *b);

#endif

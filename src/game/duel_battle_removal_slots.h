#ifndef GAME_DUEL_BATTLE_REMOVAL_SLOTS_H
#define GAME_DUEL_BATTLE_REMOVAL_SLOTS_H

#include "../types.h"

/* The two card-record indices the battle removes, -1 when unused. The
 * phase entry walks them by presentation cursor; DUEL_BATTLE_REMOVAL_SLOT
 * indexes the pair starting at the first. */
extern s8 gDuel_bBattleRemovalSlot0;
extern s8 gDuel_bBattleRemovalSlot1;

#define DUEL_BATTLE_REMOVAL_SLOT(index) ((&gDuel_bBattleRemovalSlot0)[index])

#endif

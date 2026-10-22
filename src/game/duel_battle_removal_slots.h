#ifndef GAME_DUEL_BATTLE_REMOVAL_SLOTS_H
#define GAME_DUEL_BATTLE_REMOVAL_SLOTS_H

#include "../types.h"

#ifdef D_8009B208_AS_REPLAY_BYTES
extern s8 gDuel_bBattleRemovalSlot0;
extern s8 gDuel_bBattleRemovalSlot1;
#else
extern s8 gDuel_bBattleRemovalSlot0[8];
#endif

#endif

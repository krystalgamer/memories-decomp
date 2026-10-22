#ifndef GAME_DUEL_SESSION_CONTROLS_H
#define GAME_DUEL_SESSION_CONTROLS_H

#include "../types.h"

#ifdef D_8009B368_IN_DATA
extern u8 gDuel_bPostDuelMainMode __attribute__((section(".data")));
#else
extern u8 gDuel_bPostDuelMainMode;
#endif

#ifdef D_8009B369_IN_DATA
extern u8 gDuel_bResumeFromAnimatedBattle __attribute__((section(".data")));
#else
extern u8 gDuel_bResumeFromAnimatedBattle;
#endif

#ifdef D_8009B370_AS_BYTE_ARRAY
extern u8 gDuel_wCampaignSceneOnWin[];
#else
extern u16 gDuel_wCampaignSceneOnWin;
#endif
extern u16 gDuel_wCampaignSceneOnLoss;

#ifdef D_8009B374_IN_DATA
extern u16 gDuel_wAnimatedBattleBgmCommand __attribute__((section(".data")));
#else
extern u16 gDuel_wAnimatedBattleBgmCommand;
#endif

#endif

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

/* The win/loss scene pair. Main_RunDuelAndLibrary indexes it by
 * gDuel_bPlayerLost through an absolute address, which cc1psx builds only for
 * an unknown-bound array; the scalar writers reach it gp-relative. */
#ifdef GDUEL_CAMPAIGN_SCENE_ON_WIN_AS_PAIR
extern u16 gDuel_wCampaignSceneOnWin[];
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

#ifndef GAME_DUEL_SESSION_CONTROLS_H
#define GAME_DUEL_SESSION_CONTROLS_H

#include "../types.h"

/* A byte Main_RunDuel reads into a local after File_WaitForTransfers and,
 * past its nop barrier, copies into gMain_bActiveModeState. Four functions store it:
 * DebugMenu_EnterDuel stores 0 (then calls Main_StartDuel),
 * Main_RunTwoPlayerDuelSetup stores
 * 8 when MainMenu_UpdateValueSetup returned 1, Text_StartCampaignDuel
 * stores 2 at the end of its setup, and the free_duel overlay's
 * FreeDuel_UpdateScreen stores 6 after its Main_StartDuel call. Every
 * retail access is a byte (sb, and Main_RunDuel's lbu); no C unit defines
 * it. Initial value not read.
 *
 * The `u8 [9]` and `u8 []` two of them used to declare were accessed only at
 * [0]; as the notes on gDuel_bSide0ControllerSelector and gDuel_bPlayerLost
 * say, such a size is a threshold, not a length. */
extern u8 gDuel_bPostDuelMainMode;

extern u8 gDuel_bResumeFromAnimatedBattle;

/* The campaign scene to return to after the duel, indexed by
 * gDuel_bPlayerLost: [0] after a win, [1] after a loss. Main_StartDuel and
 * Text_StartCampaignDuel write both; Main_RunDuel reads the selected one. */
extern u16 gDuel_awCampaignSceneByOutcome[2];

/* DuelScene_UpdateBattle (src/game/duel_scene_battle.c) reads it through a
 * %hi/%lo pair into the load's own register, which is the .data form. */
extern u16 gDuel_wAnimatedBattleBgmCommand;

#endif

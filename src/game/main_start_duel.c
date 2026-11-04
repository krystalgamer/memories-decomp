#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "main_start_duel.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
#include "../unmatched.h"
#include "duel_session_controls.h"
#include "main_mode_state.h"
#include "ai_opponent_data.h"
#include "duel_terrain_boost.h"

void Main_StartDuel(s32 side0_controller, s32 opponent_id, s32 win_scene, s32 loss_scene)
{
    gDuel_wBgmId = 0x7270;
    gDuel_wAnimatedBattleBgmCommand = 0x7280;
    gDuel_bSide0ControllerSelector[0] = side0_controller;
    gDuel_bOpponentID = opponent_id;
    gDuel_wCampaignSceneOnWin = win_scene;
    gDuel_wCampaignSceneOnLoss = loss_scene;
    gDuel_bTerrain = 0;
    gDuel_bResumeFromAnimatedBattle = 0;
    gMain_bActiveModeState = 3;
}

#ifndef MEMORIES_DECOMP_MAIN_START_DUEL_H
#define MEMORIES_DECOMP_MAIN_START_DUEL_H

#include "../types.h"

/* Arms a duel: stores side0_controller into gDuel_bSide0ControllerSelector[0], opponent_id into gDuel_bOpponentID and
 * win_scene/loss_scene into the halfwords gDuel_wCampaignSceneOnWin/gDuel_wCampaignSceneOnLoss, resets the terrain and
 * sets gMain_bActiveModeState to 3.
 *
 * The parameters are s32 even though every store is a byte or a halfword.
 * Retail's Free Duel overlay passes a computed grid index as opponent_id without
 * narrowing it, and an s8 parameter makes that caller emit an sll/sra pair
 * retail does not have. The definition stores through sb/sh either way, and
 * the two resident callers pass constants, so neither side moves. */
void Main_StartDuel(s32 side0_controller, s32 opponent_id, s32 win_scene, s32 loss_scene);

#endif

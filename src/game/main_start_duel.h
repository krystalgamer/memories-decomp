#ifndef MEMORIES_DECOMP_MAIN_START_DUEL_H
#define MEMORIES_DECOMP_MAIN_START_DUEL_H

#include "../types.h"

/* Arms a duel: stores arg0 into gDuel_bSide0ControllerSelector[0], arg1 into gDuel_bOpponentID and
 * arg2/arg3 into the halfwords gDuel_wCampaignSceneOnWin/gDuel_wCampaignSceneOnLoss, resets the terrain and
 * sets gMain_bActiveModeState to 3.
 *
 * The parameters are s32 even though every store is a byte or a halfword.
 * Retail's Free Duel overlay passes a computed grid index as arg1 without
 * narrowing it, and an s8 parameter makes that caller emit an sll/sra pair
 * retail does not have. The definition stores through sb/sh either way, and
 * the two resident callers pass constants, so neither side moves. */
void Main_StartDuel(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

#endif

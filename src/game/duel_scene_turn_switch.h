#ifndef MEMORIES_DECOMP_DUEL_SCENE_TURN_SWITCH_H
#define MEMORIES_DECOMP_DUEL_SCENE_TURN_SWITCH_H

#include "../types.h"

/* gDuel_apfnSceneStateHandler duel phase entry: the side-swap phase. On its first tick it starts
 * the swap view tween through Duel_StartViewTween -- the direction taken from
 * D_8009B1D5 -- and sets display request 0x10. When that request clears,
 * it flips D_8009B1D5, repoints gDuel_pActiveSideState and D_8009B22C at the new
 * acting side, and hands the scene to phase 2. */
void DuelScene_UpdateTurnSwitch(void);

#endif

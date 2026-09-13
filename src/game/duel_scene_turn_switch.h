#ifndef MEMORIES_DECOMP_DUEL_SCENE_TURN_SWITCH_H
#define MEMORIES_DECOMP_DUEL_SCENE_TURN_SWITCH_H

#include "../types.h"

/* gDuel_apfnSceneStateHandler duel phase entry: the side-swap phase. On its first tick it starts
 * the swap animation through func_80022D94 -- the direction taken from
 * D_8009B1D5 -- and sets the D_8009B162 timer to 0x10; when that timer reaches
 * zero it flips D_8009B1D5, repoints D_8009B1C8 and D_8009B22C at the new
 * acting side, and hands the scene to phase 2. */
void DuelScene_UpdateTurnSwitch(void);

#endif

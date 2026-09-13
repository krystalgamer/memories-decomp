#ifndef GAME_DUEL_SCENE_STATE_H
#define GAME_DUEL_SCENE_STATE_H

#include "../types.h"

#define DUEL_SCENE_PHASE_MASK 0xF
#define DUEL_SCENE_FLAG_INITIALIZED 0x8000

/* Scene phase and update flags. All measured accesses use this halfword;
 * the gap up to D_8009B244 is not evidence of a wider object. */
extern u16 gDuel_wSceneStateFlags;

#endif

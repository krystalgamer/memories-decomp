#ifndef MEMORIES_DECOMP_DUEL_SCENE_RESUME_H
#define MEMORIES_DECOMP_DUEL_SCENE_RESUME_H

#include "../types.h"

/* gDuel_apfnSceneStateHandler duel phase entry: the duel-resume phase. It re-sets up every
 * occupied record of both card ranges, restoring the flags and position each
 * one had, spawns the side indicator objects, and then either replays the
 * field effects or starts the BGM before fading in. Later ticks walk the
 * D_8009B208 replay list one entry per tick. */
#ifdef D_8009B208_AS_REPLAY_BYTES
/* DuelScene_UpdateBattle (src/game/duel_scene_battle.c) owns the first two
 * bytes as named scalars, the attacker's and the defender's card slot, and
 * defines both tentatively: retail's listing writes them as two gp-relative
 * symbols with load-delay nops, which only definitions in that unit give. */
extern s8 D_8009B208;
extern s8 D_8009B209;
#else
extern s8 D_8009B208[8];
#endif

void DuelScene_UpdateResume(void);

#endif

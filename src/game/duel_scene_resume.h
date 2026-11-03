#ifndef MEMORIES_DECOMP_DUEL_SCENE_RESUME_H
#define MEMORIES_DECOMP_DUEL_SCENE_RESUME_H

#include "../types.h"

/* gDuel_apfnSceneStateHandler duel phase entry: the duel-resume phase. It re-sets up every
 * occupied record of both card ranges, restoring the flags and position each
 * one had, spawns the side indicator objects, and then either replays the
 * field effects or starts the BGM before fading in. Later ticks walk the
 * gDuel_bBattleRemovalSlot0 replay list (duel_battle_removal_slots.h)
 * one entry per tick. */
void DuelScene_UpdateResume(void);

#endif

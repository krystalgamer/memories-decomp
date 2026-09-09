#ifndef MEMORIES_DECOMP_FUNC_8001825C_H
#define MEMORIES_DECOMP_FUNC_8001825C_H

#include "../types.h"

/* D_80090998 duel phase entry: the duel-resume phase. It re-sets up every
 * occupied record of both card ranges, restoring the flags and position each
 * one had, spawns the side indicator objects, and then either replays the
 * field effects or starts the BGM before fading in. Later ticks walk the
 * D_8009B208 replay list one entry per tick. */
void func_8001825C(void);

#endif

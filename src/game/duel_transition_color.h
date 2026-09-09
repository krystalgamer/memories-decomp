#ifndef MEMORIES_DECOMP_DUEL_TRANSITION_COLOR_H
#define MEMORIES_DECOMP_DUEL_TRANSITION_COLOR_H

#include "../types.h"

/* Drives the duel-to-menu colour transition one frame: pulls a fresh value
   off rand(), folds it into a triangle wave, and writes the resulting grey
   into the two packet colour bytes before handing off to the effect state
   machine. */
s32 func_80033BE8(void);

#endif

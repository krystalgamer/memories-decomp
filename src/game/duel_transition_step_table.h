#ifndef MEMORIES_DECOMP_DUEL_TRANSITION_STEP_TABLE_H
#define MEMORIES_DECOMP_DUEL_TRANSITION_STEP_TABLE_H

#include "../types.h"

/* The build-deck pane transition step table and the decimal divisor ladder,
   declared here so the source that defines them and the two that read them
   cannot drift apart. */
extern void (*D_80090DF8[])(u8 *);
extern s32 D_80090E0C[];

#endif

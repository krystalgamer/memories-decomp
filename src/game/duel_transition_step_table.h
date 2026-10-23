#ifndef MEMORIES_DECOMP_DUEL_TRANSITION_STEP_TABLE_H
#define MEMORIES_DECOMP_DUEL_TRANSITION_STEP_TABLE_H

#include "../types.h"

struct BuildDeckTransitionState;

/* The build-deck pane transition step table and the decimal divisor ladder,
   declared here so the source that defines them and the two that read them
   cannot drift apart. */
extern void (*gBuildDeck_apfnStepHandlers[])(struct BuildDeckTransitionState *);
extern s32 D_80090E0C[];

/* Logical divisor-prefix view; the opaque suffix keeps its native owner. */
#define gText_DecimalDivisorView D_80090E0C

#endif

#ifndef MEMORIES_DECOMP_FUNC_80032B38_H
#define MEMORIES_DECOMP_FUNC_80032B38_H

#include "build_deck_transition_state.h"

/* Marks the build-deck transition record initialized and reports whether its
 * bit was already set. */
s32 BuildDeck_TestAndSetInitialized(BuildDeckTransitionState *state);

#endif

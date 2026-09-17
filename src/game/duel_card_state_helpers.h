#ifndef MEMORIES_DECOMP_DUEL_CARD_STATE_HELPERS_H
#define MEMORIES_DECOMP_DUEL_CARD_STATE_HELPERS_H

#include "../types.h"

/* Resets both sides' active-card state, calling func_80027DF8 once per side
 * over D_801AB00C (duel_card_state_helpers.c:8).
 *
 * Two sources declared this for themselves before this header existed:
 * DuelScene_UpdateHandActions (src/candidates/func_8001BD88.c) and
 * DuelScene_UpdateFieldActions (src/candidates/func_8001D670.c), with the same
 * spelling the definition uses. It is the declaration this header exists to
 * hold.
 *
 * The unit's other function, func_80028260, is deliberately not declared here:
 * it has no consumer outside the unit, so a declaration would widen this
 * header for nobody. */
void func_80028220(void);

#endif

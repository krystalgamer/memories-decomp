#ifndef MEMORIES_DECOMP_DUEL_CARD_STATE_HELPERS_H
#define MEMORIES_DECOMP_DUEL_CARD_STATE_HELPERS_H

#include "../types.h"

/* Resets both sides' active-card state, calling func_80027DF8 once per side
 * over D_801AB00C (duel_card_state_helpers.c:8).
 *
 * Two sources declared this for themselves before this header existed:
 * DuelScene_UpdateHandActions (duel_scene_hand_actions.c) and
 * DuelScene_UpdateFieldActions (src/game/duel_scene_field_actions.c), with the same
 * spelling the definition uses. It is the declaration this header exists to
 * hold.
 *
 * The unit's other function, DuelCard_DecodeTaggedSlotIndex, is deliberately
 * not declared here: it has no consumer outside the unit, so a declaration
 * would widen this header for nobody. */
void func_80028220(void);

#endif

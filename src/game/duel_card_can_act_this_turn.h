#ifndef MEMORIES_DECOMP_DUEL_CARD_CAN_ACT_THIS_TURN_H
#define MEMORIES_DECOMP_DUEL_CARD_CAN_ACT_THIS_TURN_H

#include "../types.h"
#include "duel_card.h"

/* True only for an occupied duel-card record that is not marked used this
 * turn: the body tests DUEL_CARD_FLAG_OCCUPIED and then
 * DUEL_CARD_FLAG_USED_THIS_TURN on the record's flags
 * (duel_selected_card_checks.c).
 *
 * DuelScene_UpdateFieldActions (src/game/duel_scene_field_actions.c) is the only
 * source outside the unit that names it, and it declared the prototype for
 * itself because the defining source had no header. The spelling here is the
 * definition's own, int return included. */
int DuelCard_CanActThisTurn(DuelCardRecord *object);

#endif

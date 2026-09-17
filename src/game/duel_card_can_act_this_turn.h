#ifndef MEMORIES_DECOMP_DUEL_CARD_CAN_ACT_THIS_TURN_H
#define MEMORIES_DECOMP_DUEL_CARD_CAN_ACT_THIS_TURN_H

#include "../types.h"
#include "duel_card.h"

/* True only for an occupied duel-card record that is not marked used this
 * turn: the body tests DUEL_CARD_FLAG_OCCUPIED and then
 * DUEL_CARD_FLAG_USED_THIS_TURN on the record's flags
 * (duel_card_can_act_this_turn.c:5).
 *
 * DuelScene_UpdateFieldActions (src/candidates/func_8001D670.c) is the only
 * source outside the unit that names it, and it declared the prototype for
 * itself because the unit had no header. The spelling here is the
 * definition's own, int return included. */
int DuelCard_CanActThisTurn(DuelCardRecord *object);

#endif

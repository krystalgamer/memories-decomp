#ifndef MEMORIES_DECOMP_SCRIPT_OP_JUMP_IF_DECK_INCOMPLETE_H
#define MEMORIES_DECOMP_SCRIPT_OP_JUMP_IF_DECK_INCOMPLETE_H

#include "../types.h"

/* D_80090C50 handler: the conditional script jump on deck state. Always
 * consumes a 16-bit offset, and repoints D_8009B290 into D_801A8000 only when
 * Duel_IsPlayerDeckComplete reports the deck is NOT full -- so the jump is the
 * deck-incomplete path and falling through is the complete one. */
void Script_OpJumpIfDeckIncomplete(void);

#endif

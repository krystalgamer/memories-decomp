#ifndef MEMORIES_DECOMP_SCRIPT_OP_JUMP_IF_DECK_INCOMPLETE_H
#define MEMORIES_DECOMP_SCRIPT_OP_JUMP_IF_DECK_INCOMPLETE_H

#include "../types.h"

/* gScript_apfnCommandHandlers handler: the conditional script jump on deck state. Always
 * consumes a 16-bit offset, and repoints gScript_StreamCursor into D_801A8000 only when
 * Duel_IsPlayerDeckComplete reports the deck is NOT full -- so the jump is the
 * deck-incomplete path and falling through is the complete one. */
void Script_OpJumpIfDeckIncomplete(void);

#endif

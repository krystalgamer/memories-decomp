#ifndef MEMORIES_DECOMP_SCRIPT_OP_SAVE_PROMPT_H
#define MEMORIES_DECOMP_SCRIPT_OP_SAVE_PROMPT_H

#include "../types.h"

/* D_80090C50 handler: the dialog choice step, and the deck test it shares with
 * Script_OpJumpIfDeckIncomplete.
 *
 * Duel_IsPlayerDeckComplete reports whether the player's deck is full -- 1 when no
 * DECK_SIZE entry of gDuel_awPlayerDeck is zero, 0 as soon as one is -- and
 * Script_OpJumpIfDeckIncomplete branches the script on it. It lives here because this is the
 * unit that defines it; nothing about it is specific to the choice step. */
void Script_OpSavePrompt(void);
s32 Duel_IsPlayerDeckComplete(void);

#endif

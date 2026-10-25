#ifndef MEMORIES_DECOMP_SCRIPT_OP_SAVE_PROMPT_H
#define MEMORIES_DECOMP_SCRIPT_OP_SAVE_PROMPT_H

#include "../types.h"

/* Retained LE16 scene operand across prompt phases. The halfword workspace
 * stores write the scene byte and opaque companion; only the build-deck
 * handoff explicitly narrows it to u8. This does not assert allocation
 * extent or a shared record with the neighboring event flags. */
extern u16 gScript_SavePromptSceneOperand;

/* gScript_apfnCommandHandlers handler: the dialog choice step, and the deck test it shares with
 * Script_OpJumpIfDeckIncomplete.
 *
 * Duel_IsPlayerDeckComplete reports whether the player's deck is full -- 1 when no
 * DECK_SIZE entry of gDuel_awPlayerDeck is zero, 0 as soon as one is -- and
 * Script_OpJumpIfDeckIncomplete branches the script on it. It lives here because this is the
 * unit that defines it; nothing about it is specific to the choice step. */
#ifdef VERSION_EUROPE
extern void Script_OpSavePrompt(void) asm("func_8002EF3C");
#else
extern void Script_OpSavePrompt(void);
#endif
s32 Duel_IsPlayerDeckComplete(void);

#endif

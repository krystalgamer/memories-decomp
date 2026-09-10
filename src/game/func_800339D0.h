#ifndef MEMORIES_DECOMP_FUNC_800339D0_H
#define MEMORIES_DECOMP_FUNC_800339D0_H

#include "../types.h"

/* Exit step 4 of the Build Deck step table D_80090DF8
 * (duel_transition_step_table.c). It asks for confirmation and either
 * returns to the step saved in next_state or writes the deck out and clears
 * the state word. */
void func_800339D0(u8 *state);

/* The per-frame Build Deck driver. It pulses the two pane colours with a
 * triangle wave, dispatches D_80090DF8[D_8009B2FC->state & 0x3F] once
 * DuelEffect_UpdateState reports idle, and returns the state word. The exit
 * step above clears that word when it commits the deck, and both callers
 * (Main_RunBuildDeckMenu and Main_RunDuel, both now in src/candidates/)
 * leave the menu when
 * it reads zero. */
s32 func_80033BE8(void);

#endif

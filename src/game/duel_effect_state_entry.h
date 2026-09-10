#ifndef MEMORIES_DECOMP_DUEL_EFFECT_STATE_ENTRY_H
#define MEMORIES_DECOMP_DUEL_EFFECT_STATE_ENTRY_H

#include "../types.h"

/* The first two entries of the memory card dialog step table D_80090F9C
 * (mem_card_dialog_steps.c). Both latch DUEL_EFFECT_STATE_FLAG_INITIALIZED
 * once, seed D_8009B3EB's state and then step the load machine
 * func_8003E490. func_8003E7D4 starts it in state 0; func_8003E80C starts it
 * in state 1 and also raises 0x200 in gMemCard_wDialogFlags. */
void func_8003E7D4(void);
void func_8003E80C(void);

#endif

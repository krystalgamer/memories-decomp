#ifndef MEMORIES_DECOMP_MEM_CARD_DIALOG_LOAD_STEPS_H
#define MEMORIES_DECOMP_MEM_CARD_DIALOG_LOAD_STEPS_H

#include "../types.h"

/* The first two entries of the memory card dialog step table D_80090F9C
 * (mem_card_dialog_steps.c). Both latch DUEL_EFFECT_STATE_FLAG_INITIALIZED
 * once, seed D_8009B3EB's state and then step the load machine
 * MemCardDialog_UpdateLoad. MemCardDialog_StepLoad starts it in state 0, the
 * confirmation prompt; MemCardDialog_StepLoadUnprompted starts it in state 1
 * and also raises 0x200 in gMemCard_wDialogFlags. */
void MemCardDialog_StepLoad(void);
void MemCardDialog_StepLoadUnprompted(void);

#endif

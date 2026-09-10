#ifndef MEMORIES_DECOMP_MEM_CARD_DIALOG_SAVE_STEPS_H
#define MEMORIES_DECOMP_MEM_CARD_DIALOG_SAVE_STEPS_H

#include "../types.h"

/* The third and fourth entries of the memory card dialog step table
 * D_80090F9C (mem_card_dialog_steps.c). MemCardDialog_StepSave latches
 * DUEL_EFFECT_STATE_FLAG_INITIALIZED once, starts D_8009B3EB in state 0 and
 * steps the save machine MemCardDialog_UpdateSave. MemCardDialog_StepNone is
 * empty: the table slot is filled, but selecting it does nothing. */
void MemCardDialog_StepSave(void);
void MemCardDialog_StepNone(void);

#endif

#ifndef MEMORIES_DECOMP_DUEL_EFFECT_LATE_STATE_H
#define MEMORIES_DECOMP_DUEL_EFFECT_LATE_STATE_H

#include "../types.h"

/* The third and fourth entries of the memory card dialog step table
 * D_80090F9C (mem_card_dialog_steps.c). func_8003EE90 latches
 * DUEL_EFFECT_STATE_FLAG_INITIALIZED once, starts D_8009B3EB in state 0 and
 * steps the create machine func_8003E854. func_8003EEC8 is empty: the table
 * slot is filled, but selecting it does nothing. */
void func_8003EE90(void);
void func_8003EEC8(void);

#endif

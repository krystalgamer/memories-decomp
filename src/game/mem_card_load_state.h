#ifndef MEMORIES_DECOMP_MEM_CARD_LOAD_STATE_H
#define MEMORIES_DECOMP_MEM_CARD_LOAD_STATE_H

#include "../types.h"

/* One step of the memory card load dialog's state machine.
 *
 * It switches on the low nibble of D_8009B3EB and advances it, using bit
 * MEM_CARD_DIALOG_FLAG_RESULT_READY to mark that the message for the current
 * state has already been posted, so a state runs its request once and then
 * waits. gDialog_bChoice carries the player's answer back into the machine.
 *
 * duel_effect_state_entry.c is the only consumer and calls it to pump the
 * dialog; it takes no arguments and returns nothing, because the state, the
 * flag and the answer are all globals.
 *
 * mem_card_create_state.c is its sibling: the same nibble, the same flag and
 * the same choice global drive the create dialog. The name stays
 * address-based because which of the two the address belongs to is settled by
 * the file it lives in rather than by anything the function itself shows. */
void func_8003E490(void);

#endif

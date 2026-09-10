#ifndef MEMORIES_DECOMP_MEM_CARD_CREATE_STATE_H
#define MEMORIES_DECOMP_MEM_CARD_CREATE_STATE_H

#include "../types.h"

/* One step of the memory card create dialog's state machine, the sibling of
 * MemCardDialog_UpdateLoad in mem_card_load_state.c.
 *
 * It switches on the same low nibble of D_8009B3EB, uses the same
 * MEM_CARD_DIALOG_FLAG_RESULT_READY bit to post each state's message once,
 * and reads the player's answer from the same gDialog_bChoice. Where the load
 * machine only counts files, this one also weighs free blocks against the
 * blocks the save needs.
 *
 * mem_card_dialog_save_steps.c is the only consumer. It takes no arguments
 * and returns nothing, for the same reason as its sibling: state, flag and
 * answer are globals.
 *
 * The definition pins its message variable to a register. That spelling is
 * load bearing and should not be relaxed into a plain local without a
 * measurement. */
void MemCardDialog_UpdateSave(void);

#endif

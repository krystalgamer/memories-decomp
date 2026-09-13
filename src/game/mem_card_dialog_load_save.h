#ifndef MEMORIES_DECOMP_MEM_CARD_DIALOG_LOAD_SAVE_H
#define MEMORIES_DECOMP_MEM_CARD_DIALOG_LOAD_SAVE_H

#include "../types.h"

/* The first four entries of the memory-card dialog step table D_80090F9C.
 * The load and save callbacks latch DUEL_EFFECT_STATE_FLAG_INITIALIZED, seed
 * D_8009B3EB, and then pump the corresponding state machine. Step 1 skips the
 * load confirmation prompt; step 3 is an intentionally empty table slot. */
void MemCardDialog_UpdateLoad(void);
void MemCardDialog_StepLoad(void);
void MemCardDialog_StepLoadUnprompted(void);
void MemCardDialog_UpdateSave(void);
void MemCardDialog_StepSave(void);
void MemCardDialog_StepNone(void);

#endif

#include "../types.h"
#include "../psyq/libmcrd.h"
#include "duel_effect.h"
#include "file_transfer.h"
#include "mem_card.h"
#include "mem_card_dialog_load_save.h"
#include "mem_card_directory.h"
#include "save_data.h"
#include "text_staging.h"
#include "../unmatched.h"

/* The save step callback D_80090F9C selects, which latches first entry and
   runs MemCardDialog_UpdateSave, and the empty step after it. The save state
   machine itself is a candidate since #3859 (src/candidates/func_8003E854.c),
   which is what split these two from mem_card_dialog_load_save.c. */

void MemCardDialog_StepSave(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        D_8009B3EB = 0;
    }
    MemCardDialog_UpdateSave();
}

void MemCardDialog_StepNone(void)
{
}

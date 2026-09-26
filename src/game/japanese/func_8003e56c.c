#include "../../types.h"
#include "../duel_effect.h"
#include "../mem_card.h"
#include "../mem_card_dialog_load_save.h"
#include "../../unmatched.h"

/* Japanese only, with no US counterpart: MemCardDialog_StepSave's one-time
   setup, plus clearing the transfer offset and raising dialog flag 0x100,
   before the Japanese save update runs. */
void func_8003E56C(void)
{
    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        tent_MemCardTransferOffset = 0;
        tent_MemCardDialogStepState = 0;
        gMemCard_wDialogFlags |= 0x100;
    }
    MemCardDialog_UpdateSave();
}

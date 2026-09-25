#define D_8009B3C4 gJapanese_MemCardTransferOffset
#define D_8009B3EB gJapanese_MemCardDialogStepState
#define MemCardDialog_UpdateSave func_8003DDF8
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
        D_8009B3C4 = 0;
        D_8009B3EB = 0;
        gMemCard_wDialogFlags |= 0x100;
    }
    MemCardDialog_UpdateSave();
}

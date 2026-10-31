#include "../types.h"
#include "duel_effect.h"
#include "dialog_callback_state.h"
#include "mem_card_work_initialize_operation.h"
#include "mem_card_work.h"
#include "mem_card_work_state.h"
#include "../unmatched.h"

void MemCardWork_InitializeOperation(u8 value)
{
    gMemCardWork_bRootBlocks[0] = 1;
    gMemCardWork_OperationState = 0;
    gDialog_bCallbackState = 0;
    gMemCardWork_wFlags = 0x8000;
    gMemCardWork_bSlotIndex = 0;
    gMemCardWork_OperationMode = value;
}

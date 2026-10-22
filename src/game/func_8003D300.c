#include "../types.h"
#include "duel_effect.h"
#include "func_8003D300.h"
#include "mem_card_work.h"
#include "mem_card_work_state.h"
#include "../unmatched.h"

/* The size is a codegen input under -G8: it keeps this symbol out of small
 * data. Only byte zero is currently established. */

void MemCardWork_InitializeOperation(u8 value)
{
    D_800EF6E0[0] = 1;
    gMemCardWork_OperationState = 0;
    D_8009B3C1 = 0;
    gMemCardWork_wFlags = 0x8000;
    gMemCardWork_bSlotIndex = 0;
    gMemCardWork_OperationMode = value;
}

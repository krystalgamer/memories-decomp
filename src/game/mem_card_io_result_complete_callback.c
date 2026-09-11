#include "../types.h"
#include "mem_card.h"
#include "mem_card_io_result_callbacks.h"

long MemCard_SetIOResultCompleteCB(void)
{
    gMemCard_nIOResult = 0;
    return 0;
}

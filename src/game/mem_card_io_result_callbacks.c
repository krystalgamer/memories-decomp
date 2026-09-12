#include "../types.h"
#include "mem_card.h"
#include "mem_card_io_result_callbacks.h"

long MemCard_SetIOResultCompleteCB(void)
{
    gMemCard_nIOResult = 0;
    return 0;
}

long MemCard_SetIOResultTimeoutCB(void)
{
    gMemCard_nIOResult = 1;
    return 0;
}

long MemCard_SetIOResultErrorCB(void)
{
    gMemCard_nIOResult = 2;
    return 0;
}

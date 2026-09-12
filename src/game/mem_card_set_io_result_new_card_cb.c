#include "../types.h"
#include "mem_card.h"
#include "mem_card_io_result_callbacks.h"

long MemCard_SetIOResultNewCardCB(void)
{
    gMemCard_nIOResult = 3;
    return 0;
}

#include "../../types.h"
#include "../mem_card.h"
#include "../mem_card_io_result_callbacks.h"

PSXLONG MemCard_SetIOResultTimeoutCB(void)
{
    gMemCard_nIOResult = 1;
    return 0;
}

#include "../../types.h"
#include "../mem_card.h"
#include "../mem_card_io_result_callbacks.h"

PSXLONG MemCard_SetIOResultErrorCB(void)
{
    gMemCard_nIOResult = 2;
    return 0;
}

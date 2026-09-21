#define gMemCard_nIOResult gJapanese_MemCardIOResult
#include "../../types.h"
#include "../mem_card.h"
#include "../mem_card_io_result_callbacks.h"

long MemCard_SetIOResultTimeoutCB(void)
{
    gJapanese_MemCardIOResult = 1;
    return 0;
}

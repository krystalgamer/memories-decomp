#define gMemCard_nIOResult gJapanese_MemCardIOResult
#include "../../types.h"
#include "../mem_card.h"
#include "../mem_card_io_result_callbacks.h"

long MemCard_SetIOResultErrorCB(void)
{
    gJapanese_MemCardIOResult = 2;
    return 0;
}

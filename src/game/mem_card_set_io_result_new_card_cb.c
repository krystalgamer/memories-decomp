#include "../types.h"
#include "mem_card_io_result_callbacks.h"

long MemCard_SetIOResultNewCardCB(void)
{
    u8 *page = (u8 *)0x800A0000;

    *(volatile s32 *)(page - 0x4BB0) = 3;
    return 0;
}

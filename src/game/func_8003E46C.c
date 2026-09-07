#include "../types.h"

#include "mem_card.h"

extern u8 D_8009B3C6;
extern volatile u16 gMemCard_wDialogFlags;

void func_8003E46C(u8 value, u16 bits)
{
    u16 flags = gMemCard_wDialogFlags;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    D_8009B3C6 = value;
    flags &= 0xFF87;
    gMemCard_wDialogFlags = flags;
    gMemCard_wDialogFlags = flags | bits;
}

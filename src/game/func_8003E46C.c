#include "../types.h"

#include "mem_card.h"

/* The two stores below are deliberate: retail writes the masked value and
   then the value with the new bits set. Without volatile the first store is
   dead and GCC drops it, so this file reaches the word under its own linker
   name rather than forcing volatile on every reader in mem_card.h. */
extern volatile u16 gMemCard_wDialogFlags_v asm("gMemCard_wDialogFlags");

void func_8003E46C(u8 value, u16 bits)
{
    u16 flags = gMemCard_wDialogFlags_v;

    bits |= MEM_CARD_DIALOG_FLAG_RESULT_READY;
    D_8009B3C6 = value;
    flags &= 0xFF87;
    gMemCard_wDialogFlags_v = flags;
    gMemCard_wDialogFlags_v = flags | bits;
}

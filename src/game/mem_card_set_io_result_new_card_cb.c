#include "../types.h"

int MemCard_SetIOResultNewCardCB(void)
{
    register char *base asm("$3") = (char *)0x800A0000;
    register int value asm("$2") = 3;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = value;
    return 0;
}

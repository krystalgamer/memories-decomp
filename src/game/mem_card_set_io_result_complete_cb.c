#include "../types.h"

int MemCard_SetIOResultCompleteCB(void)
{
    register char *base asm("$2") = (char *)0x800A0000;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = 0;
    return 0;
}

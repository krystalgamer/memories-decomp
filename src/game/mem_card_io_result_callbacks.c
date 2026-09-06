#include "../types.h"

long MemCard_SetIOResultCompleteCB(void)
{
    register char *base asm("$2") = (char *)0x800A0000;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = 0;
    return 0;
}

long MemCard_SetIOResultTimeoutCB(void)
{
    register char *base asm("$3") = (char *)0x800A0000;
    register int value asm("$2") = 1;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = value;
    return 0;
}

long MemCard_SetIOResultErrorCB(void)
{
    register char *base asm("$3") = (char *)0x800A0000;
    register int value asm("$2") = 2;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = value;
    return 0;
}

long MemCard_SetIOResultNewCardCB(void)
{
    register char *base asm("$3") = (char *)0x800A0000;
    register int value asm("$2") = 3;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = value;
    return 0;
}

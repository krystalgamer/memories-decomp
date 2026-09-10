/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/mem_card_io_result_callbacks.c.
 */
#include "../types.h"
#include "../game/mem_card_io_result_callbacks.h"

long MemCard_SetIOResultCompleteCB(void)
{
    register char *base asm("$2") = (char *)0x800A0000;
    asm("" : "+r"(base));
    *(int *)(base - 0x4BB0) = 0;
    return 0;
}


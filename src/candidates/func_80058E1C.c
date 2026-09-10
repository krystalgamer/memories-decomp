/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_80058E1C.c.
 */
#include "../types.h"
#include "../game/model_graphics_state.h"
#include "../game/func_80058E1C.h"

int func_80058E1C(void)
{
    register unsigned int result asm("$3") = 6;

    if (D_8009AFA3 < result) {
        result = D_8009AFA3;
    }
    return result;
}

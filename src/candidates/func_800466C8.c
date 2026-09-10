/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers and 2 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_output_transition.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"

#include "../game/sound.h"
#include "../game/sound_output_transition.h"

void func_800466C8(void)
{
    register SDValue *state asm("$3") = g_SDValue;
    register SDValue *flags asm("$4");

    if (state->flags_0040 & 0x80) {
        state->field_1588 = 8;
        state->field_1584 = 255;
        asm volatile("" : : : "memory");
        state = g_SDValue;
    }
    state->field_0049 = 0;
    asm volatile("" : : : "memory");
    flags = g_SDValue;
    state->field_0512 = -64;
    flags->flags_0040 &= 0xFFFB;
}


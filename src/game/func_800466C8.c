#define G_SDVALUE_VOLATILE
#include "../types.h"
#include "sound.h"
#include "sound_output_transition.h"

void func_800466C8(void)
{
    SDValue *state = g_SDValue;
    SDValue *flags;

    if (state->flags_0040 & 0x80) {
        state->field_1588 = 8;
        state->field_1584 = 255;
        state = g_SDValue;
        state->field_0049 = 0;
        flags = g_SDValue;
        state->field_0512 = -64;
        flags->flags_0040 &= 0xFFFB;
        return;
    }
    state->field_0049 = 0;
    flags = g_SDValue;
    state->field_0512 = -64;
    flags->flags_0040 &= 0xFFFB;
}

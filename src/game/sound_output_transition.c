#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

void func_8004666C(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->field_0049 = 255;
    state = g_SDValue;
    first->field_0512 = 64;
    state->flags_0040 = (state->flags_0040 & 0xFFFC) | 4;
    if ((state->flags_0040 & 0x80) &&
        state->field_1588 >= 0) {
        state->field_1588 = -16;
        state->field_1584 = 220;
    }
}


#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_sequence_timing.h"

int func_80049A64(void *input, short value)
{
    unsigned int tag;
    SDSecondaryState *state;
    D_8009B458->flag_0500 = 1;
    tag = *(unsigned int *)input;
    if (tag != SD_SEQUENCE_TAG_SEQ && tag != SD_SEQUENCE_TAG_MIDI &&
        tag != SD_SEQUENCE_TAG_KDT && tag != SD_SEQUENCE_TAG_KDT1)
        return -1;
    state = D_8009B458;
    if (state->field_07E0 == -1) {
        state->field_07E8 = input;
        state->field_07E0 = value;
        state->field_07E2 = 2;
        state->flag_0500 = 0;
        return 0;
    }
    state->flag_0500 = 0;
    return -1;
}


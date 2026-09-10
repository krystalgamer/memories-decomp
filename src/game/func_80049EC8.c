#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_sequence_timing.h"

void func_80049EC8(s32 arg0, s32 arg1)
{
    SDSecondaryState *state = D_8009B458;
    s32 mode = state->field_07E2;

    state->field_07E4 = arg0 & SD_SECONDARY_LEVEL_MASK;
    state->field_07E6 = arg1 & SD_SECONDARY_LEVEL_MASK;
    if (mode != 2)
        SD_UpdateSecondaryObjectVolumes();
}

void func_80049F10(s16 first, s16 second)
{
    SDSecondaryState *state = D_8009B458;

    state->field_0514 = first;
    state->field_0516 = second;
    if (state->field_07E2 != 2)
        SD_UpdateSecondaryObjectVolumes();
}

s32 func_80049F50(void)
{
    s32 value;

    if (D_8009B458->field_07E2 == 1) {
        value = (s16)SD_GetSequenceStatus();
        if (value == 3)
            D_8009B458->field_07E2 = value;
    }
    return D_8009B458->field_07E2;
}

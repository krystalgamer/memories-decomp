#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_sequence_timing.h"

void SD_SetSecondarySpatialLevels(s32 arg0, s32 arg1)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 mode = state->sequence_state;

    state->spatial_level_left = arg0 & SD_SECONDARY_LEVEL_MASK;
    state->spatial_level_right = arg1 & SD_SECONDARY_LEVEL_MASK;
    if (mode != 2)
        SD_UpdateSecondaryObjectVolumes();
}

void SD_SetSecondaryMasterLevels(s16 left, s16 right)
{
    SDSecondaryState *state = gSD_pSecondaryState;

    state->master_level_left = left;
    state->master_level_right = right;
    if (state->sequence_state != 2)
        SD_UpdateSecondaryObjectVolumes();
}

s32 SD_PollSequenceState(void)
{
    s32 value;

    if (gSD_pSecondaryState->sequence_state == 1) {
        value = (s16)SD_GetSequenceStatus();
        if (value == 3)
            gSD_pSecondaryState->sequence_state = value;
    }
    return gSD_pSecondaryState->sequence_state;
}

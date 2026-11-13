#include "../types.h"
#include "sound_spatialize_object.h"

#include "sound.h"
#include "sound_spatialization.h"

void SD_UpdateSecondaryObjectVolumes(void)
{
    SDSecondaryState *state = gSD_pSecondaryState;
    s32 i;

    for (i = 0; i < state->object_count; i++) {
        if (state->objects[i].channel_index < SD_SEQUENCE_CHANNEL_COUNT) {
            SD_SpatializeSecondaryObject(
                &state->objects[i],
                &state->channels[state->objects[i].channel_index]);
            SD_SetVoiceVolume(i, gSD_pSecondaryState->objects[i].level_left,
                              gSD_pSecondaryState->objects[i].level_right);
        }
        state = gSD_pSecondaryState;
    }
}

s32 SD_CalcPitchBend(SDSecondaryObject *entry, s32 value)
{
    u8 check = value;
    int result;

    if (check < SD_SEQUENCE_PITCH_BEND_CENTER) {
        if (entry->pitch_bend_negative_scale == 0)
            return 0;
        result = -((entry->pitch_bend_negative_scale << 1) *
                   (SD_SEQUENCE_PITCH_BEND_CENTER - (u8)value));
    } else {
        if (check == SD_SEQUENCE_PITCH_BEND_CENTER)
            return 0;
        if (entry->pitch_bend_positive_scale == 0)
            return 0;
        result = (entry->pitch_bend_positive_scale << 1) *
                 ((u8)value - SD_SEQUENCE_PITCH_BEND_POSITIVE_BIAS);
    }
    return (short)result;
}

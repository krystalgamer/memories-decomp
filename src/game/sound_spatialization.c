#include "../types.h"
#include "sound_spatialize_object.h"

#include "sound.h"
#include "sound_spatialization.h"
#include "../unmatched.h"

void SD_UpdateSecondaryObjectVolumes(void)
{
    SDSecondaryState *state = D_8009B458;
    s32 i;
    s32 object_offset;
    s32 offset;

    if (state->object_count > 0) {
        s32 count;

        i = 0;
        object_offset = 0x180;
        offset = i;
        do {
            SDSecondaryState *entry =
                (SDSecondaryState *)((u8 *)state + offset);

            if (entry->objects[0].channel_index <
                SD_SEQUENCE_CHANNEL_COUNT) {
                s32 value = entry->objects[0].channel_index;
                SDSecondaryState *current;

                SD_SpatializeSecondaryObject(
                    (SDSecondaryObject *)((u8 *)state + object_offset),
                    &state->channels[value]);
                current = D_8009B458;
                SD_SetVoiceVolume(
                    i,
                    ((SDSecondaryState *)((u8 *)current + offset))
                        ->objects[0].level_left,
                    ((SDSecondaryState *)((u8 *)current + offset))
                        ->objects[0].level_right);
            }
            object_offset += SD_SECONDARY_OBJECT_SIZE;
            state = D_8009B458;
            count = state->object_count;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (++i < count);
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

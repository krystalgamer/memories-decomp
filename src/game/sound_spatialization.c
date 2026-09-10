#include "../types.h"
#include "sound_spatialize_object.h"

#include "sound.h"
#include "sound_spatialization.h"

void SD_UpdateSecondaryObjectVolumes(void)
{
    register SDSecondaryState *state asm("$6") = D_8009B458;
    register int i asm("$17");
    register int object_offset asm("$18");
    register int offset asm("$16");

    if (state->object_count > 0) {
        int count;

        i = 0;
        object_offset = 0x180;
        offset = i;
        do {
            /* Keep the channel displacement folded into both byte loads. */
            u8 *entry = (u8 *)state + offset;
            if (entry[0x183] < SD_SEQUENCE_CHANNEL_COUNT) {
                int value = entry[0x183];
                SDSecondaryState *current;

                SD_SpatializeSecondaryObject(
                    (SDSecondaryObject *)((u8 *)state + object_offset),
                    &state->channels[value]);
                current = D_8009B458;
                SD_SetVoiceVolume(i,
                    *(u16 *)((u8 *)current + offset + 0x194),
                    *(u16 *)((u8 *)current + offset + 0x196));
            }
            object_offset += SD_SECONDARY_OBJECT_SIZE;
            asm volatile("" : "+r"(object_offset));
            state = D_8009B458;
            count = state->object_count;
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
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

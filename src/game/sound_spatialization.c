#include "../types.h"
#include "sound_spatialize_object.h"

#define SDSECONDARYSTATE_CUSTOM_EXTERN
#include "sound.h"
#include "sound_spatialization.h"

extern u8 *D_8009B458;

void SD_UpdateSecondaryObjectVolumes(void)
{
    register u8 *state asm("$6") = D_8009B458;
    register int i asm("$17");
    register int object_offset asm("$18");
    register int offset asm("$16");

    if (*(short *)(state + 0x510) > 0) {
        int count;

        i = 0;
        object_offset = 0x180;
        offset = i;
        do {
            u8 *entry = state + offset;
            if (entry[0x183] < SD_SEQUENCE_CHANNEL_COUNT) {
                int value = entry[0x183];
                u8 *current;

                SD_SpatializeSecondaryObject(
                    state + object_offset,
                    state + value * SD_SEQUENCE_CHANNEL_RECORD_SIZE);
                current = D_8009B458;
                SD_SetVoiceVolume(i, *(u16 *)(current + offset + 0x194),
                                  *(u16 *)(current + offset + 0x196));
            }
            object_offset += SD_SECONDARY_OBJECT_SIZE;
            asm volatile("" : "+r"(object_offset));
            state = D_8009B458;
            count = *(short *)(state + 0x510);
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

#include "../types.h"
#include "../unmatched.h"
#include "sound.h"
#include "sound_spatialization.h"
#include "sound_spatialize_object.h"

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
            u8 *entry = (u8 *)state + offset;

            if (entry[0x183] < SD_SEQUENCE_CHANNEL_COUNT) {
                s32 value = entry[0x183];
                SDSecondaryState *current;

                SD_SpatializeSecondaryObject(
                    (SDSecondaryObject *)((u8 *)state + object_offset),
                    &state->channels[value]);
                current = D_8009B458;
                SD_SetVoiceVolume(
                    i,
                    *(u16 *)((u8 *)current + offset + 0x194),
                    *(u16 *)((u8 *)current + offset + 0x196));
            }
            object_offset += SD_SECONDARY_OBJECT_SIZE;
            state = D_8009B458;
            count = state->object_count;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (++i < count);
    }
}

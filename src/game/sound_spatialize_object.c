#include "../types.h"
#include "sound_spatialize_object.h"
#include "sound_sequence_constants.h"

void SD_SpatializeSecondaryObject(SDSecondaryObject *object,
                                 SDSecondaryRecord *channel)
{
    s32 pan;
    s32 level;
    s32 left;
    s32 right;

    if (gSD_pSecondaryState->pan_center_override != 0) {
        pan = SD_SECONDARY_PAN_CENTER;
    } else {
        pan = gSD_pSecondaryState->transfer.vab_pan + object->program_pan +
              object->tone_pan + channel->pan -
              SD_SECONDARY_PAN_SUM_BIAS;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= SD_SECONDARY_PAN_LIMIT) {
        pan = SD_SECONDARY_PAN_MAX;
    }
    object->pan = pan;

    level = gSD_pSecondaryState->transfer.vab_master_volume *
            (u16)gSD_pSecondaryState->sequence_level;
    level = (level * channel->expression * channel->volume) >> 14;
    level = (level * object->program_volume * object->tone_volume) >> 14;

    if (pan >= SD_SECONDARY_PAN_CENTER) {
        if (pan == SD_SECONDARY_PAN_CENTER) {
            left = right = level;
        } else {
            right = level;
            left = ((SD_SECONDARY_PAN_CENTER -
                     (pan & SD_SECONDARY_PAN_HALF_MASK)) * (right << 1)) >> 7;
        }
    } else {
        left = level;
        right = (pan * (left << 1)) >> 7;
    }

    left = (left * gSD_pSecondaryState->spatial_level_left) >> 7;
    right = (right * gSD_pSecondaryState->spatial_level_right) >> 7;
    object->level_left = (left * (object->velocity & 0x7F)) >> 7;
    object->level_right = (right * (object->velocity & 0x7F)) >> 7;
}

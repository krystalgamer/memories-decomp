#include "../types.h"
#include "sound_spatialize_object.h"
#include "sound_sequence_constants.h"

void SD_SpatializeSecondaryObject(SDSecondaryObject *object,
                                 SDSecondaryRecord *channel)
{
    s32 pan;
    SDSecondaryState *state1;
    SDSecondaryState *state2;
    SDSecondaryState *state3;
    s32 level;
    s32 product;
    s32 left;
    s32 right;
    s32 center;

    state1 = gSD_pSecondaryState;
    if (state1->pan_center_override != 0) {
        pan = SD_SECONDARY_PAN_CENTER;
    } else {
        pan = state1->transfer.vab_pan + object->program_pan +
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

    state2 = gSD_pSecondaryState;
    level = state2->transfer.vab_master_volume * (u16)state2->sequence_level;
    level = level * channel->expression;
    level = level * channel->volume;
    level = level >> 14;
    level = level * object->program_volume;
    product = level * object->tone_volume;
    level = product >> 14;

    left = level;
    if (pan >= SD_SECONDARY_PAN_CENTER) {
        right = level;
        center = SD_SECONDARY_PAN_CENTER;
        if (pan == center) {
            right = (pan * (left << 1)) >> 7;
            left = right;
        } else {
            left = ((center - (pan & SD_SECONDARY_PAN_HALF_MASK)) * (right << 1)) >> 7;
        }
    } else {
        right = (pan * (left << 1)) >> 7;
    }

    state3 = gSD_pSecondaryState;
    pan = left * state3->spatial_level_left;
    right = right * state3->spatial_level_right;
    left = pan >> 7;
    level = left * (*(volatile u8 *)&object->velocity & 0x7F);
    right = right >> 7;
    pan = right * (*(volatile u8 *)&object->velocity & 0x7F);
    object->level_left = level >> 7;
    object->level_right = pan >> 7;
}

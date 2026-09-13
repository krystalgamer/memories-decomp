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

    state1 = D_8009B458;
    if (state1->field_0815 != 0) {
        pan = SD_SECONDARY_PAN_CENTER;
    } else {
        pan = state1->transfer.field_001B + object->field_000A +
              object->field_000B + channel->pan -
              SD_SECONDARY_PAN_SUM_BIAS;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= SD_SECONDARY_PAN_LIMIT) {
        pan = SD_SECONDARY_PAN_MAX;
    }
    object->pan = pan;

    state2 = D_8009B458;
    level = state2->transfer.field_0018 * (u16)state2->field_0512;
    level = level * channel->expression;
    level = level * channel->volume;
    level = level >> 14;
    level = level * object->field_0008;
    product = level * object->field_0009;
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

    state3 = D_8009B458;
    pan = left * state3->field_07E4;
    right = right * state3->field_07E6;
    left = pan >> 7;
    level = left * (*(volatile u8 *)&object->field_000E & 0x7F);
    right = right >> 7;
    pan = right * (*(volatile u8 *)&object->field_000E & 0x7F);
    object->level_left = level >> 7;
    object->level_right = pan >> 7;
}

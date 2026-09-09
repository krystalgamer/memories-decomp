#include "../types.h"
#include "sound_spatialize_object.h"
#include "sound_sequence_constants.h"

extern u8 *D_8009B458;

/* The object parameter is copied into a $7-pinned local rather than used
   directly; that pin is the matching device, so the parameter keeps a
   separate name. `channel` is used as passed. */
void SD_SpatializeSecondaryObject(u8 *object_arg, u8 *channel)
{
    register u8 *object asm("$7");
    s32 pan;
    u8 *state1;
    u8 *state2;
    u8 *state3;
    s32 level;
    s32 product;
    s32 left;
    register s32 right asm("$4");
    s32 center;

    object = object_arg;
    state1 = D_8009B458;
    if (state1[0x815] != 0) {
        pan = SD_SECONDARY_PAN_CENTER;
    } else {
        pan = state1[0x4BF] + object[0xA] + object[0xB] + channel[1] -
              SD_SECONDARY_PAN_SUM_BIAS;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= SD_SECONDARY_PAN_LIMIT) {
        pan = SD_SECONDARY_PAN_MAX;
    }
    object[0xC] = pan;

    state2 = D_8009B458;
    level = state2[0x4BC] * *(u16 *)(state2 + 0x512);
    level = level * channel[5];
    level = level * channel[3];
    level = level >> 14;
    level = level * object[8];
    product = level * object[9];
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
    pan = left * *(s16 *)(state3 + 0x7E4);
    right = right * *(s16 *)(state3 + 0x7E6);
    left = pan >> 7;
    level = left * (*(volatile u8 *)(object + 0xE) & 0x7F);
    right = right >> 7;
    pan = right * (*(volatile u8 *)(object + 0xE) & 0x7F);
    *(u16 *)(object + 0x14) = level >> 7;
    *(u16 *)(object + 0x16) = pan >> 7;
}

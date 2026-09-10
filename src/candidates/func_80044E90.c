/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_mix.c.
 */
#include "../types.h"
#include "../psyq/libcd.h"
#include "../game/func_80044DC0.h"
#include "../game/sound.h"
#include "../game/sound_mix.h"

void func_80044E90(s32 value)
{
    register s32 saved asm("$5") = value;

    if ((u16)(value - 1) < SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = -SD_CHANNEL_VOLUME_MAX - saved;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    } else if ((u16)(value + SD_CHANNEL_VOLUME_MAX) <
        SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = -SD_CHANNEL_VOLUME_MAX - saved;
    } else {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    }
    func_80044DC0(g_SDValue->cd_volume);
}


#include "../types.h"
#include "func_80044DC0.h"
#include "sound.h"
#include "sound_mix.h"

void func_80044E90(s32 value)
{
    s16 saved = value;

    if ((u16)((u32)value - 1) < SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = -SD_CHANNEL_VOLUME_MAX - saved;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    } else if ((u16)((u32)value + SD_CHANNEL_VOLUME_MAX) <
        SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = -SD_CHANNEL_VOLUME_MAX - saved;
    } else {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    }
    func_80044DC0(g_SDValue->cd_volume);
}

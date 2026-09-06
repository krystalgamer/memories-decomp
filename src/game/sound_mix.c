#include "../types.h"
#include "sound.h"

extern void func_80044DC0(s32);
extern void func_8007CDC0(u8 *);

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

void func_80044F58(s32 value)
{
    u8 data[4];
    SDValue *first;
    SDValue *second;

    g_SDValue->mix_multiplier = 7;
    data[0] = value;
    data[3] = 0;
    if (g_SDValue->output_type == 0) {
        data[1] = 0;
        data[2] = value;
    } else {
        data[1] = value;
        data[2] = 0;
    }
    func_8007CDC0(data);
    first = g_SDValue;
    first->field_0049 = value;
    second = g_SDValue;
    first->cd_volume = value;
    second->field_0512 = 0;
}

s32 func_80044FE4(void)
{
    return g_SDValue->cd_volume;
}

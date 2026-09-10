#include "../types.h"
#include "../psyq/libcd.h"
#include "func_80044DC0.h"
#include "sound.h"
#include "sound_mix.h"

void func_80044F58(s32 value)
{
    CdlATV data;
    SDValue *first;
    SDValue *second;

    g_SDValue->mix_multiplier = 7;
    data.val0 = value;
    data.val3 = 0;
    if (g_SDValue->output_type == 0) {
        data.val1 = 0;
        data.val2 = value;
    } else {
        data.val1 = value;
        data.val2 = 0;
    }
    CdMix(&data);
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

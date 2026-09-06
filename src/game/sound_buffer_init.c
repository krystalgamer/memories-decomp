#include "../types.h"
#include "sound.h"

extern void func_80044DC0(s32);

void func_80044DA0(void);

void func_80044D48(void)
{
    SDValue *base;

    func_80044DA0();
    func_80044DC0(255);
    base = g_SDValue;
    base->buffer_ptrs_153C[0] = base->buffer_053C[0];
    base->buffer_ptrs_153C[1] = base->buffer_053C[1];
    base->buffer_ptrs_153C[2] = base->buffer_053C[2];
    base->field_0512 = 0;
    base->buffer_ptrs_153C[3] = base->buffer_053C[3];
}

void func_80044DA0(void)
{
    g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
    g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
}

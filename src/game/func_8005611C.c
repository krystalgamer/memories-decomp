#include "../types.h"
#include "model.h"

void func_8005B5FC(s32 *destination, s32 value, u32 count);
void Model_InitLightTriplet(s32 index);

void func_8005611C(s32 arg0)
{
    u8 *p;
    u8 *q;
    s32 i;
    s32 n;

    p = (u8 *)D_800F2C40 + arg0 * MODEL_SLOT_SIZE;
    func_8005B5FC((s32 *)p, 0, 0x388);

    *(s32 *)(p + 0xDA8) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDA4) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDA0) = MODEL_FIXED_HALF;
    *(s32 *)(p + 0xDB8) = MODEL_FIXED_ONE;
    *(s32 *)(p + 0xDB4) = MODEL_FIXED_ONE;
    *(s32 *)(p + 0xDB0) = MODEL_FIXED_ONE;
    p[0xDC2] = 0x80;
    p[0xDC1] = 0x80;
    p[0xDC0] = 0x80;
    p[0xE0C] = 7;
    p[0xE0D] = 8;
    p[0xE14] = 0xFF;
    p[0xDC3] = 0;
    *(s16 *)(p + 0xE0A) = 0x1000;
    p[0xE1D] = 0;
    p[0xDFE] = arg0;
    p[0xDFF] = 0;
    *(u16 *)(p + 0xDFC) = 0xFFFF;
    *(u16 *)(p + 0xDFA) = 0xFFFF;

    n = 1;
    i = 3;
    q = p + i;
    for (; i >= 0; q--, i--) {
        q[0xBF4] = n;
    }

    for (i = 0; i < 0x40; i++) {
        *(s32 *)(p + i * 4 + 0xBF8) &= 0x8000FFFF;
        p[i * 4 + 0xBF9] = 0;
        p[i * 4 + 0xBF8] = 0;
    }

    *(s32 *)(p + 0xD08) = -1;
    *(s32 *)(p + 0xD0C) = -1;
    *(s32 *)(p + 0xD10) = -1;
    p[0xE16] = 0x3E;
    p[0xE1F] = 0;
    Model_InitLightTriplet(arg0);
}

#include "../types.h"

typedef struct {
    u16 field_00;
    u16 field_02;
    u16 field_04;
    u16 field_06;
    u16 field_08;
    u16 field_0A;
    u16 field_0C;
    u16 field_0E;
    u16 field_10;
    s16 field_12;
    s32 field_14;
    s32 field_18;
} ModelTextureParams;

extern int func_800598E4();
extern int LoadImage2();
extern int IsIdleGPU(int);
extern int GsGetTimInfo();

u32 func_80058A7C(int side, int mode, ModelTextureParams *params)
{
    register int offset;
    u32 high;
    u32 low;

    side &= 1;
    offset = side << 8;
    params->field_04 -= 0x280;
    params->field_04 += offset;
    if (params->field_12 < 0x100) {
        params->field_10 -= 0x80;
        if (params->field_12 == 8) {
            params->field_12 = side + 0xF2;
        }
    } else {
        params->field_10 -= 0x280;
        params->field_10 += offset;
    }
    high = ((params->field_00 & 3) << 7) | ((mode & 3) << 5) |
           (((params->field_06 & 0x100) << 16) >> 20) |
           ((params->field_04 & 0x3FF) >> 6) |
           ((params->field_06 & 0x200) << 2);
    low = (*(u16 *)&params->field_12 << 6) |
          ((params->field_10 >> 4) & 0x3F);
    return (high << 16) | (low & 0xFFFF);
}

s32 func_80058B4C(u8 *data, s32 arg1, s32 mode, s32 arg3, s32 x, s32 y,
                  s32 z, s32 w)
{
    u16 bounds[4];
    s32 high;
    s32 low;

    high = 0;
    low = high;

    if (func_800598E4(arg1, 0x80400000) > 0) {
        GsGetTimInfo(0x80400004, data);
        if (mode < 2) {
            if (*(s32 *)(data + 4) == 0) {
                *(s16 *)(data + 4) = 0xC0;
                *(s16 *)(data + 6) = 0x100;
            }
            if (*(s32 *)(data + 0x10) == 0) {
                *(s16 *)(data + 0x10) = 0x200;
                *(s16 *)(data + 0x12) = 0xF2;
            }
            if (x >= 0) {
                *(s16 *)(data + 4) = x;
            } else {
                *(u16 *)(data + 4) =
                    *(u16 *)(data + 4) + ((mode & 1) << 8);
            }
            if (y >= 0) {
                *(s16 *)(data + 6) = y;
            }
            if (z >= 0) {
                *(s16 *)(data + 0x10) = z;
            }
            if (w >= 0) {
                *(s16 *)(data + 0x12) = w;
            } else {
                *(u16 *)(data + 0x12) = *(u16 *)(data + 0x12) + mode;
            }
        } else if (mode >= 3) {
            func_80058A7C(mode - 3, arg3, (ModelTextureParams *)data);
        }

        bounds[0] = *(u16 *)(data + 4);
        bounds[1] = *(u16 *)(data + 6);
        bounds[2] = *(u16 *)(data + 8);
        bounds[3] = *(u16 *)(data + 0xA);
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(bounds, *(s32 *)(data + 0xC)) != 0) {
        }
        bounds[0] = *(u16 *)(data + 0x10);
        bounds[1] = *(u16 *)(data + 0x12);
        bounds[2] = *(u16 *)(data + 0x14);
        bounds[3] = *(u16 *)(data + 0x16);
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(bounds, *(s32 *)(data + 0x18)) != 0) {
        }
        while (IsIdleGPU(3) != 0) {
        }
        high = ((*(u16 *)data & 3) << 7) | ((arg3 & 3) << 5) |
               (((s32)(*(u16 *)(data + 6) & 0x100) << 16) >> 20) |
               ((*(u16 *)(data + 4) & 0x3FF) >> 6) |
               ((*(u16 *)(data + 6) & 0x200) << 2);
        low = (*(u16 *)(data + 0x12) << 6) |
              ((*(u16 *)(data + 0x10) >> 4) & 0x3F);
    }

    return (high << 16) | (low & 0xFFFF);
}

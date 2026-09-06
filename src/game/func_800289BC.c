#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;
extern u8 D_800EA0E8[];

void func_800289BC(u8 *p, s32 mode)
{
    u8 *e;
    RECT *rect;
    s32 b;
    s32 x;

    if (mode == 0) {
        *(s32 *)(p + 0x1C) = 0x3800;
        D_8009B0F4 &= 0xFFDCFFFF;
        *(s32 *)(p + 0xC) = D_8009B118;
        *(s32 *)(p + 8) = D_8009B118;
        p[0x46] = 1;
        return;
    }

    e = D_800EA0E8 + (*(s32 *)(p + 0x38) << 6);

    rect = (RECT *)(e + 8);
    b = D_8009B118;
    *(s16 *)(e + 0xC) = 0x33;
    *(s16 *)(e + 0xE) = 0x60;
    *(u16 *)(e + 8) = *(u16 *)(e + 0x28);
    *(u16 *)(e + 0xA) = *(u16 *)(e + 0x2A);
    LoadImage(rect, (u32 *)b);

    rect = (RECT *)(e + 0x10);
    x = *(u16 *)(e + 0x2C);
    b = D_8009B118 + 0x2640;
    *(s16 *)(e + 0x14) = 0x100;
    *(s16 *)(e + 0x16) = 1;
    *(u16 *)(e + 0x10) = x;
    *(u16 *)(e + 0x12) = *(u16 *)(e + 0x2E);
    LoadImage(rect, (u32 *)b);

    rect = (RECT *)(e + 0x18);
    x = *(u16 *)(e + 0x28);
    b = D_8009B118 + 0x2840;
    *(s16 *)(e + 0x1C) = 0x18;
    *(s16 *)(e + 0x1E) = 0xE;
    *(u16 *)(e + 0x18) = x;
    *(u16 *)(e + 0x1A) = *(u16 *)(e + 0x2A) + 0x60;
    LoadImage(rect, (u32 *)b);

    rect = (RECT *)(e + 0x20);
    b = D_8009B118 + 0x2AE0;
    *(s16 *)(e + 0x24) = 8;
    *(s16 *)(e + 0x26) = 0x58;
    *(u16 *)(e + 0x20) = *(u16 *)(e + 0x28) + 0x38;
    *(u16 *)(e + 0x22) = *(u16 *)(e + 0x2A);
    LoadImage(rect, (u32 *)b);
}

#include "../types.h"

extern u32 D_8009B09C[];
extern u8 *D_8009B2FC;
extern void (*D_80090DF8[])(u8 *);

extern s32 rand(void);
extern s32 DuelEffect_UpdateState(void);

s32 func_80033BE8(void)
{
    s32 intensity;
    s32 color;
    u8 *base;
    u8 *first;
    u8 *second;

    rand();

    intensity = D_8009B09C[0] & 0x3F;
    if (intensity >= 0x20) {
        intensity = 0x3F - intensity;
    }

    base = D_8009B2FC;
    color = intensity * 2 + 0x40;
    first = *(u8 **)(base + 0x2D38);
    second = *(u8 **)(base + 0x5A84);

    second[0xE] = color;
    second[0xD] = color;
    second[0xC] = color;
    first[0xE] = color;
    first[0xD] = color;
    first[0xC] = color;

    if (DuelEffect_UpdateState() == 0) {
        D_80090DF8[*(u16 *)(D_8009B2FC + 0x633E) & 0x3F](D_8009B2FC);
    }

    return *(u16 *)(D_8009B2FC + 0x633E);
}

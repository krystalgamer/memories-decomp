#include "../types.h"

extern u8 D_8009B34E;
extern u8 D_8009B355;

extern s32 func_80036D3C(u8 *);

void func_800389D8(u8 *object)
{
    s32 offset = *(s8 *)(object + 0x58) * 4;
    s32 value;

    *(s32 *)(object + offset) += D_8009B34E * 2;
    value = func_80036D3C(object);
    offset = *(s8 *)(object + 0x58) * 4;
    *(s32 *)(object + offset) =
        (*(s32 *)(object + offset) & 0xFFFF0000) | (value & 0xFFFF);
}

void func_80038A44(u8 *object)
{
    s32 offset = *(s8 *)(object + 0x58) * 4;
    s32 value;

    *(s32 *)(object + offset) += D_8009B355 * 2;
    value = func_80036D3C(object);
    offset = *(s8 *)(object + 0x58) * 4;
    *(s32 *)(object + offset) =
        (*(s32 *)(object + offset) & 0xFFFF0000) | (value & 0xFFFF);
}

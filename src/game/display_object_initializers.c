#include "../types.h"

void func_800427DC(u8 *object, int value)
{
    u16 flags = *(u16 *)(object + 8);

    *(u32 *)(object + 0x54) = 0;
    *(u32 *)(object + 0x4C) = 0;
    *(u32 *)(object + 0x44) = 0;
    *(u32 *)(object + 0x3C) = 0;
    *(u32 *)(object + 0x34) = 0;
    *(u32 *)(object + 0x2C) = 0;
    *(u32 *)(object + 0x10) = 0;
    object[0x21] = 0;
    object[0x20] = 0;
    object[0x22] = 0;
    *(u16 *)(object + 0x1C) = 0;
    *(u16 *)(object + 0x1A) = 0;
    *(u16 *)(object + 0x18) = 0;
    object[0x5A] = value;
    *(u16 *)(object + 8) = flags | 8;
}

void func_80042824(u8 *object, int value)
{
    u32 initial = 0x00808080;
    u16 flags = *(u16 *)(object + 8);

    *(u32 *)(object + 0x68) = initial;
    *(u32 *)(object + 0x5C) = initial;
    *(u32 *)(object + 0x50) = initial;
    *(u32 *)(object + 0x44) = initial;
    *(u32 *)(object + 0x38) = initial;
    *(u32 *)(object + 0x2C) = initial;
    *(u32 *)(object + 0x10) = 0;
    object[0x21] = 0;
    object[0x20] = 0;
    object[0x22] = 0;
    *(u16 *)(object + 0x1C) = 0;
    *(u16 *)(object + 0x1A) = 0;
    *(u16 *)(object + 0x18) = 0;
    object[0x72] = value;
    *(u16 *)(object + 8) = flags | 8;
}

#include "../types.h"

extern void func_80036C14(void *, s32);

void func_80038024(u8 *object, s32 value)
{
    object[0x34] = object[0x34];
    *(u16 *)(object + 0x34) |= 0x80;
    func_80036C14(object, value);
    *(u16 *)(object + 0x34) &= 0xFF7F;
    *(u16 *)(object + 0x38) += 0x10;
}

extern u8 D_8009B344;

void func_80038070(void *object)
{
    func_80038024(object, D_8009B344);
}

void func_80038094(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];

    func_80038024(object, *(*stream)++);
}

void func_800380D4(u8 *object)
{
    register u8 **stream;
    register u8 *current;
    register u32 value;

    *(u16 *)(object + 0x38) = 0;
    stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    current = *stream;
    value = current[0];
    current++;
    *stream = current;
    *(u16 *)(object + 0x3A) += (s8)value;
}

void func_80038110(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    register u8 **slot = stream;
    register u8 *current = *slot;
    register u32 value = current[0];

    current++;
    *slot = current;
    *(u16 *)(object + 0x38) += value;
}

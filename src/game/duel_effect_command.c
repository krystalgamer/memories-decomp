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

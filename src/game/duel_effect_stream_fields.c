#include "../types.h"

extern s32 func_80036D3C(void *);

void func_80038334(u8 *object)
{
    register u8 **stream __asm__("$3");
    register u8 *current __asm__("$2");
    register u8 value __asm__("$5");

    stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    current = *stream;
    value = *current++;
    *stream = current;
    object[0x5A] = value;
    stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    current = *stream;
    value = *current++;
    *stream = current;
    object[0x5B] = value;
}

void func_80038388(u8 *object)
{
    *(u16 *)(object + 0x38) = func_80036D3C(object);
}

void func_800383B0(u8 *object)
{
    object[0x60] = 0;
    object[0x61] = func_80036D3C(object);
}

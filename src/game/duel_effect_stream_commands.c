#include "../types.h"

extern u16 D_8009B33A;
extern s32 D_8009B350;

void func_80037D2C(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    register s32 temporary asm("$2");
    register u32 combined asm("$3");
    register u32 value asm("$5");
    register u8 **slot asm("$4");

    temporary = *(s8 *)(object + 0x58);
    combined = D_8009B33A;
    asm("" : "+r"(temporary), "+r"(combined));
    slot = &((u8 **)object)[temporary];
    asm("" : "+r"(slot), "+r"(combined));
    combined -= 0xF0;
    temporary = (s32)*slot;
    combined <<= 8;
    value = *(u8 *)temporary;
    temporary++;
    *slot = (u8 *)temporary;
    temporary = -1;
    value |= combined;
    D_8009B33A = value;
    asm("" : "+r"(temporary) : : "memory");
    D_8009B350 = temporary;
}

void func_80037D6C(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    u8 *current = *stream;
    u8 value = current[0];

    *stream = current + 1;
    object[0x51] = value;
    D_8009B350 = 1;
}

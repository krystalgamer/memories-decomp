#include "../types.h"

extern signed char gDialog_bChoiceCount;
extern unsigned char D_8009B34C;
extern int D_8009B340;

void func_80037CE0(volatile unsigned char *object) {
    register unsigned short flags __asm__("$2");
    register unsigned char control __asm__("$3");
    if (object[0x56] >= gDialog_bChoiceCount) {
        object[0x51] = 1;
        flags = *(volatile unsigned short *)(object + 0x34);
        object[0x56] = 0;
        control = D_8009B34C;
        D_8009B340 = 0;
        flags &= 0xEFFF;
        control &= 0x30;
        *(unsigned short *)(object + 0x34) = flags;
        if (control != 0) gDialog_bChoiceCount = 2;
    }
}

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

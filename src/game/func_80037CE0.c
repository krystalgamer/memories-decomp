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

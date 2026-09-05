#include "../types.h"
#include "campaign_flags.h"

extern u8 D_801D0000[];

void Library_UpdateCardUsedFlag(s32 arg0)
{
    register s32 t asm("v0") = arg0 & CAMPAIGN_FLAG_ID_MASK;
    register s32 i asm("a2") = t >> 3;
    register s32 test asm("v0") = arg0 & 0x8000;
    register u8 *p asm("v1");
    register u8 v asm("v0");

    if (test) {
        register s32 bit2 asm("v1") = arg0 & 7;
        register s32 c asm("v0") = 0x80;
        register s32 mask asm("a0") = c >> bit2;

        p = &D_801D0000[i];
        v = p[0x618];
        p[0x618] = v & ~mask;
    } else {
        register s32 bit asm("a1") = arg0 & 7;
        register s32 c asm("a0") = 0x80;

        p = &D_801D0000[i];
        v = p[0x618];
        p[0x618] = v | (c >> bit);
    }
}

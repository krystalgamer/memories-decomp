#include "../types.h"
#include "campaign_flags.h"

extern u8 D_801D0000[];

void Library_UpdateCardUsedFlag(s32 arg0)
{
    register s32 t asm("v0") = arg0 & CAMPAIGN_FLAG_ID_MASK;
    register s32 i asm("a2") = t >> CAMPAIGN_FLAG_BYTE_SHIFT;
    register s32 test asm("v0") = arg0 & CAMPAIGN_FLAG_CLEAR_MODIFIER;
    register u8 *p asm("v1");
    register u8 v asm("v0");

    if (test) {
        register s32 bit2 asm("v1") = arg0 & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        register s32 c asm("v0") = CAMPAIGN_FLAG_BYTE_HIGH_BIT;
        register s32 mask asm("a0") = c >> bit2;

        p = &D_801D0000[i];
        v = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = v & ~mask;
    } else {
        register s32 bit asm("a1") = arg0 & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        register s32 c asm("a0") = CAMPAIGN_FLAG_BYTE_HIGH_BIT;

        p = &D_801D0000[i];
        v = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = v | (c >> bit);
    }
}

#include "../types.h"
#include "campaign_flags.h"
#include "save_data.h"

void Library_UpdateCardUsedFlag(s32 flag)
{
    s32 masked = flag & CAMPAIGN_FLAG_ID_MASK;
    s32 index = masked >> CAMPAIGN_FLAG_BYTE_SHIFT;
    s32 clear;

    clear = flag & CAMPAIGN_FLAG_CLEAR_MODIFIER;
    if (clear) {
        s32 bit = flag & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        s32 high = CAMPAIGN_FLAG_BYTE_HIGH_BIT;
        u8 *p;
        u8 value;

        flag = high >> bit;
        p = &D_801D0000[index];
        value = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = value & ~flag;
    } else {
        s32 bit = flag & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        s32 high = CAMPAIGN_FLAG_BYTE_HIGH_BIT;
        u8 *p;
        u8 value;

        p = &D_801D0000[index];
        value = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = value | (high >> bit);
    }
}

#include "../types.h"
#include "campaign_flags.h"

extern u8 D_801D0000[];

/* A normal query returns the bit mask, not a normalized Boolean. */
s32 Campaign_TestStoryFlag(s32 arg0)
{
    s32 i = (arg0 & CAMPAIGN_FLAG_ID_MASK) >> CAMPAIGN_FLAG_BYTE_SHIFT;
    s32 m = CAMPAIGN_FLAG_BYTE_HIGH_BIT >> (arg0 & CAMPAIGN_FLAG_BIT_INDEX_MASK);
    u8 *p = &D_801D0000[i];
    s32 v = p[CAMPAIGN_FLAG_BANK_OFFSET] & m;

    if (arg0 & CAMPAIGN_FLAG_CLEAR_MODIFIER) {
        return v == 0;
    }
    return v;
}

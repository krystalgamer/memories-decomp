#include "../types.h"
#include "campaign_flags.h"
#include "save_data.h"

/* A normal query returns the bit mask, not a normalized Boolean. */
s32 Campaign_TestStoryFlag(s32 arg0)
{
    s32 i = (arg0 & CAMPAIGN_FLAG_ID_MASK) >> CAMPAIGN_FLAG_BYTE_SHIFT;
    s32 m = CAMPAIGN_FLAG_BYTE_HIGH_BIT >> (arg0 & CAMPAIGN_FLAG_BIT_INDEX_MASK);
    s32 v = ((SaveDataWorkspace *)D_801D0000)->state.campaign_flags[i] & m;

    if (arg0 & CAMPAIGN_FLAG_CLEAR_MODIFIER) {
        return v == 0;
    }
    return v;
}

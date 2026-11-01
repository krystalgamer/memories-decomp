#include "../types.h"
#include "campaign_flags.h"
#include "save_data.h"
#include "save_data_workspace.h"

/* A normal query returns the bit mask, not a normalized Boolean. */
s32 Campaign_TestStoryFlag(s32 arg0)
{
    s32 i = (arg0 & CAMPAIGN_FLAG_ID_MASK) >> CAMPAIGN_FLAG_BYTE_SHIFT;
    s32 m = CAMPAIGN_FLAG_BYTE_HIGH_BIT >> (arg0 & CAMPAIGN_FLAG_BIT_INDEX_MASK);
    s32 v = ((SaveDataWorkspace *)gSaveData_Workspace)->state.campaign_flags[i] & m;

    if (arg0 & CAMPAIGN_FLAG_CLEAR_MODIFIER) {
        return v == 0;
    }
    return v;
}

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
        p = &gSaveData_Workspace[index];
        value = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = value & ~flag;
    } else {
        s32 bit = flag & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        s32 high = CAMPAIGN_FLAG_BYTE_HIGH_BIT;
        u8 *p;
        u8 value;

        p = &gSaveData_Workspace[index];
        value = p[CAMPAIGN_FLAG_BANK_OFFSET];
        p[CAMPAIGN_FLAG_BANK_OFFSET] = value | (high >> bit);
    }
}

/* Apply the requested set/clear state only if needed; return the prior test result. */
s32 Campaign_EnsureStoryFlag(s32 value)
{
    s32 result = Campaign_TestStoryFlag(value);

    if (result == 0)
        Library_UpdateCardUsedFlag(value);
    return result;
}

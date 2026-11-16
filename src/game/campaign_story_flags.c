#include "../types.h"
#include "campaign_flags.h"
#include "mem_card.h"
#include "duel_effect_object_pool.h"

/* A normal query returns the bit mask, not a normalized Boolean. */
s32 Campaign_TestStoryFlag(s32 flag)
{
    s32 i = (flag & CAMPAIGN_FLAG_ID_MASK) >> CAMPAIGN_FLAG_BYTE_SHIFT;
    s32 m = CAMPAIGN_FLAG_BYTE_HIGH_BIT >> (flag & CAMPAIGN_FLAG_BIT_INDEX_MASK);
    s32 v = gSaveData_Workspace.state.campaign_flags[i] & m;

    if (flag & CAMPAIGN_FLAG_CLEAR_MODIFIER) {
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
        SaveDataWorkspace *workspace;
        u8 value;

        flag = high >> bit;
        workspace = &gSaveData_Workspace;
        value = workspace->state.campaign_flags[index];
        workspace->state.campaign_flags[index] = value & ~flag;
    } else {
        s32 bit = flag & CAMPAIGN_FLAG_BIT_INDEX_MASK;
        s32 high = CAMPAIGN_FLAG_BYTE_HIGH_BIT;
        SaveDataWorkspace *workspace;
        u8 value;

        workspace = &gSaveData_Workspace;
        value = workspace->state.campaign_flags[index];
        workspace->state.campaign_flags[index] = value | (high >> bit);
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

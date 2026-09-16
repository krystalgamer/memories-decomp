#include "../types.h"
#include "campaign_flags.h"

/* Apply the requested set/clear state only if needed; return the prior test result. */
s32 Campaign_EnsureStoryFlag(s32 value)
{
    s32 result = Campaign_TestStoryFlag(value);

    if (result == 0)
        Library_UpdateCardUsedFlag(value);
    return result;
}

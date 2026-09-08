#include "../types.h"
#include "campaign_flags.h"

extern int Campaign_TestStoryFlag(int);
/* Apply the requested set/clear state only if needed; return the prior test result. */
int func_8002CD48(int value) {
    int result = Campaign_TestStoryFlag(value);
    if (result == 0) Library_UpdateCardUsedFlag(value);
    return result;
}

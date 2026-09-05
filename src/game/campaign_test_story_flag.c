#include "../types.h"
#include "campaign_flags.h"

extern u8 D_801D0000[];

s32 Campaign_TestStoryFlag(s32 arg0)
{
    s32 i = (arg0 & CAMPAIGN_FLAG_ID_MASK) >> 3;
    s32 m = 0x80 >> (arg0 & 7);
    u8 *p = &D_801D0000[i];
    s32 v = p[0x618] & m;

    if (arg0 & 0x8000) {
        return v == 0;
    }
    return v;
}

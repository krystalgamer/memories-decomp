#include "../types.h"
#include "mem_card.h"
#include "mem_card_begin_request.h"

extern char gMemCard_bRequestStep;

s32 MemCard_BeginRequest(s32 chan, s32 request)
{
    if (gMemCard_bRequest >= 0)
        return 0;
    gMemCard_bRetries = 10;
    gMemCard_bChannel = chan;
    gMemCard_bRequest = request;
    gMemCard_bRequestStep = 0;
    gMemCard_bLoadStep = 0;
    gMemCard_nIOResult = -1;
    return 1;
}

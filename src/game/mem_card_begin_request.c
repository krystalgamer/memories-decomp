#include "../types.h"
#include "mem_card.h"
#include "mem_card_begin_request.h"

extern char D_8009B44F;

s32 MemCard_BeginRequest(s32 chan, s32 request)
{
    if (D_8009B43E >= 0)
        return 0;
    D_8009B43C = 10;
    D_8009B437 = chan;
    D_8009B43E = request;
    D_8009B44F = 0;
    D_8009B43D = 0;
    gMemCard_nIOResult = -1;
    return 1;
}

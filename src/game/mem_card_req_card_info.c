#include "../types.h"
#include "mem_card.h"
#include "../psyq/libapi.h"
#include "mem_card_begin_request.h"
#include "io_event_helpers.h"

int MemCard_ReqCardInfo(int chan)
{
    int result;
    if (MemCard_BeginRequest(chan, 1)) {
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "../types.h"
#include "mem_card_directory.h"
#include "mem_card.h"
#include "../psyq/libapi.h"
#include "io_event_helpers.h"
#include "mem_card_io_result_callbacks.h"

void MemCard_InitIOEvents(void)
{
    register long *items;
    register long (*cb0)(void);
    register long (*cb1)(void);
    register long (*cb2)(void);
    int count;
    {
        register long *base = gMemCard_aIOEventHandles;
        gMemCard_bRequest = -1;
        gMemCard_bDirFlags = 0;
        gMemCard_pDirEntries = 0;
        items = gMemCard_aIOEventHandles;
        EnterCriticalSection();
        cb0 = MemCard_SetIOResultCompleteCB;
        base[0] = OpenEvent(SwCARD, EvSpIOE, EvMdINTR, cb0);
        cb1 = MemCard_SetIOResultTimeoutCB;
        items[1] = OpenEvent(SwCARD, EvSpTIMOUT, EvMdINTR, cb1);
    }
    cb2 = MemCard_SetIOResultErrorCB;
    items[2] = OpenEvent(SwCARD, EvSpERROR, EvMdINTR, cb2);
    {
        register long (*cb3)(void) = MemCard_SetIOResultNewCardCB;
        items[3] = OpenEvent(SwCARD, EvSpNEW, EvMdINTR, cb3);
        items[4] = OpenEvent(HwCARD, EvSpIOE, EvMdINTR, cb0);
        items[5] = OpenEvent(HwCARD, EvSpTIMOUT, EvMdINTR, cb1);
        items[6] = OpenEvent(HwCARD, EvSpERROR, EvMdINTR, cb2);
        items[7] = OpenEvent(HwCARD, EvSpNEW, EvMdINTR, cb3);
    }
    count = 8;
    do {
        EnableEvent(*items++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

void MemCard_ClearCard(int chan)
{
    int count = 10;

    do {
        MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
        _card_clear(chan);
        while (gMemCard_nIOResult < 0) {
        }
        if (gMemCard_nIOResult != 1)
            break;
        count--;
    } while (count > 0);
}

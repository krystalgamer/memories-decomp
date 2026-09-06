#include "../types.h"
#include "../psyq/libapi.h"

extern signed char D_8009B43E;
extern u8 D_8009B44E;
extern int D_8009B444;
extern volatile int gMemCard_nIOResult;
extern long gMemCard_aIOEventHandles[];
extern long D_800F2AF0[];
extern void func_80043D48(long *);
extern long MemCard_SetIOResultCompleteCB(void);
extern long MemCard_SetIOResultTimeoutCB(void);
extern long MemCard_SetIOResultErrorCB(void);
extern long MemCard_SetIOResultNewCardCB(void);

void MemCard_InitIOEvents(void)
{
    register long *items;
    register long (*cb0)(void);
    register long (*cb1)(void);
    register long (*cb2)(void);
    int count;
    {
        register long *base = gMemCard_aIOEventHandles;
        D_8009B43E = -1;
        D_8009B44E = 0;
        D_8009B444 = 0;
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

void func_80044038(int value)
{
    int count = 10;

    do {
        func_80043D48(D_800F2AF0);
        _card_clear(value);
        while (gMemCard_nIOResult < 0) {
        }
        if (gMemCard_nIOResult != 1)
            break;
        count--;
    } while (count > 0);
}

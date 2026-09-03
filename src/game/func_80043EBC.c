#include "../types.h"

extern signed char D_8009B43E;
extern u8 D_8009B44E;
extern int D_8009B444;
extern void *D_800F2AE0[];
extern void func_800738B0(void);
extern void *func_80073860(int, int, int, void *);
extern void func_80073890(void *);
extern void func_800738C0(void);
extern void MemCard_SetIOResultCompleteCB(void);
extern void MemCard_SetIOResultErrorCB(void);
extern void MemCard_SetIOResultTimeoutCB(void);
extern void MemCard_SetIOResultNewCardCB(void);

void func_80043EBC(void)
{
    register void **items;
    register void *cb0;
    register void *cb1;
    register void *cb2;
    int count;
    {
        register void **base = D_800F2AE0;
        D_8009B43E = -1;
        D_8009B44E = 0;
        D_8009B444 = 0;
        items = D_800F2AE0;
        func_800738B0();
        cb0 = MemCard_SetIOResultCompleteCB;
        base[0] = func_80073860(0xF4000001, 4, 0x1000, cb0);
        cb1 = MemCard_SetIOResultErrorCB;
        items[1] = func_80073860(0xF4000001, 0x100, 0x1000, cb1);
    }
    cb2 = MemCard_SetIOResultTimeoutCB;
    items[2] = func_80073860(0xF4000001, 0x8000, 0x1000, cb2);
    {
        register void *cb3 = MemCard_SetIOResultNewCardCB;
        items[3] = func_80073860(0xF4000001, 0x2000, 0x1000, cb3);
        items[4] = func_80073860(0xF0000011, 4, 0x1000, cb0);
        items[5] = func_80073860(0xF0000011, 0x100, 0x1000, cb1);
        items[6] = func_80073860(0xF0000011, 0x8000, 0x1000, cb2);
        items[7] = func_80073860(0xF0000011, 0x2000, 0x1000, cb3);
    }
    count = 8;
    do {
        func_80073890(*items++);
        count--;
    } while (count != 0);
    func_800738C0();
}

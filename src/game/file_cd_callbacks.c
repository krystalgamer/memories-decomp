#include "../types.h"

extern char D_8009B104[1];
extern volatile s32 D_8009B0F4;
extern u16 D_8009B100;
extern u8 D_8009B114;
extern s32 D_8009B130;
extern s32 D_8009B134;
extern s32 D_8009B138;
extern u8 D_800E9EA7[9];

extern void func_80013C28(void);
extern void func_8007B1F4(s32, void *, void *, s32);
extern void func_8007B468();
extern void func_8007DD50(void *, s32);
extern void func_8007DE38(s32);

void func_800140A0(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        func_8007B468(0xA0, D_8009B104, 6, func_800140A0, -1);
    } else if (event == 2) {
        func_8007DE38(1);
        func_8007DD50(func_80013C28, -1);
        D_8009B114 = 0;
        D_8009B138 = 0;
        D_8009B0F4 &= ~0x400;
    }
}

void func_80014134(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        func_8007B468(0xA0, D_8009B104, 0x15, func_80014134, -1);
    } else if (event == 2) {
        D_8009B0F4 &= ~0x400;
    }
}

void func_800141A8(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        func_8007B1F4(9, 0, func_800141A8, -1);
    } else if (event == 2) {
        D_800E9EA7[0] = 1;
        D_8009B0F4 &= ~0x400;
    }
}

void func_80014220(int event)
{
    event &= 0xFF;
    if (event == 5) {
        D_8009B130++;
        func_8007B1F4(9, 0, func_80014220, -1);
    } else if (event == 2) {
        __asm__ volatile(
            "sh $4, %%gp_rel(D_8009B100)($28)"
            : : : "memory"
        );
        D_8009B0F4 &= ~0x400;
    }
}

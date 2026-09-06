#include "../../types.h"

extern u8 D_801845C0[];
extern u8 D_801845BE;
extern u8 *D_801845D8;
extern void *D_801845A0;
extern void *D_801845A4;
extern void *D_801845B0[];
extern s32 D_800E9DB0;
extern void func_8004036C(void *);

void MainMenu_FinishValueSetup(void)
{
    u8 *state = D_801845C0;

    *(u16 *)(*(u8 **)(state + 4)) = *(u16 *)state;
    *(u16 *)(*(u8 **)(state + 0x10)) = *(u16 *)(state + 0xC);
    *D_801845D8 = (D_801845BE != 1);
    func_8004036C(D_801845A0);
    D_801845A0 = 0;
    func_8004036C(D_801845A4);
    D_801845A4 = 0;
    func_8004036C(D_801845B0[0]);
    D_801845B0[0] = 0;
    func_8004036C(D_801845B0[1]);
    D_801845B0[1] = 0;
    func_8004036C(D_801845B0[2]);
    D_801845B0[2] = 0;
    D_800E9DB0 = 0;
}

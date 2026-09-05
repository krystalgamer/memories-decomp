#include "../types.h"

extern u8 D_800EF668[];
extern s32 InitPAD(void *, s32, void *, s32);
extern s32 StartPAD(void);
extern void Input_ResetPads(void);
extern u8 D_8009B39C, D_8009B3A2;

void Input_InitPads(void)
{
    InitPAD(D_800EF668, 0x22, D_800EF668 + 0x22, 0x22);
    StartPAD();
    D_8009B39C = 0x18;
    D_8009B3A2 = 0x14;
    Input_ResetPads();
}

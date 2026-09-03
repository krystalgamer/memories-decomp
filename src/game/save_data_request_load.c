#include "../types.h"

extern u8 D_801D3200[];
extern u8 D_80010384[];
extern void func_8003F758(u8 *, s32, u8 *, s32);

void SaveData_RequestLoad(void)
{
    /* The symbolic store changes the target $at scheduling and relocation. */
    *(u8 *)0x8009B0D1 = 0;
    func_8003F758(D_801D3200, 0x680, D_80010384, 0);
}

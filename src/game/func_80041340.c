#include "../types.h"

extern s16 D_800EFE38[];
extern u8 D_80090FCC[];
extern u8 D_80090FDC[];
extern void (*D_80090FB0[])(void);
extern void func_80087870();
extern void func_80087890();
extern void func_800857E0();
extern void SetGeomScreen(long);
extern void SetGeomOffset(long, long);

void func_80041340(void)
{
    s32 i;
    func_80087870(96, 96, 96);
    func_80087890(0, 0, 0);
    func_800857E0(0, D_80090FCC);
    func_800857E0(1, D_80090FDC);
    func_800857E0(2, D_80090FDC);
    for (i = 6; i >= 0; i--) {
        if (D_800EFE38[i] >= 0) {
            SetGeomScreen(150);
            SetGeomOffset(0, 0);
            D_80090FB0[i]();
        }
    }
}

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

extern s16 D_800EFE38[];
extern u8 D_80090FCC[];
extern u8 D_80090FDC[];
extern void (*D_80090FB0[])(void);

void func_80041340(void)
{
    s32 i;
    SetBackColor(96, 96, 96);
    SetFarColor(0, 0, 0);
    GsSetFlatLight(0, (GsF_LIGHT *)D_80090FCC);
    GsSetFlatLight(1, (GsF_LIGHT *)D_80090FDC);
    GsSetFlatLight(2, (GsF_LIGHT *)D_80090FDC);
    for (i = 6; i >= 0; i--) {
        if (D_800EFE38[i] >= 0) {
            SetGeomScreen(150);
            SetGeomOffset(0, 0);
            D_80090FB0[i]();
        }
    }
}

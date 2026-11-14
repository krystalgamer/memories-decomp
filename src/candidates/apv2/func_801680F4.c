#include "../../types.h"
extern s32 D_80169474;
extern s32 D_801694D0;
void func_801680F4(void)
{
    switch (*(u8 *)0xBFC7FF52) { case 'A': D_80169474 = 1; D_801694D0 = 1; break; case 'E': D_80169474 = 0; D_801694D0 = 2; break; case 'J': D_80169474 = 1; D_801694D0 = 0; break; default: D_80169474 = 1; D_801694D0 = 0; break; }
}

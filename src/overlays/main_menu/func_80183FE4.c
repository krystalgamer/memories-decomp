#include "../../types.h"

extern void *D_801845DC;
extern void *D_801845E0;
extern s32 D_800E9DB4;
extern void func_8004036C(void *);

void func_80183FE4(void)
{
    func_8004036C(D_801845DC);
    D_801845DC = 0;
    func_8004036C(D_801845E0);
    D_801845E0 = 0;
    D_800E9DB4 = 0;
}

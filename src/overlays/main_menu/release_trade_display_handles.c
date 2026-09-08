#include "../../types.h"
#include "entrypoints.h"
#include "../../game/display_object_api.h"

extern void *D_801845DC;
extern void *D_801845E0;
extern s32 D_800E9DB4;

void MainMenu_ReleaseTradeDisplayHandles(void)
{
    func_8004036C(D_801845DC);
    D_801845DC = 0;
    func_8004036C(D_801845E0);
    D_801845E0 = 0;
    D_800E9DB4 = 0;
}

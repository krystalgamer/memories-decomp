#include "../../types.h"

extern void *D_801695F8[];
extern void func_8004036C(void *);

void CampaignMap_ClearLocationObjects(void)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        func_8004036C(D_801695F8[i]);
        D_801695F8[i] = 0;
    }
}

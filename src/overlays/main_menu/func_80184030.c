#include "../../types.h"

extern u16 D_80185C9C[][11];
extern void func_801840F8(s32, s32, s32);

void func_80184030(s32 slot, s32 amount)
{
    s32 i;

    i = 0;
    while (i < D_80185C9C[slot][0]) {
        func_801840F8(slot, D_80185C9C[slot][i + 1], amount);
        i++;
    }
}

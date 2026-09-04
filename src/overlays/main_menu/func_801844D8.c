#include "../../types.h"

extern u8 *D_801845E0;
extern u8 D_80185C8C[];
extern u8 D_801845FC[];
extern void func_80060E70(void *, s32, s32, s32);

void func_801844D8(s32 index)
{
    s32 flags;

    flags = D_801845E0[0x69] - 4;
    func_80060E70(index * 2888 + (*(u16 *)(D_80185C8C + index * 4) * 4 + D_801845FC),
                  index, flags & (1 << index), flags);
}

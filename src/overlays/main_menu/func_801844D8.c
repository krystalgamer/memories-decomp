#include "../../types.h"

typedef struct {
    s16 id;
    u16 count;
} MainMenuCardCount;

extern MainMenuCardCount D_801845FC[];
extern u16 D_80185C8C[][2];
extern u8 *D_801845E0;

extern void func_80060E70(void *, s32, s32, s32);

void func_801844D8(s32 index)
{
    s32 flags;

    flags = D_801845E0[0x69] - 4;
    func_80060E70((void *)(index * 2888 + (s32)D_801845FC + D_80185C8C[index][0] * 4), index,
                  flags & (1 << index), flags);
}

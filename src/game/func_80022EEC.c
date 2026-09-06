#include "../types.h"
#include "duel_selection_layout.h"

extern u8 D_800E9F10[];
extern u8 D_8009B1D5;
extern void func_8004036C();

void func_80022EEC(u8 *arg0)
{
    u8 *base = D_800E9F10;
    s32 offset = *(s16 *)(arg0 + 0x2C) * DUEL_SELECTION_RECORD_SIZE;
    u8 *parent;
    offset += D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
    parent = *(u8 **)(base + offset);
    if (parent == 0) {
        func_8004036C(arg0);
    } else {
        *(u16 *)(arg0 + 0x30) =
            *(u16 *)(parent + 0x30) + *(u16 *)(arg0 + 0x28);
        *(u16 *)(arg0 + 0x32) =
            *(u16 *)(parent + 0x32) + *(u16 *)(arg0 + 0x2A);
        if (parent[0x6C] == 0) {
            arg0[0x6C] = 0;
            *(s32 *)(arg0 + 0x24) = 0;
        }
    }
}

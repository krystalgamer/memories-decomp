#include "../types.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"

extern u8 D_800E9F10[];
void func_80017708(void) {
    s32 row, j;
    for (row = 0; row < DUEL_SIDE_COUNT; row++) {
        u8 *p = D_800E9F10 + row * DUEL_SELECTION_SIDE_SIZE;
        for (
            j = 0;
            j < DUEL_SELECTION_RECORDS_PER_SIDE;
            p += DUEL_SELECTION_RECORD_SIZE, j++
        ) {
            *(u32 *)(p + 0x00) = 0;
            *(u32 *)(p + 0x04) = 0;
            *(u32 *)(p + 0x08) = 0;
            p[0x18] = 0;
            p[0x13] = 1;
            p[0x17] = j;
            p[0x14] = (j != 3) ? j : 1;
        }
    }
    D_800E9F10[0x13] = 0;
    D_800E9F10[DUEL_SELECTION_SIDE_SIZE + 0x13] = 0;
}

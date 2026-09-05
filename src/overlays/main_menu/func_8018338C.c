#include "../../types.h"

typedef struct {
    s16 id;
    u16 count;
} MainMenuCard;

typedef struct {
    s32 entries[6];
} MainMenuComparators;

typedef struct {
    u8 *object;
    s32 pad[5];
} MainMenuState;

extern s32 D_80180000[];
extern u8 D_801D1200[];
extern MainMenuCard D_801845FC[][722];
extern u8 D_80185CCC[];
extern MainMenuState D_801A8000[];
extern void func_80184030(s32, s32);
extern void func_8008E400(void *, s32, s32, s32);

void func_8018338C(s32 slot, s32 force)
{
    MainMenuComparators comparators;
    u8 *row;
    u8 *counts;
    s32 i;
    s32 mode;
    s32 id;

    mode = D_80185CCC[slot];
    comparators = *(MainMenuComparators *)&D_80180000[1];

    if (force != 0 || mode == 0) {
        row = D_801D1200 + slot * 0x1000;
        counts = row + 0x50;
        for (i = 0; i < 0x2D2; i++) {
            if (counts[i] != 0) {
                id = i + 1;
            } else {
                id = 0;
            }
            D_801845FC[slot][i].id = id;
            D_801845FC[slot][i].count = counts[i];
        }
        for (i = 0; i < 2; i++) {
            func_80184030(i, -1);
        }
    }

    if (mode != 0) {
        func_8008E400(D_801845FC[slot], 0x2D2, 4, comparators.entries[mode - 1]);
    }

    D_801A8000[slot].object[0x69] = mode;
}

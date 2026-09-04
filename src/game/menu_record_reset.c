#include "../types.h"

typedef struct {
    s32 grid[4][3];
    s8 field_30;
    u8 pad_31;
    u8 field_32;
    u8 pad_33[7];
    u8 field_3A;
    u8 field_3B;
    u8 pad_3C[0x10];
} MenuRecord;

extern MenuRecord D_800EB010[];
extern s8 D_8015C410[5];

s8 *func_80039E9C(void)
{
    s32 i;
    s32 column;
    s32 empty;
    s8 *entry;

    for (i = 2; i >= 0; i--) {
        MenuRecord *record = &D_800EB010[i];

        record->field_30 = -1;
        record->field_32 = 0;
        record->field_3A = 0;
        record->field_3B = 0;
        for (column = 2; column >= 0; column--) {
            record->grid[0][column] = 0;
            record->grid[1][column] = 0;
            record->grid[2][column] = 0;
            record->grid[3][column] = 0;
        }
    }

    empty = -1;
    i = 4;
    entry = D_8015C410 + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
}

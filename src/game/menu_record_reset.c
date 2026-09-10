#include "../types.h"
#include "func_8003A560.h"
#include "menu_record.h"
#include "menu_record_reset.h"

extern s8 D_8015C410[DISPLAY_EFFECT_VRAM_SLOT_COUNT];

s8 *func_80039E9C(void)
{
    s32 i;
    s32 column;
    s32 empty;
    s8 *entry;

    for (i = MENU_RECORD_COUNT - 1; i >= 0; i--) {
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
    i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1;
    entry = D_8015C410 + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
}

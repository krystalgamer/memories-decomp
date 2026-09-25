#include "../types.h"
#include "func_8003A560.h"
#include "menu_record.h"
#include "menu_record_reset.h"
#ifdef VERSION_JAPAN
#include "high_memory_addresses.h"
#endif

s8 *func_80039E9C(void)
{
    s32 i;
    s32 column;
#ifdef VERSION_JAPAN
    s32 has_more;
#else
    s32 empty;
    s8 *entry;
#endif

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

#ifdef VERSION_JAPAN
    i = 0;
    do {
        ((s8 *)D_80010000)[0x7FFF0 + i] = -1;
        i++;
        has_more = i < DISPLAY_EFFECT_VRAM_SLOT_COUNT;
    } while (has_more);
    /* The final false loop predicate is the Japanese null return. */
    return (s8 *)has_more;
#else
    empty = -1;
    i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1;
    entry = D_8015C410 + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
#endif
}

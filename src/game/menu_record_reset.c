#include "../types.h"
#include "display_effect_update_resource_acquisition.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "menu_record_reset.h"
#ifdef VERSION_JAPAN
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#endif

s8 *DisplayEffect_ResetMenuRecords(void)
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
        MenuRecord *record = &gDisplayEffect_aMenuRecords[i];

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
        ((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + i] = -1;
        i++;
        has_more = i < DISPLAY_EFFECT_VRAM_SLOT_COUNT;
    } while (has_more);
    /* The final false loop predicate is the Japanese null return. */
    return (s8 *)has_more;
#else
    empty = -1;
    i = DISPLAY_EFFECT_VRAM_SLOT_COUNT - 1;
    entry = gDisplayEffect_CachedResourceIds + i;
    do {
        *entry = empty;
        i--;
        entry--;
    } while (i >= 0);
    return entry;
#endif
}

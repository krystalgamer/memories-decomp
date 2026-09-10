#include "../types.h"
#include "func_8003B378.h"
#include "display_effect_process_menu_records.h"
#include "menu_record.h"
#include "display_effect_step_table.h"

void DisplayEffect_ProcessMenuRecords(s32 value) {
    MenuRecord *record = D_800EB010;
    s32 i = MENU_RECORD_COUNT - 1;
    void (**t)(u8 *) = D_80090F68;

    do {
        if (record->field_30 >= 0) {
            if (record->field_32 & 0x40) {
                func_8003B378((u8 *)record, value);
            }

            if (record->display_effect_step != 0) {
                t[record->display_effect_step & 0x1F]((u8 *)record);
            }
        }
        record++;
        i--;
    } while (i >= 0);
}

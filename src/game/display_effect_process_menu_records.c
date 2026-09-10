#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_config.h"
#include "func_8003B378.h"
#include "display_effect_process_menu_records.h"
#include "menu_record.h"
#include "display_effect_step_table.h"

void func_8003B378(MenuRecord *p, s32 n) {
    s32 f;
    s32 g;
    s32 c;
    s32 v;

    f = p->field_32;
    if ((f & 0x10) != 0) {
        p->field_32 = f & 0xFC;
        return;
    }

    if (*(u8 **)&p->grid[0][1] != 0) {
        if ((f & 1) != 0) {
            if (*(s16 *)(*(u8 **)&p->grid[0][1] + 0x5A) == 0) {
                *(s16 *)&p->field_3E =
                    (rand() & DISPLAY_EFFECT_DELAY_MASK) +
                    DISPLAY_EFFECT_DELAY_BASE;
                p->field_32 = p->field_32 & 0xFE;
                func_80040410(
                    (DisplayObjectConfig *)*(u8 **)&p->grid[0][1], 0);
            }
        } else {
            v = p->field_3E - 1;
            *(s16 *)&p->field_3E = v;
            if ((s16)v <= 0) {
                p->field_32 = p->field_32 | 1;
                func_80040410(
                    (DisplayObjectConfig *)*(u8 **)&p->grid[0][1], 1);
            }
        }
    }

    if (*(u8 **)&p->grid[0][2] == 0) {
        return;
    }

    g = p->field_32;
    if ((g & 2) != 0) {
        c = p->field_3B - 1;
        p->field_3B = c;
        if (*(s16 *)(*(u8 **)&p->grid[0][2] + 0x5A) != 0) {
            return;
        }
        if ((s8)c > 0) {
            return;
        }
        if (n <= 0) {
            p->field_3B = 0;
            p->field_32 = p->field_32 & 0xFD;
            return;
        }
    } else {
        if (n < 0) {
            goto slow;
        }
        p->field_32 = g | 2;
    }

    p->field_3B = 6;
    p->field_3A = n;
    func_80040410((DisplayObjectConfig *)*(u8 **)&p->grid[0][2], n);
    return;

slow:
    p->field_3B = p->field_3B + 1;
    if ((s8)p->field_3B < 6) {
        return;
    }
    func_80040424((DisplayObjectConfig *)*(u8 **)&p->grid[0][2], 0);
}

void DisplayEffect_ProcessMenuRecords(s32 value) {
    MenuRecord *record = D_800EB010;
    s32 i = MENU_RECORD_COUNT - 1;
    void (**t)(u8 *) = D_80090F68;

    do {
        if (record->field_30 >= 0) {
            if (record->field_32 & 0x40) {
                func_8003B378(record, value);
            }

            if (record->display_effect_step != 0) {
                t[record->display_effect_step & 0x1F]((u8 *)record);
            }
        }
        record++;
        i--;
    } while (i >= 0);
}

#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#define DEBUG_FONT_FORMATS_AS_U8_ARRAYS
#include "debug_font_format_data.h"
#include "duel_effect.h"
#include "debug_menu_update.h"
#include "func_8003B6AC.h"
#include "../psyq/rand.h"
#include "input.h"
#include "text_box_lifecycle.h"
#include "../unmatched.h"
#include "frontend_step_tables.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "debug_menu_update_cursor_layout.h"
#include "text_box_runtime.h"

void DebugMenu_Update(void) {
    DuelEffectChannel *boxes;
    s32 i;
    s8 d;
    s32 one;
    s32 k;
    s32 e;
    s32 t;

    rand();
    e = D_8009B2EB;
    i = 0;
    if (e != 0) {
        if (gDebugMenu_bPage != DEBUG_MENU_PAGE_PRIMARY) {
            gDebugMenu_apfnAlternatePageSteps[e & FRONTEND_STEP_INDEX_MASK]();
        } else {
            gDebugMenu_apfnPrimaryPageSteps[e & FRONTEND_STEP_INDEX_MASK]();
        }
        return;
    }
    for (; i < 0x16; i++) {
        FntPrint(D_8009AF54);
    }
    if ((gInput_wPad1Repeat & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Repeat & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                if (gDebugMenu_bCursor + DEBUG_MENU_COLUMN_ENTRY_COUNT <
                    DEBUG_MENU_ENTRY_COUNT) {
                    gDebugMenu_bCursor =
                        gDebugMenu_bCursor + DEBUG_MENU_COLUMN_ENTRY_COUNT;
                }
            } else {
                if (gDebugMenu_bCursor - DEBUG_MENU_COLUMN_ENTRY_COUNT >= 0) {
                    gDebugMenu_bCursor =
                        gDebugMenu_bCursor - DEBUG_MENU_COLUMN_ENTRY_COUNT;
                }
            }
        }
        if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
            t = (u8)gDebugMenu_bCursor;
            if (gDebugMenu_bCursor >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                d = t - 1;
                gDebugMenu_bCursor = d;
                if (d < DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_EXIT;
                }
            } else {
                d = t - 1;
                gDebugMenu_bCursor = d;
                if (d < 0) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_TITLE;
                }
            }
        }
        if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
            t = (u8)gDebugMenu_bCursor;
            if (gDebugMenu_bCursor >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                d = t + 1;
                gDebugMenu_bCursor = d;
                if (d >= DEBUG_MENU_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_NAME;
                }
            } else {
                d = t + 1;
                gDebugMenu_bCursor = d;
                if (d >= DEBUG_MENU_COLUMN_ENTRY_COUNT) {
                    gDebugMenu_bCursor = DEBUG_MENU_ENTRY_3D;
                }
            }
        }
        DebugMenu_UpdateCursorLayout();
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        k = DEBUG_MENU_ENTRY_EXIT;
        if (gDebugMenu_bCursor != k) {
            gDebugMenu_bCursor = k;
            DebugMenu_UpdateCursorLayout();
            return;
        }
        D_8009B2EB = 0x14;
        return;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_SELECT) != 0) {
        one = 1;
        func_8003B6AC(one, one);
        gDebugMenu_bPage = gDebugMenu_bPage ^ one;
        TextBox_Create(1, gDebugMenu_bPage + 0xF, 0x10, 0x10, 0x120, 0xA0);
        /* The array base has to stay in a local: writing `&D_800EB0F8[1]`
         * directly folds the record offset into the address computation and
         * drops the `addiu` retail keeps for the call argument. */
        boxes = D_800EB0F8;
        boxes[1].field_5A = 0x10;
        boxes[1].field_5B = 0x10;
        func_80039A14(&boxes[1]);
        return;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) != 0) {
        D_8009B2EB = gDebugMenu_bCursor + 1;
    }
}

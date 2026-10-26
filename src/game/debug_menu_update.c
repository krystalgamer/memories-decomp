/* The debug menu's per-frame update and its bust-up entry step, in address
   order. DebugMenu_Update runs the active step from the primary or alternate
   page table. Otherwise it prints the menu and handles the pad: the
   directions move the cursor, Select flips the page and rebuilds the second
   text box, and confirm selects the step under the cursor. DebugMenu_UpdateBustUpEntry is entry 6 of the primary page table;
   the empty func_8003134C before it (absent from the European image) is the
   per-frame call it makes. */

#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define DEBUG_FONT_FORMATS_AS_U8_ARRAYS

#include "../types.h"
#include "text_box_blocking_build.h"
#include "display_effect_update_menu_record.h"
#include "rand_get_interval.h"
#include "func_8003134C.h"
#include "menu_record.h"
#include "display_menu_records.h"
#include "display_object.h"
#include "display_object_brightness.h"
#include "display_object_layout.h"
#include "campaign_scene_package.h"
#include "frontend_debug_state.h"
#include "debug_menu_update.h"
#include "debug_font_format_data.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_set_entry_range_layout.h"
#include "../psyq/rand.h"
#include "input.h"
#include "text_box_lifecycle.h"
#include "frontend_step_tables.h"
#include "frontend_debug_tables.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "debug_menu_update_cursor_layout.h"
#include "text_box_runtime.h"
#include "../unmatched.h"

/* The second text box, and the cancel and confirm buttons. The Japanese
   channel record is 0x60 bytes and that release swaps Cross for Circle, so a
   regional build supplies its own. */
#ifndef DEBUG_MENU_SECOND_BOX
#define DEBUG_MENU_SECOND_BOX(boxes) (&(boxes)[1])
#endif
#ifndef DEBUG_MENU_CANCEL_BUTTON
#define DEBUG_MENU_CANCEL_BUTTON PAD_BUTTON_CANCEL
#define DEBUG_MENU_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void DebugMenu_Update(void) {
    DuelEffectChannel *boxes;
    s32 i;
    s8 d;
    s32 one;
    s32 k;
    s32 e;
    s32 t;

    rand();
    e = gDebugMenu_bStep;
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
        FntPrint(gDebugMenu_szLineBreak);
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
    if ((gInput_wPad1Pressed & DEBUG_MENU_CANCEL_BUTTON) != 0) {
        k = DEBUG_MENU_ENTRY_EXIT;
        if (gDebugMenu_bCursor != k) {
            gDebugMenu_bCursor = k;
            DebugMenu_UpdateCursorLayout();
            return;
        }
        gDebugMenu_bStep = 0x14;
        return;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_SELECT) != 0) {
        one = 1;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        /* The Japanese and European builds make neither this call nor the
           two field stores below. */
        func_8003B6AC(one, one);
#endif
        gDebugMenu_bPage = gDebugMenu_bPage ^ one;
        TextBox_Create(1, gDebugMenu_bPage + 0xF, 0x10, 0x10, 0x120, 0xA0);
        /* The array base has to stay in a local: writing `&gText_aChannels[1]`
         * directly folds the record offset into the address computation and
         * drops the `addiu` retail keeps for the call argument. */
        boxes = gText_aChannels;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
        boxes[1].glyph_advance_x = 0x10;
        boxes[1].line_advance_y = 0x10;
#endif
        TextBox_BuildBlocking(DEBUG_MENU_SECOND_BOX(boxes));
        return;
    }
    if ((gInput_wPad1Pressed & DEBUG_MENU_CONFIRM_MASK) != 0) {
        gDebugMenu_bStep = gDebugMenu_bCursor + 1;
    }
}

#ifndef VERSION_EUROPE
void func_8003134C(void)
{
}
#endif

void DebugMenu_UpdateBustUpEntry(void)
{
#ifdef VERSION_EUROPE
    /* The European entry (the same slot of the debug menu's handler table)
       only clears the step flags; the European image has no
       func_8003134C. */
    gDebugMenu_bStep = 0;
#else
    u8 flags = gDebugMenu_bStep;

    if (!(flags & FRONTEND_STEP_FLAG_ENTERED)) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        D_8009B2DE = 0;
        Campaign_LoadScenePackage(0);
    }

    func_8003134C();

    if (D_8009B2DE == 0) {
        DisplayObject *first = gDisplayObject_apBrightnessPool[0];
        DisplayObject *second = gDebugMenu_pCursorQuad;
        u16 second_flags;

        first->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;

        second_flags = second->flags;
        gDebugMenu_bStep = 0;
        second_flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        second->flags = second_flags;
    } else {
        s32 value = Rand_GetInterval(4);
        DisplayEffect_UpdateMenuRecord(gDisplayEffect_aMenuRecords, value);
    }
#endif
}

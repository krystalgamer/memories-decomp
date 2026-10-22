#include "../types.h"
#include "text_box_blocking_build.h"
#include "display_object_core.h"
#include "duel_interface_setup.h"
#include "debug_value_editor_config.h"
#include "duel_effect.h"
#include "frontend_debug_tables.h"
#include "fade.h"
#include "debug_menu_update_cursor_layout.h"
#include "duel_effect_set_entry_range_layout.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "../unmatched.h"

#define DISPLAY_OBJECT_WORD_BYTES(field) ((u8 *)&(field))

void DebugMenu_Init(void)
{
    DuelEffectChannel *text_box;
    DisplayObject *object;

    D_8009B2B2 = 0x80;
    gDebugMenu_bStep = 0;
    gDebugMenu_bPage = DEBUG_MENU_PAGE_PRIMARY;
    /* The Japanese and European builds make neither the func_8003B6AC call
       nor the two text-box field stores. */
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    func_8003B6AC(1, 1);
#endif
    text_box = TextBox_Create(1, 15, 16, 16, 0x120, 0xA0);
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    text_box->field_5A = 16;
    text_box->field_5B = 16;
#endif
    TextBox_BuildBlocking(text_box);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    gDebugMenu_pCursorQuad = object;
    DisplayObject_InitializeGouraudQuad(object, 0);
    DISPLAY_OBJECT_WORD_BYTES(object->field_44)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_3C)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_34)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_2C)[1] = 0xC0;
    DebugMenu_UpdateCursorLayout();
    Fade_WaitIn();
}

void DebugMenu_OpenValueEditor(
    u8 *data,
    s32 field_B4,
    s32 field_B5,
    s32 field_B6,
    s32 field_B8,
    s32 field_C0,
    s32 field_E0
)
{
    gDebug_bEditorFlags = 0;
    gDebug_pszEditorFormat = data;
    gDebug_abEditorCaretColumns = field_B4;
    gDebug_bEditorCaretColumn1 = field_B5;
    gDebug_bEditorCaretColumn2 = field_B6;
    gDebug_bEditorDigit = 0;
    gDebug_bEditorRow = 0;
    gDebug_bEditorLeadingLineCount = field_B8;
    gDebug_bEditorDigitCount2 = field_C0;
    gDebug_bEditorDigitCount1 = field_C0;
    gDebug_abEditorDigitCounts = field_C0;
    gDebug_bEditorRowCount = field_E0;
}

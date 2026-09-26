#include "../types.h"
#include "display_object_core.h"
#include "duel_interface_setup.h"
#include "duel_effect.h"
#include "frontend_debug_tables.h"
#include "fade.h"
#include "debug_menu_update_cursor_layout.h"
#include "func_8003B6AC.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "../unmatched.h"

#define DISPLAY_OBJECT_WORD_BYTES(field) ((u8 *)&(field))

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DEBUG_MENU_INIT)
void DebugMenu_Init(void)
{
    DuelEffectChannel *text_box;
    DisplayObject *object;

    D_8009B2B2 = 0x80;
    D_8009B2EB = 0;
    gDebugMenu_bPage = DEBUG_MENU_PAGE_PRIMARY;
    /* The Japanese build makes neither the func_8003B6AC call nor the two
       text-box field stores. */
#ifndef VERSION_JAPAN
    func_8003B6AC(1, 1);
#endif
    text_box = TextBox_Create(1, 15, 16, 16, 0x120, 0xA0);
#ifndef VERSION_JAPAN
    text_box->field_5A = 16;
    text_box->field_5B = 16;
#endif
    func_80039A14(text_box);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    D_8009B2E4 = object;
    DisplayObject_InitializeGouraudQuad(object, 0);
    DISPLAY_OBJECT_WORD_BYTES(object->field_44)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_3C)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_34)[1] = 0xC0;
    DISPLAY_OBJECT_WORD_BYTES(object->field_2C)[1] = 0xC0;
    DebugMenu_UpdateCursorLayout();
    Fade_WaitIn();
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80030250)
void func_80030250(
    u8 *data,
    s32 field_B4,
    s32 field_B5,
    s32 field_B6,
    s32 field_B8,
    s32 field_C0,
    s32 field_E0
)
{
    tent_DebugInterfaceFlags = 0;
    tent_DebugInterfaceData = data;
    tent_DebugInterfaceFieldB4 = field_B4;
    tent_DebugInterfaceFieldB5 = field_B5;
    tent_DebugInterfaceFieldB6 = field_B6;
    gDebug_bEditorDigit = 0;
    gDebug_bEditorRow = 0;
    tent_DebugInterfaceFieldB8 = field_B8;
    tent_DebugInterfaceFieldC2 = field_C0;
    tent_DebugInterfaceFieldC1 = field_C0;
    tent_DebugInterfaceFieldC0 = field_C0;
    tent_DebugInterfaceFieldE0 = field_E0;
}
#endif

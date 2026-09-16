#include "../types.h"
#include "../game/display_object.h"
#include "../game/debug_menu_update_cursor_layout.h"
#include "../game/duel_effect.h"
#include "../game/frontend_debug_tables.h"
#include "../game/frontend_debug_state.h"

void DebugMenu_UpdateCursorLayout(void)
{
    struct LayoutSource *source = (struct LayoutSource *)&D_800EB15C;
    s32 first = source->base;
    DisplayObject *target = D_8009B2E4;
    s32 index = gDebugMenu_bCursor;
    s32 second;
    s32 third;

    if (index >= 10) {
        first += source->spacing / 2 - 16;
    }
    target->field_38.h.field_38 = first;
    target->position.h.field_28 = first;

    second = first + source->spacing / 2 - 16;
    target->field_40.h.field_40 = second;
    target->field_30.h.field_30 = second;

    {
        s32 quotient = index / 10;
        s8 remainder = (s8)(index - quotient * 10);
        third = source->row + remainder * 16;
    }
    target->field_30.h.field_32 = third;
    target->position.h.field_2A = third;
    third += 16;
    target->field_40.h.field_42 = third;
    target->field_38.h.field_3A = third;
}

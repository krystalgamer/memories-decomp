#define DUEL_EFFECT_SIGNED_HALFWORD(field) (*(s16 *)&(field))
#include "../types.h"
#include "text_box_wrap_line.h"
#include "../unmatched.h"
#include "dialog_choice_control.h"
#include "dialog_choice.h"
#include "text_stream_commands.h"

s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object) {
    if (DUEL_EFFECT_SIGNED_HALFWORD(object->pen_x) >= object->rect_width) {
        object->pen_x = 0;
        object->pen_y += object->line_advance_y;
    }
    if (object->rect_height <
        DUEL_EFFECT_SIGNED_HALFWORD(object->pen_y) + object->line_advance_y) {
        object->pen_y -= object->line_advance_y;
        return 1;
    }
    return 0;
}

void Text_TryCompleteChoiceLayout(volatile DuelEffectChannel *object)
{
    u32 flags;
    u8 control;

    if (object->explicit_newline_count >= gDialog_bChoiceCount) {
        object->playback_state = 1;
        flags = object->flags_34;
        object->explicit_newline_count = 0;
        control = gDialog_bChoiceControl;
        gText_ChoiceLayoutCallback = 0;
        flags &= 0xEFFF;
        control &= DIALOG_CHOICE_CONTROL_LAYOUT_MASK;
        ((DuelEffectChannel *)object)->flags_34 = flags;
        if (control != 0)
            gDialog_bChoiceCount = 2;
    }
}

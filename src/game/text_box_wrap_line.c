#include "../types.h"
#include "text_box_wrap_line.h"

#define DUEL_EFFECT_SIGNED_HALFWORD(field) (*(s16 *)&(field))

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

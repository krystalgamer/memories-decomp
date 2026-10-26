#include "../types.h"
#include "text_box_wrap_line.h"

#define DUEL_EFFECT_SIGNED_HALFWORD(field) (*(s16 *)&(field))

s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object) {
    if (DUEL_EFFECT_SIGNED_HALFWORD(object->field_38) >= object->field_3E) {
        object->field_38 = 0;
        object->field_3A += object->line_advance_y;
    }
    if (object->field_42 <
        DUEL_EFFECT_SIGNED_HALFWORD(object->field_3A) + object->line_advance_y) {
        object->field_3A -= object->line_advance_y;
        return 1;
    }
    return 0;
}

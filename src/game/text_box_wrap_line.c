#include "../types.h"
#include "text_box_wrap_line.h"

#define DUEL_EFFECT_SIGNED_HALFWORD(field) (*(s16 *)&(field))

s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object) {
    if (DUEL_EFFECT_SIGNED_HALFWORD(object->field_38) >= object->field_3E) {
        object->field_38 = 0;
        object->field_3A += object->field_5B;
    }
    if (object->field_42 <
        DUEL_EFFECT_SIGNED_HALFWORD(object->field_3A) + object->field_5B) {
        object->field_3A -= object->field_5B;
        return 1;
    }
    return 0;
}

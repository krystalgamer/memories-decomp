#include "../types.h"
#include "func_80037C74.h"

s32 func_80037C74(DuelEffectChannel *object) {
    if (*(s16 *)&object->field_38 >= object->field_3E) {
        object->field_38 = 0;
        object->field_3A += object->field_5B;
    }
    if (object->field_42 < *(s16 *)&object->field_3A + object->field_5B) {
        object->field_3A -= object->field_5B;
        return 1;
    }
    return 0;
}

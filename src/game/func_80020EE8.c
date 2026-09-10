#include "../types.h"
#include "display_object_lifecycle.h"
#include "display_object_layout.h"
#include "display_object_api.h"
#include "duel_card_display_state.h"
#include "func_80020EE8.h"

void func_80020EE8(DuelCardDisplayObject *object)
{
    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    object->field_21 -= 2;
    if (object->field_21 < 0xC0) {
        func_8004036C((u8 *)object);
    }
}

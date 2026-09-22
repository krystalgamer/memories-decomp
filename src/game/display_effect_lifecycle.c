#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_core.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_EFFECT_LATCH)
s32 func_80039F1C(DisplayEffectState *object)
{
    u8 state = object->state;

    if ((state & 0x80) == 0) {
        object->state = state | 0x80;
        return 0;
    }
    return 1;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_EFFECT_INITIALIZE)
void func_80039F44(DisplayEffectState *object)
{
    object->field_34 = 0x68;
    object->field_32 = 0;
    object->state = 0;
    object->field_31 = 0;
    object->field_36 = 0xB2;
    object->field_3E =
        (rand() & DISPLAY_EFFECT_DELAY_MASK) + DISPLAY_EFFECT_DELAY_BASE;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_EFFECT_RELEASE_OBJECTS)
void func_80039F90(void **objects)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        DisplayObject_ReleaseIfPresent(objects[i]);
        objects[i] = 0;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_EFFECT_RELEASE_RECORD)
void func_80039FD4(MenuRecord *record)
{
    record->field_30 = -1;
    func_80039F90((void **)record->grid[0]);
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_EFFECT_STATE_RESET)
void func_80039FF8(DisplayEffectState *object)
{
    u8 flags = object->field_32;

    if ((flags & 3) == 0) {
        object->field_32 = flags | 0x10;
        object->state = 0;
    }
}
#endif

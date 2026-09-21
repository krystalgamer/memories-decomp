#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_core.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"

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

void func_80039F90(void **objects)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        DisplayObject_ReleaseIfPresent(objects[i]);
        objects[i] = 0;
    }
}

void func_80039FD4(MenuRecord *record)
{
    record->field_30 = -1;
    func_80039F90((void **)record->grid[0]);
}

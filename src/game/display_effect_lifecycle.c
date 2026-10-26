#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_core.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"

s32 DisplayEffect_MarkInitialized(DisplayEffectState *object)
{
    u8 state = object->state;

    if ((state & 0x80) == 0) {
        object->state = state | 0x80;
        return 0;
    }
    return 1;
}

void DisplayEffect_ResetRecordState(DisplayEffectState *object)
{
    object->field_34 = 0x68;
    object->variant_update_flags = 0;
    object->state = 0;
    object->resource_entry_index = 0;
    object->field_36 = 0xB2;
    object->variant_countdown =
        (rand() & DISPLAY_EFFECT_DELAY_MASK) + DISPLAY_EFFECT_DELAY_BASE;
}

void DisplayEffect_ReleaseObjectRow(void **objects)
{
    s32 i;

    for (i = 2; i >= 0; i--) {
        DisplayObject_ReleaseIfPresent(objects[i]);
        objects[i] = 0;
    }
}

void DisplayEffect_ReleaseRecord(MenuRecord *record)
{
    record->effect_id = -1;
    DisplayEffect_ReleaseObjectRow((void **)record->grid[0]);
}

void DisplayEffect_FinishStepWhenIdle(DisplayEffectState *object)
{
    u8 flags = object->variant_update_flags;

    if ((flags & 3) == 0) {
        object->variant_update_flags = flags | 0x10;
        object->state = 0;
    }
}

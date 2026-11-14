#include "../types.h"
#include "../psyq/rand.h"
#include "display_effect_constants.h"
#include "display_object_system.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"

s32 DisplayEffect_MarkInitialized(MenuRecord *record)
{
    u8 state = record->display_effect_step;

    if ((state & 0x80) == 0) {
        record->display_effect_step = state | 0x80;
        return 0;
    }
    return 1;
}

void DisplayEffect_ResetRecordState(MenuRecord *record)
{
    record->x = 0x68;
    record->variant_update_flags = 0;
    record->display_effect_step = 0;
    record->resource_entry_index = 0;
    record->y = 0xB2;
    record->variant_countdown =
        (rand() & DISPLAY_EFFECT_DELAY_MASK) + DISPLAY_EFFECT_DELAY_BASE;
}

void DisplayEffect_ReleaseObjectRow(DisplayObject **objects)
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
    DisplayEffect_ReleaseObjectRow(record->grid[0]);
}

void DisplayEffect_FinishStepWhenIdle(MenuRecord *record)
{
    u8 flags = record->variant_update_flags;

    if ((flags & 3) == 0) {
        record->variant_update_flags = flags | 0x10;
        record->display_effect_step = 0;
    }
}

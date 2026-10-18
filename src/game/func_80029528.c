#include "../types.h"
#include "display_object_core.h"
#include "duel_effect_resource_record.h"
#include "card_preview_objects.h"

void DuelEffect_ReleaseResourceObjects(s32 index)
{
    DuelEffectResourceRecord *entry = &D_800EA0E8[index];

    DisplayObject_ReleaseIfPresent(entry->primary_object);
    DisplayObject_ReleaseIfPresent(entry->secondary_object);
    entry->secondary_object = 0;
    entry->primary_object = 0;
}

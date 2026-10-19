#include "../types.h"
#include "duel_effect_resource_record.h"
#include "duel_effect_clear_resource_object_pointers.h"

void DuelEffect_ClearResourceObjectPointers(int index)
{
    DuelEffectResourceRecord *entry = &gCardPreview_Resources[index];

    entry->secondary_object = 0;
    entry->primary_object = 0;
}

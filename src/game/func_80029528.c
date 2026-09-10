#include "../types.h"
#include "display_object_api.h"
#include "duel_effect_resource_record.h"
#include "func_800291E0.h"

void func_80029528(s32 index)
{
    DuelEffectResourceRecord *entry = &D_800EA0E8[index];

    func_8004036C(entry->object_00);
    func_8004036C(entry->object_04);
    entry->object_04 = 0;
    entry->object_00 = 0;
}

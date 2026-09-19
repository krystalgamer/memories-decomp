#include "../types.h"
#include "model_handler_registry.h"
#include "func_8005A53C.h"

void func_8005A53C(s32 (*fn)(s32), ModelHandlerRunEntry *entry, s32 arg2,
                   s32 count)
{
    s32 i;
    s32 key;
    s32 value;
    s32 kind;
    s32 step;
    s32 result;

    for (i = count - 1; i != -1; i--) {
        key = Model_FindHandlerKey(entry->key);
        value = entry->metadata;
        kind = key >> 24;
        step = (value & 0xFFFF) + 1;
        value &= 0xFFFF0000;
        if (((u32)kind < 2 && (key & 0xFFFF) != 0 && value != 0) ||
            (kind == 1 && (key & 0xFFFF) == 0)) {
            result = fn(entry->key);
            if (result != -1) {
                entry->key = result;
            }
        }
        entry = (ModelHandlerRunEntry *)&((u32 *)entry)[step];
    }
}

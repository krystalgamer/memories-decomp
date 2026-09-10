#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

extern u8 tail_data_start[];

s32 func_8004002C(void)
{
    DisplayObject *entry = D_800F0548;
    s32 i;

    for (i = DISPLAY_OBJECT_RESERVED_CAPACITY;
         i < DISPLAY_OBJECT_POOL_CAPACITY;
         i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_8004006C(void)
{
    DisplayObject *entry = D_800EFE48;
    s32 i;

    for (i = 0; i < DISPLAY_OBJECT_POOL_CAPACITY; i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

void *func_800400AC(s32 index, s32 key)
{
    DisplayObject *slot;
    s16 other;
    u16 initialized;

    if (index < 0) {
        return 0;
    }

    slot = &D_800EFE48[index];
    initialized = slot->flags & DISPLAY_OBJECT_FLAG_ALLOCATED;
    D_8009B412++;
    if (!initialized) {
        other = D_800EFE38[key];
        if (other < 0) {
            D_800F2878[key] = index;
            slot->next = -1;
            slot->previous = -1;
        } else {
            D_800EFE48[other].previous = index;
            slot->next = (u16)D_800EFE38[key];
        }

        slot->previous = -1;
        D_800EFE38[key] = index;
        /* A fresh object starts with rotation off. */
        slot->attribute = GsROTOFF;
        slot->flags = DISPLAY_OBJECT_RENDERABLE_MASK;
        slot->ot_index = 2;
        slot->field_54 = tail_data_start;
        slot->field_6C = 0;
        slot->update = 0;
        slot->field_1E = key;
        slot->field_0A = index;
        slot->field_0B = 0;
        slot->field_0C = 0x00808080;
        slot->field_20.word = 0;
        slot->field_10 = 0;
        slot->field_1C = 0;
        slot->field_1A = 0;
        slot->field_18 = 0;
        slot->field_66 = 0;
        slot->field_48.word = 0;
        slot->field_44.word = 0x10001000;
        slot->field_40.word = 0;
        slot->field_5E = 0;
        slot->field_65 = 0;
        slot->field_16 = 0;
        slot->field_14 = D_8009AF74[slot->ot_index];
    }
    return slot;
}

void func_8004020C(DisplayObject *slot)
{
    s32 first = slot->previous;
    s32 second = slot->next;

    D_8009B410++;

    if (first < 0) {
        s16 index = slot->field_1E;
        D_800EFE38[index] = second;
        if (second >= 0) {
            D_800EFE48[second].previous = -1;
        }
    } else {
        D_800EFE48[first].next = second;
        if (second >= 0) {
            D_800EFE48[second].previous = first;
        }
    }

    slot->flags = 0;
}

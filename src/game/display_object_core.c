#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "display_object_render_sprite_list.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "ordering_tables.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"

extern u8 tail_data_start[];

s32 DisplayObject_FindFreeGeneralSlot(void)
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

s32 DisplayObject_FindFreeSlot(void)
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

void *DisplayObject_AcquireSlot(s32 index, s32 key)
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

void DisplayObject_Release(DisplayObject *slot)
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

void DisplayObject_MoveToListHead(DisplayObject *slot, s32 key)
{
    u16 saved = slot->flags;
    s32 v;

    DisplayObject_Release(slot);
    v = D_800EFE38[key];
    if (v < 0) {
        D_800F2878[key] = slot->field_0A;
        slot->next = -1;
        slot->previous = -1;
    } else {
        D_800EFE48[v].previous = slot->field_0A;
        slot->next = D_800EFE38[key];
    }
    slot->previous = -1;
    D_800EFE38[key] = slot->field_0A;
    slot->flags = saved;
}

void DisplayObject_ReleaseIfPresent(void *object)
{
    if (object != 0) {
        DisplayObject_Release((DisplayObject *)object);
    }
}

void DisplayObject_ResetPool(void){int i=0;int neg=-1;s16*a; s16*b;a=D_800F2878;b=D_800EFE38;D_8009B410=0;D_8009B412=0;for(;i<DISPLAY_OBJECT_LIST_COUNT;i++){*b=neg;*a=neg;a++;b++;}{DisplayObject*p=D_800EFE48;for(i=DISPLAY_OBJECT_POOL_CAPACITY-1;i>=0;i--){p->flags=0;p++;}}}

void DisplayObject_Reset(void)
{
    DisplayObject_ResetPool();
}

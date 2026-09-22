#include "../types.h"
#include "display_object_core.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "file_transfer.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "script_image_objects.h"

#ifndef VERSION_JAPAN
void ScriptImage_TransferCallback(FileTransferDescriptor *obj, s32 mode)
{
    switch (mode) {
    case 0:
        obj->field_30.h.field_32 = 0x100;
        obj->field_30.h.counter = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        obj->done = 2;
        obj->phase_size = (s32)obj->callback_data << FILE_SECTOR_SHIFT;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118;
        obj->value_08 = D_8009B118;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0;
        obj->y = 0xF0;
        obj->w = 0x100;
        obj->h = 4;
        LoadImage2((RECT *)obj, (u32 *)D_8009B118);
        break;
    }
}

void ScriptImage_RequestTransfer(
    volatile ScriptImageObjectSet *owner, s32 value)
{
    s32 index;
    s32 mode;
    s32 stride;
    s32 base;
    FileTransferDescriptor *object;

    if (owner) {
        owner->image_id = value;
    }

    index = ((value >> 4) & 15) * 10 + (value & 15);
    mode = value >> 8;
    switch (mode) {
    case 0:
        stride = 0x21;
        base = 0;
        break;
    case 1:
        stride = 0x51;
        base = 0x672;
        break;
    case 2:
        stride = 0x71;
        base = 0x13BC;
        break;
    default:
        return;
    }
    object = File_TryRequestAsyncTransfer(
        0, 0, base + index * stride + 0x21D5, stride,
        ScriptImage_TransferCallback, 0, 0
    );
    object->callback_data = (void *)(stride - 1);
    D_8009B0F4 =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_SCRIPT_IMAGE_RELEASE_OBJECTS)
void ScriptImage_ReleaseObjects(ScriptImageObjectSet *set)
{
    ScriptImageEntry *entries = set->entries;
    s32 i;

    set->image_id = -1;
    for (i = 0; i < 3; i++, entries++) {
        DisplayObject_ReleaseIfPresent(entries->pointer);
        entries->pointer = 0;
        entries->value = 0;
    }
}
#endif

#ifndef VERSION_JAPAN
void ScriptImage_CreateObject(ScriptImageEntry *entry, s32 size, s32 mode)
{
    ScriptImageEntry *record = entry;
    DisplayObject *object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);

    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 2, 0, 0, size, mode);
    DisplayObject_SetDepthOffset(object, (s8)mode);
    record->field_10 = 1;
    if (mode == 2) {
        record->value = 1;
        object->attribute |= (GsALON | GsAONE);
    } else {
        record->value = 0;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
    }
    /* Through the parameter rather than record: with every store on record,
       GCC keeps a second copy of the pointer and six instructions change. */
    entry->pointer = object;
}
#endif

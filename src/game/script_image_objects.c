#include "../types.h"
#include "display_object_core.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "file_transfer_buffer.h"
#include "script_image_objects.h"
#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
#include "duel_effect_resource_setup.h"
#endif

/* Regional values: the image row's upload position, (0, 0xF0) in the US build
   and (0x280, 0xD0) in the European one, whose wrapper defines its own. */
#ifndef SCRIPT_IMAGE_SCREEN_X
#define SCRIPT_IMAGE_SCREEN_X 0
#endif
#ifndef SCRIPT_IMAGE_SCREEN_Y
#define SCRIPT_IMAGE_SCREEN_Y 0xF0
#endif

void ScriptImage_TransferCallback(FileTransferDescriptor *obj, s32 mode)
{
    switch (mode) {
    case 0:
        obj->field_30.h.field_32 = 0x100;
        obj->field_30.h.counter = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        gFile_dwTransferState &= 0xFFDDFFFF;
        gFile_dwTransferState |= 0x10000;
        obj->transfer_mode = 2;
        obj->phase_size = (s32)obj->callback_data << FILE_SECTOR_SHIFT;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        obj->alternate_sector_destination = gFile_TransferBufferBase;
        obj->sector_destination = gFile_TransferBufferBase;
        obj->transfer_mode = 1;
        break;

    case 2:
        obj->x = SCRIPT_IMAGE_SCREEN_X;
        obj->y = SCRIPT_IMAGE_SCREEN_Y;
        obj->w = 0x100;
        obj->h = 4;
        LoadImage2((RECT *)obj, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#ifndef SCRIPT_IMAGE_TRANSFER_BASE
#define SCRIPT_IMAGE_TRANSFER_BASE 0x21D5
#endif

void ScriptImage_RequestTransfer(
    volatile ScriptImageObjectSet *owner, s32 value)
{
    s32 index;
    s32 mode;
    s32 stride;
    s32 base;
    FileTransferDescriptor *object;

#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
    if (value == 0x10) {
        if (gText_bLanguageIndex == 1) {
            value = 0x42;
        } else if (gText_bLanguageIndex == 2) {
            value = 0x44;
        } else if (gText_bLanguageIndex == 3) {
            value = 0x46;
        } else if (gText_bLanguageIndex == 4) {
            value = 0x48;
        }
    } else if (value == 0x11) {
        if (gText_bLanguageIndex == 1) {
            value = 0x43;
        } else if (gText_bLanguageIndex == 2) {
            value = 0x45;
        } else if (gText_bLanguageIndex == 3) {
            value = 0x47;
        } else if (gText_bLanguageIndex == 4) {
            value = 0x49;
        }
    }
#endif

    if (owner) {
#ifdef SCRIPT_IMAGE_LOCALIZED_IDS
        /* The localized binaries do not repeat the shift after this store. */
        ((ScriptImageObjectSet *)owner)->image_id = value;
#else
        owner->image_id = value;
#endif
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
        0, 0, base + index * stride + SCRIPT_IMAGE_TRANSFER_BASE, stride,
        ScriptImage_TransferCallback, 0, 0
    );
    object->callback_data = (void *)(stride - 1);
    gFile_dwTransferState =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

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

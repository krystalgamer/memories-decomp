#include "../types.h"
#include "display_object_api.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "file_transfer.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "script_image_objects.h"

void func_8002DDFC(FileTransferDescriptor *obj, s32 mode)
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
        obj->mode = (s32)obj->callback_data << FILE_SECTOR_SHIFT;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->mode = FILE_SECTOR_SIZE;
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

void func_8002DF2C(volatile u8 *owner, s32 value)
{
    s32 index;
    s32 mode;
    s32 stride;
    s32 base;
    FileTransferDescriptor *object;

    if (owner) {
        *(s16 *)(owner + 0x3C) = value;
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
        func_8002DDFC, 0, 0
    );
    object->callback_data = (void *)(stride - 1);
    D_8009B0F4 =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void func_8002E00C(ScriptImageEntry *entries)
{
    s32 i;

    *(s16 *)((u8 *)entries + 0x3C) = -1;
    for (i = 0; i < 3; i++, entries++) {
        func_8004036C(entries->pointer);
        entries->pointer = 0;
        entries->value = 0;
    }
}

void func_8002E060(u8 *owner, s32 size, s32 mode)
{
    u8 *object = func_800400AC(func_8004002C(), 2);

    func_800404CC(object, 0, 0, 2, 0, 0, size, mode);
    func_800428EC(object, (s8)mode);
    owner[0x10] = 1;
    if (mode == 2) {
        *(s16 *)(owner + 4) = 1;
        *(u32 *)(object + 4) |= (GsALON | GsAONE);
    } else {
        *(s16 *)(owner + 4) = 0;
        *(u32 *)(object + 4) |= 0x01000000;
    }
    *(void **)owner = object;
}

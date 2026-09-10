#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"

void Main_LoadBootPackageStage(FileTransferDescriptor *obj, s32 stage) {
    switch (stage) {
    case 0:
        obj->field_30.h.counter = 0x280;
        obj->field_30.h.field_32 = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        obj->mode = 0x18000;
        D_8009B0F4 |= 0x10000;
        obj->done = 2;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        obj->mode = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118;
        obj->value_08 = D_8009B118;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)D_8009B118);
        obj->mode = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118 + 0x1000;
        obj->value_08 = D_8009B118 + 0x1000;
        obj->done = 1;
        break;

    case 3:
        obj->x = 0x280;
        obj->y = 0xE8;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *)(D_8009B118 + 0x1000));
        obj->mode = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101D8;
        obj->value_08 = (s32)D_800101D8;
        obj->done = 1;
        break;
    }
}

#define D_8009B118_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "../unmatched.h"

#include "high_memory_addresses.h"
extern u8 D_8009B058[];

void MainMenu_LoadPackageStage(FileTransferDescriptor *object, s32 stage) {
    RECT rect;

    switch (stage) {
    case 0:
        object->field_30.h.counter = 0x200;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x20000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->field_30.h.counter = 0x380;
        object->field_30.h.field_32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x10000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 2:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->mode = 0x1000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect = *(RECT *)D_8009B058;
        LoadImage2(&rect, (u32 *)D_801DD000);
        object->mode = 0x8000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8001002C;
        object->value_08 = D_8001002C;
        object->done = 1;
        break;

    case 4:
        object->value_0C = (s32)D_801AF800;
        object->value_08 = (s32)D_801AF800;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}

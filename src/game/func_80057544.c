#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010008 __attribute__((section(".data")));
extern u8 D_801DD800[];
extern u8 D_800F5694[];

void func_80057544(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;

    switch (mode) {
    case 0:
        object->mode = 0x9000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010008;
        object->value_08 = D_80010008;
        object->done = 1;
        break;

    case 1:
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

    case 2:
        object->value_0C = (s32)D_801DD800;
        object->value_08 = (s32)D_801DD800;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect0.x = 0x200;
        rect0.y = 0xF1;
        rect0.w = 0x100;
        rect0.h = 1;
        LoadImage2(&rect0, (u32 *)D_801DD800);
        object->value_0C = (s32)(D_801DD800 - 0x800);
        object->value_08 = (s32)(D_801DD800 - 0x800);
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 4:
        rect1.x = 0x200;
        rect1.y = 0xF4;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
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

    case 5:
        D_800F5694[0] = 1;
        break;
    }
}

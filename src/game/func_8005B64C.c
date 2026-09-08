#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_8001002C __attribute__((section(".data")));
extern u8 D_8009B058[];
extern u8 D_801DD000[];
extern u8 D_801AF800[];

void func_8005B64C(FileTransferDescriptor *object, s32 mode) {
    RECT rect;

    switch (mode) {
    case 0:
        object->counter = 0x200;
        object->field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->mode = 0x20000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->counter = 0x380;
        object->field_32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->mode = 0x10000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 2:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->mode = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect = *(RECT *)D_8009B058;
        LoadImage2(&rect, (u32 *)D_801DD000);
        object->mode = 0x8000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8001002C;
        object->value_08 = D_8001002C;
        object->done = 1;
        break;

    case 4:
        object->value_0C = (s32)D_801AF800;
        object->value_08 = (s32)D_801AF800;
        object->mode = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}

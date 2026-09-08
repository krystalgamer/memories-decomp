#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010000 __attribute__((section(".data")));
extern s32 D_800101D8 __attribute__((section(".data")));
extern u8 D_801AF000[];

void func_8003B808(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->counter = 0;
        object->field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x10000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = 0;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->mode = 0x18000;
        object->value_0C = D_80010000;
        object->value_08 = D_80010000;
        object->done = 1;
        break;

    case 4:
        object->mode = 0x2800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_800101D8;
        object->value_08 = D_800101D8;
        object->done = 1;
        break;
    }
}

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118;
extern s32 D_800101D8;
extern u8 D_801A8000[];

void func_8003BD14(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
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
        object->mode = 0x2000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        object->x = 0x100;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 0x10;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        object->value_0C = (s32)D_801A8000;
        object->value_08 = (s32)D_801A8000;
        object->mode = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        object->mode = 0x7800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->value_0C = D_800101D8;
        object->value_08 = D_800101D8;
        object->done = 1;
        break;
    }
}

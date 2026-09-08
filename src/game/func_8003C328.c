#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118;
extern u8 D_801AF000[];

void func_8003C328(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_32 = 0x100;
        object->counter = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        object->mode = 0x18000;
        D_8009B0F4 |= 0x10000;
        object->done = 2;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->mode = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
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
        D_8009B0F4 &= 0xFFDCFFFF;
        object->done = 1;
        break;
    }
}

#define D_8009B118_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define D_800101D8_IN_DATA
#include "file_transfer.h"
#include "func_8003BF00.h"
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

void func_8003BF00(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->mode = 0x3000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = (u32)D_800101D8;
        object->value_08 = (u32)D_800101D8;
        object->done = 1;
        break;

    case 1:
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->mode = 0x43000;
        object->value_0C = (s32)D_80010000;
        object->value_08 = (s32)D_80010000;
        object->done = 1;
        break;

    case 2:
        object->value_0C = (s32)D_801AF000;
        object->value_08 = (s32)D_801AF000;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        object->field_30.h.counter = 0x1C0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x8000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 4:
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 5:
        object->x = 0x100;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 4;
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
        break;
    }
}

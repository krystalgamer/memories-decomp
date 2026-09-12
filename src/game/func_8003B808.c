#define D_8009B118_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define D_800101D8_IN_DATA
#include "file_transfer.h"
#include "func_8003B808.h"
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

void func_8003B808(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = FILE_SECTOR_SIZE;
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
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->value_0C = (s32)D_80010000;
        object->value_08 = (s32)D_80010000;
        object->done = 1;
        break;

    case 4:
        object->phase_size = 5 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = (u32)D_800101D8;
        object->value_08 = (u32)D_800101D8;
        object->done = 1;
        break;
    }
}

#define D_8009B118_IN_DATA
#include "../types.h"
#include "model_word_memory.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "model.h"
#include "../unmatched.h"
#include "file_transfer_steps.h"
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

/* Two transfer-phase callbacks of the func_8003B808 family, kept in one
   translation unit because they are the same routine over different assets:
   each takes the descriptor plus a phase index, and each switch arm programs
   the same fields -- phase_size, the value_08/value_0C source window, done,
   and the field_30 halfword pair -- while clearing and setting the same bits of
   D_8009B0F4_abs.

   Both own a .rodata jump table, and the two tables are adjacent in the image
   (0x1F84 for the six-case switch, 0x1F9C for the eleven-case one). The merged
   object therefore owns one 0x44-byte .rodata run at 0x1F84, which is why the
   two split.yaml subsegments collapse into a single entry. Definition order is
   address order and must stay that way: it is what puts the six-case table
   ahead of the eleven-case one. */

extern u8 D_801DD800[];
extern u8 D_800F5694[];

extern u8 D_801A8000[];

void func_80057544(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;

    switch (mode) {
    case 0:
        object->phase_size = 18 * FILE_SECTOR_SIZE;
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
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 2:
        object->value_0C = (s32)D_801DD800;
        object->value_08 = (s32)D_801DD800;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect0.x = 0x200;
        rect0.y = 0xF1;
        rect0.w = 0x100;
        rect0.h = 1;
        LoadImage2(&rect0, (u32 *)D_801DD800);
        object->value_0C = (s32)(D_801DD800 - FILE_SECTOR_SIZE);
        object->value_08 = (s32)(D_801DD800 - FILE_SECTOR_SIZE);
        object->phase_size = FILE_SECTOR_SIZE;
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
        object->phase_size = 32 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 5:
        D_800F5694[0] = 1;
        break;
    }
}

void func_800577B0(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;
    ModelSlot *dst;
    u8 *src;

    switch (mode) {
    case 0:
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = (s32)D_80010000;
        object->value_08 = (s32)D_80010000;
        object->done = 1;
        break;

    case 1:
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->field_30.h.counter = 0;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 96 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 2:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect0.y = 0xF8;
        rect0.w = 0x100;
        rect0.x = 0;
        rect0.h = 8;
        LoadImage2(&rect0, (u32 *)D_801DD000);
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010014;
        object->value_08 = D_80010014;
        object->done = 1;
        break;

    case 4:
        object->phase_size = 10 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010018;
        object->value_08 = D_80010018;
        object->done = 1;
        break;

    case 6:
        rect1.x = 0x100;
        rect1.y = 0xF0;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
        object->field_30.h.counter = 0x180;
        object->w = 0x40;
        object->field_30.h.field_32 = 0x100;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->phase_size = 8 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 7:
        object->value_0C = (s32)D_801A8000;
        object->value_08 = (s32)D_801A8000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 8:
        object->done = 3;
        object->field_30.word = 0xD810;
        object->phase_size = 50 * FILE_SECTOR_SIZE;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 5:
    case 9:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 10:
        dst = D_800F2C40;
        src = D_801DD000;
        func_8005B620((s32 *)dst->sound_entries, (const s32 *)src, 0x40);
        *(ModelSlotCF8BlockWords *)&dst->field_CF8 =
            *(ModelSlotCF8BlockWords *)(src + 0x100);
        *(s32 *)&dst->field_CF8.field_0C[2] = -1;
        *(s32 *)&dst->field_CF8.field_0C[4] = -1;
        *(s32 *)&dst->field_CF8.field_0C[6] = -1;
        *(s16 *)&dst->field_CF8.field_00[0] = 0;
        *(s16 *)&dst->field_CF8.field_00[2] = 0;
        dst->field_E14 = 1;
        break;
    }
}

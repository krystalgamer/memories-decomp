/*
 * Contiguous model file-transfer phase callbacks. The first handles MRG
 * transfer phases for the two duel slots. The entry slot and
 * alternate-palette selector are captured before dispatch; the finish stage
 * reloads the slot overrides after the copy and optional state callback.
 *
 * All 1,992 text bytes and the 17-entry table match the uniform G8 profile.
 * Separate cleared/reloaded flag captures retain the upload-stage ordering.
 * The direct image argument and post-call source cursor share the same
 * staging buffer without extending the cursor's lifetime across the upload.
 */
#include "../types.h"
#include "file_transfer.h"
#include "model.h"
#include "model_word_memory.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

#define D_8009B118_IS_POINTER_IN_DATA
#include "../unmatched.h"
#include "file_transfer_steps.h"
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#include "duel_load_package_stage.h"

void func_80056D7C(FileTransferDescriptor *d, s32 stage)
{
    RECT rect0;
    RECT rect1;
    s32 index;
    s32 alternate;
    ModelSlot *slot;

    index = (s32)d->callback_data;
    alternate = d->position;
    slot = &D_800F2C40[index];
    switch (stage) {
    case 0: {
        u8 *destination;
        if (index == 0) {
            destination = D_80010000;
        } else {
            destination = D_80010004;
        }
        d->value_08 = d->value_0C = (u32)destination;
        d->phase_size = 0x30000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    }
    case 1:
        d->field_30.h.counter = index << 8;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        d->phase_size = 0x18000;
        D_8009B0F4_abs |= 0x10000;
        d->done = 2;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + 0x800);
        break;
    case 2:
        d->value_08 = d->value_0C = (u32)D_801DD000;
        d->phase_size = 0x1000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 3: {
        u32 mask;
        u8 *source;
        RECT *rect;
        do { rect = &rect0; } while (0);
        rect0.x = index << 8;
        rect0.y = 0xF8;
        rect0.w = 0x100;
        rect0.h = 8;
        LoadImage2(rect, (u32 *)D_801DD000);
        source = D_801DD000;
        do { mask = 0xFFDCFFFF; } while (0);
        source += 0x1000;
        d->value_08 = d->value_0C = (u32)source;
        d->phase_size = 0x800;
        D_8009B0F4_abs &= mask;
        d->done = 1;
        break;
    }
    case 4:
        if (alternate == 0) {
            rect1.x = 0x200;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)D_801DE000);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = D_8009B0F4_abs & 0xFFDDFFFF;
                D_8009B0F4_abs = cleared;
                do { loaded = D_8009B0F4_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                D_8009B0F4_abs = loaded | enabled;
            }
            d->done = 2;
            d->phase_size = 0x8000;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + 0x800);
        } else {
            d->phase_size = 0x8000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 6:
        if (alternate == 1) {
            rect1.x = 0x200;
            rect1.y = index + 0xF2;
            rect1.w = 0x100;
            rect1.h = 1;
            LoadImage2(&rect1, (u32 *)D_801DD000);
            d->field_30.h.counter = (index << 8) + 0xC0;
            d->w = 0x40;
            d->h = 0x10;
            {
                u32 cleared, loaded, enabled;
                cleared = D_8009B0F4_abs & 0xFFDDFFFF;
                D_8009B0F4_abs = cleared;
                do { loaded = D_8009B0F4_abs; } while (0);
                do { enabled = 0x10000; } while (0);
                d->field_30.h.field_32 = 0x100;
                D_8009B0F4_abs = loaded | enabled;
            }
            d->done = 2;
            d->phase_size = 0x8000;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + 0x800);
        } else {
            d->phase_size = 0x8000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 7:
        if (alternate == 0 && index == 0) {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010014;
            d->done = 1;
        } else {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 8:
        if (alternate == 0 && index == 1) {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010018;
            d->done = 1;
        } else {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 9:
        if (alternate == 1 && index == 0) {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010014;
            d->done = 1;
        } else {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 10:
        if (alternate == 1 && index == 1) {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = D_80010018;
            d->done = 1;
        } else {
            d->phase_size = 0x5000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 11:
        if (index == 0) {
            d->phase_size = 0x1000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = (u32)D_8001000C;
            d->done = 1;
        } else {
            d->phase_size = 0x1000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 12:
        if (index == 1) {
            d->phase_size = 0x1000;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->value_08 = d->value_0C = (u32)D_80010010;
            d->done = 1;
        } else {
            d->phase_size = 0x1000;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        }
        break;
    case 13:
        if (slot->field_E1D != 0) {
            d->phase_size = 0x800;
            D_8009B0F4_abs &= 0xFFFCFFFF;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        } else {
            d->value_08 = d->value_0C = (u32)(D_801A8000 + index * 0x800);
            d->phase_size = 0x800;
            D_8009B0F4_abs &= 0xFFDCFFFF;
            d->done = 1;
        }
        break;
    case 14:
        if (slot->field_E1D != 0) {
            u32 mask;
            do { mask = 0xFFFCFFFF; } while (0);
            d->phase_size = 0x19000;
            D_8009B0F4_abs &= mask;
            D_8009B0F4_abs |= 0x200000;
            d->done = 1;
        } else {
            d->done = 3;
            d->field_30.word = index * 0x19000 + 0xD810;
            d->phase_size = 0x19000;
            d->value_08 = (u32)D_8009B118;
            d->value_0C = (u32)(D_8009B118 + 0x800);
        }
        break;
    case 5:
    case 15:
        d->value_08 = d->value_0C = (u32)D_801DD000;
        d->phase_size = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 16: {
        u8 *source;
        source = D_801DD000;
        func_8005B620((s32 *)slot->sound_entries, (const s32 *)source, 0x40);
        *(ModelSlotCF8BlockWords *)&slot->field_CF8 =
            *(ModelSlotCF8BlockWords *)(source + 0x100);
        if (slot->field_CF8.prefix.values.field_08 & 0x20200000) {
            func_80059284(index, 4);
        }
        if (slot->field_DFA < 0xFFFF) {
            slot->field_CF8.prefix.values.field_00 = slot->field_DFA;
        }
        if (slot->field_DFC < 0xFFFF) {
            slot->field_CF8.prefix.values.field_02 = slot->field_DFC;
        }
        if (slot->field_E1D != 0) {
            slot->field_CF8.field_10 = -1;
            slot->field_CF8.field_14 = -1;
            slot->field_CF8.field_18 = -1;
        }
        slot->field_E14 = 1;
        break;
    }
    }
}

/* Two transfer-phase callbacks of the func_8003B808 family, kept in one
   translation unit because they are the same routine over different assets:
   each takes the descriptor plus a phase index, and each switch arm programs
   the same fields -- phase_size, the value_08/value_0C source window, done,
   and the field_30 halfword pair -- while clearing and setting the same bits of
   D_8009B0F4_abs.

   Both own a .rodata jump table, and the two tables are adjacent in the image
   (0x1F84 for the six-case switch, 0x1F9C for the eleven-case one). Together
   with func_80056D7C's 17-entry table and the retail zero word after it, the
   complete object owns the 0x8C-byte .rodata run at 0x1F3C. Definition order
   is address order and must stay that way. */

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
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
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
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
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
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
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
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
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
        object->value_08 = (u32)D_8009B118;
        object->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
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
        dst->field_CF8.field_10 = -1;
        dst->field_CF8.field_14 = -1;
        dst->field_CF8.field_18 = -1;
        dst->field_CF8.prefix.values.field_00 = 0;
        dst->field_CF8.prefix.values.field_02 = 0;
        dst->field_E14 = 1;
        break;
    }
}

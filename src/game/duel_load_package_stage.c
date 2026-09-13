/*
 * All 948 bytes and the 52-byte jump table match with the existing uniform
 * gcc_2_8_1_g8_split profile. Separate source scopes for image stages 6 and
 * 12 recover their shared machine tail without merging it with stages 0/10.
 *
 * Stage 10 uses separate cleared and reloaded flag values. The reload
 * capture preserves its register lifetime; the scoped enable-mask value
 * places the mask before the zero-offset store in the load-delay slot.
 * The thirteen stage contracts, both image submissions and volatile flag
 * accesses are preserved without register bindings or instruction assembly.
 */
#define DUEL_PACKAGE_STAGE_RAW_ARENAS
#define D_8009B118_IS_POINTER_IN_DATA
#include "../types.h"
#include "duel_check_ritual.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "file_transfer.h"
#include "graphics_frame.h"
#include "../unmatched.h"
#include "duel_card_checks.h"
#include "duel_load_package_stage.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage)
{
    u32 flags;
    u32 mask;

    switch (stage) {
    case 0:
        d->field_30.h.counter = 0x300;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        d->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        d->done = 2;
        d->phase_size = 64 * FILE_SECTOR_SIZE;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;
    case 1:
        d->phase_size = 4 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_8009B118;
        d->done = 1;
        break;
    case 2:
        D_800E9D70[0].x = 0x100;
        D_800E9D70[0].y = 0xF0;
        D_800E9D70[0].w = 0x100;
        D_800E9D70[0].h = 0x10;
        LoadImage2(&D_800E9D70[0], (u32 *)D_8009B118);
        d->value_08 = d->value_0C = (u32)gDuel_awEquipTable;
        d->phase_size = 5 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 3:
        d->value_08 = d->value_0C = (u32)gDuel_aFusionTable;
        d->phase_size = 32 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 4:
        d->value_08 = d->value_0C = (u32)gDuel_awRitualData;
        d->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 5:
        d->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_8009B118;
        d->done = 1;
        break;
    case 6: {
        u32 flags;
        D_800E9D70[0].x = 0;
        D_800E9D70[0].y = 0xF0;
        D_800E9D70[0].w = 0x100;
        D_800E9D70[0].h = 8;
        LoadImage2(&D_800E9D70[0], (u32 *)D_8009B118);
        d->field_30.h.counter = 0x200;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        mask = 0xFFDDFFFF;
        flags = D_8009B0F4_abs & mask;
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            u8 *image_ptr;

            D_8009B0F4_abs = flags;
            mask = 0x10000;
            d->phase_size = mask;
            D_8009B0F4_abs |= mask;
            d->done = 2;
            d->h = 0x10;
            image_ptr = D_8009B118;
            d->value_08 = (u32)image_ptr;
            d->value_0C = (u32)(image_ptr + FILE_SECTOR_SIZE);
            break;
        }
    }
    case 7:
        mask = 0xFFDCFFFF;
        d->phase_size = 44 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= mask;
        d->value_08 = d->value_0C = (u32)D_800101DC;
        d->done = 1;
        break;
    case 8:
        d->value_08 = d->value_0C = (u32)D_801A8000;
        d->phase_size = 3 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 9:
        d->value_08 = d->value_0C = (u32)D_801A9800;
        d->phase_size = 3 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 10: {
        u32 cleared;
        u32 loaded;
        u32 enabled;
        d->field_30.h.counter = 0x340;
        d->w = 0x40;
        d->h = 0x10;
        cleared = D_8009B0F4_abs & 0xFFDDFFFF;
        D_8009B0F4_abs = cleared;
        do { loaded = D_8009B0F4_abs; } while (0);
        do { enabled = 0x10000; } while (0);
        d->field_30.h.field_32 = 0;
        D_8009B0F4_abs = loaded | enabled;
        d->done = 2;
        d->phase_size = 8 * FILE_SECTOR_SIZE;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + FILE_SECTOR_SIZE);
        break;
    }
    case 11:
        d->phase_size = 5 * FILE_SECTOR_SIZE;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_80010000;
        d->done = 1;
        break;
    case 12: {
        u32 flags;
        d->field_30.h.counter = 0x280;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        mask = 0xFFDDFFFF;
        flags = D_8009B0F4_abs & mask;
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            u8 *image_ptr;

            D_8009B0F4_abs = flags;
            mask = 0x10000;
            d->phase_size = mask;
            D_8009B0F4_abs |= mask;
            d->done = 2;
            d->h = 0x10;
            image_ptr = D_8009B118;
            d->value_08 = (u32)image_ptr;
            d->value_0C = (u32)(image_ptr + FILE_SECTOR_SIZE);
            break;
        }
    }
    }
}

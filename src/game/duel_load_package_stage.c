#define D_8009B118_IS_POINTER_IN_DATA
#include "../types.h"
#include "duel_check_ritual.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "file_transfer.h"
#include "graphics_frame.h"
#include "../unmatched.h"
#include "duel_card_checks.h"

extern u8 *D_80010000 __attribute__((section(".data")));
extern u8 *D_800101DC __attribute__((section(".data")));
extern u8 D_801A8000[];
extern u8 D_801A9800[];

void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage)
{
    register u32 flags asm("$2");
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
        d->mode = 0x20000;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + 0x800);
        break;
    case 1:
        d->mode = 0x2000;
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
        d->mode = 0x2800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 3:
        d->value_08 = d->value_0C = (u32)gDuel_aFusionTable;
        d->mode = 0x10000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 4:
        d->value_08 = d->value_0C = (u32)gDuel_awRitualData;
        d->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 5:
        d->mode = 0x1000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_8009B118;
        d->done = 1;
        break;
    case 6:
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
        goto image_stage;
    case 7:
        mask = 0xFFDCFFFF;
        d->mode = 0x16000;
        D_8009B0F4_abs &= mask;
        d->value_08 = d->value_0C = (u32)D_800101DC;
        d->done = 1;
        break;
    case 8:
        d->value_08 = d->value_0C = (u32)D_801A8000;
        d->mode = 0x1800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 9:
        d->value_08 = d->value_0C = (u32)D_801A9800;
        d->mode = 0x1800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 10:
        d->field_30.h.counter = 0x340;
        d->w = 0x40;
        d->h = 0x10;
        flags = D_8009B0F4_abs & 0xFFDDFFFF;
        D_8009B0F4_abs = flags;
        flags = D_8009B0F4_abs;
        d->field_30.h.field_32 = 0;
        D_8009B0F4_abs = flags | 0x10000;
        d->done = 2;
        d->mode = 0x4000;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + 0x800);
        break;
    case 11:
        d->mode = 0x2800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_80010000;
        d->done = 1;
        break;
    case 12:
        d->field_30.h.counter = 0x280;
        d->field_30.h.field_32 = 0x100;
        d->w = 0x40;
        mask = 0xFFDDFFFF;
        flags = D_8009B0F4_abs & mask;
    image_stage:
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            register u8 *image_ptr asm("$3");

            D_8009B0F4_abs = flags;
            mask = 0x10000;
            d->mode = mask;
            D_8009B0F4_abs |= mask;
            d->done = 2;
            d->h = 0x10;
            image_ptr = D_8009B118;
            d->value_08 = (u32)image_ptr;
            d->value_0C = (u32)(image_ptr + 0x800);
            break;
        }
    }
}

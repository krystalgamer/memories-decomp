#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#include "file_transfer.h"

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern u8 *D_8009B118 __attribute__((section(".data")));
extern u8 *D_80010000 __attribute__((section(".data")));
extern u8 *D_800101DC __attribute__((section(".data")));
extern u8 D_800E9D70[100];
#define D_800E9D70 (*(RECT *)D_800E9D70)
extern u8 D_801A8000[];
extern u8 D_801A9800[];
extern u16 gDuel_awEquipTable[];
extern u16 gDuel_aFusionTable[];
extern u16 gDuel_awRitualData[];

void Duel_LoadPackageStage(FileTransferDescriptor *d, s32 stage)
{
    register u32 flags asm("$2");
    u32 mask;

    switch (stage) {
    case 0:
        d->counter = 0x300;
        d->field_32 = 0x100;
        d->field_04 = 0x40;
        d->field_06 = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        d->done = 2;
        d->mode = 0x20000;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + 0x800);
        break;
    case 1:
        d->mode = 0x2000;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_8009B118;
        d->done = 1;
        break;
    case 2:
        D_800E9D70.x = 0x100;
        D_800E9D70.y = 0xF0;
        D_800E9D70.w = 0x100;
        D_800E9D70.h = 0x10;
        LoadImage2(&D_800E9D70, (u32 *)D_8009B118);
        d->value_08 = d->value_0C = (u32)gDuel_awEquipTable;
        d->mode = 0x2800;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 3:
        d->value_08 = d->value_0C = (u32)gDuel_aFusionTable;
        d->mode = 0x10000;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 4:
        d->value_08 = d->value_0C = (u32)gDuel_awRitualData;
        d->mode = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 5:
        d->mode = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_8009B118;
        d->done = 1;
        break;
    case 6:
        D_800E9D70.x = 0;
        D_800E9D70.y = 0xF0;
        D_800E9D70.w = 0x100;
        D_800E9D70.h = 8;
        LoadImage2(&D_800E9D70, (u32 *)D_8009B118);
        d->counter = 0x200;
        d->field_32 = 0x100;
        d->field_04 = 0x40;
        mask = 0xFFDDFFFF;
        flags = D_8009B0F4 & mask;
        goto image_stage;
    case 7:
        mask = 0xFFDCFFFF;
        d->mode = 0x16000;
        D_8009B0F4 &= mask;
        d->value_08 = d->value_0C = (u32)D_800101DC;
        d->done = 1;
        break;
    case 8:
        d->value_08 = d->value_0C = (u32)D_801A8000;
        d->mode = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 9:
        d->value_08 = d->value_0C = (u32)D_801A9800;
        d->mode = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->done = 1;
        break;
    case 10:
        d->counter = 0x340;
        d->field_04 = 0x40;
        d->field_06 = 0x10;
        flags = D_8009B0F4 & 0xFFDDFFFF;
        D_8009B0F4 = flags;
        flags = D_8009B0F4;
        d->field_32 = 0;
        D_8009B0F4 = flags | 0x10000;
        d->done = 2;
        d->mode = 0x4000;
        d->value_08 = (u32)D_8009B118;
        d->value_0C = (u32)(D_8009B118 + 0x800);
        break;
    case 11:
        d->mode = 0x2800;
        D_8009B0F4 &= 0xFFDCFFFF;
        d->value_08 = d->value_0C = (u32)D_80010000;
        d->done = 1;
        break;
    case 12:
        d->counter = 0x280;
        d->field_32 = 0x100;
        d->field_04 = 0x40;
        mask = 0xFFDDFFFF;
        flags = D_8009B0F4 & mask;
    image_stage:
        {
            /* Keep this tail distinct from the $v0 tails in cases 0 and 10. */
            register u8 *image_ptr asm("$3");

            D_8009B0F4 = flags;
            mask = 0x10000;
            d->mode = mask;
            D_8009B0F4 |= mask;
            d->done = 2;
            d->field_06 = 0x10;
            image_ptr = D_8009B118;
            d->value_08 = (u32)image_ptr;
            d->value_0C = (u32)(image_ptr + 0x800);
            break;
        }
    }
}

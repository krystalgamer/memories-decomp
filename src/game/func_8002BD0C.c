#include "../types.h"
#include "file_transfer_descriptor_view.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

extern s32 D_8009B118 __attribute__((section(".data")));
extern u8 D_800E9D70[100];
extern void func_80048D08(s32 mode, s32 buffer);

#define gStageRect (*(RECT *)D_800E9D70)

void func_8002BD0C(FileTransferDescriptorView *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field30.h.lo = 0x300;
        object->field30.h.hi = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x20000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->field1C = 0x2000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field30.h.lo = 0x240;
        object->field30.h.hi = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x18000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 4:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field46 = 3;
        object->field30.w = 0x26810;
        object->field1C = 0xA000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 3:
    case 5:
        object->field1C = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 6:
        func_80048D08(1, D_8009B118);
        break;
    }
}

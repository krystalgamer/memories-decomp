#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "sound_voice_data.h"
#include "graphics_frame.h"

extern s32 D_8009B118 __attribute__((section(".data")));
#define gStageRect (D_800E9D70[0])

void func_8002BD0C(FileTransferDescriptor *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x20000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 1:
        object->mode = 0x2000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4_abs &= 0xFFDDFFFF;
        D_8009B0F4_abs |= 0x10000;
        object->done = 2;
        object->mode = 0x18000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 4:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)D_8009B118);
        object->done = 3;
        object->field_30.word = 0x26810;
        object->mode = 0xA000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 3:
    case 5:
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_8009B118;
        object->value_08 = D_8009B118;
        object->done = 1;
        break;

    case 6:
        func_80048D08(1, D_8009B118);
        break;
    }
}

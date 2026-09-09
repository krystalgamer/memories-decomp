#include "../types.h"
#include "model.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"

typedef struct {
    u32 w[7];
} Block28;

extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010000 __attribute__((section(".data")));
extern s32 D_80010014 __attribute__((section(".data")));
extern s32 D_80010018 __attribute__((section(".data")));
extern u8 D_801A8000[];
extern void func_8005B620(u8 *dst, u8 *src, s32 count);

void func_800577B0(FileTransferDescriptor *object, s32 mode) {
    RECT rect0;
    RECT rect1;
    u8 *dst;
    u8 *src;

    switch (mode) {
    case 0:
        object->mode = 0x30000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010000;
        object->value_08 = D_80010000;
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
        object->mode = 0x30000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 2:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->mode = 0x1000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 3:
        rect0.y = 0xF8;
        rect0.w = 0x100;
        rect0.x = 0;
        rect0.h = 8;
        LoadImage2(&rect0, (u32 *)D_801DD000);
        object->mode = 0x5000;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->value_0C = D_80010014;
        object->value_08 = D_80010014;
        object->done = 1;
        break;

    case 4:
        object->mode = 0x5000;
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
        object->mode = 0x4000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 7:
        object->value_0C = (s32)D_801A8000;
        object->value_08 = (s32)D_801A8000;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 8:
        object->done = 3;
        object->field_30.word = 0xD810;
        object->mode = 0x19000;
        object->value_08 = D_8009B118;
        object->value_0C = D_8009B118 + 0x800;
        break;

    case 5:
    case 9:
        object->value_0C = (s32)D_801DD000;
        object->value_08 = (s32)D_801DD000;
        object->mode = 0x800;
        D_8009B0F4_abs &= 0xFFDCFFFF;
        object->done = 1;
        break;

    case 10:
        dst = (u8 *)D_800F2C40;
        src = D_801DD000;
        func_8005B620(dst + 0xBF8, src, 0x40);
        *(Block28 *)(dst + 0xCF8) = *(Block28 *)(src + 0x100);
        *(s32 *)(dst + 0xD08) = -1;
        *(s32 *)(dst + 0xD0C) = -1;
        *(s32 *)(dst + 0xD10) = -1;
        *(s16 *)(dst + 0xCF8) = 0;
        *(s16 *)(dst + 0xCFA) = 0;
        dst[0xE14] = 1;
        break;
    }
}

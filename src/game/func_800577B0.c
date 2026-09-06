#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 field08;
    s32 field0C;
    u8 pad10[0xC];
    s32 field1C;
    u8 pad20[0x10];
    union {
        struct {
            s16 lo;
            s16 hi;
        } h;
        s32 w;
    } field30;
    u8 pad34[0x12];
    u8 field46;
} Object;

typedef struct {
    u32 w[7];
} Block28;

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010000 __attribute__((section(".data")));
extern s32 D_80010014 __attribute__((section(".data")));
extern s32 D_80010018 __attribute__((section(".data")));
extern u8 D_801A8000[];
extern u8 D_801DD000[];
extern u8 D_800F2C40[];
extern void func_8005B620(u8 *dst, u8 *src, s32 count);

void func_800577B0(Object *object, s32 mode) {
    RECT rect0;
    RECT rect1;
    u8 *dst;
    u8 *src;

    switch (mode) {
    case 0:
        object->field1C = 0x30000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_80010000;
        object->field08 = D_80010000;
        object->field46 = 1;
        break;

    case 1:
        object->field30.h.hi = 0x100;
        object->w = 0x40;
        object->field30.h.lo = 0;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x30000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 2:
        object->field0C = (s32)D_801DD000;
        object->field08 = (s32)D_801DD000;
        object->field1C = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 3:
        rect0.y = 0xF8;
        rect0.w = 0x100;
        rect0.x = 0;
        rect0.h = 8;
        LoadImage2(&rect0, (u32 *)D_801DD000);
        object->field1C = 0x5000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_80010014;
        object->field08 = D_80010014;
        object->field46 = 1;
        break;

    case 4:
        object->field1C = 0x5000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_80010018;
        object->field08 = D_80010018;
        object->field46 = 1;
        break;

    case 6:
        rect1.x = 0x100;
        rect1.y = 0xF0;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
        object->field30.h.lo = 0x180;
        object->w = 0x40;
        object->field30.h.hi = 0x100;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x4000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 7:
        object->field0C = (s32)D_801A8000;
        object->field08 = (s32)D_801A8000;
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 8:
        object->field46 = 3;
        object->field30.w = 0xD810;
        object->field1C = 0x19000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 5:
    case 9:
        object->field0C = (s32)D_801DD000;
        object->field08 = (s32)D_801DD000;
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 10:
        dst = D_800F2C40;
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

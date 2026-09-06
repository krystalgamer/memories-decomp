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
    s16 field30;
    s16 field32;
    u8 pad34[0x12];
    u8 field46;
} Object;

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B118 __attribute__((section(".data")));
extern s32 D_80010008 __attribute__((section(".data")));
extern u8 D_801DD000[];
extern u8 D_801DD800[];
extern u8 D_800F5694[];

void func_80057544(Object *object, s32 mode) {
    RECT rect0;
    RECT rect1;

    switch (mode) {
    case 0:
        object->field1C = 0x9000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_80010008;
        object->field08 = D_80010008;
        object->field46 = 1;
        break;

    case 1:
        object->field30 = 0x200;
        object->field32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x20000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 2:
        object->field0C = (s32)D_801DD800;
        object->field08 = (s32)D_801DD800;
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 3:
        rect0.x = 0x200;
        rect0.y = 0xF1;
        rect0.w = 0x100;
        rect0.h = 1;
        LoadImage2(&rect0, (u32 *)D_801DD800);
        object->field0C = (s32)(D_801DD800 - 0x800);
        object->field08 = (s32)(D_801DD800 - 0x800);
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 4:
        rect1.x = 0x200;
        rect1.y = 0xF4;
        rect1.w = 0x100;
        rect1.h = 2;
        LoadImage2(&rect1, (u32 *)D_801DD000);
        object->field30 = 0x380;
        object->field32 = 0;
        object->w = 0x40;
        object->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        object->field46 = 2;
        object->field1C = 0x10000;
        object->field08 = D_8009B118;
        object->field0C = D_8009B118 + 0x800;
        break;

    case 5:
        D_800F5694[0] = 1;
        break;
    }
}

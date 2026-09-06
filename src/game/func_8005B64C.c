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
extern s32 D_8001002C __attribute__((section(".data")));
extern u8 D_8009B058[];
extern u8 D_801DD000[];
extern u8 D_801AF800[];

void func_8005B64C(Object *object, s32 mode) {
    RECT rect;

    switch (mode) {
    case 0:
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

    case 1:
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

    case 2:
        object->field0C = (s32)D_801DD000;
        object->field08 = (s32)D_801DD000;
        object->field1C = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 3:
        rect = *(RECT *)D_8009B058;
        LoadImage2(&rect, (u32 *)D_801DD000);
        object->field1C = 0x8000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_8001002C;
        object->field08 = D_8001002C;
        object->field46 = 1;
        break;

    case 4:
        object->field0C = (s32)D_801AF800;
        object->field08 = (s32)D_801AF800;
        object->field1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;
    }
}

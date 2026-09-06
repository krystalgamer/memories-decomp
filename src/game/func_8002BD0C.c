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

extern volatile u32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B118 __attribute__((section(".data")));
extern u8 D_800E9D70[100];
extern void func_80048D08(s32 mode, s32 buffer);

#define gStageRect (*(RECT *)D_800E9D70)

void func_8002BD0C(Object *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field30.h.lo = 0x300;
        object->field30.h.hi = 0x100;
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
        object->field1C = 0x2000;
        D_8009B0F4 &= 0xFFDCFFFF;
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
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
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
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 6:
        func_80048D08(1, D_8009B118);
        break;
    }
}

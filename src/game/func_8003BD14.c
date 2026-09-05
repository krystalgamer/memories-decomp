#include "../types.h"

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

extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;
extern s32 D_800101D8;
extern u8 D_801A8000[];

extern void func_80081DE8(Object *object, s32 *data);

void func_8003BD14(Object *object, s32 mode) {
    switch (mode) {
    case 0:
        object->field30 = 0x300;
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
        object->field1C = 0x2000;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_8009B118;
        object->field08 = D_8009B118;
        object->field46 = 1;
        break;

    case 2:
        object->x = 0x100;
        object->y = 0xF0;
        object->w = 0x100;
        object->h = 0x10;
        func_80081DE8(object, (s32 *)D_8009B118);
        object->field0C = (s32)D_801A8000;
        object->field08 = (s32)D_801A8000;
        object->field1C = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field46 = 1;
        break;

    case 3:
        object->field1C = 0x7800;
        D_8009B0F4 &= 0xFFDCFFFF;
        object->field0C = D_800101D8;
        object->field08 = D_800101D8;
        object->field46 = 1;
        break;
    }
}

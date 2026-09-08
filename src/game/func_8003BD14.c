#include "../types.h"
#include "ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;
extern s32 D_800101D8;
extern u8 D_801A8000[];

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
        LoadImage2((RECT *)object, (u32 *)D_8009B118);
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

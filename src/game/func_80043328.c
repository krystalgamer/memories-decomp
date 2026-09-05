#include "../types.h"

typedef struct LocalObj {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 f8;
    s32 fC;
    u8 pad10[0x0C];
    s32 f1C;
    u8 pad20[0x10];
    s16 f30;
    s16 f32;
    u8 pad34[0x12];
    u8 f46;
} LocalObj;
extern u8 D_800101D8[];
extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;
extern int LoadImage2();

void func_80043328(LocalObj *obj, s32 mode) {
    switch (mode) {
    case 0:
        obj->f30 = 0x280;
        obj->f32 = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        obj->f1C = 0x18000;
        D_8009B0F4 |= 0x10000;
        obj->f46 = 2;
        obj->f8 = D_8009B118;
        obj->fC = D_8009B118 + 0x800;
        break;

    case 1:
        obj->f1C = 0x1000;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->fC = D_8009B118;
        obj->f8 = D_8009B118;
        obj->f46 = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2(obj, (s32 *)D_8009B118);
        obj->f1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->fC = D_8009B118 + 0x1000;
        obj->f8 = D_8009B118 + 0x1000;
        obj->f46 = 1;
        break;

    case 3:
        obj->x = 0x280;
        obj->y = 0xE8;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2(obj, (s32 *)(D_8009B118 + 0x1000));
        obj->f1C = 0x1800;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->fC = *(s32 *)(D_800101D8);
        obj->f8 = *(s32 *)(D_800101D8);
        obj->f46 = 1;
        break;
    }
}

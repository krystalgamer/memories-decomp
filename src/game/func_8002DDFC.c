#include "../types.h"

struct Obj {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 f8;
    s32 fC;
    char pad1[0x1C - 0x10];
    s32 f1C;
    char pad2[0x30 - 0x20];
    s16 f30;
    s16 f32;
    char pad3[0x38 - 0x34];
    s32 f38;
    char pad4[0x46 - 0x3C];
    u8 f46;
};

extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;
extern void LoadImage2(struct Obj *, s32 *);

void func_8002DDFC(struct Obj *obj, s32 mode)
{
    switch (mode) {
    case 0:
        obj->f32 = 0x100;
        obj->f30 = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        D_8009B0F4 |= 0x10000;
        obj->f46 = 2;
        obj->f1C = obj->f38 << 11;
        obj->f8 = D_8009B118;
        obj->fC = D_8009B118 + 0x800;
        break;

    case 1:
        obj->f1C = 0x800;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->fC = D_8009B118;
        obj->f8 = D_8009B118;
        obj->f46 = 1;
        break;

    case 2:
        obj->x = 0;
        obj->y = 0xF0;
        obj->w = 0x100;
        obj->h = 4;
        LoadImage2(obj, (s32 *)D_8009B118);
        break;
    }
}

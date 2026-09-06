#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

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

typedef struct {
    void *pointer;
    s16 value;
    u8 pad_06[14];
} ScriptImageEntry;

extern volatile u32 D_8009B0F4;
extern s32 D_8009B118;

extern u8 *func_80014EEC(s32, s32, s32, s32, void *, s32, s32);
extern void func_8004036C(void *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(void *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428EC(void *, s32);

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
        LoadImage2((RECT *)obj, (u32 *)D_8009B118);
        break;
    }
}

void func_8002DF2C(volatile u8 *owner, s32 value)
{
    s32 index;
    s32 mode;
    s32 stride;
    s32 base;
    u8 *object;

    if (owner) {
        *(s16 *)(owner + 0x3C) = value;
    }

    index = ((value >> 4) & 15) * 10 + (value & 15);
    mode = value >> 8;
    switch (mode) {
    case 0:
        stride = 0x21;
        base = 0;
        break;
    case 1:
        stride = 0x51;
        base = 0x672;
        break;
    case 2:
        stride = 0x71;
        base = 0x13BC;
        break;
    default:
        return;
    }
    object = func_80014EEC(
        0, 0, base + index * stride + 0x21D5, stride,
        func_8002DDFC, 0, 0
    );
    *(s32 *)(object + 0x38) = stride - 1;
    D_8009B0F4 = *(u32 *)(object + 0x2C) | 0x10;
}

void func_8002E00C(ScriptImageEntry *entries)
{
    s32 i;

    *(s16 *)((u8 *)entries + 0x3C) = -1;
    for (i = 0; i < 3; i++, entries++) {
        func_8004036C(entries->pointer);
        entries->pointer = 0;
        entries->value = 0;
    }
}

void func_8002E060(u8 *owner, s32 size, s32 mode)
{
    u8 *object = func_800400AC(func_8004002C(), 2);

    func_800404CC(object, 0, 0, 2, 0, 0, size, mode);
    func_800428EC(object, (s8)mode);
    owner[0x10] = 1;
    if (mode == 2) {
        *(s16 *)(owner + 4) = 1;
        *(u32 *)(object + 4) |= 0x50000000;
    } else {
        *(s16 *)(owner + 4) = 0;
        *(u32 *)(object + 4) |= 0x01000000;
    }
    *(void **)owner = object;
}

#include "../types.h"
#include "display_object_layout.h"

typedef float f32;
typedef double f64;
typedef char M2C_UNK;
typedef struct { s32 words[9]; } Mtx32;
typedef u8 State;
typedef u8 Record;
typedef u8 Pair;
typedef u8 Controller;
typedef u8 Entry;
typedef u8 Rec;
typedef u8 Block;
typedef struct { u32 words[2]; } Blk8;

extern s32 func_80035E20();
extern s32 func_80039140();
extern s32 func_8004002C();
extern s32 func_8004006C();
extern s32 func_800400AC();
extern s32 func_8004036C();
extern s32 func_800427DC();
extern s32 func_800428EC();
extern s32 func_80042918();

void func_800391E4(u8 *p) {
    u8 *e;
    s32 v;
    s32 b;
    s32 f;

    e = *(u8 **)(p + 0x28);
    if (e == (u8 *)0) {
        e = func_800400AC(func_8004006C(), 6);
        b = p[0x57];
        *(s16 *)(e + 0x40) = 0x280;
        *(s16 *)(e + 0x42) = 0xE8;
        e[0x66] = 0xA;
        f = *(u16 *)(e + 8);
        *(s32 *)(e + 0x4C) = (s32)func_80035E20;
        e[0x67] = b;
        *(s16 *)(e + 8) = f | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        *(s32 *)(p + 0x28) = (s32)e;
    }

    func_80042918(e);
    func_800428EC(e, *(s8 *)(p + 0x59));

    *(s16 *)(e + 0x30) = *(u16 *)(p + 0x3C);
    *(s16 *)(e + 0x32) = *(u16 *)(p + 0x40);
    *(s16 *)(e + 0x3C) = *(u16 *)(p + 0x3E);
    *(s16 *)(e + 0x3E) = *(u16 *)(p + 0x42);
    v = (s16)*(u16 *)(p + 0x3E) / 2;
    *(s16 *)(e + 0x18) = v;
    *(s16 *)(e + 0x48) = v;
    v = (s16)*(u16 *)(p + 0x42) / 2;
    *(s16 *)(e + 0x1A) = v;
    *(s16 *)(e + 0x4A) = v;

    if ((*(u16 *)(p + 0x34) & 0x20) != 0) {
        if (*(s32 *)(p + 0x2C) != 0) {
            func_8004036C(*(s32 *)(p + 0x2C));
        }
        e = func_800400AC(func_8004002C(), 4);
        func_800427DC(e, 1);
        *(s32 *)(p + 0x2C) = (s32)e;
        *(s32 *)(e + 4) = *(s32 *)(e + 4) | 0x60000000;
        func_80042918(e);
        func_800428EC(e, (s8)(p[0x59] - 1));
        *(s32 *)(e + 0x54) = 0xA0A0A0;
        *(s32 *)(e + 0x4C) = 0xA0A0A0;
        *(s32 *)(e + 0x34) = 0xA0A0A0;
        *(s32 *)(e + 0x2C) = 0xA0A0A0;
        *(s32 *)(e + 0x44) = 0x808080;
        *(s32 *)(e + 0x3C) = 0x808080;
        func_80039140(p);
    }
}

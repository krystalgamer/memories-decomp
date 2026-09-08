#include "../../../../src/types.h"

typedef struct {
    u32 field_0;
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s16 field_C;
    u8 field_E;
    u8 field_F;
    u16 field_10;
    s16 field_12;
    u32 field_14;
    u32 field_18;
    u32 field_1C;
    u32 field_20;
} Params;

typedef struct {
    u8 field_0;
    u8 field_1;
    u8 field_2;
    u8 field_3;
    u8 field_4;
    u8 field_5;
    u8 field_6;
    u8 field_7;
    u16 field_8;
    u8 field_A;
    u8 field_B;
} Ctx;

typedef struct {
    u32 field_0;
    u32 field_4;
} Extra;

typedef struct {
    u32 field_0;
    u32 field_4;
    u16 field_8;
    u16 field_A;
    u32 field_C;
    u32 field_10;
    s16 field_14;
    s16 field_16;
    s16 field_18;
    s16 field_1A;
    u32 field_1C;
    u32 field_20;
    u8 pad24[0x30 - 0x24];
    s16 field_30;
    s16 field_32;
    u8 pad34[0x40 - 0x34];
    u16 field_40;
    u16 field_42;
    u32 field_44;
} Win;

typedef struct {
    u32 field_0;
    s32 field_4;
    u8 pad8[0x20 - 0x8];
    u32 field_20;
    u8 pad24[0x30 - 0x24];
    u16 field_30;
    u16 field_32;
    u8 pad34[0x3C - 0x34];
    u32 field_3C;
    u32 field_40;
    u32 field_44;
    u8 pad48[0x54 - 0x48];
    Win *field_54;
    u8 pad58[0x5C - 0x58];
    u16 field_5C;
    u16 field_5E;
    u8 pad60[0x66 - 0x60];
    u8 field_66;
    u8 field_67;
    u8 field_68;
} Obj;

typedef struct {
    u8 pad0[0x32];
    s16 field_32;
    s16 field_34;
    s16 field_36;
    s16 field_38;
    u8 field_3A;
    u8 field_3B;
    u8 field_3C;
    u8 pad3D[0x40 - 0x3D];
} Rec;

extern Rec D_800EA0E8[];

extern s32 func_80041F90(Obj *, s32, s32, Extra *);
extern void func_80042188(Params *, Ctx *, s32, s32, Extra *);
extern void Text_EncodeDecimalDigits(s32, s32, u8 *);

void func_80028B08(Obj *obj, s32 arg1) {
    u8 buf1[5];
    u8 buf2[5];
    Extra *EXT;
    Params *PRM;
    Ctx *CTX;
    Win *win;
    Rec *rec;
    s32 arg;
    s32 i;
    u16 flags;
    u32 f4;
    u32 tile;
    s32 white;
    s32 wrap;

    white = 0xF8;
    wrap = 0xFFFF;
    win = obj->field_54;
    if (obj->field_4 < 0) {
        return;
    }
    flags = win->field_8;
    if ((flags & 0x40) == 0) {
        return;
    }
    CTX = (Ctx *)0x1F800344;
    PRM = (Params *)0x1F800320;
    EXT = (Extra *)0x1F800398;
    arg = ((win->field_14 - 1) & 0xFFFF) | 0x10000;
    if (flags & 0x4) {
        obj->field_20 = win->field_20;
        f4 = obj->field_4 & 0xF7FFFFFF;
        obj->field_44 = win->field_44;
        obj->field_4 = f4;
        f4 = f4 | (win->field_4 & 0x08000000);
        obj->field_4 = f4;
        if (func_80041F90(
                obj, win->field_30 + win->field_18,
                win->field_32 + win->field_1A, EXT
            ) <= 0) {
            return;
        }
        CTX->field_3 = 9;
        arg = ((win->field_14 - 1) & 0xFFFF) | 0xF0000;
        *(u32 *)&CTX->field_4 = win->field_C;
        CTX->field_7 = 0x2C;
    }

    PRM->field_0 = obj->field_4;
    PRM->x = win->field_30 + 0x13;
    PRM->w = 0x66;
    PRM->h = 0x60;
    PRM->y = win->field_32 + 0x32;
    PRM->field_14 = win->field_C;
    *(u32 *)&PRM->field_10 = obj->field_40;
    *(u16 *)&PRM->field_E = obj->field_5C;
    PRM->field_C = obj->field_66;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    CTX->field_7 = CTX->field_7 | 2;
    PRM->x = win->field_30 + 0xC;
    PRM->w = 0x60;
    PRM->h = 0xE;
    PRM->field_12 = white;
    PRM->field_F = PRM->field_F + 0x60;
    PRM->field_0 = (PRM->field_0 & 0xFEFFFFFF) | 0x60000000;
    PRM->field_10 = 0x1E0;
    PRM->y = win->field_32 + 0xE;
    func_80042188(PRM, CTX, arg1, arg, EXT);

    EXT->field_4 = 0;
    PRM->field_C = 0x1F;
    PRM->field_10 = PRM->field_10 + 0x10;
    PRM->x = win->field_30 + obj->field_30;
    PRM->y = win->field_32 + obj->field_32;
    *(u32 *)&PRM->w = obj->field_3C;
    *(u16 *)&PRM->field_E = obj->field_5E;
    rec = &D_800EA0E8[obj->field_67];
    if (obj->field_68 < 0x14) {
        if (rec->field_3C & 0x80) {
            PRM->field_12 = PRM->field_12 + 1;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->field_12 = white;
        PRM->field_F = PRM->field_F + *(u8 *)&PRM->h;
        PRM->y = PRM->y + (PRM->h + wrap);
        if (rec->field_3C & 0x40) {
            PRM->field_12 = 0xF9;
        }
        func_80042188(PRM, CTX, arg1, arg, EXT);
        PRM->field_12 = white;

        i = rec->field_32 + rec->field_36;
        if (i >= 10000) {
            i = 9999;
        }
        Text_EncodeDecimalDigits(i, 4, buf1);
        i = rec->field_34 + rec->field_38;
        if (i >= 10000) {
            i = 9999;
        }
        Text_EncodeDecimalDigits(i, 4, buf2);

        PRM->field_F = (PRM->field_F & 0x80) + 0x10;
        PRM->x = win->field_30 + 0x61;
        PRM->y = win->field_32 + 0x9D;
        *(u32 *)&PRM->w = 0x000D0006;
        if (rec->field_3C & 0x80) {
            PRM->field_12 = 0xF9;
        }
        i = 3;
        do {
            PRM->field_E = buf1[i] * 6 + 0x10;
            func_80042188(PRM, CTX, arg1, arg, EXT);
            PRM->x = PRM->x + 6;
            i--;
        } while (i >= 0);

        PRM->x = win->field_30 + 0x61;
        PRM->field_12 = white;
        PRM->y = win->field_32 + 0xAB;
        if (rec->field_3C & 0x40) {
            PRM->field_12 = 0xF9;
        }
        i = 3;
        do {
            PRM->field_E = buf2[i] * 6 + 0x10;
            func_80042188(PRM, CTX, arg1, arg, EXT);
            PRM->x = PRM->x + 6;
            i--;
        } while (i >= 0);

        PRM->x = win->field_30 + 0x77;
        PRM->field_10 = 0x1C0;
        *(u32 *)&PRM->w = 0x00090009;
        PRM->field_E = 0;
        PRM->field_12 = white;
        PRM->y = win->field_32 + 0x20;
        if (rec->field_3A != 0) {
            i = 0;
            do {
                func_80042188(PRM, CTX, arg1, arg, EXT);
                PRM->x = PRM->x - 9;
                i++;
            } while (i < (s32)rec->field_3A);
        }
    } else {
        func_80042188(PRM, CTX, arg1, arg, EXT);
    }

    PRM->x = win->field_30 + 0x6E;
    *(u32 *)&PRM->w = 0x00100010;
    PRM->y = win->field_32 + 0xD;
    tile = rec->field_3B << 4;
    PRM->field_E = tile;
    PRM->field_F = PRM->field_F & 0x80;
    PRM->field_12 = 0xFF;
    PRM->field_10 = win->field_40 + (u8)tile;
    func_80042188(PRM, CTX, arg1, arg, EXT);
}

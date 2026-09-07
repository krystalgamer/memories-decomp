#include "../../types.h"

typedef struct {
    u8 t0, t1, t2, len;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 r1, g1, b1, p1;
    s16 x1, y1;
    u8 r2, g2, b2, p2;
    s16 x2, y2;
    u8 r3, g3, b3, p3;
    s16 x3, y3;
} POLY_G4;

typedef struct {
    u8 t0, t1, t2, len;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u8 r1, g1, b1, p1;
    s16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    u8 r2, g2, b2, p2;
    s16 x2, y2;
    u8 u2, v2;
    u16 pad2;
    u8 r3, g3, b3, p3;
    s16 x3, y3;
    u8 u3, v3;
    u16 pad3;
} POLY_GT4;

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad0A[36 - 10];
    s32 f24;
    u8 pad28[48 - 40];
    s16 f30;
    s16 f32;
} Widget;

extern Widget *D_801845B0[];
extern u8 D_801845BC[];
extern u16 D_801845C0[];
extern u32 *D_800E9D90[];

extern s32 MainMenu_CountDecimalDigits(s32);
extern void func_80084320(void *, u32 *, s32);

void func_80181728(void)
{
    POLY_GT4 digit;
    POLY_G4 bar;
    Widget *w;
    Widget *mk;
    s32 first;
    s32 second;
    s32 x;
    s32 i;
    s32 count;
    s32 width;
    s32 c106;
    s32 c114;
    s32 c112;
    s32 c120;
    s32 d134;
    s32 d142;
    s32 d112;
    s32 d120;

    first = D_801845C0[1];
    second = D_801845C0[7];
    mk = D_801845B0[2];
    if (D_801845BC[2] == 0) {
        x = 116;
    } else {
        x = 220;
    }
    mk->f30 = x;
    D_801845B0[2]->f32 = 74;

    w = D_801845B0[0];
    if (w->f24 != 0 || D_801845BC[0] == 2) {
        w->f8 |= 0x40;
        w = D_801845B0[0];
        if (w->f24 == 0) {
            w->f30 = first * 128 / 8000 + 176;
            D_801845B0[0]->f32 = 111;
        }
    } else {
        w->f8 &= 0xFFBF;
        D_801845B0[0]->f30 = D_801845B0[2]->f30;
        D_801845B0[0]->f32 = D_801845B0[2]->f32;
    }

    w = D_801845B0[1];
    if (w->f24 != 0 || D_801845BC[1] == 2) {
        w->f8 |= 0x40;
        w = D_801845B0[1];
        if (w->f24 == 0) {
            w->f30 = second * 128 / 8000 + 176;
            D_801845B0[1]->f32 = 139;
        }
    } else {
        w->f8 &= 0xFFBF;
        D_801845B0[1]->f30 = D_801845B0[2]->f30;
        D_801845B0[1]->f32 = D_801845B0[2]->f32;
    }

    bar.len = 8;
    bar.code = 56;
    bar.r0 = 64;
    bar.g0 = 32;
    bar.b0 = 32;
    bar.r1 = 255;
    bar.g1 = 32;
    bar.b1 = 32;
    bar.r2 = 64;
    bar.g2 = 32;
    bar.b2 = 32;
    bar.r3 = 255;
    bar.g3 = 32;
    bar.b3 = 32;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 107;
    bar.y1 = 107;
    bar.y2 = 115;
    bar.y3 = 115;
    bar.x1 = first * 128 / 8000 + 176;
    bar.x3 = bar.x1;
    func_80084320(&bar, D_800E9D90[2], 2048);

    bar.r0 = 32;
    bar.g0 = 32;
    bar.b0 = 64;
    bar.r1 = 32;
    bar.g1 = 32;
    bar.b1 = 255;
    bar.r2 = 32;
    bar.g2 = 32;
    bar.b2 = 64;
    bar.r3 = 32;
    bar.g3 = 32;
    bar.b3 = 255;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 135;
    bar.y1 = 135;
    bar.y2 = 143;
    bar.y3 = 143;
    bar.x1 = second * 128 / 8000 + 176;
    bar.x3 = bar.x1;
    func_80084320(&bar, D_800E9D90[2], 2048);

    digit.len = 12;
    digit.code = 60;
    digit.tpage = 11;
    digit.clut = 16041;
    digit.r0 = 255;
    digit.g0 = 255;
    digit.b0 = 255;
    digit.r1 = 255;
    digit.g1 = 255;
    digit.b1 = 255;
    digit.r2 = 255;
    digit.g2 = 255;
    digit.b2 = 255;
    digit.r3 = 255;
    digit.g3 = 255;
    digit.b3 = 255;

    width = MainMenu_CountDecimalDigits(8000);
    count = MainMenu_CountDecimalDigits(first);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        c106 = 106;
        c114 = 114;
        c112 = 112;
        c120 = 120;
        digit.y0 = c106;
        digit.x1 = digit.x0 + 8;
        digit.y1 = c106;
        digit.x2 = digit.x0;
        digit.y2 = c114;
        digit.x3 = digit.x1;
        digit.y3 = c114;
        digit.v0 = c112;
        digit.v1 = c112;
        digit.v2 = c120;
        digit.v3 = c120;
        digit.u0 = (first % 10) * 8 - 128;
        digit.u1 = (first % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        func_80084320(&digit, D_800E9D90[2], 2048);
        first = first / 10;
    }

    count = MainMenu_CountDecimalDigits(second);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        d134 = 134;
        d142 = 142;
        d112 = 112;
        d120 = 120;
        digit.y0 = d134;
        digit.x1 = digit.x0 + 8;
        digit.y1 = d134;
        digit.x2 = digit.x0;
        digit.y2 = d142;
        digit.x3 = digit.x1;
        digit.y3 = d142;
        digit.v0 = d112;
        digit.v1 = d112;
        digit.v2 = d120;
        digit.v3 = d120;
        digit.u0 = (second % 10) * 8 - 128;
        digit.u1 = (second % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        func_80084320(&digit, D_800E9D90[2], 2048);
        second = second / 10;
    }
}

#include "../types.h"

typedef void (*ObjFn)(u8 *);

extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;
extern u8 D_800E9D90[];
extern s16 D_800EFE40[];
extern s16 D_800EFE42[];
extern u8 D_800EFE48[];

s32 func_80041E7C(s32 arg0, s32 arg1, s32 arg2, u8 *arg3);
void func_80042188(s32 arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

/* Walks the 0x70-byte object list from the head index at D_800EFE40, calls
 * each object's callback, and for every visible object copies its vertices
 * into the scratchpad quad at 0x1F800344, offsets them by the viewport origin
 * unless bit 3 is set, runs the bit-2 clip test through
 * func_80041E7C, and submits the quad -- twice when the +0x5A flag asks for
 * the second texture -- through func_80042188. */
void func_80040DD8(void) {
    u8 *g;
    u8 *h;
    u8 *e;
    u8 *tb;
    ObjFn fn;
    s32 eight;
    s32 hi;
    s32 bit;
    s32 v;
    s32 i;
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 fl;
    s32 dx;
    s32 dy;
    s32 x0;
    s32 x1;
    s32 x2;

    g = (u8 *)0x1F800344;
    h = (u8 *)0x1F800398;
    i = D_800EFE40[0];

    if (i >= 0) {
        eight = 8;
        hi = 0x38;
        tb = D_800E9D90;
        bit = 0x40000;

        do {
            e = (u8 *)D_800EFE48 + i * 0x70;
            fn = *(ObjFn *)(e + 0x24);
            i = *(s16 *)(e + 2);
            if (fn != (ObjFn)0) {
                fn(e);
            }
            if (((*(u16 *)(e + 8) & 0xC0) ^ 0xC0) == 0) {
                v = *(s32 *)(e + 4);
                w0 = *(s32 *)(e + 0x28);
                w1 = *(s32 *)(e + 0x30);
                w2 = *(s32 *)(e + 0x38);
                w3 = *(s32 *)(e + 0x40);
                *(s32 *)(g + 8) = w0;
                *(s32 *)(g + 0x10) = w1;
                *(s32 *)(g + 0x18) = w2;
                *(s32 *)(g + 0x20) = w3;
                *(s32 *)(g + 4) = *(s32 *)(e + 0x2C);
                *(s32 *)(g + 0xC) = *(s32 *)(e + 0x34);
                *(s32 *)(g + 0x14) = *(s32 *)(e + 0x3C);
                w4 = *(s32 *)(e + 0x44);
                g[3] = eight;
                g[7] = hi;
                fl = *(u16 *)(e + 8);
                *(s32 *)(g + 0x1C) = w4;

                if ((fl & 8) == 0) {
                    dx = gGraphics_sViewportX;
                    *(u16 *)(g + 8) = *(u16 *)(g + 8) - dx;
                    *(u16 *)(g + 0x10) = *(u16 *)(g + 0x10) - dx;
                    *(u16 *)(g + 0x18) = *(u16 *)(g + 0x18) - dx;
                    *(u16 *)(g + 0x20) = *(u16 *)(g + 0x20) - dx;
                    dx = gGraphics_sViewportY;
                    *(u16 *)(g + 0xA) = *(u16 *)(g + 0xA) - dx;
                    *(u16 *)(g + 0x12) = *(u16 *)(g + 0x12) - dx;
                    *(u16 *)(g + 0x1A) = *(u16 *)(g + 0x1A) - dx;
                    *(u16 *)(g + 0x22) = *(u16 *)(g + 0x22) - dx;
                }

                if ((*(u16 *)(e + 8) & 4) != 0) {
                    if (func_80041E7C(*(s32 *)(e + 0x20),
                                      (s16)*(u16 *)(g + 8) + *(s16 *)(e + 0x18),
                                      (s16)*(u16 *)(g + 0xA) +
                                          *(s16 *)(e + 0x1A),
                                      h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                func_80042188(v, g, *(s32 *)(tb + e[0x17] * 4),
                              *(u16 *)(e + 0x14) | bit, h);

                if (e[0x5A] != 0) {
                    x0 = *(s32 *)(e + 0x48);
                    x1 = *(s32 *)(e + 0x50);
                    *(s32 *)(g + 8) = x0;
                    *(s32 *)(g + 0x10) = x1;
                    *(s32 *)(g + 4) = *(s32 *)(e + 0x4C);
                    x2 = *(s32 *)(e + 0x54);
                    g[3] = eight;
                    g[7] = hi;
                    *(s32 *)(g + 0xC) = x2;
                    func_80042188(v, g, *(s32 *)(tb + e[0x17] * 4),
                                  *(u16 *)(e + 0x14) | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}

/* The 12-wide, 0x3C-high sibling renderer for the list rooted at
 * D_800EFE42. */
void func_80041068(void) {
    u8 *g;
    u8 *h;
    u8 *e;
    u8 *tb;
    ObjFn fn;
    s32 twelve;
    s32 hi;
    s32 bit;
    s32 v;
    s32 i;
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 fl;
    s32 dx;
    s32 x0;
    s32 x1;
    s32 x2;

    g = (u8 *)0x1F800344;
    h = (u8 *)0x1F800398;
    i = D_800EFE42[0];

    if (i >= 0) {
        twelve = 0xC;
        hi = 0x3C;
        tb = D_800E9D90;
        bit = 0x50000;

        do {
            e = (u8 *)D_800EFE48 + i * 0x70;
            fn = *(ObjFn *)(e + 0x24);
            i = *(s16 *)(e + 2);
            if (fn != (ObjFn)0) {
                fn(e);
            }
            if (((*(u16 *)(e + 8) & 0xC0) ^ 0xC0) == 0) {
                v = *(s32 *)(e + 4);
                w0 = *(s32 *)(e + 0x28);
                w1 = *(s32 *)(e + 0x34);
                w2 = *(s32 *)(e + 0x40);
                w3 = *(s32 *)(e + 0x4C);
                *(s32 *)(g + 8) = w0;
                *(s32 *)(g + 0x14) = w1;
                *(s32 *)(g + 0x20) = w2;
                *(s32 *)(g + 0x2C) = w3;
                *(s32 *)(g + 4) = *(s32 *)(e + 0x2C);
                *(s32 *)(g + 0x10) = *(s32 *)(e + 0x38);
                *(s32 *)(g + 0x1C) = *(s32 *)(e + 0x44);
                *(s32 *)(g + 0x28) = *(s32 *)(e + 0x50);
                *(u16 *)(g + 0xC) = *(u16 *)(e + 0x30);
                *(u16 *)(g + 0x18) = *(u16 *)(e + 0x3C);
                *(u16 *)(g + 0x24) = *(u16 *)(e + 0x48);
                w4 = *(u16 *)(e + 0x54);
                g[3] = twelve;
                g[7] = hi;
                fl = *(u16 *)(e + 8);
                *(u16 *)(g + 0x30) = w4;

                if ((fl & 8) == 0) {
                    dx = gGraphics_sViewportX;
                    *(u16 *)(g + 8) = *(u16 *)(g + 8) - dx;
                    *(u16 *)(g + 0x14) = *(u16 *)(g + 0x14) - dx;
                    *(u16 *)(g + 0x20) = *(u16 *)(g + 0x20) - dx;
                    *(u16 *)(g + 0x2C) = *(u16 *)(g + 0x2C) - dx;
                    dx = gGraphics_sViewportY;
                    *(u16 *)(g + 0xA) = *(u16 *)(g + 0xA) - dx;
                    *(u16 *)(g + 0x16) = *(u16 *)(g + 0x16) - dx;
                    *(u16 *)(g + 0x22) = *(u16 *)(g + 0x22) - dx;
                    *(u16 *)(g + 0x2E) = *(u16 *)(g + 0x2E) - dx;
                }

                if ((*(u16 *)(e + 8) & 4) != 0) {
                    if (func_80041E7C(*(s32 *)(e + 0x20),
                                      (s16)*(u16 *)(g + 8) + *(s16 *)(e + 0x18),
                                      (s16)*(u16 *)(g + 0xA) +
                                          *(s16 *)(e + 0x1A),
                                      h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                func_80042188(v, g, *(s32 *)(tb + e[0x17] * 4),
                              *(u16 *)(e + 0x14) | bit, h);

                if (e[0x72] != 0) {
                    x0 = *(s32 *)(e + 0x58);
                    x1 = *(s32 *)(e + 0x64);
                    *(s32 *)(g + 8) = x0;
                    *(s32 *)(g + 0x14) = x1;
                    *(s32 *)(g + 4) = *(s32 *)(e + 0x5C);
                    *(s32 *)(g + 0x10) = *(s32 *)(e + 0x68);
                    *(u16 *)(g + 0xC) = *(u16 *)(e + 0x60);
                    x2 = *(u16 *)(e + 0x6C);
                    g[3] = twelve;
                    g[7] = hi;
                    *(u16 *)(g + 0x18) = x2;
                    func_80042188(v, g, *(s32 *)(tb + e[0x17] * 4),
                                  *(u16 *)(e + 0x14) | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}

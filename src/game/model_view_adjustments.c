#include "../types.h"
#include "model.h"

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

extern s16 D_8009B47C;
extern u8 D_8009B478;
extern u8 D_800F2B20[];
extern s16 D_800F2B4A[];
extern u8 D_800F56F0[];
extern s32 func_80051350(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_8005F1B8(s32 arg0, s32 arg1);
extern void func_80058434(
    s32 arg0,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4
);

void func_80052528(void) {
    u8 *b;
    s32 x;
    s32 f;
    s32 v;
    s32 lo;
    s32 c;
    s32 d;

    if (*(s16 *)&D_8009B47C >= 0x801) {
        b = D_800F56F0;
        x = *(s32 *)(b + 0x10);
        f = 0;
        if (x >= -0xC7 && *(s32 *)(b + 4) < x) {
            if (x < -0x64) {
                f = (-200 - x) * MODEL_FIXED_ONE / -100;
            } else {
                f = MODEL_FIXED_ONE;
            }
        }

        v = (f * 3755 + (MODEL_FIXED_ONE - f) * 3968) /
            MODEL_FIXED_ONE;
        lo = v - 0x10;

        do {
            c = *(s16 *)&D_8009B47C;
            d = v + 0x10;
        } while (0);

        if (d < c) {
            d = d - c;
            if (d < -1) {
                d = -1;
            }
            func_80058434(1, 0, d, 0, 0);
        } else if (c < 0xEAB) {
            func_80058434(1, 0, 0xEAB - c, 0, 0);
        } else if (c < lo) {
            d = lo - c;
            if (d >= 5) {
                d = 4;
            }
            func_80058434(1, 0, d, 0, 0);
        }
    } else {
        func_80058434(1, 0, -4, 0, 0);
    }
}

void func_80052694(s32 arg0) {
    u8 *e;
    u8 *g;
    s32 n;
    s32 lo;
    s32 hi;
    s32 z;
    s32 d;
    s32 t;

    n = D_800F2B4A[0];
    lo = 0x2BC;
    hi = 0x3E8;
    z = 0;

    if (n >= 0) {
        lo = func_8005F1B8(n, lo);
        if (lo >= hi) {
            hi = func_8005F1B8(n, hi);
        }
    }

    if (func_80051350(1, 0x96, 0) != 0) {
        e = D_800F2B20;
        if (*(u16 *)(e + 4) < 0x10) {
            *(u16 *)(e + 4) = *(u16 *)(e + 4) + 1;
        }
        e[1] = e[1] & 0xFE;
    } else {
        g = D_800F2B20;
        if (*(u16 *)(g + 6) < *(u16 *)(g + 4)) {
            *(u16 *)(g + 4) = *(u16 *)(g + 4) - 1;
        }
    }

    func_80052528();

    t = *(s16 *)&D_8009B478;
    if (t < 0x12C) {
        if (t < 0x96) {
            d = 0x96 - t;
            func_80058434(1, z, 0, d, 0);
        } else {
            d = 0x12C - t;
            if (d >= 0xB) {
                d = 0xA;
            }
            func_80058434(1, z, 0, d, 0);
        }
    } else if (t < lo) {
        d = lo - t;
        if (d >= 6) {
            d = 5;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    if (arg0 != 0) {
        d = *(s16 *)&D_8009B478;
        if (hi < d) {
            goto last;
        }
        if (lo < d) {
            d = lo - d;
            if (d < -0xA) {
                d = -0xA;
            }
            func_80058434(1, 0, 0, d, 0);
        }
    }

    if (hi < *(s16 *)&D_8009B478) {
last:
        d = hi - *(s16 *)&D_8009B478;
        if (d < -0x14) {
            d = -0x14;
        }
        func_80058434(1, 0, 0, d, 0);
    }

    func_80052528();
}

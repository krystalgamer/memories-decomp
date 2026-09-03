#include "../types.h"

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

extern s32 func_80040410();
extern s32 func_80040424();
extern s32 rand();

void func_8003B378(u8 *p, s32 n) {
    s32 f;
    s32 g;
    s32 c;
    s32 v;

    f = p[0x32];
    if ((f & 0x10) != 0) {
        p[0x32] = f & 0xFC;
        return;
    }

    if (*(u8 **)(p + 4) != 0) {
        if ((f & 1) != 0) {
            if (*(s16 *)(*(u8 **)(p + 4) + 0x5A) == 0) {
                *(s16 *)(p + 0x3E) = (rand() & 0xFF) + 0x3C;
                p[0x32] = p[0x32] & 0xFE;
                func_80040410(*(u8 **)(p + 4), 0);
            }
        } else {
            v = *(u16 *)(p + 0x3E) - 1;
            *(s16 *)(p + 0x3E) = v;
            if ((s16)v <= 0) {
                p[0x32] = p[0x32] | 1;
                func_80040410(*(u8 **)(p + 4), 1);
            }
        }
    }

    if (*(u8 **)(p + 8) == 0) {
        return;
    }

    g = p[0x32];
    if ((g & 2) != 0) {
        c = p[0x3B] - 1;
        p[0x3B] = c;
        if (*(s16 *)(*(u8 **)(p + 8) + 0x5A) != 0) {
            return;
        }
        if ((s8)c > 0) {
            return;
        }
        if (n <= 0) {
            p[0x3B] = 0;
            p[0x32] = p[0x32] & 0xFD;
            return;
        }
    } else {
        if (n < 0) {
            goto slow;
        }
        p[0x32] = g | 2;
    }

    p[0x3B] = 6;
    p[0x3A] = n;
    func_80040410(*(u8 **)(p + 8), n);
    return;

slow:
    p[0x3B] = p[0x3B] + 1;
    if ((s8)p[0x3B] < 6) {
        return;
    }
    func_80040424(*(u8 **)(p + 8), 0);
}

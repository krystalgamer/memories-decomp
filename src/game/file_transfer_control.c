#include "../types.h"
#include "file_constants.h"

extern volatile u16 D_8009B124;
extern volatile u16 D_8009B112;
extern volatile s32 D_8009B0E8;
extern volatile s32 D_8009B0F4;
extern s32 D_8009B134;
extern u8 D_800E9E18[];
extern u8 D_800E9E60[];
extern u8 D_800E9EC0[];

extern void File_ActivateTransfer(void);
extern void func_8001455C(void);
extern u8 *File_InitTransferDescriptor(
    u8 *, s32, u8 *, s32, s32, void *, s32, s32
);

void func_80014A5C(s32 arg0)
{
    if (D_8009B124 != 0) {
        D_8009B124 = 0;
        return;
    }
    D_8009B124 = 1;
    if (D_8009B0E8 != 0) {
        return;
    }
    D_8009B0E8 = 1;
    if (!(D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x20)) {
        File_ActivateTransfer();
    }
    if (D_8009B0F4 & 0x10) {
        if (D_8009B134 != 0 && !(D_8009B134 & 0x40)) {
            D_8009B134 |= 0x40;
            D_800E9E60[0x46] = 5;
            D_800E9E60[0x47] = 0;
        }
        func_8001455C();
    } else {
        D_8009B134 = 0;
    }
    D_8009B0E8 = 0;
}

typedef struct {
    u8 gap0[12];
    s32 value_c;
    s32 value_10;
    s32 value_14;
    s32 value_18;
} Shared14B30;

typedef struct {
    u8 gap0[8];
    s32 value_8;
    s32 value_c;
    u8 gap10[12];
    s32 value_1c;
    u8 gap20[16];
    s32 value_30;
    u8 gap34[12];
    s32 value_40;
    u8 gap44[2];
    u8 mode_46;
} Object14B30;

extern Shared14B30 D_801D4200;
extern u8 D_801D4200_raw[] asm("D_801D4200");
extern s32 D_8009B118;
extern void (*D_8009B128)(void);
extern void func_80014B30_callback(void) asm("func_80014B30");

typedef struct {
    s32 w[8];
} Blk32;

void func_80014B30(Object14B30 *object, s32 mode)
{
    Shared14B30 *shared;
    s32 value;
    s32 base;
    s32 position;
    void (*callback)(void);

    shared = &D_801D4200;
    if (mode == 1)
        goto reduced;
    if (mode >= 2)
        goto high;
    if (mode == 0)
        goto full;
    return;
high:
    if (mode == 2)
        goto tail;
    return;
full:
    if (shared->value_14 == 0) {
        object->value_40 = 2;
        goto reduced;
    }
    object->mode_46 = 3;
    base = D_8009B118;
    object->value_8 = base;
    object->value_c = base + FILE_SECTOR_SIZE;
    object->value_30 = shared->value_c;
    value = shared->value_14;
    object->value_1c = value;
    goto fix;
reduced:
    if (shared->value_18 == 0)
        goto tail;
    position = shared->value_10;
    D_8009B0F4 &= 0xFFDCFFFF;
    object->value_c = position;
    object->value_8 = position;
    object->mode_46 = 1;
    value = shared->value_18;
    object->value_1c = value;
fix:
    if (value < 0)
        object->value_1c = -(value << FILE_SECTOR_SHIFT);
    return;
tail:
    callback = D_8009B128;
    if (callback != 0)
        callback();
}

s32 func_80014C40(u8 *p, u8 *q) {
    u8 *e;
    u8 *r;
    void *f;
    s32 a;
    s32 b;
    s32 c;
    s32 n;
    s32 m;
    s32 v;
    s32 w;
    s32 t;

    if (p == (u8 *)0) {
        return D_8009B0F4 & 0x20;
    }

    a = *(s32 *)(p + 0x14);
    b = *(s32 *)(p + 0x18);
    c = *(s16 *)(p + 0x1C);

    if ((a | b | c) == 0) {
        return (s32)func_80013A94(*(s32 *)p, *(s32 *)(p + 4));
    }

    if (c != 0) {
        if (c < 0) {
            D_8009B112 = D_8009B112 & 0x3FFC;
            D_8009B112 = D_8009B112 | 2;
            return 1;
        }
        n = *(s32 *)D_800E9EC0 + *(s32 *)(p + 4);
        return func_80013B68(n, n + c, p[0x1F], p[0x1E]);
    }

    a = a + b;
    if (a == 0) {
        return 0;
    }

    {
        w = *(s32 *)p;
        /* Borrowed local: `m`'s real assignment is three lines down and this
         * one is dead, but it ties the negation's pseudo to m's allocation
         * and rotates $s2/$s3/$s4 into retail's order. A fresh name, six
         * declaration orders and four statement orders are all 6. */
        m = a;
        t = -m;
        r = D_801D4200_raw;
        *(Blk32 *)(r + 0x20) = *(Blk32 *)p;
        m = *(s32 *)(p + 4);
        D_8009B0F4 = D_8009B0F4 & ~0x20;
        v = w | 0x1400000;
        f = func_80014B30_callback;

        if ((D_8009B0F4 & 0x10) != 0) {
            if ((D_8009B0F4 & 0x80000) != 0) {
                func_80015010();
            }
        }

        File_InitTransferDescriptor(D_800E9E18, v, q, m, t, f, 0, (s32)p);
        e = D_800E9E18;
        D_8009B0F4 = D_8009B0F4 | 0x20;
    }

    return (s32)e;
}

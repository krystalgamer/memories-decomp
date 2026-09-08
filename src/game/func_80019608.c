#include "../types.h"

extern u8 D_801A7AD8[];
extern u8 *D_800E9EF0[];
extern u8 *D_8009B1C8;
extern u16 D_8009B23A;
extern u8 D_8009B174;
/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what gives the
   store below the retail load-delay nop. c_symbols.ld overrides the common
   symbol, so no storage is allocated here. */
u16 D_8009B150;
extern u32 D_8009B0F4 __attribute__((section(".data")));
extern u32 D_8009B134 __attribute__((section(".data")));

extern void func_80029164(s32, s32);
extern void func_80024914(u8 *);
extern void func_80026BA4(s32, s32);
extern u8 *func_800291E0(s32, s32, s32);
extern void func_800428EC(u8 *, s32);
extern void func_8004036C(u8 *);
extern void func_8001944C(u8 *);
extern u8 *func_80019564(u8 *);
extern void func_80029528(s32);

void func_80019608(void)
{
    u8 *p;
    u8 *slot;
    u8 *q0;
    u8 *q1;
    u8 state;
    u16 flags;
    u16 f2;
    u16 f3;
    u8 st;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 arg;
    /* The resize branch below is the one place where no source shape reaches
       retail's allocation: unpinned, the field read and the offset value share
       a register and the two independent values in the fade-out do not. The
       matched siblings func_8001944C.c and func_80037DA4.c use the same
       device.  v4 and v5 share $5 because their live ranges are disjoint,
       which is what retail does. */
    register s32 fld __asm__("$3");
    register s32 v4 __asm__("$5");
    register s32 v5 __asm__("$5");

    p = D_800E9EF0[0];
    flags = D_8009B23A;
    if ((flags & 0x8000) == 0) {
        D_8009B23A = flags | 0xC000;
        slot = &D_801A7AD8[p[0x6A] * 0x1C];
        arg = *(s16 *)(slot + 0xC);
        D_8009B150 = *(u16 *)(slot + 0xC);
        func_80029164(0, arg);
        if (p[0x68] == 0x14) {
            D_8009B1C8[5] = D_8009B1C8[5] + 1;
        }
        func_80024914(slot);
        D_8009B174 = 1;
    }
    state = D_8009B174;
    switch (state & 0xF) {
    case 1:
        if ((state & 0x80) == 0) {
            if (((D_8009B0F4 & 0x2000030) | D_8009B134) != 0) {
                return;
            }
            f2 = *(u16 *)(p + 8);
            D_8009B174 = state | 0x80;
            *(u16 *)(p + 8) = f2 | 4;
            p = func_800291E0(0, -1, -1);
            *(u16 *)(p + 0x30) = 0x5A;
            *(u16 *)(p + 0x32) = 0x16;
            p[0x21] = 0xC0;
            func_800428EC(p, -0xA);
            *(u16 *)(p + 8) = (*(u16 *)(p + 8) | 4) & 0xFFBF;
            D_800E9EF0[1] = p;
            return;
        }
        if ((state & 0x40) == 0) {
            v1 = p[0x21] + 6;
            p[0x21] = v1;
            if ((u8)v1 < 0x40) {
                return;
            }
            D_8009B174 = D_8009B174 | 0x40;
            func_8004036C(p);
            q1 = D_800E9EF0[1];
            D_800E9EF0[0] = 0;
            *(u16 *)(q1 + 8) = *(u16 *)(q1 + 8) | 0x40;
            return;
        }
        p = D_800E9EF0[1];
        if ((state & 0x20) == 0) {
            v2 = p[0x21] + 6;
            p[0x21] = v2;
            if ((s8)v2 < 0) {
                return;
            }
            *(u16 *)(p + 0x60) = 0x1E;
            p[0x21] = 0;
            f3 = *(u16 *)(p + 8);
            st = D_8009B174 | 0x20;
            *(u16 *)(p + 8) = f3 & 0xFFFB;
            D_8009B174 = st;
            return;
        }
        v3 = *(u16 *)(p + 0x60) - 1;
        *(u16 *)(p + 0x60) = v3;
        if ((s16)v3 > 0) {
            return;
        }
        D_8009B174 = 2;
        return;
    case 2:
        func_80026BA4((s16)D_8009B150, 0);
        D_8009B174 = 3;
        return;
    case 3:
        if ((state & 0x80) == 0) {
            p = D_800E9EF0[1];
            D_8009B174 = state | 0x80;
            func_8001944C(p);
            D_800E9EF0[0] = func_80019564(p);
            *(u32 *)(D_800E9EF0[0] + 4) = *(u32 *)(D_800E9EF0[0] + 4) | 0x50000000;
            *(u32 *)(D_800E9EF0[0] + 4) = *(u32 *)(D_800E9EF0[0] + 4) & 0xF7FFFFFF;
            D_800E9EF0[1] = func_80019564(p);
            func_800428EC(D_800E9EF0[1], -1);
            *(u32 *)(D_800E9EF0[1] + 4) = *(u32 *)(D_800E9EF0[1] + 4) | 0x60000000;
            *(u32 *)(D_800E9EF0[1] + 4) = *(u32 *)(D_800E9EF0[1] + 4) & 0xF7FFFFFF;
            func_80029528(0);
            return;
        }
        q0 = D_800E9EF0[0];
        fld = *(s16 *)(q0 + 0x44);
        v4 = fld + 0x80;
        q1 = D_800E9EF0[1];
        *(u16 *)(q1 + 0x46) = v4;
        *(u16 *)(q1 + 0x44) = v4;
        *(u16 *)(q0 + 0x46) = v4;
        *(u16 *)(q0 + 0x44) = v4;
        v5 = D_800E9EF0[0][0xC] - 4;
        if (v5 < 0) {
            v5 = 0;
        }
        v5 = v5 | ((v5 << 16) | (v5 << 8));
        *(u32 *)(D_800E9EF0[0] + 0xC) = v5;
        *(u32 *)(D_800E9EF0[1] + 0xC) = v5;
        if (v5 != 0) {
            return;
        }
        func_8004036C(D_800E9EF0[0]);
        func_8004036C(D_800E9EF0[1]);
        D_8009B174 = 4;
        return;
    case 4:
        if ((state & 0x80) == 0) {
            D_8009B174 = state | 0x80;
            func_80026BA4((s16)D_8009B150, 1);
            return;
        }
        D_8009B23A = 5;
        return;
    }
}

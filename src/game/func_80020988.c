#include "../types.h"

extern u8 D_8009B152;
extern u8 *D_8009B188;
extern u8 *D_8009B18C;
extern u8 *D_8009B1CC;
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));

s32 func_8004002C(void);
u8 *func_800400AC(s32 arg0, s32 arg1);
void func_8004036C(s32 arg0);
void func_800404CC(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);

s32 func_80020988(void) {
    u8 *q;
    s32 one;
    s32 b;
    s32 c;
    s32 v;
    u8 w;

    if ((D_8009B152 & 0x80) == 0) {
        D_8009B152 = D_8009B152 | 0x80;
        one = 1;
        q = func_800400AC(func_8004002C(), 2);
        b = 0xB;
        c = 0x20C;
        func_800404CC(q, *(s16 *)(D_8009B1CC + 0x30) - 8,
                      *(s16 *)(D_8009B1CC + 0x32) + 0x1E, 3, one, 2, b, c);
        func_80042918(q);
        func_800428EC(q, 0xA);
        *(u16 *)(q + 8) = *(u16 *)(q + 8) | 0x28;
        D_8009B188 = q;

        q = func_800400AC(func_8004002C(), 2);
        func_800404CC(q, *(s16 *)(D_8009B1CC + 0x30) + 0x3C,
                      *(s16 *)(D_8009B1CC + 0x32) + 0x1E, 3, one, 0, b, c);
        func_80042918(q);
        func_800428EC(q, 0xA);
        *(u16 *)(q + 8) = *(u16 *)(q + 8) | 0x28;
        D_8009B18C = q;
        return 0;
    }

    q = D_8009B1CC;

    if ((D_8009B152 & 0x40) != 0) {
        *(u16 *)(q + 8) = *(u16 *)(q + 8) | 4;
        v = q[0x21] + q[0x60];
        q[0x21] = v;
        w = v;
        if ((v & 0x7F) != 0) {
            return 0;
        }
        if ((w & 0xFF) == 0) {
            *(u16 *)(q + 8) = *(u16 *)(q + 8) & 0xFFFB;
        }
        do { D_8009B152 = D_8009B152 & 0xBF; } while (0);
        return 0;
    }

    if ((gInput_wPad1Repeat & 0xA000) != 0) {
        *(s16 *)(q + 0x60) = 0x10;
        if ((gInput_wPad1Repeat & 0x2000) != 0) {
            *(s16 *)(q + 0x60) = -0x10;
        }
        D_8009B152 = D_8009B152 | 0x40;
        return 0;
    }

    if ((gInput_wPad1Pressed & 0xE0) != 0) {
        func_8004036C((s32)D_8009B188);
        func_8004036C((s32)D_8009B18C);
        if ((gInput_wPad1Pressed & 0x20) != 0) {
            return -1;
        }
        if (q[0x21] == 0) {
            return 1;
        }
        return 2;
    }
    return 0;
}

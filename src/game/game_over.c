#include "../types.h"

extern u8 D_801AF000[];
extern s32 D_8009B378;
extern u8 D_800E9ECE[];
extern u8 D_800E9ECF[];
/* The retail tail load uses an absolute, self-clobbering v1 address lifetime. */
extern u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern s32 func_8004002C(void);
extern u8 *func_800400AC();
extern void func_80042A00(void *);
extern void func_80040410(u8 *, s32);
extern void func_80040510();
extern void func_800428A8();
extern void func_800428EC();
extern void func_800429D8();
extern void func_8003FF08();
extern void func_800157DC();
extern s32 Rand_GetInterval(s32);

void func_8003C950(void)
{
    u8 *object;
    object = func_800400AC(func_8004002C(), 3);
    func_80040510(object, 0, 0, 320, 240, 0, 0, 16, 0, 240);
    *(s32 *)(object + 4) |= 0x1000000;
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 448, 192, 0, 0, 0, 18, 1, D_801AF000);
    object[94] = 128;
    func_800428EC(object, 4);
    object[108] = 0;
    *(u16 *)(object + 8) |= 40;
    func_800429D8(object);
    D_8009B378 = (s32)object;
    func_8003FF08(0x7300);
    func_800157DC();
    D_800E9ECF[0] = 2;
}

s32 func_8003CA5C(void)
{
    u8 *p;
    s16 value;
    register u32 tail_bits __asm__("$3");

    p = (u8 *)D_8009B378;
    func_80042A00(p);
    value = *(s16 *)(p + 0x5A);
    if (value == 0) {
        switch (p[0x6C]) {
        case 0:
            func_80040410(p, 1);
            p[0x6C] = 1;
            *(s16 *)(p + 0x36) = -0xC0;
            break;
        case 1:
            func_800429D8(p);
            func_80040410(p, 2);
            p[0x6C] = 2;
            break;
        case 2:
            func_80040410(p, 0);
            p[0x6C] = 0;
            if (*(s16 *)(p + 0x30) < -0x2F) {
                *(s16 *)(p + 0x30) = 0x160;
                *(s16 *)(p + 0x32) = Rand_GetInterval(0xB0) + 0x30;
            }
            break;
        }
    }
    {
        s32 result = 1;

        if (D_800E9ECE[0] & 0x80) {
            return result;
        }

        tail_bits = gInput_wPad1Pressed;
        if (tail_bits & 0xE0) {
            result = 0;
            /* Distinct exits preserve the retail branch-and-delay-slot shape. */
            if (D_8009B378 != 0) {
                return result;
            } else {
                return result;
            }
        }
        return result;
    }
}

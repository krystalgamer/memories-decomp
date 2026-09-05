#include "../types.h"

extern u8 *D_8009B378;
extern u8 D_800E9ECE[];
/* The retail tail load uses an absolute, self-clobbering v1 address lifetime. */
extern u16 D_8009B398 __attribute__((section(".data")));
extern void func_80042A00(void *);
extern void func_800429D8(void *);
extern void func_80040410(u8 *, s32);
extern s32 Rand_GetInterval(s32);

s32 func_8003CA5C(void)
{
    u8 *p;
    s16 value;
    register u32 tail_bits __asm__("$3");

    p = D_8009B378;
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

        tail_bits = D_8009B398;
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

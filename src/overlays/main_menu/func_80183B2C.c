#include "../../types.h"

extern u8 *D_801845EC[];
extern u16 D_80185C9C[][11];
extern u8 D_80185CC8;
extern u8 D_80185CC9;

extern void func_80183E8C(s32 x, s32 y, s32 value);
extern void func_80184344(s32 x, s32 y, s32 index);
extern void func_80184454(int column);

void func_80183B2C(void)
{
    u8 *volatile obj[2];
    s32 i;

    obj[0] = D_801845EC[0];
    obj[1] = D_801845EC[2];

    if (D_80185CC8 != 0) {
        func_80184454(0);
        obj[0][0xC] = obj[0][0xD] = obj[0][0xE] = 0x40;
        *(s16 *)(obj[0] + 0x60) = 2;
    } else {
        obj[0][0xC] += obj[0][0x60];
        obj[0][0xD] += obj[0][0x60];
        obj[0][0xE] += obj[0][0x60];
        if (obj[0][0xC] < 0x41 || (s8)obj[0][0xC] < 0) {
            *(s16 *)(obj[0] + 0x60) *= -1;
        }
    }

    if (D_80185CC9 != 0) {
        func_80184454(1);
        obj[1][0xC] = obj[1][0xD] = obj[1][0xE] = 0x40;
        *(s16 *)(obj[1] + 0x60) = 2;
    } else {
        obj[1][0xC] += obj[1][0x60];
        obj[1][0xD] += obj[1][0x60];
        obj[1][0xE] += obj[1][0x60];
        if (obj[1][0xC] < 0x41 || (s8)obj[1][0xC] < 0) {
            *(s16 *)(obj[1] + 0x60) *= -1;
        }
    }

    i = 0;
    if (D_80185C9C[0][0] != 0) {
        do {
            func_80183E8C((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC,
                          D_80185C9C[0][i + 1]);
            func_80184344((i % 5) * 28 + 0x10, (i / 5) * 17 + 0xBC,
                          D_80185C9C[0][i + 1]);
            i++;
        } while (i < D_80185C9C[0][0]);
    }

    i = 0;
    if (D_80185C9C[1][0] != 0) {
        do {
            func_80183E8C((i % 5) * 28 + 0xB0, (i / 5) * 17 + 0xBC,
                          D_80185C9C[1][i + 1]);
            func_80184344((i % 5) * 28 + 0xB0, (i / 5) * 17 + 0xBC,
                          D_80185C9C[1][i + 1]);
            i++;
        } while (i < D_80185C9C[1][0]);
    }
}

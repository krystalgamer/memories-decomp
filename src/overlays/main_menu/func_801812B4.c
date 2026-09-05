#include "../../types.h"

extern u8 *D_801845B0[];
extern u8 D_801845BC[];
extern u16 D_801845C0[];
extern volatile u16 D_8009B394[];
extern volatile u16 D_8009B398[];
extern void func_80048658(s32, s32, s32);
extern void func_80181F20(s32, s32);

s32 func_801812B4(void)
{
    s32 busyA;
    s32 busyB;
    s32 step;
    u16 value;

    busyA = (*(void **)(D_801845B0[0] + 0x24) != 0);
    busyB = (*(void **)(D_801845B0[1] + 0x24) != 0);

    if (D_801845C0[0] != D_801845C0[1]) {
        step = D_801845C0[0] - D_801845C0[1];
        if (step < 0) {
            step = D_801845C0[1] - D_801845C0[0];
        }
        if (D_801845C0[1] < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[1] < D_801845C0[0]) {
            D_801845C0[1] = D_801845C0[1] + step;
        } else {
            D_801845C0[1] = D_801845C0[1] - step;
        }
        busyA++;
    }

    if (D_801845C0[6] != D_801845C0[7]) {
        step = D_801845C0[6] - D_801845C0[7];
        if (step < 0) {
            step = D_801845C0[7] - D_801845C0[6];
        }
        if (D_801845C0[7] < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[7] < D_801845C0[6]) {
            D_801845C0[7] = D_801845C0[7] + step;
        } else {
            D_801845C0[7] = D_801845C0[7] - step;
        }
        busyB++;
    }

    if (busyA == 0 && busyB == 0) {
        if ((D_8009B398[0] & 0x20) || (D_8009B398[1] & 0x20)) {
            func_80048658(8, 0xFF, 0);
            return -1;
        }
        if ((D_8009B398[0] & 0x800) || (D_8009B398[1] & 0x800)) {
            func_80048658(7, 0xFF, 0);
            return 1;
        }
    }

    if (busyA == 0) {
        if (D_801845BC[0] < 2) {
            if (D_8009B394[0] & 0xA000) {
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[0] & 0x8000) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[0] = D_801845BC[2];
                if (D_801845BC[1] < 2) {
                    D_801845BC[1] = D_801845BC[2];
                }
            } else if (D_8009B394[0] & 0x4000) {
                func_80181F20(0, 2);
            }
        } else {
            if (D_8009B394[0] & 0xA000) {
                value = D_801845C0[0];
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[0] & 0x8000) {
                    value = (value - 0x1F4 > 0) ? (value - 0x1F4) : 1;
                } else if (value < 2) {
                    value = 0x1F4;
                } else {
                    value = (value + 0x1F4 < 0x1F41) ? (value + 0x1F4) : 0x1F40;
                }
                D_801845C0[0] = value;
            } else if (D_8009B394[0] & 0x1000) {
                func_80181F20(0, D_801845BC[2]);
            }
        }
    }

    if (busyB == 0) {
        if (D_801845BC[1] < 2) {
            if (D_8009B394[1] & 0xA000) {
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[1] & 0x8000) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[1] = D_801845BC[2];
                if (D_801845BC[0] < 2) {
                    D_801845BC[0] = D_801845BC[2];
                }
            } else if (D_8009B394[1] & 0x4000) {
                func_80181F20(1, 2);
            }
        } else {
            if (D_8009B394[1] & 0xA000) {
                value = D_801845C0[6];
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[1] & 0x8000) {
                    value = (value - 0x1F4 > 0) ? (value - 0x1F4) : 1;
                } else if (value < 2) {
                    value = 0x1F4;
                } else {
                    value = (value + 0x1F4 < 0x1F41) ? (value + 0x1F4) : 0x1F40;
                }
                D_801845C0[6] = value;
            } else if (D_8009B394[1] & 0x1000) {
                func_80181F20(1, D_801845BC[2]);
            }
        }
    }

    return 0;
}

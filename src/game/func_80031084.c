#include "../types.h"

typedef struct {
    u8 unk0[0x28];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s16 unk34;
    s16 unk36;
    s16 unk38;
    s16 unk3A;
    u8 unk3C[0x17];
    u8 unk53;
    u8 unk54;
    u8 unk55[2];
    u8 unk57;
    u8 unk58;
    u8 unk59;
    u8 unk5A;
    u8 unk5B;
    s16 unk5C;
    s16 unk5E;
    u8 unk60;
    u8 unk61;
    u8 unk62[2];
} Rec64;

extern void (*D_80090D7C[])(void);
extern void (*D_80090D84[])(void);
extern u8 D_8009AF54[];
extern u8 D_8009B2EB;
extern u8 D_8009B2F0;
extern s8 D_8009B2F1;
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern Rec64 D_800EB0F8[];

void func_80031084(void) {
    u8 *b;
    s32 i;
    s8 d;
    s32 one;
    s32 k;
    s32 e;
    s32 t;

    rand();
    e = D_8009B2EB;
    i = 0;
    if (e != 0) {
        if (D_8009B2F0 != 0) {
            D_80090D7C[e & 0x1F]();
        } else {
            D_80090D84[e & 0x1F]();
        }
        return;
    }
    for (; i < 0x16; i++) {
        FntPrint(D_8009AF54);
    }
    if ((gInput_wPad1Repeat & 0xF000) != 0) {
        if ((gInput_wPad1Repeat & 0xA000) != 0) {
            if ((gInput_wPad1Repeat & 0x2000) != 0) {
                if (D_8009B2F1 + 0xA < 0x14) {
                    D_8009B2F1 = D_8009B2F1 + 0xA;
                }
            } else {
                if (D_8009B2F1 - 0xA >= 0) {
                    D_8009B2F1 = D_8009B2F1 - 0xA;
                }
            }
        }
        if ((gInput_wPad1Repeat & 0x1000) != 0) {
            t = (u8)D_8009B2F1;
            if (D_8009B2F1 >= 0xA) {
                d = t - 1;
                D_8009B2F1 = d;
                if (d < 0xA) {
                    D_8009B2F1 = 0x13;
                }
            } else {
                d = t - 1;
                D_8009B2F1 = d;
                if (d < 0) {
                    D_8009B2F1 = 9;
                }
            }
        }
        if ((gInput_wPad1Repeat & 0x4000) != 0) {
            t = (u8)D_8009B2F1;
            if (D_8009B2F1 >= 0xA) {
                d = t + 1;
                D_8009B2F1 = d;
                if (d >= 0x14) {
                    D_8009B2F1 = 0xA;
                }
            } else {
                d = t + 1;
                D_8009B2F1 = d;
                if (d >= 0xA) {
                    D_8009B2F1 = 0;
                }
            }
        }
        func_800300C8();
    }
    if ((gInput_wPad1Pressed & 0x20) != 0) {
        k = 0x13;
        if (D_8009B2F1 != k) {
            D_8009B2F1 = k;
            func_800300C8();
            return;
        }
        D_8009B2EB = 0x14;
        return;
    }
    if ((gInput_wPad1Pressed & 0x100) != 0) {
        one = 1;
        func_8003B6AC(one, one);
        D_8009B2F0 = D_8009B2F0 ^ one;
        TextBox_Create(1, D_8009B2F0 + 0xF, 0x10, 0x10, 0x120, 0xA0);
        b = (u8 *)D_800EB0F8;
        b[0xBE] = 0x10;
        b[0xBF] = 0x10;
        func_80039A14(b + 0x64);
        return;
    }
    if ((gInput_wPad1Pressed & 0xC0) != 0) {
        D_8009B2EB = D_8009B2F1 + 1;
    }
}

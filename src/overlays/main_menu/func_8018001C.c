#include "../../types.h"

extern u8 D_801AF800[];
extern u8 *D_80184558;
extern u8 *D_8018455C;
extern u8 *D_80184560;
extern u8 *gMain_apMenuEntries[];
extern u8 gMain_bMenuID;
extern u8 D_80184595;
extern u8 D_80184596;
extern u8 D_80184597;
extern u8 D_80184598;
extern u8 D_80184599;
extern u8 D_8018459A;
extern u8 D_8018459B;
extern u8 D_8018459C;
extern u8 D_8018459D;
extern void (*D_800E9DB0)(void);

extern void func_80180B4C(void);
extern void func_80180D2C(s32);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_80040410(u8 *, s32);
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_800428EC(void *, s32);
extern void func_80042918(void *);
extern void func_80047314(s32);

void func_8018001C(s32 unused, s32 menu)
{
    u8 *object;
    u8 *entry;
    u8 *third;
    u8 *fourth;
    s32 i;
    s32 y;
    s32 value;
    u16 state;

    gMain_bMenuID = menu % 11;

    object = func_800400AC(func_8004002C(), 2);
    D_80184558 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0, 5, 0, 0, 0x1A, 1, D_801AF800);
        *(u32 *)(D_80184558 + 4) |= 0x1000000;
        *(u16 *)(D_80184558 + 8) |= 0x28;
        func_800428EC(D_80184558, 0);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_8018455C = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 2, 0x1A, 1, D_801AF800);
        *(u32 *)(D_8018455C + 4) |= 0x1000000;
        *(u16 *)(D_8018455C + 8) |= 0x28;
        func_800428EC(D_8018455C, 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_80184560 = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 1, 0x1A, 1, D_801AF800);
        *(u32 *)(D_80184560 + 4) |= 0x1000000;
        *(u16 *)(D_80184560 + 8) |= 0x28;
        func_80042918(D_80184560);
        third = D_80184560;
        third[0x6C] = 0x3C;
        fourth = D_80184560;
        *(s16 *)(third + 0x60) = -2;
        *(u16 *)(fourth + 0x36) = 0;
    }

    for (i = 0; i < 11; i++) {
        entry = func_800400AC(func_8004002C(), 2);
        if (i < 5) {
            y = i * 32 + 50;
        } else {
            y = (i - 5) * 32 + 42;
        }
        if (entry != 0) {
            func_800428A8(entry, 0xA0, y, 0, 0, 0, 0x18, 0, D_801AF800);
            value = i * 2 | (gMain_bMenuID != i);
            *(u32 *)(entry + 4) |= 0x1000000;
            *(u16 *)(entry + 8) = (*(u16 *)(entry + 8) | 8) & ~0x40;
            func_80040410(entry, value);
            func_80042918(entry);
            gMain_apMenuEntries[i] = entry;
        } else {
            gMain_apMenuEntries[i] = 0;
        }
    }

    D_80184595 = 0;
    D_80184596 = 0;
    D_80184597 = 0;
    if (gMain_bMenuID != 0) {
        state = *(u16 *)(D_80184560 + 8);
        D_80184597 = 0x80;
        *(u16 *)(D_80184560 + 8) = state & ~0x40;
    }
    D_80184598 = 0;
    D_80184599 = 0;
    D_8018459A = 0;
    D_8018459B = 0;
    D_8018459C = 0;
    D_8018459D = 0;
    func_80180D2C(0);
    D_800E9DB0 = func_80180B4C;
    func_80047314(0x7000);
}

#include "../../types.h"

extern u8 D_801AF800[];
extern u8 *D_801845A0;
extern u8 *D_801845A4;
extern u8 *D_801845B0[];
extern u8 D_801845BC[];
extern u8 D_801845C0[];
extern u8 *D_801845D8;
extern void (*D_800E9DB0)(void);
extern void func_80181728(void);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(void *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428A8(void *, s32, s32, s32, s32, s32, s32, s32, void *);
extern void func_800428EC(void *, s32);

void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle)
{
    u8 *object;
    u8 *state;

    object = func_800400AC(func_8004002C(), 2);
    D_801845A0 = object;
    if (object != 0) {
        func_800404CC(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        *(u16 *)(D_801845A0 + 8) |= 0x28;
        func_800428EC(D_801845A0, -2);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845A4 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0xA, 6, 0, 0, 0xE, 5, D_801AF800);
        *(u16 *)(D_801845A4 + 8) |= 0x28;
        func_800428EC(D_801845A4, -1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[0] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[0], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)(D_801845B0[0] + 8) |= 0x28;
        func_800428EC(D_801845B0[0], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[1] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[1], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)(D_801845B0[1] + 8) |= 0x28;
        func_800428EC(D_801845B0[1], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[2] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[2], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)(D_801845B0[2] + 8) |= 0x28;
        func_800428EC(D_801845B0[2], 1);
    }

    state = D_801845C0;
    D_801845D8 = toggle;
    D_801845BC[2] = (*toggle == 0);
    D_801845BC[0] = D_801845BC[1] = 2;
    *(u16 **)(state + 4) = first;
    *(u16 **)(state + 0x10) = second;
    *(u16 *)state = *(u16 *)(state + 2) = *first;
    *(u16 *)(state + 0xC) = *(u16 *)(state + 0xE) = *second;
    D_800E9DB0 = func_80181728;
}

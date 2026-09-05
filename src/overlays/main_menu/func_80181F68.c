#include "../../types.h"

typedef struct {
    u8 *object;
    s32 unk4;
} MainMenuSlot;

typedef struct {
    s16 a;
    s16 b;
} MainMenuPair;

extern u8 *D_801845DC;
extern u8 *D_801845E0;
extern MainMenuSlot D_801845EC[];
extern MainMenuPair D_80185C8C[];
extern u16 D_80185C9C[][11];
extern u8 D_80185CC8[];
extern u8 D_80185CCA[];
extern u8 D_80185CCC[];
extern u8 D_80185CCE;
extern u8 D_80185CCF;
extern u8 D_80185CD0;
extern u8 D_80185CD1;
extern u8 D_8009B0C0;
extern void (*D_800E9DB4)(void);
extern void func_80183B2C(void);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(void *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428EC(void *, s32);
extern void func_80061008(s32, s32, s32, s32);
extern void func_8018338C(s32, s32);
extern void func_801844D8(s32);

void func_80181F68(void)
{
    u8 *object;
    u8 *entry;
    s32 i;

    object = func_800400AC(func_8004002C(), 2);
    D_801845DC = object;
    if (object != 0) {
        func_800404CC(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        *(u16 *)(D_801845DC + 8) |= 0x28;
        func_800428EC(D_801845DC, -2);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845E0 = object;
    if (object != 0) {
        func_800404CC(object, 0, -3, 0, 4, 4, 0xC, 0x208);
        *(u16 *)(D_801845E0 + 8) |= 0x28;
        func_800428EC(D_801845E0, -1);
    }

    func_80061008(0, 0x25, 0xA0, 0x25);

    for (i = 0; i < 2; i++) {
        entry = func_800400AC(func_8004002C(), 2);
        if (entry != 0) {
            func_800404CC(entry, i * 0xA0 + 0x1E, 0x24, 0, 4, 8, 0xC, 0x208);
            *(u16 *)(entry + 8) |= 0x28;
            func_800428EC(entry, 0);
            *(s16 *)(entry + 0x60) = -2;
            D_801845EC[i].object = entry;
        } else {
            D_801845EC[i].object = 0;
        }
        D_801845EC[i].unk4 = 0;
        D_80185C8C[i].b = 0;
        D_80185C8C[i].a = 0;
        D_80185C9C[i][0] = 0;
        D_80185CC8[i] = 0;
        D_80185CCA[i] = 0;
        D_80185CCC[i] = 0;
        func_8018338C(i, 1);
        func_801844D8(i);
    }

    D_80185CCE = 0;
    D_80185CCF = 0;
    D_80185CD0 = 0;
    D_80185CD1 = 0;
    D_800E9DB4 = func_80183B2C;
    D_8009B0C0 = 1;
}

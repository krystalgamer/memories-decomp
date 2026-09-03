#include "../types.h"

typedef struct {
    s32 value[18];
} Block72;

typedef struct {
    s32 value[8];
} Block32;

extern char D_8009B104[1];
extern char D_8009B11C[1];
extern volatile u16 D_8009B100;
extern volatile s32 D_8009B0F4;
extern s32 D_8009B130;
extern s32 D_800E9E90[3];
extern u8 D_800E9E60[];
extern u8 D_800E9E18[];
extern u8 D_801D4200[];
extern u16 D_8009B112;

extern void func_8007B1F4(s32, void *, void *, s32);
extern void func_8007B468();
extern s32 func_8007E710(s32);

void func_80014294(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        func_8007B1F4(0xD, D_8009B11C, func_80014294, -1);
    } else if (event == 2) {
        D_8009B100 = 4;
        D_8009B0F4 &= ~0x400;
    }
}

void func_80014308(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        func_8007B468(0x4A, D_8009B104, 0x1B, func_80014308, -1);
    } else if (event == 2) {
        D_8009B100 = 5;
        D_8009B0F4 |= 0x1000;
        D_8009B0F4 &= ~0x400;
    }
}

void func_80014390(u8 event, s32 arg1)
{
    s32 value;
    s32 *destination;

    if (event == 2) {
        destination = D_800E9E90;
        value = func_8007E710(arg1);
        if (value > 0)
            *destination = value;
        D_8009B0F4 &= ~0x800;
    }
}

void File_ActivateTransfer(void) {
    *(Block72 *)D_800E9E60 = *(Block72 *)D_800E9E18;
    *(Block32 *)D_801D4200 = *(Block32 *)(D_801D4200 + 32);
    if (D_800E9E60[70] == 4)
        D_8009B112 |= 1;
    D_8009B0F4 = *(s32 *)(D_800E9E60 + 44) | 16;
}

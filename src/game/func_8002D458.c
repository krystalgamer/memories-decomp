#include "../types.h"

extern u8 D_8009B268;
extern u8 D_8009B26C;
extern u8 D_8009B26D;
extern s16 D_801D07DC[];
extern u8 D_8009B27A __attribute__((section(".data")));
extern u8 D_8009B3D4 __attribute__((section(".data")));
extern u8 D_8009B0D1 __attribute__((section(".data")));
extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));

void func_8003BBF8(void);
void func_8016AA6C(void);
void func_80033C90(void);

void func_8002D458(s32 mode)
{
    D_8009B268 = 1;
    D_8009B26D = mode;

    switch (mode) {
    case 0:
        func_8003BBF8();
        func_8016AA6C();
        D_801D07DC[0] = 0x30;
        D_8009B27A = 0x30;
        D_8009B3D4 = 1;
        D_8009B0D1 = 0;
        D_8009B26C = 2;
        break;
    case 2:
        D_8009B26C = 0x10;
        break;
    case 3:
        D_8009B26C = 0xE;
        break;
    case 8:
        D_8009B26C = 4;
        break;
    case 5:
        D_8009B27A = D_801D07DC[0];
        D_8009B26C = 2;
        break;
    case 6:
        gFreeDuel_bReturnFlags = 0;
        D_8009B26C = 6;
        break;
    case 7:
        func_80033C90();
        D_8009B268 = 0;
        break;
    case 4:
        D_8009B26C = 0xB;
        D_8009B268 = 0;
        break;
    case 9:
        D_8009B26C = 0xA;
        D_8009B268 = 0;
        break;
    case 10:
    default:
        D_8009B26C = 0;
        break;
    }
}

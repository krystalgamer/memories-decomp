
#include "../../../src/types.h"
#include "../../../src/psyq/libgte.h"
#include "../../../src/psyq/libgpu.h"
#include "../../../src/psyq/libetc.h"

extern u16 D_8009B098;
extern volatile u8 D_8009B0C0;
extern volatile s32 D_8009B0C8;
u8 D_8009B0C1;
extern s32 D_8009B0D8;
extern volatile unsigned char D_8009AFA3 __attribute__((section(".data")));
extern unsigned char D_8009AFA4 __attribute__((section(".data")));
extern s32 D_8009B0CC;

void func_80012DB4(void)
{
    s32 v;

    if ((D_8009B098 & 0x8000) == 0) {
        DrawSync(0);
    }
    while (D_8009B0C8 < D_8009B0C0) {
    }
    v = D_8009B0C8;
    D_8009B0C1 = v;
    if ((v & 0xFF) != 0) {
        D_8009B0C1 = 1;
    }
    D_8009B0D8 = D_8009B0C1 + 1;
    if (D_8009AFA4 != 0) {
        D_8009AFA3 = 2;
    } else {
        D_8009AFA3 = D_8009B0D8;
    }
    D_8009AFA4 = 0;
    D_8009B0C8 = -1;
    VSync(0);
    D_8009B0CC = D_8009B0CC + 1;
}

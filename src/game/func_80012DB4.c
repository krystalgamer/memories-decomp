#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"

extern u16 D_8009B098;
extern volatile u8 D_8009B0C0;
extern volatile s32 D_8009B0C8;
/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store below. c_symbols.ld overrides this
   common symbol, so no storage is allocated here. */
u8 D_8009B0C1;
extern s32 D_8009B0D8;
extern unsigned char D_8009AFA3 __attribute__((section(".data")));
extern unsigned char D_8009AFA4 __attribute__((section(".data")));
extern s32 D_8009B0CC;

/* The frame-sync step. Waits out the GPU unless bit 0x8000 of D_8009B098 is
   set, spins until D_8009B0C8 reaches D_8009B0C0, clamps the low byte of
   D_8009B0C8 into D_8009B0C1 at 1, publishes D_8009B0C1 + 1 to D_8009B0D8,
   writes either 2 or that count to D_8009AFA3 depending on D_8009AFA4, clears
   D_8009AFA4, resets D_8009B0C8 and bumps the frame counter. */
void func_80012DB4(void)
{
    if ((D_8009B098 & 0x8000) == 0) {
        DrawSync(0);
    }
    while (D_8009B0C8 < D_8009B0C0) {
    }

    D_8009B0C1 = D_8009B0C8;
    if (D_8009B0C1 & 0xFF) {
        D_8009B0C1 = 1;
    }
    D_8009B0D8 = D_8009B0C1 + 1;

    D_8009AFA3 = D_8009AFA4 ? 2 : D_8009B0D8;

    D_8009AFA4 = 0;
    D_8009B0C8 = -1;

    VSync(0);

    D_8009B0CC++;
}

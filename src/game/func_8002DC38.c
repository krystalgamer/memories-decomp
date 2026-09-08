
#include "../types.h"
#include "card_constants.h"
#include "text_box_lifecycle.h"
#include "../overlays/main_menu/entrypoints.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store of D_8009B269 into it. c_symbols.ld
   overrides this common symbol, so no storage is allocated here. */
u8 D_8009B26C;
extern u8 D_8009B269;
extern u16 D_8009B230 __attribute__((section(".data")));
extern u16 D_8009B234 __attribute__((section(".data")));
extern u16 D_8009B236 __attribute__((section(".data")));
extern u8 D_8009B368 __attribute__((section(".data")));
extern u8 D_800EB0F8[];

extern void func_80039A14(void *);
extern void SD_BGMPlay(u32);
extern void func_80015A00(void);
extern void SD_BGMFadeOut(void);
extern void Fade_WaitOut(void);
extern void func_80024DC8(s32, s32, s32, s32);

void func_8002DC38(void)
{
    s32 result;

    if ((D_8009B26C & 0x40) == 0) {
        D_8009B26C = D_8009B26C | 0x40;
        D_8009B236 = DUEL_STARTING_LIFE_POINTS;
        D_8009B234 = DUEL_STARTING_LIFE_POINTS;
        MainMenu_StartValueSetup(&D_8009B234, &D_8009B236, (u8 *)&D_8009B230);
        TextBox_CreateFlagged(0, 0x25, 0x34, 0xB4, 0xD8, 0x20, 0x20);
        func_80039A14(D_800EB0F8);
        TextBox_Create(1, 0x26, 0xE, 0x66, 0x100, 0x30);
        func_80039A14(&D_800EB0F8[0x64]);
        SD_BGMPlay(0x72C0);
        func_80015A00();
    }

    result = MainMenu_UpdateValueSetup();
    if (result != 0) {
        SD_BGMFadeOut();
        Fade_WaitOut();
        MainMenu_FinishValueSetup();
        if (result == 1) {
            func_80024DC8(-1, -1, 0, 0);
            D_8009B368 = 8;
        } else {
            D_8009B26C = D_8009B269;
        }
    }
}


#include "../types.h"
#include "../psyq/libgte.h"
#include "fade.h"

typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    u8 b6;
    u8 b7;
} Anim;

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the first read of it below. c_symbols.ld overrides
   this common symbol, so no storage is allocated here. */
u8 D_8009B26C;
extern u8 D_8009B0C0 __attribute__((section(".data")));
extern u8 gDuel_bTerrain __attribute__((section(".data")));
extern u8 D_8009B269;
extern Anim D_800EF658[];

extern void func_800530C4(void);
extern void func_800533D8(void);
extern void Model_SetSlotProperties(s32, ...);
extern void func_80059C24(void);
extern s32 func_80059C88(void);
extern s32 func_800534B8(void);
extern void SD_KeyOffVoiceSlots(void);
extern void SD_BGMFadeOut(void);
extern void func_80059CE4(void);

void Main_RunAnimatedBattle(void)
{
    Anim *p;
    u8 f;
    s32 v;

    SetGeomOffset(0xA0, 0x78);
    SetGeomScreen(0x12C);
    f = D_8009B26C;
    if ((f & 0x40) == 0) {
        D_8009B26C = f | 0x40;
        D_8009B0C0 = 1;
        func_800530C4();
        func_800533D8();
        p = D_800EF658;
        if (p->f0 == 0x309) {
            D_8009B26C = D_8009B26C | 0x20;
            func_80059C24();
        } else {
            Model_SetSlotProperties(0, p->f0 - 1, p->f2, p->f4, p->b7, p->b6);
            p++;
            Model_SetSlotProperties(1, p->f0 - 1, p->f2, p->f4, p->b7, p->b6);
            Model_SetSlotProperties(2, gDuel_bTerrain);
        }
        func_800159D8();
    } else {
        if (f & 0x20) {
            v = func_80059C88();
        } else {
            v = func_800534B8();
        }
        if (v != 0) {
            SD_KeyOffVoiceSlots();
            SD_BGMFadeOut();
            D_8009B26C = D_8009B269;
        }
        func_80059CE4();
    }
}

#define D_8009B0C0_IN_DATA
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "model_scene_states.h"
#include "../psyq/libgte.h"
#include "fade.h"
#include "sound.h"
#include "main_modes.h"
#include "../unmatched.h"
#include "model_cleanup.h"
#include "model_scene_setup.h"
#include "duel_terrain_boost.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the first read of it below. c_symbols.ld overrides
   this common symbol, so no storage is allocated here. */
u8 D_8009B26C;
extern u8 D_8009B269;
extern AnimatedBattleModelProperties D_800EF658[];

void Main_RunAnimatedBattle(void)
{
    AnimatedBattleModelProperties *p;
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
        if (p->model_id == 0x309) {
            D_8009B26C = D_8009B26C | 0x20;
            func_80059C24();
        } else {
            Model_SetSlotProperties(
                0,
                p->model_id - 1,
                p->field_02,
                p->field_04,
                p->field_07,
                p->field_06
            );
            p++;
            Model_SetSlotProperties(
                1,
                p->model_id - 1,
                p->field_02,
                p->field_04,
                p->field_07,
                p->field_06
            );
            Model_SetSlotProperties(2, gDuel_bTerrain);
        }
        Fade_WaitInitIn();
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

#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "input.h"
#include "duel_effect.h"
#include "display_object_api.h"
#include "text_box_lifecycle.h"
#include "../overlays/main_menu/entrypoints.h"
#include "fade.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "main_modes.h"
#include "duel_reward_setup.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, which is the form the
   target uses for all three. c_symbols.ld overrides these common symbols, so
   no storage is allocated here. */
u8 D_8009B269;
u8 D_8009B26C;
u8 D_8009B26E;

/* Spelt as an incomplete array so -G8 keeps the four-byte slot out of small
   data; the target addresses it with a %hi/%lo pair and keeps the high half
   in $s2 across the tail. The section(".data") spelling is not equivalent
   here - it leaves GCC emitting the assembler macro form instead. */
extern u8 D_800E9EF0[];
#define gTradeObj (*(DisplayObject **)D_800E9EF0)

void Main_RunTrade(void)
{
    DuelEffectChannel *box;
    DisplayObject *obj;
    s32 y;

    if ((D_8009B26C & 0x40) == 0) {
        D_8009B26C = D_8009B26C | 0x40;
        func_80032328();
        SD_BGMPlay(0x72D0);
        MainMenu_InitTradeScreen();
        D_8009B26E = 1;
        box = TextBox_CreateFlagged(0, 0xB, 0x18, 0x20, 0x110, 0xA0, 0x20);
        box->field_59 = 0x10;
        func_80039A14((u8 *)box);
        obj = func_800400AC(func_8004002C(), 2);
        func_800404CC(obj, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        obj->flags = obj->flags | 0x20;
        obj->attribute = obj->attribute | GsALON;
        func_80042918(obj);
        func_800428EC((u8 *)obj, 0xF);
        gTradeObj = obj;
        Fade_WaitIn();
    }

    box = D_800EB0F8;
    obj = gTradeObj;
    switch (D_8009B26E) {
    case 1:
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0xE0) != 0) {
            SD_SEPlayFull(0x1E);
            func_80043178((DisplayObjectSnapshot *)obj);
            obj->field_60 = 0x400;
            D_8009B26E = 2;
        }
        break;
    case 2:
        y = *(u16 *)&obj->field_60 - 0x20;
        obj->field_60 = y;
        func_80043230((DisplayObjectPosition *)obj, 0, 0xF0, (s16)y);
        TextBox_SetPos(
            (u8 *)box,
            *(s16 *)&obj->field_30.h.field_30 + 0x18,
            *(s16 *)&obj->field_30.h.field_32 + 0x20
        );
        if (obj->field_60 <= 0) {
            D_8009B26E = 0;
            TextBox_Destroy(box);
            func_8004036C(gTradeObj);
            gTradeObj = 0;
        }
        break;
    default:
        if (DuelEffect_UpdateState() == 0) {
            if (MainMenu_UpdateTradeScreen() != 0) {
                SD_BGMFadeOut();
                Fade_WaitOut();
                MainMenu_ReleaseTradeDisplayHandles();
                D_8009B26C = D_8009B269;
            }
        }
        break;
    }
}

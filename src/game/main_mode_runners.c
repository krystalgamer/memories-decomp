#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "display_object_interpolation.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "input.h"
#include "duel_effect.h"
#include "display_object_core.h"
#include "text_box_lifecycle.h"
#include "../overlays/main_menu/trade_helpers.h"
#include "fade.h"
#include "file_transfer.h"
#include "game_over.h"
#include "main_modes.h"
#include "main_services.h"
#include "options.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "duel_reward_setup.h"
#include "display_object_work_slots.h"
#include "../unmatched.h"
#include "main_mode_state.h"

/* The grouped mode runners use common-symbol forms overridden by the tracked
   link symbols, so these definitions do not allocate storage here. */
u8 D_8009B269;
u8 D_8009B26C;
u8 D_8009B26E;

void Main_RunOptionsMenu(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestOptionsPackage();
        Options_Init();
        Fade_WaitIn();
    }
    if (Options_Update() == 0) {
        u8 value;

        SD_BGMFadeOut();
        value = D_8009B269;
        D_8009B26C = value;
    }
}

void Main_RunGameOver(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestGameOverPackage();
        GameOver_Init();
    }

    if (func_8003CA5C() == 0) {
        u8 mode;

        SD_BGMFadeOut();
        Fade_WaitOut();
        mode = D_8009B269;
        D_8009B26C = mode;
        if (mode != 0) {
            D_8009B268 = 1;
            D_8009B26D = 0;
            D_8009B26C = 8;
            longjmp(D_800E9DC0, 1);
        }
    }
}

/* gMain_apfnModeRunner entry 13: the developer-mode slot's runner is empty in
   the retail build. */
void Main_RunUnusedDeveloperMode(void)
{
}

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
        func_80039A14(box);
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(obj, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        obj->flags = obj->flags | 0x20;
        obj->attribute = obj->attribute | GsALON;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0xF);
        D_800E9EF0[0] = obj;
        Fade_WaitIn();
    }

    box = D_800EB0F8;
    obj = D_800E9EF0[0];
    switch (D_8009B26E) {
    case 1:
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0xE0) != 0) {
            SD_SEPlayFull(0x1E);
            DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
            obj->field_60 = 0x400;
            D_8009B26E = 2;
        }
        break;
    case 2:
        y = *(u16 *)&obj->field_60 - 0x20;
        obj->field_60 = y;
        Widget_SlideSine((DisplayObjectPosition *)obj, 0, 0xF0, (s16)y);
        TextBox_SetPos(
            box,
            *(s16 *)&obj->field_30.h.field_30 + 0x18,
            *(s16 *)&obj->field_30.h.field_32 + 0x20
        );
        if (obj->field_60 <= 0) {
            D_8009B26E = 0;
            TextBox_Destroy(box);
            DisplayObject_ReleaseIfPresent(D_800E9EF0[0]);
            D_800E9EF0[0] = 0;
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

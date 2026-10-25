

#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#define D_8009B0C0_IN_DATA
#define D_8009B368_IN_DATA
#define D_8009B230_IN_DATA
#define D_8009B234_IN_DATA
#define D_8009B236_IN_DATA
#include "../types.h"
#include "text_box_blocking_build.h"
#include "display_object_interpolation.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "input.h"
#include "duel_effect.h"
#include "text_channel_state.h"
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
#include "card_list_setup.h"
#include "display_object_work_slots.h"
#include "../unmatched.h"
#include "main_mode_state.h"
#include "save_data.h"
#include "campaign_flags.h"
#include "credits_secret_numbers.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "data_transfer_request.h"
#include "text_box_update_channels.h"
#include "graphics_frame.h"
#include "model.h"
#include "model_scene_controllers.h"
#include "model_scene_states.h"
#include "model_cleanup.h"
#include "duel_effect_reset_state.h"
#include "menu_record_reset.h"
#include "duel_side_state.h"
#include "main_start_duel.h"
#include "card_constants.h"
#include "../overlays/main_menu/value_setup.h"
#include "duel_session_controls.h"
#include "main_frame.h"
#include "main_debug.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"

#ifndef MAIN_RUN_TRADE_TEXT_BOX_HEIGHT
#define MAIN_RUN_TRADE_TEXT_BOX_HEIGHT 0x20
#endif

#ifndef MAIN_RUN_TRADE_TEXT_BOX_FLAGS
#define MAIN_RUN_TRADE_TEXT_BOX_FLAGS 0x20
#endif

#ifndef MAIN_RUN_TRADE_SCREEN_HEIGHT
#define MAIN_RUN_TRADE_SCREEN_HEIGHT 0xF0
#endif

#ifndef MAIN_RUN_TRADE_TEXT_BOX_Y_OFFSET
#define MAIN_RUN_TRADE_TEXT_BOX_Y_OFFSET 0x20
#endif

#ifndef MAIN_RUN_OPTIONS_INIT
#define MAIN_RUN_OPTIONS_INIT() \
    do { \
        Options_Init(); \
        Fade_WaitIn(); \
    } while (0)
#endif

#ifndef MAIN_RUN_OPTIONS_IS_DONE
#define MAIN_RUN_OPTIONS_IS_DONE(result) ((result) == 0)
#endif

#ifndef MAIN_RUN_OPTIONS_FINISH
#define MAIN_RUN_OPTIONS_FINISH() SD_BGMFadeOut()
#endif

/* The grouped mode runners use common-symbol forms overridden by the tracked
   link symbols, so these definitions do not allocate storage here. */
u8 gMain_bReturnMode;
u8 gMain_bActiveModeState;
u8 gMain_bModeRunnerStep;

void Main_RunOptionsMenu(void)
{
    u8 flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestOptionsPackage();
        MAIN_RUN_OPTIONS_INIT();
    }
    if (MAIN_RUN_OPTIONS_IS_DONE(Options_Update())) {
        u8 value;

        MAIN_RUN_OPTIONS_FINISH();
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}

void Main_RunGameOver(void)
{
    u8 flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        File_RequestGameOverPackage();
        GameOver_Init();
    }

    if (GameOver_Update() == 0) {
        u8 mode;

        SD_BGMFadeOut();
        Fade_WaitOut();
        mode = gMain_bReturnMode;
        gMain_bActiveModeState = mode;
        if (mode != 0) {
            D_8009B268 = 1;
            gMain_bRequestedMenuID = 0;
            gMain_bActiveModeState = 8;
            longjmp(gMain_RecoveryJmpBuf, 1);
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

    if ((gMain_bActiveModeState & 0x40) == 0) {
        gMain_bActiveModeState = gMain_bActiveModeState | 0x40;
        CardList_LoadSharedGraphics();
        SD_BGMPlay(0x72D0);
        MainMenu_InitTradeScreen();
        gMain_bModeRunnerStep = 1;
        box = TextBox_CreateFlagged(
            0,
            0xB,
            0x18,
            MAIN_RUN_TRADE_TEXT_BOX_HEIGHT,
            0x110,
            0xA0,
            MAIN_RUN_TRADE_TEXT_BOX_FLAGS
        );
        box->field_59 = 0x10;
        TextBox_BuildBlocking(box);
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(obj, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        obj->flags = obj->flags | 0x20;
        obj->attribute = obj->attribute | GsALON;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0xF);
        gDisplayObject_apWorkSlots[0] = obj;
        Fade_WaitIn();
    }

    box = gText_aChannels;
    obj = gDisplayObject_apWorkSlots[0];
    switch (gMain_bModeRunnerStep) {
    case 1:
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0xE0) != 0) {
            SD_SEPlayFull(0x1E);
            DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
            obj->field_60 = 0x400;
            gMain_bModeRunnerStep = 2;
        }
        break;
    case 2:
        y = *(u16 *)&obj->field_60 - 0x20;
        obj->field_60 = y;
        Widget_SlideSine(
            (DisplayObjectPosition *)obj,
            0,
            MAIN_RUN_TRADE_SCREEN_HEIGHT,
            (s16)y
        );
        TextBox_SetPos(
            box,
            *(s16 *)&obj->field_30.h.field_30 + 0x18,
            *(s16 *)&obj->field_30.h.field_32 + MAIN_RUN_TRADE_TEXT_BOX_Y_OFFSET
        );
        if (obj->field_60 <= 0) {
            gMain_bModeRunnerStep = 0;
            TextBox_Destroy(box);
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[0]);
            gDisplayObject_apWorkSlots[0] = 0;
        }
        break;
    default:
        if (DuelEffect_UpdateState() == 0) {
            if (MainMenu_UpdateTradeScreen() != 0) {
                SD_BGMFadeOut();
                Fade_WaitOut();
                MainMenu_ReleaseTradeDisplayHandles();
                gMain_bActiveModeState = gMain_bReturnMode;
            }
        }
        break;
    }
}

/*
 * Keep the staging pair absolute: `Main_RunCredits`
 *
 * The 540-byte runner at `0x8002DA1C` matches under the existing uniform
 * `gcc_2_8_1_g8_split` profile. Its six terminal refinement attempts failed
 * to link because an eight-byte scalar view of `gText_StagedValues` selected
 * out-of-range GP-relative relocations. The existing incomplete
 * `TextStagingValues` array and its `pair` member keep both staging stores
 * absolute without a new alias, profile, or inflated declaration.
 *
 * The source uses the shared save-state layout and reads `duelist_code` as
 * `u32` for the target's unsigned modulo-five sequence. The secret-number
 * table, memory-card operations, text-box lifecycle and model-scene calls use
 * their existing canonical declarations. The frame-delay byte selects the
 * existing DATA view; the active-mode byte remains scalar and GP-relative
 * through the canonical scalar view selected in unmatched.h.
 *
 * The shared completion-initialization label, save retry, secret-number
 * display and credits-scene phases retain their original branch order and
 * one-time flags. No register pins, inline assembly or local externs remain.
 * The canonical history and six-row terminal refinement history are preserved,
 * followed by one post-terminal resolution with this source evidence.
 */

#ifndef MAIN_RUN_CREDITS_GEOM_OFFSET_Y
#define MAIN_RUN_CREDITS_GEOM_OFFSET_Y 0x78
#endif

#ifndef MAIN_RUN_CREDITS_GEOM_SCREEN
#define MAIN_RUN_CREDITS_GEOM_SCREEN MODEL_DEFAULT_PROJECTION
#endif

#ifndef MAIN_RUN_CREDITS_TEXT_BOX_Y
#define MAIN_RUN_CREDITS_TEXT_BOX_Y 0x20
#endif

#ifndef MAIN_RUN_CREDITS_TEXT_BOX_FLAGS
#define MAIN_RUN_CREDITS_TEXT_BOX_FLAGS 8
#endif

#ifndef MAIN_RUN_CREDITS_COMPLETION_FLAG
#define MAIN_RUN_CREDITS_COMPLETION_FLAG 8
#endif

void Main_RunCredits(void)
{
    SaveDataState *save_state;
    DuelEffectChannel *textbox;
    u16 *number;
    u16 *table;
    s32 one;
    s32 flag;
    s32 mode;
    s32 state;
    s32 phase;

    SetGeomOffset(0xA0, MAIN_RUN_CREDITS_GEOM_OFFSET_Y);
    SetGeomScreen(MAIN_RUN_CREDITS_GEOM_SCREEN);

    mode = gMain_bActiveModeState;
    if ((mode & 0x40) == 0) {
        gMain_bActiveModeState = mode | 0x40;
        Fade_InitIn();
    initialize_completion:
        save_state = (SaveDataState *)gDuel_awPlayerDeck;
        *(s16 *)&save_state->campaign_scene_index = 0x30;
        for (flag = 0x20; flag < 0x120; flag++) {
            Library_UpdateCardUsedFlag(flag | 0x8000);
        }
        gMain_bModeRunnerStep = 0;
        save_state->field_3DE = save_state->field_3DE | 3;
    }

    state = gMain_bModeRunnerStep;
    one = 1;
    phase = state & 0xF;

    if (phase == one) {
        goto show_secret_number;
    }
    if (phase < 2) {
        if (phase == 0) {
            goto save_completion;
        }
        return;
    }
    if (phase == 2) {
        goto run_credits_scene;
    }
    return;

save_completion:
    if ((state & 0x80) == 0) {
        gMain_bModeRunnerStep = state | 0x80;
        SaveData_RequestWrite();
    }
    state = MemCardDialog_Poll();
    if (state == 0) {
        return;
    }
    if (state == 2) {
        goto initialize_completion;
    }
    gMain_bModeRunnerStep = one;
    return;

show_secret_number:
    if ((state & 0x80) == 0) {
        gMain_bModeRunnerStep = state | 0x80;
        state = ((u32)((SaveDataState *)gDuel_awPlayerDeck)->duelist_code) %
            CREDITS_SECRET_NUMBER_COUNT;
        table = (u16 *)gCredits_awSecretNumbers;
        number = &table[state * 2];
        gText_StagedValues[0].pair.lo = number[0];
        gText_StagedValues[0].pair.hi = number[1];
        TextBox_CreateFlagged(
            0,
            0x23,
            0x10,
            0x70,
            0x120,
            MAIN_RUN_CREDITS_TEXT_BOX_Y,
            MAIN_RUN_CREDITS_TEXT_BOX_FLAGS
        );
    }
    TextBox_UpdateChannels();
    textbox = gText_aChannels;
    if ((textbox->flags_34 & MAIN_RUN_CREDITS_COMPLETION_FLAG) == 0) {
        TextBox_Destroy(textbox);
        gMain_bModeRunnerStep = 2;
    }
    return;

run_credits_scene:
    if ((state & 0x80) == 0) {
        gMain_bModeRunnerStep = state | 0x80;
#ifdef MAIN_RUN_CREDITS_EUROPE_SCENE_CALLBACKS
        DuelEffect_ResetState();
        DisplayEffect_ResetMenuRecords();
#endif
        gGraphics_bSyncVBlankTarget = one;
        Model_ResetSceneView();
        Model_ResetSceneState();
        Model_StartCreditsPresentation();
        return;
    }
    Model_IsCreditsPresentationComplete();
#ifdef MAIN_RUN_CREDITS_EUROPE_SCENE_CALLBACKS
    TextBox_UpdateChannels();
#endif
    Model_UpdateScene();
}

/* The first text box's x and width; the Japanese build draws it wider. */
#ifndef TWO_PLAYER_SETUP_BOX_X
#define TWO_PLAYER_SETUP_BOX_X 0x34
#define TWO_PLAYER_SETUP_BOX_WIDTH 0xD8
#endif
#ifndef TWO_PLAYER_SETUP_BOX_Y
#define TWO_PLAYER_SETUP_BOX_Y 0xB4
#endif
#ifndef TWO_PLAYER_SETUP_BOX_HEIGHT
#define TWO_PLAYER_SETUP_BOX_HEIGHT 0x20
#endif
#ifndef TWO_PLAYER_SETUP_BOX_FLAGS
#define TWO_PLAYER_SETUP_BOX_FLAGS 0x20
#endif
#ifndef TWO_PLAYER_SETUP_SECOND_BOX_X
#define TWO_PLAYER_SETUP_SECOND_BOX_X 0xE
#endif
/* The text-box channel record; the Japanese one is 0x60 bytes. */
#ifndef TWO_PLAYER_SETUP_CHANNEL_TYPE
#define TWO_PLAYER_SETUP_CHANNEL_TYPE DuelEffectChannel
#endif

void Main_RunTwoPlayerDuelSetup(void)
{
    s32 result;

    if ((gMain_bActiveModeState & 0x40) == 0) {
        gMain_bActiveModeState = gMain_bActiveModeState | 0x40;
        gDuel_wTwoPlayerSide1LifePoints = DUEL_STARTING_LIFE_POINTS;
        gDuel_wTwoPlayerSide0LifePoints = DUEL_STARTING_LIFE_POINTS;
        MainMenu_StartValueSetup(&gDuel_wTwoPlayerSide0LifePoints, &gDuel_wTwoPlayerSide1LifePoints, (u8 *)&gDuel_wTwoPlayerCardViewMode);
        TextBox_CreateFlagged(
            0,
            0x25,
            TWO_PLAYER_SETUP_BOX_X,
            TWO_PLAYER_SETUP_BOX_Y,
            TWO_PLAYER_SETUP_BOX_WIDTH,
            TWO_PLAYER_SETUP_BOX_HEIGHT,
            TWO_PLAYER_SETUP_BOX_FLAGS
        );
        TextBox_BuildBlocking(gText_aChannels);
#ifdef TWO_PLAYER_SETUP_SECOND_BOX_FLAGS
        TextBox_CreateFlagged(
            1,
            0x26,
            TWO_PLAYER_SETUP_SECOND_BOX_X,
            0x66,
            0x100,
            0x30,
            TWO_PLAYER_SETUP_SECOND_BOX_FLAGS
        );
#else
        TextBox_Create(
            1,
            0x26,
            TWO_PLAYER_SETUP_SECOND_BOX_X,
            0x66,
            0x100,
            0x30
        );
#endif
        TextBox_BuildBlocking((DuelEffectChannel *)&(
            (TWO_PLAYER_SETUP_CHANNEL_TYPE *)gText_aChannels)[1]);
        SD_BGMPlay(0x72C0);
        Fade_WaitIn();
    }

    result = MainMenu_UpdateValueSetup();
    if (result != 0) {
        SD_BGMFadeOut();
        Fade_WaitOut();
        MainMenu_FinishValueSetup();
        if (result == 1) {
            Main_StartDuel(-1, -1, 0, 0);
            gDuel_bPostDuelMainMode = 8;
        } else {
            gMain_bActiveModeState = gMain_bReturnMode;
        }
    }
}

void Main_Loop(void) {
    Main_PrepareFrontendLoop();
    for (;;) {
        u8 v;
        Main_AdvanceFrame();
        v = gMain_bActiveModeState;
        if ((v & 0x80) == 0) {
            gMain_bActiveModeState = v | 0x80;
            Main_ResetFrontendRuntime();
        } else {
            gMain_apfnModeRunner[v & 0x1F]();
            if ((gMain_bActiveModeState & 0x40) == 0) Fade_WaitOut();
        }
    }
}

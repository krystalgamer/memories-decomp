#include "../types.h"
#include "display_object_system.h"
#include "main.h"
#include "duel_effect_text.h"
#include "input.h"
#include "main_mode_runners.h"
#include "main_services.h"
#include "duel_effect_states.h"

void Main_ResetFrontendRuntime(void)
{
    gGraphics_bSyncVBlankTarget = 0;
    DisplayObject_Reset();
    DuelEffect_ResetState();
    DisplayEffect_ResetMenuRecords();
    Main_ClearFrameServiceCallbacks();
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    gDuel_bEffectState = 0;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
}

#include "debug_menu.h"
#include "main_modes.h"
#include "main_mode_runners.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bReturnMode;
u8 gMain_bActiveModeState;

void Main_PrepareFrontendLoop(void)
{
    Main_ResetFrontendRuntime();
}

void Main_RunDebugMenu(void)
{
    u8 flags = gMain_bActiveModeState;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        DebugMenu_Init();
    } else {
        DebugMenu_Update();
        if ((gMain_bActiveModeState & MAIN_MODE_STATE_STARTED) == 0)
            gMain_bReturnMode = MAIN_MODE_DEBUG;
    }
}

#include "ai.h"
#include "script_scene.h"

/* Zero-initialized storage this unit owns at 0x800EAE88-0x800EAED8, in
 * address order (#2602). Each common is aligned by its size, so the slot
 * table starts at +0x10; its three slots and the image-id halfword fill
 * the 0x40 bytes before gDebug_szEditorCaretLine, matching
 * SceneScript_UpdateSlots' sweep. */
AiSelection gAi_Selection;
SceneScriptSlotTable gSceneScript_Slots;

#include "duel_effect_states.h"
#include "fade.h"
#include "sound.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bActiveModeState;

void Main_RunCampaign(void) {
    u8 flags = gMain_bActiveModeState;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        Campaign_LoadScenePackage(gCampaignSceneIndex);
        gDuel_bEffectState = 0;
        return;
    }

    SceneScript_UpdateSlots(gSceneScript_Slots.slots);
    Script_RunTick();
    if (gMain_bActiveModeState & MAIN_MODE_STATE_STARTED)
        return;
    SD_BGMFadeOut();
    Fade_WaitOut();
}

#include "ai_script.h"
#include "../game/duel_scene.h"
#include "../game/main.h"
#include "../game/build_deck.h"
#include "../game/mem_card.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_modes.h"
#include "../game/main_mode_runners.h"
#include "../game/main_services.h"
#include "../game/sound.h"
#include "../game/sound_effect.h"
#include "../game/library.h"
#include "duel_session_controls.h"

u8 gMain_bActiveModeState;

#include "../game/shared_work_base.h"
#include "../game/main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bModeRunnerStep;

void Main_RunDuel(void)
{
    u8 value = gMain_bActiveModeState;
    int state;

    if (!(value & MAIN_MODE_STATE_STARTED)) {
        gMain_bActiveModeState = value | MAIN_MODE_STATE_STARTED;
        gMain_bModeRunnerStep = 1;
        if (!gDuel_bResumeFromAnimatedBattle && gDuel_bOpponentID >= 0)
            gMain_bModeRunnerStep = 0;
        gGraphics_abOrderingTableLengths[3] = 10;
        return;
    }

    value = gMain_bModeRunnerStep;
    state = value & MAIN_MODE_RUNNER_STEP_MASK;
    switch (state) {
    case 0:
        if (!(value & MAIN_MODE_RUNNER_STEP_STARTED)) {
            gMain_bModeRunnerStep = value | MAIN_MODE_RUNNER_STEP_STARTED;
            gBuildDeck_bConfirmMode = 0x80;
            BuildDeck_InitScreen(
                (BuildDeckTransitionState *)gRuntime_pSharedWorkBase,
                gDuel_awPlayerDeck, 0, 0x80);
            Fade_WaitIn();
        } else if (BuildDeck_Update() == 0) {
            SD_BGMFadeOut();
            Fade_WaitOut();
            Main_ResetFrontendRuntime();
            gMain_bModeRunnerStep = 1;
        }
        break;
    case 1:
        if (!(value & MAIN_MODE_RUNNER_STEP_STARTED)) {
            gMain_bModeRunnerStep = value | MAIN_MODE_RUNNER_STEP_STARTED;
            Duel_InitScene();
        } else {
            DuelScene_UpdateWithSideInput();
            if (gDuel_wOutcomeFlags & 0x2000)
                gMain_bModeRunnerStep = 2;
        }
        break;
    case 2:
    {
        u16 *table = gDuel_awCampaignSceneByOutcome;
        u8 next;

        Fade_WaitOut();
        SD_BGMFadeOut();
        SD_ReplaceValueLink(2);
        Main_ClearFrameServiceCallbacks();
        gGraphics_abOrderingTableLengths[3] = 6;
        Main_AdvanceFrames(4);
        File_WaitForTransfers();
        next = gDuel_bPostDuelMainMode;
        gMain_bActiveModeState = next;
        if (gMain_bActiveModeState == state)
            gCampaignSceneIndex = table[gDuel_bPlayerLost];
        break;
    }
    }
}

void Main_RunLibraryMenu(void){unsigned char f=gMain_bActiveModeState;if((f&MAIN_MODE_STATE_STARTED)==0){gMain_bActiveModeState=f|MAIN_MODE_STATE_STARTED;Library_InitializeScreen();Fade_WaitIn();}else{Library_UpdateScreen();if((gMain_bActiveModeState& MAIN_MODE_STATE_STARTED)==0){gGraphics_bSyncVBlankTarget=0;SD_BGMFadeOut();Fade_WaitOut();Main_ClearFrameServiceCallbacks();SD_ResetPendingEntries();SD_ReplaceValueLink(2);Main_AdvanceFrames(4);File_WaitForTransfers();}}}

#include "main.h"
#include "model_scene_states.h"
#include "model.h"
#include "../psyq/libgte.h"
#include "fade.h"
#include "sound.h"
#include "main_modes.h"
#include "model_scene_runtime.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bReturnMode;

/* Regional value: the geometry offset y, half the screen height (0x78 ->
 * 0x80). The European build (src/game/european/) defines its own. */
#ifndef ANIMATED_BATTLE_GEOM_OFFSET_Y
#define ANIMATED_BATTLE_GEOM_OFFSET_Y 0x78
#endif

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the first read of it below. c_symbols.ld overrides
   this common symbol, so no storage is allocated here. */
u8 gMain_bActiveModeState;

void Main_RunAnimatedBattle(void)
{
    AnimatedBattleModelProperties *p;
    u8 f;
    s32 v;

    SetGeomOffset(0xA0, ANIMATED_BATTLE_GEOM_OFFSET_Y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
    f = gMain_bActiveModeState;
    if ((f & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = f | MAIN_MODE_STATE_STARTED;
        gGraphics_bSyncVBlankTarget = 1;
        Model_ResetSceneView();
        Model_ResetSceneState();
        p = gMain_aAnimatedBattleModels;
        if (p->model_id == MODEL_SPECIAL_BATTLE_ID) {
            gMain_bActiveModeState = gMain_bActiveModeState | MAIN_MODE_STATE_EXODIA;
            Model_StartExodiaPresentation();
        } else {
            Model_SetSlotProperties(
                0,
                p->model_id - 1,
                p->attack_compare_override,
                p->defense_compare_override,
                p->guardian_star_selection,
                p->defense_position
            );
            p++;
            Model_SetSlotProperties(
                1,
                p->model_id - 1,
                p->attack_compare_override,
                p->defense_compare_override,
                p->guardian_star_selection,
                p->defense_position
            );
            Model_SetSlotProperties(2, gDuel_bTerrain);
        }
        Fade_WaitInitIn();
    } else {
        if (f & MAIN_MODE_STATE_EXODIA) {
            v = Model_IsExodiaPresentationComplete();
        } else {
            v = ModelDebug_UpdateController();
        }
        if (v != 0) {
            SD_KeyOffVoiceSlots();
            SD_BGMFadeOut();
            gMain_bActiveModeState = gMain_bReturnMode;
        }
        Model_UpdateScene();
    }
}

#include "duel_effect_text.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "build_deck.h"
#include "file_transfer.h"
#include "mem_card.h"
#include "main.h"
#include "frontend_package_stages.h"
#include "main_services.h"
#include "main_modes.h"
#include "fade.h"
#include "sound.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bReturnMode;

void Main_RunCampaignMap(void){unsigned char f=gMain_bActiveModeState;if((f&MAIN_MODE_STATE_STARTED)==0){gMain_bActiveModeState=f|MAIN_MODE_STATE_STARTED;gGraphics_abOrderingTableLengths[3]=10;File_RequestEgyptOverworldPackage();CampaignMap_SetLocation(gCampaignMap_bEntryLocation);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((gMain_bActiveModeState& MAIN_MODE_STATE_STARTED)==0){Main_ClearFrameServiceCallbacks();gGraphics_abOrderingTableLengths[3]=6;Main_AdvanceFrame();}}}

/* The common definition makes MASPSX preserve the load-delay nop before the
 * final gMain_bActiveModeState store. c_symbols.ld supplies the retail storage address. */
u8 gMain_bActiveModeState;

void Main_RunBuildDeckMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        BuildDeck_InitScreen(
            (BuildDeckTransitionState *)gRuntime_pSharedWorkBase,
            gDuel_awPlayerDeck,
            0,
            gBuildDeck_bConfirmMode
        );
        Fade_WaitIn();
    } else if (BuildDeck_Update() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        Fade_WaitOut();
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}

void Main_RunFreeDuelMenu(void) {
    unsigned char flags = gMain_bActiveModeState;
    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        Main_InitFreeDuelMenu();
        Fade_WaitIn();
    }
    FreeDuel_Entry();
    if ((gMain_bActiveModeState & MAIN_MODE_STATE_STARTED) == 0) SD_BGMFadeOut();
}

#include "display_object_system.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "input.h"
#include "../overlays/main_menu/trade_helpers.h"
#include "fade.h"
#include "file_transfer.h"
#include "game_over.h"
#include "main_modes.h"
#include "main_services.h"
#include "options.h"
#include "sound.h"
#include "build_deck.h"
#include "display_object_work_slots.h"
#include "main_mode_state.h"
#include "mem_card.h"
#include "campaign_flags.h"
#include "main_mode_runners.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "main.h"
#include "model_scene_states.h"
#include "card_constants.h"
#include "../overlays/main_menu/value_setup.h"
#include "duel_session_controls.h"
#include "main_mode_runners.h"
#include "main_mode_runners.h"
#include "../overlays/password/name_entry_main.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/main_menu/frontend.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bRequestedMenuID;
u8 D_8009B268;

/* Initialized data this unit owns at 0x80090B50. */
u16 gCredits_awSecretNumbers[CREDITS_SECRET_NUMBER_COUNT][2] = {
    {0x21fa, 0x15ff},
    {0x0eb8, 0x24da},
    {0x1c93, 0x0e98},
    {0x2448, 0x1917},
    {0x14d1, 0x1d6e},
};

/* Initialized data this unit owns at 0x80090B64: the mode dispatch table Main_Loop indexes
   with the low five bits of the frontend state byte. */
MainModeRunner gMain_apfnModeRunner[MAIN_MODE_COUNT] = {
    Main_RunDebugMenu,
    Main_RunAnimatedBattle,
    Main_RunCampaign,
    Main_RunDuel,
    Main_RunLibraryMenu,
    Main_RunCampaignMap,
    Main_RunFreeDuelMenu,
    Main_RunBuildDeckMenu,
    Main_RunMenu,
    Main_RunNameEntry,
    Main_RunPasswordMenu,
    Main_RunOptionsMenu,
    Main_RunGameOver,
    Main_RunUnusedDeveloperMode,
    Main_RunTrade,
    Main_RunCredits,
    Main_RunTwoPlayerDuelSetup,
};

void Main_ApplyMenuSelection(MainMenuSelection selection)
{
    D_8009B268 = 1;
    gMain_bRequestedMenuID = selection;

    switch (selection) {
    case MAIN_MENU_SELECTION_NEW_GAME:
        File_RequestNameEntryPackage();
        NameEntry_Main();
        gCampaignSavedSceneIndex[0] = 0x30;
        gCampaignSceneIndex = 0x30;
        gSaveData_AllowIdentityBypass = 1;
        D_8009B0D1 = 0;
        gMain_bActiveModeState = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_TWO_PLAYER_DUEL:
        gMain_bActiveModeState = MAIN_MODE_TWO_PLAYER_DUEL_SETUP;
        break;
    case MAIN_MENU_SELECTION_TRADE:
        gMain_bActiveModeState = MAIN_MODE_TRADE;
        break;
    case MAIN_MENU_SELECTION_LIBRARY:
        gMain_bActiveModeState = MAIN_MODE_LIBRARY;
        break;
    case MAIN_MENU_SELECTION_CAMPAIGN:
        gCampaignSceneIndex = gCampaignSavedSceneIndex[0];
        gMain_bActiveModeState = MAIN_MODE_CAMPAIGN;
        break;
    case MAIN_MENU_SELECTION_FREE_DUEL:
        gFreeDuel_bReturnFlags = 0;
        gMain_bActiveModeState = MAIN_MODE_FREE_DUEL;
        break;
    case MAIN_MENU_SELECTION_BUILD_DECK:
        BuildDeck_EnterNarrowConfirmMode();
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_OPTIONS:
        gMain_bActiveModeState = MAIN_MODE_OPTIONS;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_PASSWORD:
        gMain_bActiveModeState = MAIN_MODE_PASSWORD;
        D_8009B268 = 0;
        break;
    case MAIN_MENU_SELECTION_SAVE:
    default:
        gMain_bActiveModeState = MAIN_MODE_DEBUG;
        break;
    }
}

void Main_RunMenu(void){unsigned char f=gMain_bActiveModeState;int r;if((f&MAIN_MODE_STATE_STARTED)==0){gMain_bActiveModeState=f|MAIN_MODE_STATE_STARTED;File_RequestMainMenuPackage();File_WaitForTransfers();DisplayEffect_ResetMenuRecords();MainMenu_InitFrontendMenu(D_8009B268,gMain_bRequestedMenuID);Fade_WaitIn();}rand();r=MainMenu_UpdateFrontendMenu();if(r>=0){SD_BGMFadeOut();Fade_WaitOut();MainMenu_DestroyFrontendMenu();Main_ApplyMenuSelection(r);gMain_bReturnMode=MAIN_MODE_MENU;}}

u8 gMain_bActiveModeState;

void Main_RunNameEntry(void)
{
    u8 flags = gMain_bActiveModeState;
    u8 value;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        File_RequestNameEntryPackage();
        NameEntry_Init();
    }
    if (NameEntry_PollCompletion()) {
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}

void Main_RunPasswordMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
        File_RequestPasswordPackage();
        Password_InitShopScreen();
    }
    Password_UpdateShopScreen();
}

#ifndef MAIN_RUN_TRADE_TEXT_BOX_HEIGHT
#define MAIN_RUN_TRADE_TEXT_BOX_HEIGHT 0x20
#endif

#ifndef MAIN_RUN_TRADE_TEXT_BOX_FLAGS
#define MAIN_RUN_TRADE_TEXT_BOX_FLAGS 0x20
#endif

#ifndef MAIN_RUN_TRADE_SCREEN_HEIGHT
#define MAIN_RUN_TRADE_SCREEN_HEIGHT 0xF0
#endif

#ifndef MAIN_RUN_OPTIONS_INIT
#ifdef VERSION_EUROPE
#define MAIN_RUN_OPTIONS_INIT() Options_Init(0)
#else
#define MAIN_RUN_OPTIONS_INIT() \
    do { \
        Options_Init(); \
        Fade_WaitIn(); \
    } while (0)
#endif
#endif

#ifndef MAIN_RUN_OPTIONS_IS_DONE
#ifdef VERSION_EUROPE
#define MAIN_RUN_OPTIONS_IS_DONE(result) ((result) >= 0)
#else
#define MAIN_RUN_OPTIONS_IS_DONE(result) ((result) == 0)
#endif
#endif

#ifndef MAIN_RUN_OPTIONS_FINISH
#ifdef VERSION_EUROPE
#define MAIN_RUN_OPTIONS_FINISH()
#else
#define MAIN_RUN_OPTIONS_FINISH() SD_BGMFadeOut()
#endif
#endif

/* The grouped mode runners use common-symbol forms overridden by the tracked
   link symbols, so these definitions do not allocate storage here. */
u8 gMain_bReturnMode;
u8 gMain_bModeRunnerStep;

void Main_RunOptionsMenu(void)
{
    u8 flags = gMain_bActiveModeState;

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
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

    if ((flags & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = flags | MAIN_MODE_STATE_STARTED;
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
            gMain_bActiveModeState = MAIN_MODE_MENU;
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

    if ((gMain_bActiveModeState & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = gMain_bActiveModeState | MAIN_MODE_STATE_STARTED;
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
        box->depth_offset = 0x10;
        TextBox_BuildBlocking(box);
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(obj, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        obj->flags = obj->flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
        obj->attribute = obj->attribute | GsALON;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0xF);
        gDisplayObject_apWorkSlots.slots[0] = obj;
        Fade_WaitIn();
    }

    box = gText_aChannels;
    obj = gDisplayObject_apWorkSlots.slots[0];
    switch (gMain_bModeRunnerStep) {
    case 1:
        if (((gInput_awPadPressed[0] | gInput_awPadPressed[1]) & 0xE0) != 0) {
            SD_SEPlayFull(0x1E);
            DisplayObject_SavePosition(obj);
            obj->field_60.phase = 0x400;
            gMain_bModeRunnerStep = 2;
        }
        break;
    case 2:
        y = (u16)obj->field_60.phase - 0x20;
        obj->field_60.phase = y;
        Widget_SlideSine(
            obj,
            0,
            MAIN_RUN_TRADE_SCREEN_HEIGHT,
            (s16)y
        );
        TextBox_SetPos(
            box,
            obj->xy.s.x + 0x18,
#ifdef VERSION_EUROPE
            obj->xy.s.y + 0x28
#else
            obj->xy.s.y + 0x20
#endif
        );
        if (obj->field_60.phase <= 0) {
            gMain_bModeRunnerStep = 0;
            TextBox_Destroy(box);
            DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots.slots[0]);
            gDisplayObject_apWorkSlots.slots[0] = 0;
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
 * The shared completion initialization (re-entered by the save retry
 * through a loop that an initialized runner enters at the dispatch),
 * secret-number display and credits-scene phases retain their original
 * branch order and one-time flags. No register pins, inline assembly or local externs remain.
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
    /* A failed save write re-enters the completion setup and dispatches the
       step again; the loop is entered at the dispatch when the runner is
       already initialized. */
    switch (mode & MAIN_MODE_STATE_STARTED) {
    case 0:
        gMain_bActiveModeState = mode | MAIN_MODE_STATE_STARTED;
        Fade_InitIn();
        for (;;) {
            save_state = &gSaveData_Workspace.state;
            save_state->campaign_scene.word = 0x30;
            for (flag = 0x20; flag < 0x120; flag++) {
                Library_UpdateCardUsedFlag(flag | 0x8000);
            }
            gMain_bModeRunnerStep = 0;
            save_state->credits_flags = save_state->credits_flags | 3;
    default:
            state = gMain_bModeRunnerStep;
            one = 1;
            phase = state & MAIN_MODE_RUNNER_STEP_MASK;

            switch (phase) {
            case 0:
                if ((state & MAIN_MODE_RUNNER_STEP_STARTED) == 0) {
                    gMain_bModeRunnerStep = state | MAIN_MODE_RUNNER_STEP_STARTED;
                    SaveData_RequestWrite();
                }
                state = MemCardDialog_Poll();
                if (state == 0) {
                    return;
                }
                if (state == 2) {
                    continue;
                }
                gMain_bModeRunnerStep = one;
                return;

            case 1:
                if ((state & MAIN_MODE_RUNNER_STEP_STARTED) == 0) {
                    gMain_bModeRunnerStep = state | MAIN_MODE_RUNNER_STEP_STARTED;
                    state = ((u32)gSaveData_Workspace.state.duelist_code) %
                        CREDITS_SECRET_NUMBER_COUNT;
                    table = gCredits_awSecretNumbers[0];
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

            case 2:
                if ((state & MAIN_MODE_RUNNER_STEP_STARTED) == 0) {
                    gMain_bModeRunnerStep = state | MAIN_MODE_RUNNER_STEP_STARTED;
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
                break;
            }
            return;
        }
    }
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
void Main_RunTwoPlayerDuelSetup(void)
{
    s32 result;

    if ((gMain_bActiveModeState & MAIN_MODE_STATE_STARTED) == 0) {
        gMain_bActiveModeState = gMain_bActiveModeState | MAIN_MODE_STATE_STARTED;
        gDuel_wTwoPlayerSide1LifePoints = DUEL_STARTING_LIFE_POINTS;
        gDuel_wTwoPlayerSide0LifePoints = DUEL_STARTING_LIFE_POINTS;
        MainMenu_StartValueSetup(&gDuel_wTwoPlayerSide0LifePoints, &gDuel_wTwoPlayerSide1LifePoints, &gDuel_bTwoPlayerCardViewMode);
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
        TextBox_BuildBlocking(&gText_aChannels[1]);
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
            gDuel_bPostDuelMainMode = MAIN_MODE_MENU;
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
        if ((v & MAIN_MODE_STATE_RUNTIME_RESET) == 0) {
            gMain_bActiveModeState = v | MAIN_MODE_STATE_RUNTIME_RESET;
            Main_ResetFrontendRuntime();
        } else {
            gMain_apfnModeRunner[v & MAIN_MODE_STATE_MODE_MASK]();
            if ((gMain_bActiveModeState & MAIN_MODE_STATE_STARTED) == 0) Fade_WaitOut();
        }
    }
}

#include "../types.h"
#include "../game/ai_opponent_data.h"
#include "../game/duel_check_quit_input.h"
#include "../game/duel_init_scene.h"
#include "../game/main_start_duel.h"
#include "../game/graphics_frame.h"
#include "../game/build_deck_init_screen.h"
#include "../game/build_deck_update.h"
#include "../game/save_data.h"
#include "../game/main_frame.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_modes.h"
#include "../game/main_reset_frontend_runtime.h"
#include "../game/main_services.h"
#include "../game/sound.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"
#include "../game/library_runtime.h"
#include "../game/duel_scene_update.h"
#include "../game/duel_side_state.h"
#include "duel_session_controls.h"

u8 gMain_bActiveModeState;

#include "../game/shared_work_base.h"
#include "../game/main_mode_state.h"
#include "build_deck_draw_card_list_page_data.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bModeRunnerStep;

void Main_RunDuel(void)
{
    u8 value = gMain_bActiveModeState;
    int state;

    if (!(value & 0x40)) {
        gMain_bActiveModeState = value | 0x40;
        gMain_bModeRunnerStep = 1;
        if (!gDuel_bResumeFromAnimatedBattle && gDuel_bOpponentID >= 0)
            gMain_bModeRunnerStep = 0;
        gGraphics_abOrderingTableLengths[3] = 10;
        return;
    }

    value = gMain_bModeRunnerStep;
    state = value & 0xF;
    switch (state) {
    case 0:
        if (!(value & 0x80)) {
            gMain_bModeRunnerStep = value | 0x80;
            gBuildDeck_bConfirmMode = 0x80;
            BuildDeck_InitScreen(
                (BuildDeckTransitionState *)gRuntime_pSharedWorkBase,
                (u8 *)gDuel_awPlayerDeck, 0, 0x80);
            Fade_WaitIn();
        } else if (BuildDeck_Update() == 0) {
            SD_BGMFadeOut();
            Fade_WaitOut();
            Main_ResetFrontendRuntime();
            gMain_bModeRunnerStep = 1;
        }
        break;
    case 1:
        if (!(value & 0x80)) {
            gMain_bModeRunnerStep = value | 0x80;
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

void Main_RunLibraryMenu(void){unsigned char f=gMain_bActiveModeState;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;Library_InitializeScreen();Fade_WaitIn();}else{Library_UpdateScreen();if((gMain_bActiveModeState&0x40)==0){gGraphics_bSyncVBlankTarget=0;SD_BGMFadeOut();Fade_WaitOut();Main_ClearFrameServiceCallbacks();SD_ResetPendingEntries();SD_ReplaceValueLink(2);Main_AdvanceFrames(4);File_WaitForTransfers();}}}

#define D_8009B368_IN_DATA
#define D_8009B230_IN_DATA
#define D_8009B234_IN_DATA
#define D_8009B236_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "text_box_blocking_build.h"
#include "duel_side_state.h"
#include "main_start_duel.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "card_constants.h"
#include "fade.h"
#include "main_modes.h"
#include "text_box_lifecycle.h"
#include "../overlays/main_menu/value_setup.h"
#include "sound.h"
#include "text_box_runtime.h"
#include "../unmatched.h"
#include "duel_session_controls.h"
#include "main_mode_state.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store of gMain_bReturnMode into it. c_symbols.ld
   overrides this common symbol, so no storage is allocated here. */
u8 gMain_bActiveModeState;

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

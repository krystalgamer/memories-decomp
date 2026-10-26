#define gDuel_bEffectRequestStatus_IN_DATA
#define GDIALOG_CHOICE_SIZED
#include "../types.h"
#include "duel_side_state.h"
#include "input.h"
#include "duel_check_quit_input.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_update_channels.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_state.h"
#include "duel_scene_update.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "duel_field_display_objects.h"
#include "../unmatched.h"
#include "duel_field_effects.h"
#include "dialog_choice.h"
#include "ai_opponent_data.h"

/* One frame of the duel scene. It services the quit dialog when
 * gDuel_bQuitDialogState is live -- creating the box on the first frame and
 * tearing it down once its 0x2000 flag clears -- and otherwise dispatches the
 * current scene step through the scene callback table. */
/* The quit dialog's box. The Japanese and European builds draw it at another
   size, so a regional build supplies its own. */
#ifndef DUEL_QUIT_BOX_X
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50
#define DUEL_QUIT_BOX_HEIGHT 0x24
#endif
/* Its flags: 0x40 in the European build. */
#ifndef DUEL_QUIT_BOX_FLAGS
#define DUEL_QUIT_BOX_FLAGS 0x20
#endif

void DuelScene_Update(void)
{
    u8 value;
    DuelEffectChannel *window;

    if (gDuel_wDisplayTransitionFlags != 0) {
        Duel_UpdateDisplayTransition();
    }
    DuelEffect_UpdateRequests();
    value = gDuel_bEffectRequestStatus;
    if (value & DUEL_EFFECT_REQUEST_STATUS_ACTIVE) {
        if (value & DUEL_EFFECT_REQUEST_STATUS_BLOCKING) {
            return;
        }
        gDuel_bEffectRequestStatus =
            value & ~DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
    }
    if (DuelEffect_UpdateCardEffect() != 0 || DuelEffect_UpdateState() != 0) {
        return;
    }
    value = gDuel_bQuitDialogState;
    if (value != 0) {
        if (!(value & 0x80)) {
            gDuel_bQuitDialogState = value | 0x80;
            SD_SEPlayFull(0x30);
            window =
                TextBox_CreateFlagged(3, 0x22, DUEL_QUIT_BOX_X, 0x58,
                                      DUEL_QUIT_BOX_WIDTH,
                                      DUEL_QUIT_BOX_HEIGHT, DUEL_QUIT_BOX_FLAGS);
            do {
                TextBox_UpdateChannels();
            } while (window->prompt_object == 0);
        } else {
            DuelEffectChannel *cleanup;

            TextBox_UpdateChannels();
            cleanup = (DuelEffectChannel *)D_800EB224;
            if (cleanup->flags_34 & 0x2000) {
                TextBox_Destroy(cleanup);
                gDuel_bQuitDialogState = 0;
                if (gDialog_bChoice[0] != 0) {
                    gDuel_wOutcomeFlags |= 0x2000;
                }
            }
        }
    } else {
        void (**callbacks)(void);
        u16 index;

        callbacks = gDuel_apfnSceneStateHandler;
        index = gDuel_wSceneStateFlags;
        callbacks[index & DUEL_SCENE_PHASE_MASK]();
        if (!(gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED)) {
            gDuel_bSceneSubStep = 0;
        }
    }
}

void DuelScene_UpdateWithSideInput(void)
{
    int value = 0;

    if (gDuel_bOpponentID < 0) {
        value = gDuel_bActiveSide;
        if (gDuel_bOutroWinnerSide >= 0) {
            value = gDuel_bOutroWinnerSide;
        }
    }
    if (value != 0) {
        Input_BackupPad1AndUsePad2();
        DuelScene_Update();
        Input_RestorePad1FromBackup();
    } else {
        DuelScene_Update();
    }
}

#define gDuel_bEffectRequestStatus_IN_DATA
#define GDIALOG_CHOICE_SIZED
#include "../types.h"
#include "duel_side_state.h"
#include "input.h"
#include "duel_check_quit_input.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "func_80039794.h"
#include "duel_scene_callbacks.h"
#include "duel_scene_state.h"
#include "duel_scene_update.h"
#include "duel_effect.h"
#include "duel_effect_request.h"
#include "duel_field_display_objects.h"
#include "../unmatched.h"
#include "duel_magic_effect_dispatch.h"
#include "dialog_choice.h"

/* One frame of the duel scene. It services the quit dialog when
 * gDuel_bQuitDialogState is live -- creating the box on the first frame and
 * tearing it down once its 0x2000 flag clears -- and otherwise dispatches the
 * current scene step through the scene callback table. */
void DuelScene_Update(void)
{
    u8 value;
    DuelEffectChannel *window;

    if (D_8009B162 != 0) {
        func_800235C0();
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
                TextBox_CreateFlagged(3, 0x22, 0x78, 0x58, 0x50, 0x24, 0x20);
            do {
                func_80039794();
            } while (window->field_30 == 0);
        } else {
            DuelEffectChannel *cleanup;

            func_80039794();
            cleanup = (DuelEffectChannel *)D_800EB224;
            if (cleanup->flags_34 & 0x2000) {
                TextBox_Destroy(cleanup);
                gDuel_bQuitDialogState = 0;
                if (gDialog_bChoice[0] != 0) {
                    D_8009B16C |= 0x2000;
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
            D_8009B174 = 0;
        }
    }
}

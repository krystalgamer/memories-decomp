#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/rand.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "text_box_update_channels.h"
#include "script_op_show_dialog.h"
#include "script_command_table.h"
#include "script_run_tick.h"
#include "script_state.h"
#include "script_op_load_image_scene.h"
#include "script_op_open_campaign_map.h"
#include "script_op_show_menu.h"
#include "script_op_save_prompt.h"
#include "script_op_fade_out.h"
#include "script_op_duel_result.h"
#include "script_op_jump_if_deck_incomplete.h"
#include "script_op_return_to_menu.h"
#include "script_command_busy.h"
#include "script_control_commands.h"
#include "script_flag_commands.h"
#include "script_noop_commands.h"
#include "script_image_commands.h"
#include "script_stream_commands.h"
#include "../unmatched.h"

/* Initialized data this unit owns at 0x80090C50: the 23 scene-script command handlers.
 *
 * Script_RunTick masks the command word with 0x1F before indexing, while
 * TextBox_BuildStep can redispatch the current low byte directly. The image
 * contains exactly these entries, so the declaration remains unsized rather
 * than implying every masked value is valid.
 */

ScriptCommandHandler gScript_apfnCommandHandlers[] = {
    Script_OpHalt,
    Script_OpLoadImageScene,
    Script_OpShowDialog,
    Script_OpStoryFlag,
    Script_OpStageImage,
    Script_OpShowImage,
    Script_OpViewportTween,
    Script_UpdateViewportTween,
    Script_OpOpenCampaignMap,
    Script_OpShowMenu,
    Script_OpSound,
    Script_OpFadeBgm,
    Script_OpJump,
    Script_OpSavePrompt,
    Script_OpUnusedHalt14,
    Script_OpUnusedHalt15,
    Script_OpFadeOut,
    Script_OpDuelResult,
    Script_OpGameOver,
    Script_OpCredits,
    Script_OpWait,
    Script_OpJumpIfDeckIncomplete,
    Script_OpReturnToMenu,
};

/* Regional value: the completion mask it tests (0x2008 -> 0x2010). The
 * European build (src/game/european/) defines its own. */
#ifndef SCRIPT_RUN_TICK_COMPLETION_MASK
#define SCRIPT_RUN_TICK_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#endif

void Script_RunTick(void)
{
    u16 f;
    s32 g;
    s32 b;

    rand();
    f = gScript_EventState;
    if (f & DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE) {
        TextBox_UpdateChannels();
        if ((*(u32 *)&gText_aChannels[0].flags_34 & SCRIPT_RUN_TICK_COMPLETION_MASK) == TEXT_BOX_FLAG_DONE) {
            gScript_EventState &= ~DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE;
        }
        return;
    }
    if (f == 0) {
        return;
    }
    if ((f & DUEL_EVENT_SCRIPT_FLAG_STARTED) == 0) {
        g = gScript_EventState;
        gScript_StreamCursor = D_801A8000 + ((u16 *)D_801A8000)[g];
        b = *gScript_StreamCursor++;
        gScript_DialogCommandLatch = 0;
        gScript_EventState = g | DUEL_EVENT_SCRIPT_FLAG_STARTED;
        gScript_wCommandWord = b;
    }
    if (gScript_DialogCommandLatch != 0) {
        gScript_wCommandWord = gScript_DialogCommandLatch;
    }
    while (1) {
        gScript_apfnCommandHandlers[gScript_wCommandWord & 0x1F]();
        if (gScript_wCommandWord != 0) {
            return;
        }
        gScript_wCommandWord = *gScript_StreamCursor++;
    }
}

#include "../types.h"
#include "script_command_table.h"
#include "script_op_load_image_scene.h"
#include "script_op_show_dialog.h"
#include "func_8002EB48.h"
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

/* Initialized data at 0x80090C50: the 23 scene-script command handlers.
 *
 * Script_RunTick masks the command word with 0x1F before indexing, while
 * TextBox_BuildStep can redispatch the current low byte directly. The image
 * contains exactly these entries, so the declaration remains unsized rather
 * than implying every masked value is valid.
 */

ScriptCommandHandler D_80090C50[] = {
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

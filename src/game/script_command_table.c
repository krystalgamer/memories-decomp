#include "../types.h"
#include "script_command_table.h"
#include "script_op_load_image_scene.h"
#include "func_8002E5AC.h"
#include "func_8002E6B8.h"
#include "func_8002EB48.h"
#include "func_8002EB78.h"
#include "func_8002EE94.h"
#include "func_8002F440.h"
#include "func_8002F630.h"
#include "func_8002F9D4.h"
#include "func_8002FA28.h"
#include "script_command_busy.h"
#include "script_control_commands.h"
#include "script_flag_commands.h"
#include "script_noop_commands.h"
#include "script_op_show_image.h"
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
    func_8002E3DC,
    Script_OpLoadImageScene,
    func_8002E5AC,
    func_8002E918,
    func_8002E6B8,
    Script_OpShowImage,
    Script_OpViewportTween,
    Script_UpdateViewportTween,
    func_8002EB48,
    func_8002EB78,
    Script_OpSound,
    func_8002EDB0,
    func_8002EE20,
    func_8002EE94,
    func_8002F430,
    func_8002F438,
    func_8002F440,
    func_8002F630,
    func_8002F930,
    func_8002F94C,
    func_8002F968,
    func_8002F9D4,
    func_8002FA28,
};

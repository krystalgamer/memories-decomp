#define D_800EB12C_IN_DATA
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
        if ((gText_dwChannel0State & SCRIPT_RUN_TICK_COMPLETION_MASK) == TEXT_BOX_FLAG_DONE) {
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

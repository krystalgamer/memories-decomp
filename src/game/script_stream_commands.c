#define GSD_DWCURRENTBGMCOMMAND_IS_ARRAY
#define D_8009B404_IN_DATA
#include "../types.h"
#include "script_command_busy.h"
#include "sound.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "script_state.h"
#include "script_stream_commands.h"
#include "sound_bgm_commands.h"

void Script_OpSound(void)
{
    u8 *q;
    s32 c;
    s32 f;

    if (ScriptCommand_MarkStarted() == 0) {
        q = gScript_StreamCursor;
        c = *q;
        q++;
        gScript_StreamCursor = q;
        f = c;

        if ((f & 0x3F) == 0) {
            if (f & 0x40) {
                gScript_StreamCursor = q + 2;
                SD_SEPlayFull(q[0] | (q[1] << 8));
            } else {
                gScript_StreamCursor = q + 2;
                SD_BGMPlay(q[0] | (q[1] << 8));
            }
        } else {
            if (f & 1) {
                SD_BGMPlay(gSD_dwSavedBgmCommand);
            }
            if (f & 2) {
                q = gScript_StreamCursor;
                gScript_StreamCursor = q + 2;
                gSD_dwSavedBgmCommand = q[0] | (q[1] << 8);
            }
            if (f & 4) {
                gSD_dwSavedBgmCommand = gSD_dwCurrentBgmCommand[0];
            }
        }
        if (f & 0x80) {
            return;
        }
    } else if ((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
               gFile_dwCancelRequestState_abs) {
        return;
    }
    gScript_wCommandWord = 0;
}

void Script_OpFadeBgm(void)
{
    if (ScriptCommand_MarkStarted() == 0) {
        u8 *stream = gScript_StreamCursor;
        u32 raw = *stream;
        u32 argument;
        s32 command;

        gScript_StreamCursor = stream + 1;
        argument = raw & 0x7F;
        command = raw;
        if (argument == 0) {
            SD_BGMFadeOut();
        } else {
            SD_BGMFadeOutWithStep(argument);
        }
        if (command & 0x80) {
            return;
        }
    }
    gScript_wCommandWord = 0;
}

void Script_OpJump(void)
{
    u8 *source = gScript_StreamCursor;
    u32 offset;

    gScript_StreamCursor = source + 2;
    offset = source[0] | (source[1] << 8);
    gScript_wCommandWord = 0;
    gScript_StreamCursor = D_801A8000 + offset;
}

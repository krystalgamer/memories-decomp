#define GSD_DWCURRENTBGMCOMMAND_IS_ARRAY
#include "../types.h"
#include "script_command_busy.h"
#include "sound.h"
#include "file_transfer.h"
#include "script_state.h"
#include "script_stream_commands.h"

extern s32 D_8009B404 __attribute__((section(".data")));
extern u8 D_801A8000[];

void Script_OpSound(void)
{
    u8 *q;
    s32 c;
    s32 f;

    if (func_8002E3B4() == 0) {
        q = D_8009B290;
        c = *q;
        q++;
        D_8009B290 = q;
        f = c;

        if ((f & 0x3F) == 0) {
            if (f & 0x40) {
                D_8009B290 = q + 2;
                SD_SEPlayFull(q[0] | (q[1] << 8));
            } else {
                D_8009B290 = q + 2;
                SD_BGMPlay(q[0] | (q[1] << 8));
            }
        } else {
            if (f & 1) {
                SD_BGMPlay(D_8009B404);
            }
            if (f & 2) {
                q = D_8009B290;
                D_8009B290 = q + 2;
                D_8009B404 = q[0] | (q[1] << 8);
            }
            if (f & 4) {
                D_8009B404 = gSD_dwCurrentBgmCommand[0];
            }
        }
        if (f & 0x80) {
            return;
        }
    } else if ((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
               D_8009B134_abs) {
        return;
    }
    D_8009B27C = 0;
}

void func_8002EDB0(void)
{
    if (func_8002E3B4() == 0) {
        u8 *stream = D_8009B290;
        u32 raw = *stream;
        u32 argument;
        s32 command;

        D_8009B290 = stream + 1;
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
    D_8009B27C = 0;
}

void func_8002EE20(void)
{
    u8 *source = D_8009B290;
    u32 offset;

    D_8009B290 = source + 2;
    offset = source[0] | (source[1] << 8);
    D_8009B27C = 0;
    D_8009B290 = D_801A8000 + offset;
}

#define GSD_DWCURRENTBGMCOMMAND_IS_ARRAY
#define D_8009B404_IN_DATA
#include "../types.h"
#include "func_80036D3C.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_play_sound_command.h"

void func_80038690(void *object)
{
    SD_BGMPlay((u16)func_80036D3C(object));
}

void func_800386B8(DuelEffectChannel *object)
{
    u8 **slot = (u8 **)object + object->stream_58;
    u8 *stream = *slot;
    s32 command = *stream;
    s32 op;

    *slot = stream + 1;
    op = command;
    if (op & 0x3F) {
        SD_BGMPlay(func_80036D3C((u8 *)object) & 0xFFFF);
    } else {
        if (op & 1) {
            SD_BGMPlay(D_8009B404);
        }
        if (op & 2) {
            D_8009B404 = func_80036D3C((u8 *)object) & 0xFFFF;
        }
        if (op & 4) {
            D_8009B404 = gSD_dwCurrentBgmCommand[0];
        }
    }
    if (op & 0x80) {
        object->state_51 = 0xC;
        D_8009B350 = 1;
    }
}

void DuelEffect_PlaySoundCommand(DuelEffectChannel *object) {
    u32 value = func_80036D3C((u8 *)object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        func_8003FF88((u16)value);
        D_8009B33C = func_80036D3C((u8 *)object);
        object->state_51 = 0x11;
        D_8009B350 = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}

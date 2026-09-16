#define GSD_DWCURRENTBGMCOMMAND_IS_ARRAY
#define D_8009B404_IN_DATA
#include "../types.h"
#include "text_stream_read_u16_le.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_play_sound_command.h"

void DuelEffect_PlayBgmCommand(DuelEffectChannel *object)
{
    SD_BGMPlay((u16)TextStream_ReadU16LE(object));
}

void func_800386B8(DuelEffectChannel *object)
{
    u8 **slot =
        &((TextStreamOwner *)object)->streams[object->stream_58];
    u8 *stream = *slot;
    s32 command = *stream;
    s32 op;

    *slot = stream + 1;
    op = command;
    if (op & 0x3F) {
        SD_BGMPlay(TextStream_ReadU16LE(object) & 0xFFFF);
    } else {
        if (op & 1) {
            SD_BGMPlay(D_8009B404);
        }
        if (op & 2) {
            D_8009B404 = TextStream_ReadU16LE(object) & 0xFFFF;
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
    u32 value = TextStream_ReadU16LE(object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        func_8003FF88((u16)value);
        D_8009B33C = TextStream_ReadU16LE(object);
        object->state_51 = 0x11;
        D_8009B350 = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}

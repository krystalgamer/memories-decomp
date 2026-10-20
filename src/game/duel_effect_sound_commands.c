/* Seven contiguous duel-effect command handlers from DuelEffect_PlayBgmCommand
 * (0x80038690) through func_80038898. The four sound handlers read their
 * operands from the channel's selected stream and drive SD_BGMPlay,
 * SD_SEPlayFull or the BGM fade; then come the two empty table entries and
 * the handler that reads one byte into D_8009B363 and moves the scene mode
 * to 5. */
#define GSD_DWCURRENTBGMCOMMAND_IS_ARRAY
#define D_8009B404_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY

#include "../types.h"
#include "duel_effect_sound_commands.h"
#include "text_stream_read_le.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_play_sound_command.h"
#include "duel_effect_fade_out_bgm_command.h"
#include "main_mode_state.h"
#include "../unmatched.h"

void DuelEffect_PlayBgmCommand(DuelEffectChannel *object)
{
    SD_BGMPlay((u16)TextStream_ReadU16LE(object));
}

void DuelEffect_ProcessBgmCommand(DuelEffectChannel *object)
{
    u8 *G32 *slot =
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
        gText_CommandDisposition = 1;
    }
}

void DuelEffect_PlaySoundCommand(DuelEffectChannel *object) {
    u32 value = TextStream_ReadU16LE(object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        SD_CdSoundPlay((u16)value);
        gText_CdSoundPollBudget = TextStream_ReadU16LE(object);
        object->state_51 = 0x11;
        gText_CommandDisposition = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}

void DuelEffect_FadeOutBgmCommand(DuelEffectCommand *command)
{
    u8 *G32 *slot;
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    u32 opcode;
    u32 high_bit;

    slot = &((TextStreamOwner *)command)->streams[command->cursor_slot];
    cursor = *slot;
    raw = *cursor++;
    saved_raw = raw;
    opcode = saved_raw & 0x7F;
    *slot = cursor;

    if (opcode == 0) {
        SD_BGMFadeOut();
        goto shared_test;
    } else {
        SD_BGMFadeOutWithStep(opcode);
    }

shared_test:
    high_bit = saved_raw & 0x80;
    if (high_bit != 0) {
        command->command_state = 13;
        gText_CommandDisposition = 1;
    }
}

void func_80038888(void)
{
}

void func_80038890(void)
{
}

void func_80038898(DuelEffectChannel *object)
{
    u8 *G32 *stream = &((TextStreamOwner *)object)->streams[object->stream_58];
    u8 value = *(*stream)++;
    D_8009B26C[0] = 5;
    D_8009B363[0] = value;
}

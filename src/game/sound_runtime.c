#define SD_POLL_SEQUENCE_STATE_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "sd_set_cd_volume.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_voice_constants.h"
#include "../unmatched.h"

s32 SD_EnqueueCommand(SDCommand *src) {
    SDValue *capacity_state;
    SDValue *write_state;
    SDValue *count_state;
    s32 capacity_count;
    s32 write_index;
    s32 one;

    capacity_state = g_SDValue;
    capacity_count = capacity_state->s_wSD_NUM_COMMAND;
    if (capacity_count >= SD_COMMAND_QUEUE_COUNT) {
        goto full;
    }
    {
        capacity_state->s_uSD_COMMAND_QUEUE.c[capacity_count].command = src->command;
    }

    write_state = g_SDValue;
    write_index = write_state->s_wSD_NUM_COMMAND;
    write_state->s_uSD_COMMAND_QUEUE.c[write_index] = *src;

    count_state = g_SDValue;
    one = 1;
    count_state->s_wSD_NUM_COMMAND = count_state->s_wSD_NUM_COMMAND + one;
    return one;
full:
    return 0;
}

void SD_UpdateFades(void) {
    u8 secondary_mix_changed;
    s16 output_level;

    secondary_mix_changed = 0;
    if (g_SDValue->s_wSD_SEQ_FADE_STEP != 0) {
        g_SDValue->s_wSD_SEQ_FADE_LEVEL += g_SDValue->s_wSD_SEQ_FADE_STEP;
        secondary_mix_changed = 1;
        if (g_SDValue->s_wSD_SEQ_FADE_LEVEL <= g_SDValue->s_bSD_SEQ_FADE_TARGET &&
            g_SDValue->s_wSD_SEQ_FADE_STEP < 0) {
            g_SDValue->s_wSD_SEQ_FADE_STEP = 0;
            g_SDValue->s_wSD_SEQ_FADE_LEVEL = g_SDValue->s_bSD_SEQ_FADE_TARGET;
        }
        if (g_SDValue->s_wSD_SEQ_FADE_LEVEL >= g_SDValue->s_bSD_SEQ_FADE_TARGET &&
            g_SDValue->s_wSD_SEQ_FADE_STEP > 0) {
            g_SDValue->s_wSD_SEQ_FADE_STEP = 0;
            g_SDValue->s_wSD_SEQ_FADE_LEVEL = g_SDValue->s_bSD_SEQ_FADE_TARGET;
        }
    }
    if (g_SDValue->s_wSD_BGM_FADE_STEP != 0) {
        g_SDValue->s_wSD_BGM_FADE_LEVEL += g_SDValue->s_wSD_BGM_FADE_STEP;
        secondary_mix_changed = 1;
        if (g_SDValue->s_wSD_BGM_FADE_LEVEL <= g_SDValue->s_bSD_BGM_FADE_TARGET &&
            g_SDValue->s_wSD_BGM_FADE_STEP < 0) {
            g_SDValue->s_wSD_BGM_FADE_STEP = 0;
            g_SDValue->s_wSD_BGM_FADE_LEVEL = g_SDValue->s_bSD_BGM_FADE_TARGET;
        }
        if (g_SDValue->s_wSD_BGM_FADE_LEVEL >= g_SDValue->s_bSD_BGM_FADE_TARGET &&
            g_SDValue->s_wSD_BGM_FADE_STEP > 0) {
            g_SDValue->s_wSD_BGM_FADE_STEP = 0;
            g_SDValue->s_wSD_BGM_FADE_LEVEL = g_SDValue->s_bSD_BGM_FADE_TARGET;
        }
    }
    if (secondary_mix_changed) {
        u16 scaled_mix;

        scaled_mix = g_SDValue->s_wSD_SMF_MIXER_OUT;
        if (scaled_mix) {
            scaled_mix *= g_SDValue->s_wSD_BGM_FADE_LEVEL + 1;
            scaled_mix >>= 8;
        } else {
            scaled_mix = 0;
        }
        if (scaled_mix) {
            scaled_mix *= g_SDValue->s_wSD_SEQ_FADE_LEVEL + 1;
            scaled_mix >>= 8;
        } else {
            scaled_mix = 0;
        }
        output_level = scaled_mix;
        SD_SetSecondaryMasterLevels(output_level, output_level);
    }
    if (g_SDValue->s_wSD_CD_FADE_STEP != 0) {
        g_SDValue->s_wSD_CD_VOLUME += g_SDValue->s_wSD_CD_FADE_STEP;
        if (g_SDValue->s_wSD_CD_VOLUME <= g_SDValue->s_bSD_CD_FADE_TARGET &&
            g_SDValue->s_wSD_CD_FADE_STEP < 0) {
            g_SDValue->s_wSD_CD_FADE_STEP = 0;
            g_SDValue->s_wSD_CD_VOLUME = g_SDValue->s_bSD_CD_FADE_TARGET;
        }
        if (g_SDValue->s_wSD_CD_VOLUME >= g_SDValue->s_bSD_CD_FADE_TARGET &&
            g_SDValue->s_wSD_CD_FADE_STEP > 0) {
            g_SDValue->s_wSD_CD_FADE_STEP = 0;
            g_SDValue->s_wSD_CD_VOLUME = g_SDValue->s_bSD_CD_FADE_TARGET;
        }
        output_level = g_SDValue->s_wSD_CD_VOLUME;
        SD_SetCdVolume(output_level);
    }
}

void SD_UpdateRuntime(void)
{
    SDValue *state;
    SDCommand *command_cursor;
    s32 key_off_mask;
    s32 shifted_count;
    s32 queue_byte_offset;
    s32 flags_or_command;
    SDValue *loop_state;
    SDValue *queue_state;
    SDValue *tail_state;

    SpuGetAllKeysStatus((char *)g_SDValue->s_abSD_VOICE_KEY_STATUS);
    state = g_SDValue;
    key_off_mask = (state->s_abSD_VOICE_KEY_STATUS[20] == 3) << SD_VOICE_SLOT_KEY_SHIFT;
    if (state->s_abSD_VOICE_KEY_STATUS[21] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 1;
    }
    if (state->s_abSD_VOICE_KEY_STATUS[22] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 2;
    }
    if (state->s_abSD_VOICE_KEY_STATUS[23] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 3;
    }
    if (key_off_mask != 0) {
        SpuSetKey(0, key_off_mask);
    }

    state = g_SDValue;
    if ((state->s_wSD_STATUS & 0x80) != 0 &&
        state->s_wSD_SEQ_ACCESS != -1 &&
        SD_PollSequenceState() != 1) {
        SD_StopSequence(g_SDValue->s_wSD_SEQ_ACCESS);
        state = g_SDValue;
        flags_or_command = state->s_wSD_STATUS;
        state->s_wSD_SEQ_FADE_STEP = -0x80;
        state->s_bSD_SEQ_FADE_TARGET = 0;
        flags_or_command &= 0xFF7F;
        state->s_wSD_STATUS = flags_or_command;
    }

    SD_UpdateFades();
    state = g_SDValue;
    if (state->s_bSD_ACTIVE_COMMAND != 0) {
        SD_StepActiveCommand();
        return;
    }
    if (state->s_wSD_NUM_COMMAND == 0) {
        return;
    }
    command_cursor = &state->s_uSD_COMMAND_QUEUE.c[0];
    /* Separate bodies preserve equality dispatch instead of case ranges;
       cross-jumping still shares the emitted bodies. */
    flags_or_command = state->s_uSD_COMMAND_QUEUE.c[0].command;
    switch (flags_or_command) {
    case 0x44:
        if (state->s_wSD_SEQ_FADE_STEP == 0) {
            state->s_wSD_SEQ_FADE_STEP = (u16)command_cursor->arg1;
            state->s_bSD_BGM_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x45:
        if (state->s_wSD_SEQ_FADE_STEP == 0) {
            state->s_wSD_SEQ_FADE_STEP = (u16)command_cursor->arg1;
            state->s_bSD_BGM_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x29:
        if (state->s_wSD_CD_FADE_STEP == 0) {
            state->s_wSD_CD_FADE_STEP = (u16)command_cursor->arg1;
            state->s_bSD_CD_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x2A:
        if (state->s_wSD_CD_FADE_STEP == 0) {
            state->s_wSD_CD_FADE_STEP = (u16)command_cursor->arg1;
            state->s_bSD_CD_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    }

    state = g_SDValue;
    state->s_bSD_ACTIVE_COMMAND = command_cursor->command;
    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
    state = g_SDValue;
    state->s_wSD_ACTIVE_COMMAND_VALUE = (u16)command_cursor->value;
    state->s_dwSD_ACTIVE_COMMAND_ARG0 = command_cursor->arg0;
    state->s_dwSD_ACTIVE_COMMAND_ARG1 = command_cursor->arg1;
    state->s_dwSD_ACTIVE_COMMAND_ARG2 = command_cursor->arg2;
    state->s_bSD_ACTIVE_CD_FADE_WAIT = command_cursor->wait_for_cd_fade;
    /* The second global read preserves separate pointers for the tail copy
       and queue bookkeeping after CSE. */
    tail_state = g_SDValue;
    *(SDCommandTail *)&tail_state->s_adwSD_ACTIVE_COMMAND_TAIL[0] = *(SDCommandTail *)&command_cursor->transfer_selector;
    queue_state = g_SDValue;
    queue_state->s_wSD_DEQUEUE_MARK = 8;
    queue_state->s_wSD_NUM_COMMAND = queue_state->s_wSD_NUM_COMMAND - 1;
    SD_StepActiveCommand();

    loop_state = g_SDValue;
    command_cursor++;
    shifted_count = 0;
    if (loop_state->s_wSD_NUM_COMMAND <= 0) {
        return;
    }
    queue_byte_offset = 0;
    do {
        /* Base-plus-offset order matters here. The byte store indexes the
           queue's byte view, whose ARRAY_REF keeps it; the block copy stays
           a byte-address sum, because &commands.b[queue_byte_offset] puts the offset
           first. sound.h checks the queue offset. */
        loop_state->s_uSD_COMMAND_QUEUE.b[queue_byte_offset] = command_cursor->command;
        *(SDCommand *)((u8 *)g_SDValue + queue_byte_offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
            *command_cursor;
        queue_byte_offset += sizeof(SDCommand);
        loop_state = g_SDValue;
        shifted_count += 1;
        command_cursor++;
    } while (shifted_count < loop_state->s_wSD_NUM_COMMAND);
}

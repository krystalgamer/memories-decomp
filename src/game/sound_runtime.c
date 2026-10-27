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
    capacity_count = capacity_state->command_count;
    if (capacity_count >= SD_COMMAND_QUEUE_COUNT) {
        goto full;
    }
    {
        capacity_state->commands.c[capacity_count].command = src->command;
    }

    write_state = g_SDValue;
    write_index = write_state->command_count;
    write_state->commands.c[write_index] = *src;

    count_state = g_SDValue;
    one = 1;
    count_state->command_count = count_state->command_count + one;
    return one;
full:
    return 0;
}

void SD_UpdateFades(void) {
    u8 secondary_mix_changed;
    s16 output_level;

    secondary_mix_changed = 0;
    if (g_SDValue->sequence_fade_step != 0) {
        g_SDValue->sequence_fade_level += g_SDValue->sequence_fade_step;
        secondary_mix_changed = 1;
        if (g_SDValue->sequence_fade_level <= g_SDValue->sequence_fade_target &&
            g_SDValue->sequence_fade_step < 0) {
            g_SDValue->sequence_fade_step = 0;
            g_SDValue->sequence_fade_level = g_SDValue->sequence_fade_target;
        }
        if (g_SDValue->sequence_fade_level >= g_SDValue->sequence_fade_target &&
            g_SDValue->sequence_fade_step > 0) {
            g_SDValue->sequence_fade_step = 0;
            g_SDValue->sequence_fade_level = g_SDValue->sequence_fade_target;
        }
    }
    if (g_SDValue->bgm_fade_step != 0) {
        g_SDValue->bgm_fade_level += g_SDValue->bgm_fade_step;
        secondary_mix_changed = 1;
        if (g_SDValue->bgm_fade_level <= g_SDValue->bgm_fade_target &&
            g_SDValue->bgm_fade_step < 0) {
            g_SDValue->bgm_fade_step = 0;
            g_SDValue->bgm_fade_level = g_SDValue->bgm_fade_target;
        }
        if (g_SDValue->bgm_fade_level >= g_SDValue->bgm_fade_target &&
            g_SDValue->bgm_fade_step > 0) {
            g_SDValue->bgm_fade_step = 0;
            g_SDValue->bgm_fade_level = g_SDValue->bgm_fade_target;
        }
    }
    if (secondary_mix_changed) {
        u16 scaled_mix;

        scaled_mix = g_SDValue->secondary_mix_scale;
        if (scaled_mix) {
            scaled_mix *= g_SDValue->bgm_fade_level + 1;
            scaled_mix >>= 8;
        } else {
            scaled_mix = 0;
        }
        if (scaled_mix) {
            scaled_mix *= g_SDValue->sequence_fade_level + 1;
            scaled_mix >>= 8;
        } else {
            scaled_mix = 0;
        }
        output_level = scaled_mix;
        SD_SetSecondaryMasterLevels(output_level, output_level);
    }
    if (g_SDValue->cd_fade_step != 0) {
        g_SDValue->cd_volume += g_SDValue->cd_fade_step;
        if (g_SDValue->cd_volume <= g_SDValue->cd_fade_target &&
            g_SDValue->cd_fade_step < 0) {
            g_SDValue->cd_fade_step = 0;
            g_SDValue->cd_volume = g_SDValue->cd_fade_target;
        }
        if (g_SDValue->cd_volume >= g_SDValue->cd_fade_target &&
            g_SDValue->cd_fade_step > 0) {
            g_SDValue->cd_fade_step = 0;
            g_SDValue->cd_volume = g_SDValue->cd_fade_target;
        }
        output_level = g_SDValue->cd_volume;
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

    SpuGetAllKeysStatus((char *)g_SDValue->voice_key_status);
    state = g_SDValue;
    key_off_mask = (state->voice_key_status[20] == 3) << SD_VOICE_SLOT_KEY_SHIFT;
    if (state->voice_key_status[21] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 1;
    }
    if (state->voice_key_status[22] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 2;
    }
    if (state->voice_key_status[23] == 3) {
        key_off_mask |= SD_VOICE_SLOT_MASK_BASE << 3;
    }
    if (key_off_mask != 0) {
        SpuSetKey(0, key_off_mask);
    }

    state = g_SDValue;
    if ((state->status_flags & 0x80) != 0 &&
        state->sequence_open_result != -1 &&
        SD_PollSequenceState() != 1) {
        SD_StopSequence(g_SDValue->sequence_open_result);
        state = g_SDValue;
        flags_or_command = state->status_flags;
        state->sequence_fade_step = -0x80;
        state->sequence_fade_target = 0;
        flags_or_command &= 0xFF7F;
        state->status_flags = flags_or_command;
    }

    SD_UpdateFades();
    state = g_SDValue;
    if (state->active_command != 0) {
        SD_StepActiveCommand();
        return;
    }
    if (state->command_count == 0) {
        return;
    }
    command_cursor = &state->commands.c[0];
    /* Separate bodies preserve equality dispatch instead of case ranges;
       cross-jumping still shares the emitted bodies. */
    flags_or_command = state->commands.c[0].command;
    switch (flags_or_command) {
    case 0x44:
        if (state->sequence_fade_step == 0) {
            state->sequence_fade_step = (u16)command_cursor->arg1;
            state->bgm_fade_target = (u8)command_cursor->value;
        }
        break;
    case 0x45:
        if (state->sequence_fade_step == 0) {
            state->sequence_fade_step = (u16)command_cursor->arg1;
            state->bgm_fade_target = (u8)command_cursor->value;
        }
        break;
    case 0x29:
        if (state->cd_fade_step == 0) {
            state->cd_fade_step = (u16)command_cursor->arg1;
            state->cd_fade_target = (u8)command_cursor->value;
        }
        break;
    case 0x2A:
        if (state->cd_fade_step == 0) {
            state->cd_fade_step = (u16)command_cursor->arg1;
            state->cd_fade_target = (u8)command_cursor->value;
        }
        break;
    }

    state = g_SDValue;
    state->active_command = command_cursor->command;
    g_SDValue->active_command_phase = 0;
    state = g_SDValue;
    state->active_command_value = (u16)command_cursor->value;
    state->active_command_arg0 = command_cursor->arg0;
    state->active_command_arg1 = command_cursor->arg1;
    state->active_command_arg2 = command_cursor->arg2;
    state->active_cd_fade_wait = command_cursor->wait_for_cd_fade;
    /* The second global read preserves separate pointers for the tail copy
       and queue bookkeeping after CSE. */
    tail_state = g_SDValue;
    *(SDCommandTail *)&tail_state->active_command_tail[0] = *(SDCommandTail *)&command_cursor->transfer_selector;
    queue_state = g_SDValue;
    queue_state->field_15F4 = 8;
    queue_state->command_count = queue_state->command_count - 1;
    SD_StepActiveCommand();

    loop_state = g_SDValue;
    command_cursor++;
    shifted_count = 0;
    if (loop_state->command_count <= 0) {
        return;
    }
    queue_byte_offset = 0;
    do {
        /* Base-plus-offset order matters here. The byte store indexes the
           queue's byte view, whose ARRAY_REF keeps it; the block copy stays
           a byte-address sum, because &commands.b[queue_byte_offset] puts the offset
           first. sound.h checks the queue offset. */
        loop_state->commands.b[queue_byte_offset] = command_cursor->command;
        *(SDCommand *)((u8 *)g_SDValue + queue_byte_offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
            *command_cursor;
        queue_byte_offset += sizeof(SDCommand);
        loop_state = g_SDValue;
        shifted_count += 1;
        command_cursor++;
    } while (shifted_count < loop_state->command_count);
}

/* The sound driver's CD-audio control, SD_InitBufferState (0x80044D48)
   through SD_ClearBusyFlag: the CD mix state first -- work-buffer pointer
   setup (which first centres the pan and applies CD volume 255), the pan
   reset that sets both channel volumes to SD_CHANNEL_VOLUME_MAX, the three CD
   volume/pan setters and the CD volume reader -- then the CD command queuers,
   CD-level measurement and busy-state completion. */

#include "../types.h"
#include "sd_set_cd_volume.h"
#include "sound.h"
#include "../psyq/libcd.h"
#include "sound_mix.h"

void SD_InitBufferState(void)
{
    SDValue *buffer_state;

    SD_ResetCdPan();
    SD_SetCdVolume(255);
    buffer_state = g_SDValue;
    buffer_state->decoded_cd_halves[0] = buffer_state->decoded_data.cd_left;
    buffer_state->decoded_cd_halves[1] = &buffer_state->decoded_data.cd_left[SD_MIX_SAMPLE_COUNT];
    buffer_state->decoded_cd_halves[2] = buffer_state->decoded_data.cd_right;
    buffer_state->cd_fade_step = 0;
    buffer_state->decoded_cd_halves[3] = &buffer_state->decoded_data.cd_right[SD_MIX_SAMPLE_COUNT];
}

void SD_ResetCdPan(void)
{
    g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
    g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
}

/* Sibling of SD_SetCdVolumeImmediate: builds the same 4-byte CdMix volume
   packet, but here val0/val1-or-val2 are scaled DSP-style from level instead
   of copied verbatim. g_SDValue's fields 0x514/0x515 are
   per-channel volume scalars; f42 and f533 combine with (level+1) to derive a
   shared 0-8191 "mix_factor" factor, which each channel's scalar is then
   multiplied against and rescaled (>>7) into its packet byte. f48 picks
   which of val1/val2 carries the scaled value (the other gets 0), matching
   SD_SetCdVolumeImmediate's f48-based slot selection. Reloads
   g_SDValue through a fresh alias for the trailing f510 write, same
   pattern as project_d8009b45c_reload_pointer_trick. */
void SD_SetCdVolume(s16 level) {
    SDValue *mix_state = g_SDValue;
    s32 scaled_input = (level + 1) * mix_state->mix_scale;
    s32 scaled_mix = mix_state->mix_multiplier * (scaled_input >> 8);
    s32 mix_factor = ((u32)(u16)scaled_mix) >> 3;
    CdlATV attenuation;
    s16 saved_level = level;

    attenuation.val0 = (mix_factor * mix_state->channel_volume[0]) >> 7;
    attenuation.val3 = 0;

    if (mix_state->output_type == 0) {
        attenuation.val1 = 0;
        attenuation.val2 = (mix_factor * mix_state->channel_volume[1]) >> 7;
    } else {
        attenuation.val1 = (mix_factor * mix_state->channel_volume[1]) >> 7;
        attenuation.val2 = 0;
    }

    CdMix(&attenuation);

    g_SDValue->cd_volume = saved_level;
}

void SD_SetCdPan(s32 pan)
{
    s16 signed_pan = pan;

    if ((u16)((u32)pan - 1) < SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = -SD_CHANNEL_VOLUME_MAX - signed_pan;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    } else if ((u16)((u32)pan + SD_CHANNEL_VOLUME_MAX) <
        SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = -SD_CHANNEL_VOLUME_MAX - signed_pan;
    } else {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    }
    SD_SetCdVolume(g_SDValue->cd_volume);
}

void SD_SetCdVolumeImmediate(s32 level)
{
    CdlATV attenuation;
    SDValue *volume_state;
    SDValue *step_state;

    g_SDValue->mix_multiplier = 7;
    attenuation.val0 = level;
    attenuation.val3 = 0;
    if (g_SDValue->output_type == 0) {
        attenuation.val1 = 0;
        attenuation.val2 = level;
    } else {
        attenuation.val1 = level;
        attenuation.val2 = 0;
    }
    CdMix(&attenuation);
    volume_state = g_SDValue;
    volume_state->cd_fade_target = level;
    step_state = g_SDValue;
    volume_state->cd_volume = level;
    step_state->cd_fade_step = 0;
}

s32 SD_GetCdVolume(void)
{
    return g_SDValue->cd_volume;
}

#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

/* Queues CD fade command 41: arg1 is the signed per-update step and
   value the target volume, loaded into cd_fade_step/cd_fade_target when
   SD_UpdateRuntime dequeues it. wait_for_cd_fade becomes active_cd_fade_wait; while it is
   nonzero, SD_StepActiveCommand keeps the command active until the fade
   ends. No caller is present in the retail executable or its overlays. */
s32 SD_EnqueueCdFade(s32 step, s32 target, s32 wait_for_fade) {
    SDCommand command;
    command.command = 41;
    command.arg1 = (s16) step;
    command.value = (s16) (u8) target;
    command.wait_for_cd_fade = (u8) wait_for_fade;
    return SD_EnqueueCommand(&command);
}

void SD_StartCdFade(s16 step, u8 target, s32 unused)
{
    g_SDValue->cd_fade_step = step;
    g_SDValue->cd_fade_target = target;
}

#ifndef VERSION_JAPAN
#include "sound_mix.h"

/*
 * SD_MeasureCdAudioLevel: decoded output measurement without register pins
 *
 * The 192-byte routine matches all 48 instructions under the unit's
 * gcc_2_8_1_g0 profile. The same-symbol alias below retains its measured
 * volatile pointer view while the surrounding functions keep sound.h's
 * ordinary declaration.
 *
 * Three source properties replace the historical eight register bindings and
 * compiler barrier. The volatile pointer view retains the snapshot-to-state
 * handoff; removing it loses one instruction. Returning the level directly on
 * the unmuted path gives the required return-register allocation. Finally, the
 * single-iteration level-read scope keeps the signed high-halfword read before
 * the flag read; flattening it exchanges the two words at +0x90 and +0x94.
 *
 * The routine preserves the selected CD half, all 256 signed sample squares,
 * the unsigned eight-bit shift of each square, both accumulator
 * initializations and final publications, and the low-two-bit output gate.
 */
extern SDValue *G32 volatile g_SDValue_output_level asm("g_SDValue");

s32 SD_MeasureCdAudioLevel(void)
{
    s32 decoded_half = SpuReadDecodedData(
        &g_SDValue_output_level->decoded_data, SPU_CDONLY
    );
    SDValue *choice_state = g_SDValue_output_level;
    s16 *sample_cursor;
    s32 sample_index;
    SDValue *loaded_state;
    SDValue *measurement_state;

    choice_state->decoded_half = decoded_half;
    if (decoded_half == SPU_DECODED_FIRSTHALF) {
        sample_cursor = choice_state->decoded_cd_halves[0];
    } else {
        sample_cursor = choice_state->decoded_cd_halves[1];
    }
    loaded_state = g_SDValue_output_level;
    sample_index = 0;
    measurement_state = loaded_state;
    measurement_state->output_level.sum = 0;
    measurement_state->field_1550.sum = 0;
    do {
        s32 sample_value = *sample_cursor;
        u32 sample_square = sample_value * sample_value;
        measurement_state->output_level.sum += sample_square >> 8;
        sample_index++;
        sample_cursor++;
    } while (sample_index < SD_MIX_SAMPLE_COUNT);
    {
        s32 measured_level;
        s32 output_gate_flags;
        s32 other_high_half;
        measurement_state = g_SDValue_output_level;
        do {
            measured_level = measurement_state->output_level.halves[1];
        } while (0);
        output_gate_flags = measurement_state->status_flags;
        other_high_half = measurement_state->field_1550.halves[1];
        output_gate_flags &= 3;
        measurement_state->output_level.sum = measured_level;
        measurement_state->field_1550.sum = other_high_half;
        if (!output_gate_flags) {
            return measured_level;
        }
        return 0;
    }
}

void SD_EnqueueCdStop(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->data_source_flags & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands.c[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands.c[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands.c[count - 2].command == 0x11)
                return;
        }
    }
    SD_StartCdFade(-0x20, 0, 1);
    {
        SDCommand local;
        local.command = 0x11;
        SD_EnqueueCommand(&local);
    }
}

s16 SD_EnqueueCdSePlay(u16 command_id, s32 unused)
{
    return SD_EnqueueCdPlay(command_id, 0x80);
}

s32 SD_EnqueueCdPlay(u16 command_id, s32 unused)
{
    SDValue *request_state = g_SDValue;
    u16 entry_index = command_id;
    u8 **descriptor_bank;
    s32 bank_tag;
    u8 *descriptor_entries;
    SDCommand command;

    if (request_state->data_source_flags & 0x80) {
        if ((request_state->data_source_flags & 0x40) || entry_index <= 0x9FFF) {
            if (command_id & 0x8000) {
                *(s16 *)&request_state->cd_request_code = command_id;
                switch (command_id & 0xF000) {
                case 0x8000:
                    entry_index = command_id + 0x8000;
                    descriptor_bank = (u8 **)request_state->cd_descriptor_banks[1];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    bank_tag = 0x50;
                    break;
                case 0x9000:
                    entry_index = command_id + 0x7000;
                    descriptor_bank = (u8 **)request_state->cd_descriptor_banks[0];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    bank_tag = 0x60;
                    break;
                default:
                    entry_index = entry_index + 0x6000;
                    bank_tag = 0x70;
                    descriptor_bank = (u8 **)g_SDValue->cd_descriptor_banks[2];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    break;
                }
                {
                    s32 bank_sector_offset;

                    bank_sector_offset = descriptor_bank ? (descriptor_entries ? (s32)*descriptor_bank : (s32)*descriptor_bank) : (s32)*descriptor_bank;

                    SD_DropQueuedCdPlayCommands();
                    command.command = 0x24;
                    command.value = entry_index;
                    command.arg0 = bank_sector_offset;
                    command.arg2 = (s32)descriptor_entries;
                    command.arg1 = bank_tag;
                    SD_EnqueueCommand(&command);
                    SD_ResetCdPan();
                }
                g_SDValue->status_flags = (g_SDValue->status_flags | 1) & 0xFFFB;
                return 1;
            }
        }
    }
    return 0;
}

void SD_EnqueueCdSeek(s32 command_id)
{
    SDCommand command;
    SDValue *request_state;
    SDValue *bank_state;
    SDValue *status_state;
    s32 class_or_code;
    u16 entry_index;
    u8 **descriptor_bank;
    s32 bank_tag;
    u8 *descriptor_entries;

    request_state = g_SDValue;
    entry_index = command_id;
    if ((request_state->data_source_flags & 0x80) == 0) {
        return;
    }
    if ((request_state->data_source_flags & 0x40) == 0) {
        if ((u32)(entry_index & 0xFFFF) > 0x9FFF) {
            return;
        }
    }
    if ((command_id & 0x8000) == 0) {
        return;
    }
    class_or_code = command_id & 0xF000;
    *(s16 *)&request_state->cd_request_code = command_id;
    switch (class_or_code) {
    case 0x8000:
        class_or_code = command_id + class_or_code;
        entry_index = class_or_code;
        descriptor_bank = (u8 **)request_state->cd_descriptor_banks[1];
        descriptor_entries = (u8 *)&descriptor_bank[2];
        bank_tag = 0x50;
        break;
    case 0x9000:
        entry_index = command_id + 0x7000;
        descriptor_bank = (u8 **)request_state->cd_descriptor_banks[0];
        descriptor_entries = (u8 *)&descriptor_bank[2];
        bank_tag = 0x60;
        break;
    default:
        entry_index += 0x6000;
        bank_tag = 0x70;
        bank_state = g_SDValue;
        descriptor_bank = (u8 **)bank_state->cd_descriptor_banks[2];
        descriptor_entries = (u8 *)&descriptor_bank[2];
        break;
    }
    {
        s32 bank_sector_offset;

        /* The equivalent paths preserve retail's request-store order. */
        bank_sector_offset = descriptor_bank ? (descriptor_entries ? (s32)*descriptor_bank : (s32)*descriptor_bank) : (s32)*descriptor_bank;

        SD_DropQueuedCdPlayCommands();
        command.command = 0x21;
        command.value = entry_index;
        command.arg0 = bank_sector_offset;
        command.arg2 = (s32)descriptor_entries;
        command.arg1 = bank_tag;
        SD_EnqueueCommand(&command);
    }
    status_state = g_SDValue;
    status_state->status_flags = (status_state->status_flags | 1) & 0xFFFB;
}

void SD_ClearBusyFlag(void)
{
    g_SDValue->busy = 0;
}
#endif

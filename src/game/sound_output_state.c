/* The sound driver's CD-audio control, SD_InitBufferState (0x80044D48)
   through SD_ClearBusyFlag: the CD mix state first -- work-buffer pointer
   setup (which first centres the pan and applies CD volume 255), the pan
   reset that sets both channel volumes to SD_CHANNEL_VOLUME_MAX, the three CD
   volume/pan setters and the CD volume reader -- then the CD command queuers,
   CD-level measurement and busy-state completion. */

#include "../types.h"
#include "sound_output_state.h"
#include "sound.h"
#include "../psyq/libcd.h"

void SD_InitBufferState(void)
{
    SDValue *buffer_state;

    SD_ResetCdPan();
    SD_SetCdVolume(255);
    buffer_state = g_SDValue;
    buffer_state->s_apwSD_DECODED_CD_HALF[0] = buffer_state->s_stSD_DECODED_DATA.cd_left;
    buffer_state->s_apwSD_DECODED_CD_HALF[1] = &buffer_state->s_stSD_DECODED_DATA.cd_left[SD_MIX_SAMPLE_COUNT];
    buffer_state->s_apwSD_DECODED_CD_HALF[2] = buffer_state->s_stSD_DECODED_DATA.cd_right;
    buffer_state->s_wSD_CD_FADE_STEP = 0;
    buffer_state->s_apwSD_DECODED_CD_HALF[3] = &buffer_state->s_stSD_DECODED_DATA.cd_right[SD_MIX_SAMPLE_COUNT];
}

void SD_ResetCdPan(void)
{
    g_SDValue->s_abSD_CD_CHANNEL_VOLUME[0] = SD_CHANNEL_VOLUME_MAX;
    g_SDValue->s_abSD_CD_CHANNEL_VOLUME[1] = SD_CHANNEL_VOLUME_MAX;
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
    s32 scaled_input = (level + 1) * mix_state->s_wSD_CD_MIXER_OUT;
    s32 scaled_mix = mix_state->s_bSD_CD_MIX_MULTIPLIER * (scaled_input >> 8);
    s32 mix_factor = ((u32)(u16)scaled_mix) >> 3;
    CdlATV attenuation;
    s16 saved_level = level;

    attenuation.val0 = (mix_factor * mix_state->s_abSD_CD_CHANNEL_VOLUME[0]) >> 7;
    attenuation.val3 = 0;

    if (mix_state->s_bSD_OUTPUT_TYPE == 0) {
        attenuation.val1 = 0;
        attenuation.val2 = (mix_factor * mix_state->s_abSD_CD_CHANNEL_VOLUME[1]) >> 7;
    } else {
        attenuation.val1 = (mix_factor * mix_state->s_abSD_CD_CHANNEL_VOLUME[1]) >> 7;
        attenuation.val2 = 0;
    }

    CdMix(&attenuation);

    g_SDValue->s_wSD_CD_VOLUME = saved_level;
}

void SD_SetCdPan(s32 pan)
{
    s16 signed_pan = pan;

    if ((u16)((u32)pan - 1) < SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[0] = -SD_CHANNEL_VOLUME_MAX - signed_pan;
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[1] = SD_CHANNEL_VOLUME_MAX;
    } else if ((u16)((u32)pan + SD_CHANNEL_VOLUME_MAX) <
        SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[1] = -SD_CHANNEL_VOLUME_MAX - signed_pan;
    } else {
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->s_abSD_CD_CHANNEL_VOLUME[1] = SD_CHANNEL_VOLUME_MAX;
    }
    SD_SetCdVolume(g_SDValue->s_wSD_CD_VOLUME);
}

void SD_SetCdVolumeImmediate(s32 level)
{
    CdlATV attenuation;
    SDValue *volume_state;
    SDValue *step_state;

    g_SDValue->s_bSD_CD_MIX_MULTIPLIER = 7;
    attenuation.val0 = level;
    attenuation.val3 = 0;
    if (g_SDValue->s_bSD_OUTPUT_TYPE == 0) {
        attenuation.val1 = 0;
        attenuation.val2 = level;
    } else {
        attenuation.val1 = level;
        attenuation.val2 = 0;
    }
    CdMix(&attenuation);
    volume_state = g_SDValue;
    volume_state->s_bSD_CD_FADE_TARGET = level;
    step_state = g_SDValue;
    volume_state->s_wSD_CD_VOLUME = level;
    step_state->s_wSD_CD_FADE_STEP = 0;
}

s32 SD_GetCdVolume(void)
{
    return g_SDValue->s_wSD_CD_VOLUME;
}

#include "../psyq/libspu.h"

#include "sound.h"

/* Queues CD fade command 41: arg1 is the signed per-update step and
   value the target volume, loaded into s_wSD_CD_FADE_STEP/s_bSD_CD_FADE_TARGET when
   SD_UpdateRuntime dequeues it. wait_for_cd_fade becomes s_bSD_ACTIVE_CD_FADE_WAIT; while it is
   nonzero, SD_StepActiveCommand keeps the command active until the fade
   ends. No caller is present in the retail executable or its overlays. */
s32 SD_EnqueueCdFade(s32 step, s32 target, s32 wait_for_fade) {
    SDCommand command;
    command.command = 41;
    command.args.fade.step = (s16) step;
    command.value = (s16) (u8) target;
    command.wait_for_cd_fade = (u8) wait_for_fade;
    return SD_EnqueueCommand(&command);
}

void SD_StartCdFade(s16 step, u8 target, s32 unused)
{
    g_SDValue->s_wSD_CD_FADE_STEP = step;
    g_SDValue->s_bSD_CD_FADE_TARGET = target;
}

#ifndef VERSION_JAPAN

/* Sums the squares of the current decoded CD half into s_uSD_OUTPUT_LEVEL and
 * returns its high half, or 0 while the output is gated. Every access goes
 * through g_SDValue itself: the stores through it force the reloads after the
 * call, before the loop and after it. The loop and the readback take the
 * pointer through `state`, which is what gives the copy before the loop and
 * keeps the loop's register for the readback. */
s32 SD_MeasureCdAudioLevel(void)
{
    s32 decoded_half;
    s16 *sample_cursor;
    s32 sample_index;
    s32 measured_level;
    SDValue *state;

    decoded_half = SpuReadDecodedData(&g_SDValue->s_stSD_DECODED_DATA, SPU_CDONLY);
    g_SDValue->s_dwSD_DECODED_HALF = decoded_half;
    if (decoded_half == SPU_DECODED_FIRSTHALF) {
        sample_cursor = g_SDValue->s_apwSD_DECODED_CD_HALF[0];
    } else {
        sample_cursor = g_SDValue->s_apwSD_DECODED_CD_HALF[1];
    }
    g_SDValue->s_uSD_OUTPUT_LEVEL.sum = 0;
    g_SDValue->s_uSD_OUTPUT_LEVEL_AUX.sum = 0;
    state = g_SDValue;
    for (sample_index = 0; sample_index < SD_MIX_SAMPLE_COUNT; sample_index++) {
        s32 sample_value = *sample_cursor++;
        state->s_uSD_OUTPUT_LEVEL.sum += (u32)(sample_value * sample_value) >> 8;
    }
    state = g_SDValue;
    measured_level = state->s_uSD_OUTPUT_LEVEL.halves[1];
    state->s_uSD_OUTPUT_LEVEL.sum = measured_level;
    state->s_uSD_OUTPUT_LEVEL_AUX.sum = state->s_uSD_OUTPUT_LEVEL_AUX.halves[1];
    if (!(state->s_wSD_STATUS & 3)) {
        return measured_level;
    }
    return 0;
}

void SD_EnqueueCdStop(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->s_bSD_DATA_SOURCE & 0x80) == 0)
        return;
    count = state->s_wSD_NUM_COMMAND;
    if (state->s_uSD_COMMAND_QUEUE.c[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->s_uSD_COMMAND_QUEUE.c[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->s_uSD_COMMAND_QUEUE.c[count - 2].command == 0x11)
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

    if (request_state->s_bSD_DATA_SOURCE & 0x80) {
        if ((request_state->s_bSD_DATA_SOURCE & 0x40) || entry_index <= 0x9FFF) {
            if (command_id & 0x8000) {
                *(s16 *)&request_state->s_wSD_CD_REQUEST_CODE = command_id;
                switch (command_id & 0xF000) {
                case 0x8000:
                    entry_index = command_id + 0x8000;
                    descriptor_bank = (u8 **)request_state->s_apbSD_CD_DESCRIPTOR_BANK[1];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    bank_tag = 0x50;
                    break;
                case 0x9000:
                    entry_index = command_id + 0x7000;
                    descriptor_bank = (u8 **)request_state->s_apbSD_CD_DESCRIPTOR_BANK[0];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    bank_tag = 0x60;
                    break;
                default:
                    entry_index = entry_index + 0x6000;
                    bank_tag = 0x70;
                    descriptor_bank = (u8 **)g_SDValue->s_apbSD_CD_DESCRIPTOR_BANK[2];
                    descriptor_entries = (u8 *)&descriptor_bank[2];
                    break;
                }
                {
                    s32 bank_sector_offset;

                    bank_sector_offset = descriptor_bank ? (descriptor_entries ? (s32)*descriptor_bank : (s32)*descriptor_bank) : (s32)*descriptor_bank;

                    SD_DropQueuedCdPlayCommands();
                    command.command = 0x24;
                    command.value = entry_index;
                    command.args.cd_play.bank_sector_offset = bank_sector_offset;
                    command.args.cd_play.descriptor_table = (s32)descriptor_entries;
                    command.args.cd_play.bank_tag = bank_tag;
                    SD_EnqueueCommand(&command);
                    SD_ResetCdPan();
                }
                g_SDValue->s_wSD_STATUS = (g_SDValue->s_wSD_STATUS | 1) & 0xFFFB;
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
    if ((request_state->s_bSD_DATA_SOURCE & 0x80) == 0) {
        return;
    }
    if ((request_state->s_bSD_DATA_SOURCE & 0x40) == 0) {
        if ((u32)(entry_index & 0xFFFF) > 0x9FFF) {
            return;
        }
    }
    if ((command_id & 0x8000) == 0) {
        return;
    }
    class_or_code = command_id & 0xF000;
    *(s16 *)&request_state->s_wSD_CD_REQUEST_CODE = command_id;
    switch (class_or_code) {
    case 0x8000:
        class_or_code = command_id + class_or_code;
        entry_index = class_or_code;
        descriptor_bank = (u8 **)request_state->s_apbSD_CD_DESCRIPTOR_BANK[1];
        descriptor_entries = (u8 *)&descriptor_bank[2];
        bank_tag = 0x50;
        break;
    case 0x9000:
        entry_index = command_id + 0x7000;
        descriptor_bank = (u8 **)request_state->s_apbSD_CD_DESCRIPTOR_BANK[0];
        descriptor_entries = (u8 *)&descriptor_bank[2];
        bank_tag = 0x60;
        break;
    default:
        entry_index += 0x6000;
        bank_tag = 0x70;
        bank_state = g_SDValue;
        descriptor_bank = (u8 **)bank_state->s_apbSD_CD_DESCRIPTOR_BANK[2];
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
        command.args.cd_play.bank_sector_offset = bank_sector_offset;
        command.args.cd_play.descriptor_table = (s32)descriptor_entries;
        command.args.cd_play.bank_tag = bank_tag;
        SD_EnqueueCommand(&command);
    }
    status_state = g_SDValue;
    status_state->s_wSD_STATUS = (status_state->s_wSD_STATUS | 1) & 0xFFFB;
}

void SD_ClearBusyFlag(void)
{
    g_SDValue->s_bSD_BUSY = 0;
}
#endif

#include "file_transfer.h"
#include "sound.h"
#include "sound_output_state.h"

/* Retail stores through both globals with the one-line macro form (lui and
 * %lo at each use, never $gp). */
void SD_ArmBusyCallback(void) {
    g_SDValue->s_bSD_BUSY = 1;
    gFile_SecondaryPayloadCompleteCallback = SD_ClearBusyFlag;
}

#include "sound.h"

/* Scans entries [1, count) for one carrying a space (0x20), the 0x11
   CD-track marker, or a '$' (0x24) in f0; returns 1 on the first hit. */
s32 SD_HasQueuedStreamCommand(void) {
    s32 i;

    for (i = 1; i < g_SDValue->s_wSD_NUM_COMMAND; i++) {
        s32 c = g_SDValue->s_uSD_COMMAND_QUEUE.c[i].command;
        switch (c) {
        case 0x11:
        case 0x20:
        case 0x24:
            return 1;
        }
    }
    return 0;
}

#include "sound_transfer_lifecycle.h"
#include "file_transfer.h"
#include "sound.h"
#include "sound_pending_entries.h"
#include "sound_output_state.h"
#include "sound_sequence_timing.h"
#include "../ygo_types.h"

#include "sound_command_index.h"

void SD_StepActiveCommand(void)
{
    SDValue *entry_state;
    u32 first_word;
    u32 second_word;

    switch (g_SDValue->s_bSD_ACTIVE_COMMAND) {
    case 81:
        SD_RegisterSeqBlockNotes((SDSeqBlock *)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG2,
                     (u32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG0);
        break;

    case 32:
        switch (g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE) {
        case 0:
            if (File_RequestSecondarySlot(0, 0) != 0) {
                return;
            }
            if (g_SDValue->s_dwSD_TRANSFER_HOLD != 0) {
                return;
            }
            {
                SDValue *transfer_state = g_SDValue;
                FileRequestSlot *request = gSD_pFileRequest;
                s32 spu_size;

                request->ram_destination = (s32)transfer_state->s_adwSD_ACTIVE_COMMAND_TAIL[4];
                request->spu_destination = (s32)transfer_state->s_adwSD_ACTIVE_COMMAND_TAIL[3];
                request->sector_offset = (s32)transfer_state->s_adwSD_ACTIVE_COMMAND_TAIL[1];
                request->ram_transfer_size = (s32)transfer_state->s_adwSD_ACTIVE_COMMAND_TAIL[6];
                spu_size = (s32)transfer_state->s_adwSD_ACTIVE_COMMAND_TAIL[5];
                request->cd_filter_channel = 0;
                request->range_sector_count = 0;
                request->spu_transfer_size = spu_size;
                gSD_pFileRequest->cd_filter_file = 0;
            }
            gSD_pFileRequest->file_selector = (s32)g_SDValue->s_adwSD_ACTIVE_COMMAND_TAIL[0];
            if (gSD_pFileRequest->ram_transfer_size == 0 && gSD_pFileRequest->spu_transfer_size == 0) {
                g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
                g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
                return;
            }
            switch ((s32)g_SDValue->s_adwSD_ACTIVE_COMMAND_TAIL[0] & 0xF0) {
            case 0x10:
                gSD_pFileRequest->file_selector = 4;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->s_szSD_SE_FILE);
                break;
            case 0x20:
                gSD_pFileRequest->file_selector = 5;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->s_szSD_BGM_FILE);
                break;
            case 0x40:
                gSD_pFileRequest->file_selector = 6;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->s_szSD_MASTER_XA_FILE);
                break;
            }
            SD_ArmBusyCallback();
            g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE + 1;
            return;
        case 1:
            break;
        default:
            return;
        }
        if (g_SDValue->s_bSD_BUSY != 0) {
            return;
        }
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
            return;
        }
        g_SDValue->s_wSD_STATUS = g_SDValue->s_wSD_STATUS & 0xFFFC;
        return;

    case 17:
        switch (g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE) {
        case 0:
            if (g_SDValue->s_wSD_CD_VOLUME == 0) {
                g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 1;
                return;
            }
            if (g_SDValue->s_wSD_CD_FADE_STEP < 0) {
                return;
            }
            g_SDValue->s_wSD_CD_FADE_STEP = -0x20;
            g_SDValue->s_bSD_CD_FADE_TARGET = 0;
            return;
        case 1:
            break;
        default:
            return;
        }
        g_SDValue->s_wSD_CD_REQUEST_CODE = 0xFFFF;
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
            break;
        }
        {
            SDValue *sd = g_SDValue;
            u16 flags = sd->s_wSD_STATUS;
            sd->s_bSD_ACTIVE_COMMAND_PHASE = 0;
            sd->s_wSD_STATUS = flags & 0xFFF8;
            g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        }
        return;

    case 33:
        if (File_RequestSecondarySlot(0, 0) != 0) {
            break;
        }
        {
            SoundCommandPair *descriptor;
            s32 bank_sector_offset;

            first_word = 0xFFFFFF;
            descriptor = (SoundCommandPair *)(
                (u32)&((SoundCommandPair *)0)[g_SDValue->s_wSD_ACTIVE_COMMAND_VALUE]
                + (u32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG2);
            second_word = descriptor->a;

            gSD_pFileRequest->ram_destination = 0;
            gSD_pFileRequest->ram_transfer_size = 0;
            gSD_pFileRequest->spu_destination = 0;
            gSD_pFileRequest->spu_transfer_size = 0;
            gSD_pFileRequest->range_sector_count = 0;
            gSD_pFileRequest->cd_filter_channel = g_SDValue->s_bSD_CD_FILTER_CHANNEL;
            second_word &= first_word;
            gSD_pFileRequest->cd_filter_file = g_SDValue->s_bSD_CD_FILTER_FILE;
            bank_sector_offset = (s32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG0;
            gSD_pFileRequest->file_selector = 6;
            second_word += bank_sector_offset;
            gSD_pFileRequest->sector_offset = second_word;
            File_RequestSecondarySlot(gSD_pFileRequest, 0);
        }
        break;

    case 36:
        if (File_RequestSecondarySlot(0, 0) != 0) {
            break;
        }
        {
            u32 value_mask = 0xFFFFFF;
            SoundCommandPair *descriptor;
            s32 start_sector_offset;
            u32 relative_start_sector;

            entry_state = g_SDValue;
            descriptor = (SoundCommandPair *)((u32)entry_state->s_wSD_ACTIVE_COMMAND_VALUE * 8
                                     + (u32)entry_state->s_dwSD_ACTIVE_COMMAND_ARG2);
            first_word = descriptor->a;
            second_word = descriptor->b;

            relative_start_sector = first_word & value_mask;
            value_mask &= second_word;
            entry_state->s_dwSD_CD_START_SECTOR = relative_start_sector;
            entry_state->s_bSD_CD_FILTER_FILE = (first_word & 0x1F000000) >> 24;
            entry_state->s_dwSD_CD_END_SECTOR = value_mask;
            g_SDValue->s_bSD_CD_FILTER_CHANNEL = (second_word & 0x1F000000) >> 24;
            second_word >>= 31;
            g_SDValue->s_bSD_CD_DESCRIPTOR_FLAG = second_word;
            first_word >>= 29;
            g_SDValue->s_bSD_CD_MIX_MULTIPLIER = first_word;
            g_SDValue->s_dwSD_CD_START_SECTOR =
                g_SDValue->s_dwSD_CD_START_SECTOR + (s32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG0;
            g_SDValue->s_dwSD_CD_END_SECTOR =
                g_SDValue->s_dwSD_CD_END_SECTOR + (s32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG0;
            gSD_pFileRequest->ram_destination = 0;
            gSD_pFileRequest->ram_transfer_size = 0;
            gSD_pFileRequest->spu_destination = 0;
            gSD_pFileRequest->spu_transfer_size = 0;
            gSD_pFileRequest->range_sector_count = (u16)g_SDValue->s_dwSD_CD_END_SECTOR
                                   - (u16)g_SDValue->s_dwSD_CD_START_SECTOR + 0x10;
            gSD_pFileRequest->cd_filter_channel = g_SDValue->s_bSD_CD_FILTER_CHANNEL;
            gSD_pFileRequest->cd_filter_file = g_SDValue->s_bSD_CD_FILTER_FILE;
            start_sector_offset = g_SDValue->s_dwSD_CD_START_SECTOR;
            gSD_pFileRequest->file_selector = 6;
            gSD_pFileRequest->sector_offset = start_sector_offset;
            File_RequestSecondarySlot(gSD_pFileRequest, 0);
        }
        break;

    case 41:
    case 42:
        if (g_SDValue->s_bSD_ACTIVE_CD_FADE_WAIT != 0 && g_SDValue->s_wSD_CD_FADE_STEP != 0) {
            return;
        }
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        return;

    case 72:
        if (g_SDValue->s_wSD_MUSIC_VAB_ID != 0) {
            SDMusicPackage *list = (SDMusicPackage *)g_SDValue->s_pwSD_MUSIC_TRACK;

            g_SDValue->s_wSD_MUSIC_VAB_ID = SD_VabOpenHead(
                &list->vab_header, 0, list->spu_address
            );
            if ((s16)g_SDValue->s_wSD_MUSIC_VAB_ID != 0) {
                g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
                return;
            }
        }
        if (g_SDValue->s_wSD_SEQ_ACCESS == 0) {
            if ((g_SDValue->s_wSD_STATUS & 0x80) != 0) {
                SD_StopSequence(0);
                SD_SetSecondaryMasterLevels(0, 0);
                g_SDValue->s_wSD_SEQ_FADE_LEVEL = 0;
                g_SDValue->s_wSD_SEQ_FADE_STEP = 0;
                g_SDValue->s_wSD_STATUS = g_SDValue->s_wSD_STATUS & 0xFF7F;
            }
            SD_CloseSequence(g_SDValue->s_wSD_SEQ_ACCESS);
            g_SDValue->s_wSD_SEQ_ACCESS = -1;
        }
        {
            CommandIndexTable *l;
            u32 i;

            entry_state = g_SDValue;
            l = (CommandIndexTable *)entry_state->s_pwSD_MUSIC_TRACK;
            i = entry_state->s_wSD_ACTIVE_COMMAND_VALUE & 0xF;

            if (i >= l->count) {
                g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
                return;
            }
            g_SDValue->s_wSD_SEQ_ACCESS = SD_OpenSequence(
                (u8 *)(
                    (u32)&((u8 (*)[16])0)[l->index_records[i][0]] + (u32)l),
                                             entry_state->s_wSD_MUSIC_VAB_ID);
            if ((s16)g_SDValue->s_wSD_SEQ_ACCESS == 0) {
                g_SDValue->s_wSD_OPEN_SEQUENCE_COMMAND = g_SDValue->s_wSD_ACTIVE_COMMAND_VALUE;
                SD_PlaySequence(1, 1);
                {
                    SDValue *sd = g_SDValue;
                    sd->s_bSD_SEQ_FADE_TARGET = 0xFF;
                    sd->s_wSD_STATUS = sd->s_wSD_STATUS | 0x80;
                    sd->s_wSD_SEQ_FADE_STEP = (u16)sd->s_dwSD_ACTIVE_COMMAND_ARG1;
                }
            }
        }
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        return;

    case 0:
        break;
    }

    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
    g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
}

#include "sound_output_state.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_voice_constants.h"

s32 SD_EnqueueCommand(SDCommand *src) {
    SDValue *capacity_state;
    SDValue *write_state;
    SDValue *count_state;
    s32 capacity_count;
    s32 write_index;
    s32 one;

    capacity_state = g_SDValue;
    capacity_count = capacity_state->s_wSD_NUM_COMMAND;
    if (capacity_count < SD_COMMAND_QUEUE_COUNT) {
        capacity_state->s_uSD_COMMAND_QUEUE.c[capacity_count].command = src->command;

        write_state = g_SDValue;
        write_index = write_state->s_wSD_NUM_COMMAND;
        write_state->s_uSD_COMMAND_QUEUE.c[write_index] = *src;

        count_state = g_SDValue;
        one = 1;
        count_state->s_wSD_NUM_COMMAND = count_state->s_wSD_NUM_COMMAND + one;
        return one;
    }
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
            state->s_wSD_SEQ_FADE_STEP = (u16)command_cursor->args.fade.step;
            state->s_bSD_BGM_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x45:
        if (state->s_wSD_SEQ_FADE_STEP == 0) {
            state->s_wSD_SEQ_FADE_STEP = (u16)command_cursor->args.fade.step;
            state->s_bSD_BGM_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x29:
        if (state->s_wSD_CD_FADE_STEP == 0) {
            state->s_wSD_CD_FADE_STEP = (u16)command_cursor->args.fade.step;
            state->s_bSD_CD_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    case 0x2A:
        if (state->s_wSD_CD_FADE_STEP == 0) {
            state->s_wSD_CD_FADE_STEP = (u16)command_cursor->args.fade.step;
            state->s_bSD_CD_FADE_TARGET = (u8)command_cursor->value;
        }
        break;
    }

    state = g_SDValue;
    state->s_bSD_ACTIVE_COMMAND = command_cursor->command;
    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
    state = g_SDValue;
    state->s_wSD_ACTIVE_COMMAND_VALUE = (u16)command_cursor->value;
    state->s_dwSD_ACTIVE_COMMAND_ARG0 = command_cursor->args.raw.arg0;
    state->s_dwSD_ACTIVE_COMMAND_ARG1 = command_cursor->args.raw.arg1;
    state->s_dwSD_ACTIVE_COMMAND_ARG2 = command_cursor->args.raw.arg2;
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

#include "sound.h"

#define STATE_BYTES ((u8 *)g_SDValue)

void SD_CancelMusicRequests(void)
{
    s32 i;
    s32 offset;
    s32 next_offset;
    s32 command;
    SDValue *base;

    base = g_SDValue;
    /* Flattening this initialization changes GCC 2.8.1's loop-register
     * allocation; the table and both byte offsets must remain independent. */
    do {
        i = 0;
    } while (0);
    if (base->s_wSD_NUM_COMMAND > 0) {
        while (1) {
            offset = i * SD_COMMAND_RECORD_SIZE;
            next_offset = (i + 1) * SD_COMMAND_RECORD_SIZE;
            if (base->s_wSD_NUM_COMMAND == 0) {
                goto after;
            }
            command = base->s_uSD_COMMAND_QUEUE.b[offset];
            switch (command) {
            case 0x42:
            case 0x43:
            case 0x45:
            case 0x46:
            case 0x48:
                *(SDCommand *)(STATE_BYTES + offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
                    *(SDCommand *)(STATE_BYTES + next_offset + SD_COMMAND_QUEUE_BYTE_OFFSET);
                goto decrement;
            case 0x20:
                /* Keep the byte-cursor sum: direct member access reschedules
                   the queue-base add and the following load. */
                if (*(s32 *)(STATE_BYTES + offset +
                        SD_COMMAND_QUEUE_BYTE_OFFSET +
                        (u32)&((SDCommand *)0)->transfer_selector) != 0x20) {
                    goto test;
                }
                *(SDCommand *)(STATE_BYTES + offset + SD_COMMAND_QUEUE_BYTE_OFFSET) =
                    *(SDCommand *)(STATE_BYTES + next_offset + SD_COMMAND_QUEUE_BYTE_OFFSET);
decrement:
                g_SDValue->s_wSD_NUM_COMMAND = g_SDValue->s_wSD_NUM_COMMAND - 1;
                goto test;
            default:
                i++;
            }
test:
            base = g_SDValue;
            if (i >= base->s_wSD_NUM_COMMAND) {
                break;
            }
            if (i < 0) {
                break;
            }
        }
    }
after:
    switch (g_SDValue->s_bSD_ACTIVE_COMMAND) {
    case 0x42:
    case 0x43:
    case 0x45:
    case 0x46:
    case 0x48:
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        break;
    }

    if ((g_SDValue->s_wSD_STATUS & 0x80) != 0) {
        if (g_SDValue->s_wSD_SEQ_ACCESS != -1) {
            if (SD_PollSequenceState() == 1) {
                SD_StopSequence(g_SDValue->s_wSD_SEQ_ACCESS);
            }
            g_SDValue->s_wSD_STATUS = g_SDValue->s_wSD_STATUS & 0xFF7F;
        }
    }
}

#include "sound.h"

/* Drops every queued 0x24, 0x29 and 0x2B command, shifting the next record
 * down over each one and shortening the queue, then clears the pending
 * 0x7C/0x7D pair when it holds one of those commands.
 *
 * The queue walk tests the count, the index sign and an empty queue in that
 * order: GCC rotates the three-part condition so its entry check is only
 * count > 0 and the back edge re-enters at the empty-queue test, which is
 * the retail loop shape. The switches give the retail 0x29-first compare
 * tree. */
void SD_DropQueuedCdPlayCommands(void)
{
    s32 i;

    for (i = 0; i < g_SDValue->s_wSD_NUM_COMMAND && i >= 0 &&
                g_SDValue->s_wSD_NUM_COMMAND != 0; ) {
        switch (g_SDValue->s_uSD_COMMAND_QUEUE.c[i].command) {
        case 0x24:
        case 0x29:
        case 0x2B:
            g_SDValue->s_uSD_COMMAND_QUEUE.c[i] =
                ((SDCommandShiftView *)g_SDValue)->c[i + 1];
            g_SDValue->s_wSD_NUM_COMMAND--;
            break;
        default:
            i++;
            break;
        }
    }
    switch (g_SDValue->s_bSD_ACTIVE_COMMAND) {
    case 0x24:
    case 0x29:
    case 0x2B:
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        break;
    }
}

#define G_SDVALUE_IN_DATA
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#define SOUND_TRANSFER_REQUEST_IN_DATA
#include "../types.h"
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

    switch (g_SDValue->active_command) {
    case 81:
        SD_RegisterSeqBlockNotes((SDSeqBlock *)g_SDValue->active_command_arg2,
                     (u32)g_SDValue->active_command_arg0);
        goto clear_7d_7c;

    case 32:
        switch (g_SDValue->active_command_phase) {
        case 0:
            if (File_RequestSecondarySlot(0, 0) != 0) {
                return;
            }
            if (g_SDValue->field_04CC != 0) {
                return;
            }
            {
                SDValue *sd = g_SDValue;
                FileRequestSlot *b = gSD_pFileRequest;
                s32 final_word;

                b->field_10 = (s32)sd->active_command_tail[4];
                b->field_0C = (s32)sd->active_command_tail[3];
                b->field_04 = (s32)sd->active_command_tail[1];
                b->field_18 = (s32)sd->active_command_tail[6];
                final_word = (s32)sd->active_command_tail[5];
                b->field_1F = 0;
                b->field_1C = 0;
                b->field_14 = final_word;
                gSD_pFileRequest->field_1E = 0;
            }
            gSD_pFileRequest->field_00 = (s32)g_SDValue->active_command_tail[0];
            if (gSD_pFileRequest->field_18 == 0 && gSD_pFileRequest->field_14 == 0) {
                g_SDValue->active_command_phase = 0;
                goto clear_7c;
            }
            switch ((s32)g_SDValue->active_command_tail[0] & 0xF0) {
            case 0x10:
                gSD_pFileRequest->field_00 = 4;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->se_file_name);
                break;
            case 0x20:
                gSD_pFileRequest->field_00 = 5;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->bgm_file_name);
                break;
            case 0x40:
                gSD_pFileRequest->field_00 = 6;
                File_RequestSecondarySlot(gSD_pFileRequest, g_SDValue->master_xa_file_name);
                break;
            }
            SD_ArmBusyCallback();
            g_SDValue->active_command_phase = g_SDValue->active_command_phase + 1;
            return;
        case 1:
            break;
        default:
            return;
        }
        if (g_SDValue->busy != 0) {
            return;
        }
        g_SDValue->active_command_phase = 0;
        g_SDValue->active_command = 0;
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
            return;
        }
        g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFFFC;
        return;

    case 17:
        switch (g_SDValue->active_command_phase) {
        case 0:
            if (g_SDValue->cd_volume == 0) {
                g_SDValue->active_command_phase = 1;
                return;
            }
            if (g_SDValue->cd_fade_step < 0) {
                return;
            }
            g_SDValue->cd_fade_step = -0x20;
            g_SDValue->cd_fade_target = 0;
            return;
        case 1:
            break;
        default:
            return;
        }
        g_SDValue->cd_request_code = 0xFFFF;
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
            goto clear_7d_7c;
        }
        {
            SDValue *sd = g_SDValue;
            u16 flags = sd->flags_0040;
            sd->active_command_phase = 0;
            sd->flags_0040 = flags & 0xFFF8;
            g_SDValue->active_command = 0;
        }
        return;

    case 33:
        if (File_RequestSecondarySlot(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e;
            s32 base_offset;

            first_word = 0xFFFFFF;
            e = (SoundCommandPair *)(
                (u32)&((SoundCommandPair *)0)[g_SDValue->active_command_value]
                + (u32)g_SDValue->active_command_arg2);
            second_word = e->a;

            gSD_pFileRequest->field_10 = 0;
            gSD_pFileRequest->field_18 = 0;
            gSD_pFileRequest->field_0C = 0;
            gSD_pFileRequest->field_14 = 0;
            gSD_pFileRequest->field_1C = 0;
            gSD_pFileRequest->field_1F = g_SDValue->cd_filter_channel;
            second_word &= first_word;
            gSD_pFileRequest->field_1E = g_SDValue->cd_filter_file;
            base_offset = (s32)g_SDValue->active_command_arg0;
            gSD_pFileRequest->field_00 = 6;
            second_word += base_offset;
            gSD_pFileRequest->field_04 = second_word;
            File_RequestSecondarySlot(gSD_pFileRequest, 0);
        }
        goto clear_7d_7c;

    case 36:
        if (File_RequestSecondarySlot(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            u32 value_mask = 0xFFFFFF;
            SoundCommandPair *e;
            s32 request_offset;
            u32 low_a;

            entry_state = g_SDValue;
            e = (SoundCommandPair *)((u32)entry_state->active_command_value * 8
                                     + (u32)entry_state->active_command_arg2);
            first_word = e->a;
            second_word = e->b;

            low_a = first_word & value_mask;
            value_mask &= second_word;
            entry_state->cd_start_sector_offset = low_a;
            entry_state->cd_filter_file = (first_word & 0x1F000000) >> 24;
            entry_state->cd_end_sector_offset = value_mask;
            g_SDValue->cd_filter_channel = (second_word & 0x1F000000) >> 24;
            second_word >>= 31;
            g_SDValue->field_0532 = second_word;
            first_word >>= 29;
            g_SDValue->mix_multiplier = first_word;
            g_SDValue->cd_start_sector_offset =
                g_SDValue->cd_start_sector_offset + (s32)g_SDValue->active_command_arg0;
            g_SDValue->cd_end_sector_offset =
                g_SDValue->cd_end_sector_offset + (s32)g_SDValue->active_command_arg0;
            gSD_pFileRequest->field_10 = 0;
            gSD_pFileRequest->field_18 = 0;
            gSD_pFileRequest->field_0C = 0;
            gSD_pFileRequest->field_14 = 0;
            gSD_pFileRequest->field_1C = (u16)g_SDValue->cd_end_sector_offset
                                   - (u16)g_SDValue->cd_start_sector_offset + 0x10;
            gSD_pFileRequest->field_1F = g_SDValue->cd_filter_channel;
            gSD_pFileRequest->field_1E = g_SDValue->cd_filter_file;
            request_offset = g_SDValue->cd_start_sector_offset;
            gSD_pFileRequest->field_00 = 6;
            gSD_pFileRequest->field_04 = request_offset;
            File_RequestSecondarySlot(gSD_pFileRequest, 0);
        }
        goto clear_7d_7c;

    case 41:
    case 42:
        if (g_SDValue->active_cd_fade_wait != 0 && g_SDValue->cd_fade_step != 0) {
            return;
        }
        g_SDValue->active_command = 0;
        goto clear_7d;

    case 72:
        if (g_SDValue->music_vab_open_result != 0) {
            SDMusicPackage *list = (SDMusicPackage *)g_SDValue->music_track;

            g_SDValue->music_vab_open_result = SD_VabOpenHead(
                &list->vab_header, 0, list->spu_address
            );
            if ((s16)g_SDValue->music_vab_open_result != 0) {
                g_SDValue->active_command = 0;
                return;
            }
        }
        if (g_SDValue->sequence_open_result == 0) {
            if ((g_SDValue->flags_0040 & 0x80) != 0) {
                SD_StopSequence(0);
                SD_SetSecondaryMasterLevels(0, 0);
                g_SDValue->sequence_fade_level = 0;
                g_SDValue->sequence_fade_step = 0;
                g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFF7F;
            }
            SD_CloseSequence(g_SDValue->sequence_open_result);
            g_SDValue->sequence_open_result = -1;
        }
        {
            CommandIndexTable *l;
            u32 i;

            entry_state = g_SDValue;
            l = (CommandIndexTable *)entry_state->music_track;
            i = entry_state->active_command_value & 0xF;

            if (i >= l->count) {
                g_SDValue->active_command = 0;
                return;
            }
            g_SDValue->sequence_open_result = SD_OpenSequence(
                (u8 *)(
                    (u32)&((u8 (*)[16])0)[l->index_records[i][0]] + (u32)l),
                                             entry_state->music_vab_open_result);
            if ((s16)g_SDValue->sequence_open_result == 0) {
                g_SDValue->opened_sequence_command = g_SDValue->active_command_value;
                SD_PlaySequence(1, 1);
                {
                    SDValue *sd = g_SDValue;
                    sd->sequence_fade_target = 0xFF;
                    sd->flags_0040 = sd->flags_0040 | 0x80;
                    sd->sequence_fade_step = (u16)sd->active_command_arg1;
                }
            }
        }
        g_SDValue->active_command = 0;
        goto clear_7d;

    case 0:
        break;
    }

    goto clear_7d_7c;

clear_7d:
    g_SDValue->active_command_phase = 0;
    return;

clear_7d_7c:
    g_SDValue->active_command_phase = 0;
clear_7c:
    g_SDValue->active_command = 0;
}

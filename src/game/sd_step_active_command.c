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

    switch (g_SDValue->s_bSD_ACTIVE_COMMAND) {
    case 81:
        SD_RegisterSeqBlockNotes((SDSeqBlock *)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG2,
                     (u32)g_SDValue->s_dwSD_ACTIVE_COMMAND_ARG0);
        goto clear_7d_7c;

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
                goto clear_7c;
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
            goto clear_7d_7c;
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
            goto clear_7d_7c;
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
        goto clear_7d_7c;

    case 36:
        if (File_RequestSecondarySlot(0, 0) != 0) {
            goto clear_7d_7c;
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
        goto clear_7d_7c;

    case 41:
    case 42:
        if (g_SDValue->s_bSD_ACTIVE_CD_FADE_WAIT != 0 && g_SDValue->s_wSD_CD_FADE_STEP != 0) {
            return;
        }
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        goto clear_7d;

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
        goto clear_7d;

    case 0:
        break;
    }

    goto clear_7d_7c;

clear_7d:
    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
    return;

clear_7d_7c:
    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
clear_7c:
    g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
}

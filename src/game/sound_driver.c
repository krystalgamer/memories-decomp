#define D_8009B0F0_IN_DATA
#define D_8009B120_IN_DATA
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"

#ifndef SOUND_OUTPUT_TRANSITION_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B458-0x8009B468, defined in
 * address order ahead of the headers that declare it (#2602). */
struct SDSecondaryState *gSD_pSecondaryState;
struct SDValue *g_SDValue;
struct FileRequestSlot *gSD_pFileRequest;
static u32 D_8009B464;
#else
extern struct SDSecondaryState *gSD_pSecondaryState;
extern struct SDValue *g_SDValue;
extern struct FileRequestSlot *gSD_pFileRequest;
#endif

#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

void SD_FadeInStreamAudio(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->s_bSD_CD_FADE_TARGET = 255;
    state = g_SDValue;
    first->cd_fade_step = 64;
    state->s_wSD_STATUS = (state->s_wSD_STATUS & 0xFFFC) | 4;
    if ((state->s_wSD_STATUS & 0x80) &&
        state->sequence_fade_step >= 0) {
        state->sequence_fade_step = -16;
        state->bgm_fade_target = 220;
    }
}

void SD_FadeOutStreamAudio(void)
{
    SDValue *state = g_SDValue;
    SDValue *flags;

    if (state->s_wSD_STATUS & 0x80) {
        state->sequence_fade_step = 8;
        state->bgm_fade_target = 255;
        state = g_SDValue;
        state->s_bSD_CD_FADE_TARGET = 0;
        flags = g_SDValue;
        state->cd_fade_step = -64;
        flags->s_wSD_STATUS &= 0xFFFB;
        return;
    }
    state->s_bSD_CD_FADE_TARGET = 0;
    flags = g_SDValue;
    state->cd_fade_step = -64;
    flags->s_wSD_STATUS &= 0xFFFB;
}

void SD_InitSpuCommonVolume(void)
{
    SpuCommonAttr entry;

    /* 707 is exactly these five bits, and the two fields it does not select
       are still written because retail writes them. */
    entry.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL |
                 SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX;
    entry.mvol.left = 16383;
    entry.mvol.right = 16383;
    entry.cd.volume.left = 32767;
    entry.cd.volume.right = 32767;
    entry.cd.reverb = SPU_OFF;
    entry.cd.mix = SPU_ON;
    SpuSetCommonAttr(&entry);
}

#include "file_transfer.h"
#include "sound.h"
#include "sound_mix.h"
#include "sound_init.h"
#include "sound_output_transition.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_data.h"
#include "sound_voice_selection.h"

#define SD_INIT_BLOCK_10_VIEW(block) ((SDInitBlk10 *)(block))
#define SD_INIT_BLOCK_11_VIEW(block) ((SDInitBlk11 *)(block))

/* Brings the sound subsystem's resident state up from cold. The state block
 * starts at the end of the loaded image and runs to 0x801EA7FF; it is zeroed a
 * word at a time, the two command callbacks are installed, three file names
 * are copied from the loaded image, the mode byte is seeded (0xF0 extra bits
 * when master_xa_lba_byte is set), the mix defaults are seeded from the values just written
 * at +0x1649 and +0x164A, and the subsystem initialisers run before the
 * secondary state's tick callback is armed.
 *
 * The image bounds are plain integer constants rather than `&loaded_image_end`
 * (which the linker also puts at 0x801E0000): the retail code derives
 * 0x801EA7FF from the same `lui` as the base with an `ori`, which only happens
 * when both are integers GCC can share a high half between. Through the symbol
 * they become separate relocations and the `ori` becomes a second `lui`. */
void SD_InitState(u8 master_xa_lba_byte)
{
    u32 *clear_cursor;
    SDValue *mix_state;
    SDValue *status_state;
    SDSecondaryState *callback_state;
    SDValue *bgm_target_state;
    s32 cd_default;
    s32 secondary_default;

    g_SDValue = (SDValue *)0x801E0000;
    gSD_pFileRequest = (FileRequestSlot *)0x801E1650;
    clear_cursor = (u32 *)0x801E0000;
    do {
        *clear_cursor = 0;
        clear_cursor++;
    } while (clear_cursor <= (u32 *)0x801EA7FF);
    gFile_RangeReadCallback = SD_FadeInStreamAudio;
    gFile_RangePauseCallback = SD_FadeOutStreamAudio;
    *SD_INIT_BLOCK_11_VIEW(g_SDValue->bgm_file_name) =
        *SD_INIT_BLOCK_11_VIEW(gSD_szBgmFileName);
    *SD_INIT_BLOCK_10_VIEW(g_SDValue->se_file_name) =
        *SD_INIT_BLOCK_10_VIEW(gSD_szSeFileName);
    *SD_INIT_BLOCK_10_VIEW(g_SDValue->master_xa_file_name) =
        *SD_INIT_BLOCK_10_VIEW(gSD_szMasterXaFileName);
    g_SDValue->s_bSD_DATA_SOURCE = 3;
    if (master_xa_lba_byte != 0) {
        g_SDValue->s_bSD_DATA_SOURCE |= 0xF0;
    }
    g_SDValue->s_bSD_CD_MIXER_OUT_DEFAULT = 0xFF;
    g_SDValue->s_bSD_SMF_MIXER_OUT_DEFAULT = 0xD2;
    g_SDValue->default_bgm_step = 0xFF;
    mix_state = g_SDValue;
    cd_default = mix_state->s_bSD_CD_MIXER_OUT_DEFAULT;
    secondary_default = mix_state->s_bSD_SMF_MIXER_OUT_DEFAULT;
    mix_state->s_bSD_CD_FADE_TARGET = 0xFF;
    bgm_target_state = g_SDValue;
    mix_state->s_wSD_CD_MIXER_OUT = cd_default;
    mix_state->s_wSD_SMF_MIXER_OUT = secondary_default;
    bgm_target_state->bgm_fade_target = 0xFF;
    SD_InitSecondaryWorkArea((s32 *)0x801E1670);
    SD_InitSpuCommonVolume();
    SD_InitBufferState();
    SD_InitSecondaryRuntime();
    SD_SetOutputType(0);
    gSD_pSecondaryState->vsync_update_mode = 0;
    callback_state = gSD_pSecondaryState;
    status_state = g_SDValue;
    callback_state->timer_maintenance_callback = SD_UpdateVoiceSlots;
    status_state->s_wSD_STATUS = 0;
}

#include "sound.h"
#include "sound_state_control.h"

void SD_SetSequenceVSyncMode(s32 vsync_mode)
{
    SDSecondaryState *scheduling_state;

    gSD_pSecondaryState->vsync_update_mode = vsync_mode;
    scheduling_state = gSD_pSecondaryState;
    if (scheduling_state->vsync_update_mode != 0)
        scheduling_state->timer_update_inhibit = 1;
    else
        scheduling_state->timer_update_inhibit = 0;
}

void SD_InitDataSourceFlags(s32 se_lba, s32 bgm_lba, s32 master_xa_lba)
{
    g_SDValue->s_dwSD_BANK_LOAD_PHASE = 0;
    if (se_lba == 0)
        g_SDValue->s_bSD_DATA_SOURCE &= 0xFE;
    if (bgm_lba == 0)
        g_SDValue->s_bSD_DATA_SOURCE &= 0xFD;
    if (master_xa_lba == 0)
        g_SDValue->s_bSD_DATA_SOURCE &= 0xBF;
    g_SDValue->s_wSD_STATUS |= 0xA;
}

#include "../psyq/stdio.h"
#include "../unmatched.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output.h"

/* MATCH 2026-09-06, first-day function from the m2c draft (248 instructions,
 * the sound bank loader state machine: SE header, SE groups, then the SMF and
 * CD mixer-out banks with their "VolInfo" signature checks). Jump table on
 * the state word, so only the full build proves it. Levers, in order: the
 * 8-byte SDGlobalHdrSE copied as ONE struct assignment (its halfword
 * alignment gives the two lwl/lwr pairs batched before the swl/swr pairs);
 * the state increment written INLINE in every arm rather than as a shared tail (retail cross-jumps the
 * identical ones itself); the shared SD_EnqueueRamTransfer call block placed
 * between cases 8 and 9 -- where retail has it -- so the return block is
 * preceded by the default arm and no store gets cross-jumped into it; case
 * 8's 0x3E7 arm out of line after that call block through a goto; the
 * scaled argument in case 2 as two statements against one name; the buffer
 * base assigned before the signature test; the two leading signature bytes
 * gSD_SmfBankVolInfoTag/gSD_CdBankVolInfoTag in .data (bare form beside the gp-relative
 * g_SDValue), which is why SDBankStagingBuffer starts at the second letter
 * rather than covering the whole word; and the first of its letters held in
 * a u8 local, whose zero-extension is the andi 0xFF retail keeps in a second
 * register for the last comparison.
 */

/* The European build makes none of the three printf calls. */
void SD_UpdateBankLoad(void) {
    SDBankStagingBuffer *bank_tail;
    u8 *ram_destination;
    s32 sector_offset;
    s32 ram_size;
    s32 source_selector;
    u8 signature_first;

    switch (g_SDValue->s_dwSD_BANK_LOAD_PHASE) {
    case 0:
        if (g_SDValue->s_bSD_DATA_SOURCE & 1) {
            ram_destination = (u8 *)0x801E6800;
            sector_offset = 0;
            ram_size = 0x800;
            source_selector = 0x10;
            goto call;
        }
        g_SDValue->s_dwSD_BANK_LOAD_PHASE += 6;
        return;
    case 1:
        if ((SD_GetStatusFlags() & 7) == 0) {
            g_SDValue->s_stGlobalHdrSE = *(SDGlobalHdrSE *)0x801E6800;
            g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
            return;
        }
        return;
    case 2:
#ifndef VERSION_EUROPE
        printf(gSD_szSeGroupCountFormat, g_SDValue->s_stGlobalHdrSE.s_wSD_NUM_SEGROUP);
#endif
        ram_destination = (u8 *)0x801E2800;
        sector_offset = 1;
        ram_size = g_SDValue->s_stGlobalHdrSE.s_wSD_NUM_SEGROUP;
        source_selector = 0x10;
        ram_size = ram_size * 8;
        goto call;
    case 3:
        if ((SD_GetStatusFlags() & 7) == 0) {
            SD_InitVoiceState();
            g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
            return;
        }
        break;
    case 4:
        SD_QueueValueLinkTransfer(0);
        g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
        return;
    case 5:
        if (SD_GetStatusFlags() & 7) {
            return;
        }
        g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
        return;
    case 6:
        if (g_SDValue->s_bSD_DATA_SOURCE & 2) {
            ram_destination = (u8 *)0x801E2000;
            sector_offset = 0;
            ram_size = 0x800;
            source_selector = 0x20;
            goto call;
        }
        g_SDValue->s_dwSD_BANK_LOAD_PHASE += 2;
        return;
    case 7:
        if ((SD_GetStatusFlags() & 7) == 0) {
            bank_tail = (SDBankStagingBuffer *)0x801E2000;
            if (gSD_SmfBankVolInfoTag == 'V') {
                signature_first = bank_tail->signature[0];
                if (signature_first == 'o' && 'l' == bank_tail->signature[1] &&
                    'I' == bank_tail->signature[2] && 'n' == bank_tail->signature[3] &&
                    'f' == bank_tail->signature[4] && bank_tail->signature[5] == signature_first) {
                    g_SDValue->s_bSD_SMF_MIXER_OUT_DEFAULT = bank_tail->volume;
                    g_SDValue->s_wSD_SMF_MIXER_OUT = g_SDValue->s_bSD_SMF_MIXER_OUT_DEFAULT;
#ifndef VERSION_EUROPE
                    printf(gSD_szSmfMixerOutFormat, g_SDValue->s_bSD_SMF_MIXER_OUT_DEFAULT);
#endif
                }
            }
            g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
            return;
        }
        break;
    case 8:
        if ((g_SDValue->s_bSD_DATA_SOURCE & 0x80) == 0) {
            goto no8;
        }
        ram_destination = (u8 *)0x801E8800;
        sector_offset = 0;
        ram_size = 0x2000;
        source_selector = 0x40;
    call:
        SD_EnqueueRamTransfer((s32)ram_destination, sector_offset, ram_size, source_selector);
        g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
        return;
    no8:
        g_SDValue->s_dwSD_BANK_LOAD_PHASE = 0x3E7;
        return;
    case 9:
        if ((SD_GetStatusFlags() & 7) == 0) {
            bank_tail = (SDBankStagingBuffer *)0x801E8800;
            if (gSD_CdBankVolInfoTag == 'V') {
                signature_first = bank_tail->signature[0];
                if (signature_first == 'o' && 'l' == bank_tail->signature[1] &&
                    'I' == bank_tail->signature[2] && 'n' == bank_tail->signature[3] &&
                    'f' == bank_tail->signature[4] && bank_tail->signature[5] == signature_first) {
                    g_SDValue->s_bSD_CD_MIXER_OUT_DEFAULT = bank_tail->volume;
                    g_SDValue->s_wSD_CD_MIXER_OUT = g_SDValue->s_bSD_CD_MIXER_OUT_DEFAULT;
#ifndef VERSION_EUROPE
                    printf(gSD_szCdMixerOutFormat, g_SDValue->s_bSD_CD_MIXER_OUT_DEFAULT);
#endif
                }
            }
            g_SDValue->cd_descriptor_banks[0] = (u8 *)0x801E8800;
            g_SDValue->cd_descriptor_banks[1] = (u8 *)0x801E9000;
            g_SDValue->cd_descriptor_banks[2] = (u8 *)0x801EA000;
            g_SDValue->s_dwSD_BANK_LOAD_PHASE += 1;
            return;
        }
        break;
    default:
        if ((SD_GetStatusFlags() & 7) == 0) {
            g_SDValue->s_wSD_STATUS &= 0xFFF7;
        }
        break;
    }
    return;
}

#include "sd_set_cd_volume.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_sequence_state.h"
#include "sound_output_state.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_selection.h"

#include "sound_init.h"

/* Each block re-reads g_SDValue rather than caching it once: the driver block
   is reachable through the global, so every store through one pointer forces
   the next read. */
void SD_ResetRuntime(void)
{
    SDValue *mix_state;
    SDValue *sequence_state;
    SDValue *vab_state;
    SDValue *music_state;
    SDValue *command_state;
    SDValue *payload_state;
    SDValue *status_state;
    s32 voice_slot;
    s32 vab_result;

    mix_state = g_SDValue;
    mix_state->s_wSD_CD_MIXER_OUT = mix_state->s_bSD_CD_MIXER_OUT_DEFAULT;
    mix_state->s_wSD_SMF_MIXER_OUT = mix_state->s_bSD_SMF_MIXER_OUT_DEFAULT;
    SD_SetCdVolume(0);
    sequence_state = g_SDValue;
    sequence_state->cd_fade_step = 0;
    if (sequence_state->sequence_open_result != -1) {
        if ((s16)SD_PollSequenceState() != 1) {
            SD_StopSequence(g_SDValue->sequence_open_result);
            g_SDValue->s_wSD_STATUS &= 0xFF7F;
        }
        SD_CloseSequence(g_SDValue->sequence_open_result);
    }
    vab_state = g_SDValue;
    vab_result = vab_state->music_vab_open_result;
    vab_state->opened_sequence_command = -1;
    vab_state->sequence_open_result = -1;
    if (vab_result != -1) {
        SD_ResetVabTransferState(vab_result);
    }
    music_state = g_SDValue;
    music_state->opened_music_package_id = -1;
    music_state->music_vab_open_result = -1;
    music_state->sequence_fade_step = 0;
    SpuSetKey(SPU_OFF, 0xFFFFFF);
    g_SDValue->s_bSD_VOICE_ACTIVE_MASK = 0;
    g_SDValue->s_bSD_VOICE_CURSOR = 0;
    for (voice_slot = 0; voice_slot < 4; voice_slot++) {
        g_SDValue->s_abSD_VOICE_PRIORITY[voice_slot] = 0;
        g_SDValue->s_awSD_VOICE_SE_ID[voice_slot] = 0;
    }
    command_state = g_SDValue;
    command_state->s_bSD_ACTIVE_COMMAND = 0;
    command_state->s_wSD_NUM_COMMAND = 0;
    g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
    payload_state = g_SDValue;
    payload_state->s_bSD_ACTIVE_CD_FADE_WAIT = 0;
    status_state = g_SDValue;
    payload_state->s_wSD_ACTIVE_COMMAND_VALUE = 0;
    payload_state->s_dwSD_ACTIVE_COMMAND_ARG0 = 0;
    payload_state->s_dwSD_ACTIVE_COMMAND_ARG1 = 0;
    payload_state->s_dwSD_ACTIVE_COMMAND_ARG2 = 0;
    status_state->s_wSD_STATUS = 0;
}

#include "sound_init.h"
#include "sound_pending_entries.h"
#include "sound_output.h"
void SD_Shutdown(void)
{
    SD_KeyOffVoiceSlots();
    SD_ResetPendingEntries();
    SD_Init();
    SD_ResetSecondaryRuntime();
    SpuSetIRQ(SPU_OFF);
    SpuQuit();
}

void SD_SetOutputType(s16 value)
{
    SDValue *state = g_SDValue;

    if (state->s_bSD_OUTPUT_TYPE != value) {
        state->s_bSD_OUTPUT_TYPE = value;
        if (value == 0)
            SD_EnableSecondaryPanning();
        else
            SD_DisableSecondaryPanning();
        SD_SetCdVolume(g_SDValue->cd_volume);
    }
}

s32 SD_GetOutputType(void)
{
    return g_SDValue->s_bSD_OUTPUT_TYPE;
}

void tent_SD_SetDataSourceFlags(s32 value)
{
    g_SDValue->s_bSD_DATA_SOURCE =
        (g_SDValue->s_bSD_DATA_SOURCE & 0xF0) | value;
}

s32 SD_GetStatusFlags(void)
{
    return (s16)g_SDValue->s_wSD_STATUS;
}

void SD_VSync(void)
{
    if (gSD_pSecondaryState->vsync_update_mode)
        SD_UpdateSecondarySequenceIfEnabled();
    SD_UpdateRuntime();
    if (g_SDValue->s_wSD_STATUS & 8)
        SD_UpdateBankLoad();
}

void SD_EnqueueRamTransfer(s32 ram_destination, s32 sector_offset, s32 ram_size, s32 source_selector)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->s_wSD_STATUS |= 2;
    command.command = 32;
    command.transfer_selector = source_selector;
    command.transfer_sector_offset = sector_offset;
    command.transfer_ram_destination = ram_destination;
    command.transfer_ram_size = ram_size;
    command.transfer_spu_destination = 0;
    command.transfer_spu_size = 0;
    SD_EnqueueCommand(&command);
}

void SD_EnqueueSpuTransfer(s32 spu_destination, s32 sector_offset, s32 spu_size, s32 source_selector)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->s_wSD_STATUS |= 2;
    command.command = 32;
    command.transfer_selector = source_selector;
    command.transfer_sector_offset = sector_offset;
    command.transfer_ram_destination = 0;
    command.transfer_ram_size = 0;
    command.transfer_spu_destination = spu_destination;
    command.transfer_spu_size = spu_size;
    SD_EnqueueCommand(&command);
}

void SD_EnqueueSpuRamTransfer(
    s32 spu_destination,
    s32 ram_destination,
    s32 sector_offset,
    s32 spu_size,
    s32 ram_size,
    s32 source_selector
)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->s_wSD_STATUS |= 2;
    command.command = 32;
    command.transfer_selector = source_selector;
    command.transfer_sector_offset = sector_offset;
    command.transfer_ram_destination = ram_destination;
    command.transfer_ram_size = ram_size;
    command.transfer_spu_destination = spu_destination;
    command.transfer_spu_size = spu_size;
    SD_EnqueueCommand(&command);
}

void SD_PreloadBgmAndReplaceValueLink(u32 packed_request)
{
    SD_BGMPreload(packed_request >> 16);
    SD_ReplaceValueLink(packed_request & SD_COMMAND_VALUE_MASK);
}

void SD_BGMPreload(s32 command_id)
{
    u16 saved_command = command_id;

    if ((g_SDValue->s_bSD_DATA_SOURCE & 2) == 0)
        return;

    if (command_id & 0x8000) {
        SD_EnqueueCdSeek(saved_command & SD_COMMAND_VALUE_MASK);
    } else {
        u32 masked_command = (u32)(saved_command & SD_COMMAND_VALUE_MASK);

        if (masked_command >= SD_BGM_COMMAND_BASE)
            command_id -= SD_BGM_COMMAND_BASE;
        SD_RequestMusicPackageLoad((s16)command_id, 1);
    }
}

void SD_BGMPlayDefaultStep(u32 command_id)
{
    SD_BGMPlayWithStep(command_id & SD_COMMAND_VALUE_MASK, g_SDValue->default_bgm_step);
}

void SD_BGMPlayWithStep(s32 command_id, s32 step)
{
    u16 saved_command = command_id;

    if ((g_SDValue->s_bSD_DATA_SOURCE & 2) == 0)
        return;

    if (command_id & 0x8000) {
        SD_BGMStop(SD_BGM_COMMAND_BASE);
        step = (s16)step;
        SD_EnqueueCdPlay(saved_command & SD_COMMAND_VALUE_MASK, step);
    } else {
        u32 masked_command = (u32)(saved_command & SD_COMMAND_VALUE_MASK);

        if (masked_command >= SD_BGM_COMMAND_BASE)
            command_id -= SD_BGM_COMMAND_BASE;
        step = (s16)step;
        SD_RequestBgmPlayback((s16)command_id, step);
    }
}

void SD_BGMStop(u32 command_id)
{
    SD_BGMStopWithStep(command_id & SD_COMMAND_VALUE_MASK, -32);
}

void SD_BGMStopWithStep(u16 id, s32 step)
{
    if ((id & 0x8000) != 0)
        SD_EnqueueCdStop();
    else
        SD_RequestBgmPlayback(-1, (s16)step);
}

void SD_BGMFade(s32 step, s32 target)
{
    SD_StartBgmFade(step, target);
}

void tent_SD_BGMFadeCopy(s32 value, s32 flag)
{
    tent_SD_StartBgmFadeCopy(value, flag);
}

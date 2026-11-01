#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
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
    mix_state->mix_scale = mix_state->cd_mix_default;
    mix_state->secondary_mix_scale = mix_state->secondary_mix_default;
    SD_SetCdVolume(0);
    sequence_state = g_SDValue;
    sequence_state->cd_fade_step = 0;
    if (sequence_state->sequence_open_result != -1) {
        if ((s16)SD_PollSequenceState() != 1) {
            SD_StopSequence(g_SDValue->sequence_open_result);
            g_SDValue->status_flags &= 0xFF7F;
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
    g_SDValue->voice_active_mask = 0;
    g_SDValue->voice_allocation_cursor = 0;
    for (voice_slot = 0; voice_slot < 4; voice_slot++) {
        g_SDValue->voice_priority[voice_slot] = 0;
        g_SDValue->voice_ids[voice_slot] = 0;
    }
    command_state = g_SDValue;
    command_state->active_command = 0;
    command_state->command_count = 0;
    g_SDValue->active_command_phase = 0;
    payload_state = g_SDValue;
    payload_state->active_cd_fade_wait = 0;
    status_state = g_SDValue;
    payload_state->active_command_value = 0;
    payload_state->active_command_arg0 = 0;
    payload_state->active_command_arg1 = 0;
    payload_state->active_command_arg2 = 0;
    status_state->status_flags = 0;
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

    if (state->output_type != value) {
        state->output_type = value;
        if (value == 0)
            SD_EnableSecondaryPanning();
        else
            SD_DisableSecondaryPanning();
        SD_SetCdVolume(g_SDValue->cd_volume);
    }
}

s32 SD_GetOutputType(void)
{
    return g_SDValue->output_type;
}

void tent_SD_SetDataSourceFlags(s32 value)
{
    g_SDValue->data_source_flags =
        (g_SDValue->data_source_flags & 0xF0) | value;
}

s32 SD_GetStatusFlags(void)
{
    return (s16)g_SDValue->status_flags;
}

void SD_VSync(void)
{
    if (gSD_pSecondaryState->vsync_update_mode)
        SD_UpdateSecondarySequenceIfEnabled();
    SD_UpdateRuntime();
    if (g_SDValue->status_flags & 8)
        SD_UpdateBankLoad();
}

void SD_EnqueueRamTransfer(s32 ram_destination, s32 sector_offset, s32 ram_size, s32 source_selector)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->status_flags |= 2;
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
    g_SDValue->status_flags |= 2;
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
    g_SDValue->status_flags |= 2;
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

    if ((g_SDValue->data_source_flags & 2) == 0)
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

    if ((g_SDValue->data_source_flags & 2) == 0)
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

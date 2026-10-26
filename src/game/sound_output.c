#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "sd_set_cd_volume.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_sequence_state.h"
#include "sound_output_state.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_selection.h"

#define SOUND_INIT_S16_VIEW
#include "sound_init.h"

/* Each block re-reads g_SDValue rather than caching it once: the driver block
   is reachable through the global, so every store through one pointer forces
   the next read. */
void SD_ResetRuntime(void)
{
    SDValue *p;
    SDValue *q;
    SDValue *r;
    SDValue *s;
    SDValue *t;
    SDValue *u;
    SDValue *w;
    s32 i;
    s32 value;

    p = g_SDValue;
    p->mix_scale = p->cd_mix_default;
    p->secondary_mix_scale = p->secondary_mix_default;
    SD_SetCdVolume(0);
    q = g_SDValue;
    q->cd_fade_step = 0;
    if (q->sequence_open_result != -1) {
        if ((s16)SD_PollSequenceState() != 1) {
            SD_StopSequence(g_SDValue->sequence_open_result);
            g_SDValue->status_flags &= 0xFF7F;
        }
        SD_CloseSequence(g_SDValue->sequence_open_result);
    }
    r = g_SDValue;
    value = r->music_vab_open_result;
    r->opened_sequence_command = -1;
    r->sequence_open_result = -1;
    if (value != -1) {
        SD_ResetVabTransferState(value);
    }
    s = g_SDValue;
    s->opened_music_package_id = -1;
    s->music_vab_open_result = -1;
    s->sequence_fade_step = 0;
    SpuSetKey(SPU_OFF, 0xFFFFFF);
    g_SDValue->voice_active_mask = 0;
    g_SDValue->voice_allocation_cursor = 0;
    for (i = 0; i < 4; i++) {
        g_SDValue->voice_priority[i] = 0;
        g_SDValue->voice_ids[i] = 0;
    }
    t = g_SDValue;
    t->active_command = 0;
    t->command_count = 0;
    g_SDValue->active_command_phase = 0;
    u = g_SDValue;
    u->active_cd_fade_wait = 0;
    w = g_SDValue;
    u->active_command_value = 0;
    u->active_command_arg0 = 0;
    u->active_command_arg1 = 0;
    u->active_command_arg2 = 0;
    w->status_flags = 0;
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

void SD_EnqueueRamTransfer(s32 first, s32 second, s32 third, s32 fourth)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->status_flags |= 2;
    command.command = 32;
    command.transfer_selector = fourth;
    command.transfer_sector_offset = second;
    command.transfer_ram_destination = first;
    command.transfer_ram_size = third;
    command.transfer_spu_destination = 0;
    command.transfer_spu_size = 0;
    SD_EnqueueCommand(&command);
}

void SD_EnqueueSpuTransfer(s32 first, s32 second, s32 third, s32 fourth)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->status_flags |= 2;
    command.command = 32;
    command.transfer_selector = fourth;
    command.transfer_sector_offset = second;
    command.transfer_ram_destination = 0;
    command.transfer_ram_size = 0;
    command.transfer_spu_destination = first;
    command.transfer_spu_size = third;
    SD_EnqueueCommand(&command);
}

void SD_EnqueueSpuRamTransfer(
    s32 a0,
    s32 a1,
    s32 a2,
    s32 a3,
    s32 a4,
    s32 a5
)
{
    SDCommand command;

    SD_StartCdFade(-32, 0, 1);
    g_SDValue->status_flags |= 2;
    command.command = 32;
    command.transfer_selector = a5;
    command.transfer_sector_offset = a2;
    command.transfer_ram_destination = a1;
    command.transfer_ram_size = a4;
    command.transfer_spu_destination = a0;
    command.transfer_spu_size = a3;
    SD_EnqueueCommand(&command);
}

void SD_PreloadBgmAndReplaceValueLink(u32 value)
{
    SD_BGMPreload(value >> 16);
    SD_ReplaceValueLink(value & SD_COMMAND_VALUE_MASK);
}

void SD_BGMPreload(s32 arg0)
{
    u16 saved = arg0;

    if ((g_SDValue->data_source_flags & 2) == 0)
        return;

    if (arg0 & 0x8000) {
        SD_EnqueueCdSeek(saved & SD_COMMAND_VALUE_MASK);
    } else {
        u32 masked = (u32)(saved & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE)
            arg0 -= SD_BGM_COMMAND_BASE;
        SD_RequestMusicPackageLoad((s16)arg0, 1);
    }
}

void SD_BGMPlayDefaultStep(u32 value)
{
    SD_BGMPlayWithStep(value & SD_COMMAND_VALUE_MASK, g_SDValue->field_164B);
}

void SD_BGMPlayWithStep(s32 arg0, s32 arg1)
{
    u16 saved = arg0;

    if ((g_SDValue->data_source_flags & 2) == 0)
        return;

    if (arg0 & 0x8000) {
        SD_BGMStop(SD_BGM_COMMAND_BASE);
        arg1 = (s16)arg1;
        SD_EnqueueCdPlay(saved & SD_COMMAND_VALUE_MASK, arg1);
    } else {
        u32 masked = (u32)(saved & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE)
            arg0 -= SD_BGM_COMMAND_BASE;
        arg1 = (s16)arg1;
        SD_RequestBgmPlayback((s16)arg0, arg1);
    }
}

void SD_BGMStop(u32 value)
{
    SD_BGMStopWithStep(value & SD_COMMAND_VALUE_MASK, -32);
}

void SD_BGMStopWithStep(u16 id, s32 step)
{
    if ((id & 0x8000) != 0)
        SD_EnqueueCdStop();
    else
        SD_RequestBgmPlayback_s16(-1, step);
}

void SD_BGMFade(s32 step, s32 target)
{
    SD_StartBgmFade(step, target);
}

void tent_SD_BGMFadeCopy(s32 value, s32 flag)
{
    tent_SD_StartBgmFadeCopy(value, flag);
}

#include "../types.h"
#include "file_transfer.h"
#include "main_frame.h"
#include "file_constants.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_output.h"
#include "sound_state_control.h"
#include "sound_bgm_commands.h"

void Sound_InitFrontend(void)
{
    register volatile s32 *lbas = gFile_anLba;

    gSD_bOutputType = -1;
    SD_InitDataSourceFlags(
        lbas[FILE_LBA_INDEX_SD_SE],
        lbas[FILE_LBA_INDEX_SD_BGM],
        lbas[FILE_LBA_INDEX_MASTER_XA]);
    while (SD_GetStatusFlags() & 8) {
        Main_AdvanceFrame();
    }
}

void SD_SEPlayFull(u32 value)
{
    SD_SEPlay(value & SD_COMMAND_VALUE_MASK, SD_SE_VOLUME_MAX, 0);
}

void SD_BGMPlay(u32 value)
{
    u32 command = value | SD_BGM_COMMAND_BASE;

    SD_BGMPlayDefaultStep(command & SD_COMMAND_VALUE_MASK);
    gSD_dwCurrentBgmCommand = command;
}

void SD_BGMFadeOut(void)
{
    SD_BGMFade(-8, 0);
}

void SD_BGMFadeOutWithStep(s32 value)
{
    if (value > 0)
        value = -value;
    SD_BGMFade((s16)value, 0);
}

void SD_CdSoundPlay(u32 value)
{
    SD_SEPlay((value & SD_COMMAND_VALUE_MASK) | 0x8000, SD_SE_VOLUME_MAX, 0);
}

void SD_CdSoundSeek(u32 value)
{
    SD_EnqueueCdSeek((value & SD_COMMAND_VALUE_MASK) | 0x8000);
}

void SD_CdBgmPlay(u32 value)
{
    SD_BGMPlayDefaultStep((value & SD_COMMAND_VALUE_MASK) | 0xA000);
}

void SD_StopAll(void)
{
    SD_BGMStop(0);
    SD_BGMStop(0x8000);
    SD_KeyOffVoiceSlots();
}

#define D_8009B0F0_IN_DATA
#define D_8009B120_IN_DATA
#include "../types.h"
#include "../ygo_types.h"

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
    g_SDValue->data_source_flags = 3;
    if (master_xa_lba_byte != 0) {
        g_SDValue->data_source_flags |= 0xF0;
    }
    g_SDValue->cd_mix_default = 0xFF;
    g_SDValue->secondary_mix_default = 0xD2;
    g_SDValue->default_bgm_step = 0xFF;
    mix_state = g_SDValue;
    cd_default = mix_state->cd_mix_default;
    secondary_default = mix_state->secondary_mix_default;
    mix_state->cd_fade_target = 0xFF;
    bgm_target_state = g_SDValue;
    mix_state->mix_scale = cd_default;
    mix_state->secondary_mix_scale = secondary_default;
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
    status_state->status_flags = 0;
}

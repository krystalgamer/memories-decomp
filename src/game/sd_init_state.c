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
 * word at a time, the two command callbacks are installed, three tables are
 * copied in from ROM, the mode byte is seeded (0xF0 extra bits when arg0 is
 * set), the mix defaults are seeded from the values just written at +0x1649
 * and +0x164A, and the subsystem initialisers run before the secondary state's
 * tick callback is armed.
 *
 * The image bounds are plain integer constants rather than `&loaded_image_end`
 * (which the linker also puts at 0x801E0000): the retail code derives
 * 0x801EA7FF from the same `lui` as the base with an `ori`, which only happens
 * when both are integers GCC can share a high half between. Through the symbol
 * they become separate relocations and the `ori` becomes a second `lui`. */
void SD_InitState(u8 arg0)
{
    u32 *p;
    SDValue *st;
    SDValue *st2;
    SDSecondaryState *sec;
    SDValue *q;
    s32 x;
    s32 y;

    g_SDValue = (SDValue *)0x801E0000;
    gSD_pFileRequest = (FileRequestSlot *)0x801E1650;
    p = (u32 *)0x801E0000;
    do {
        *p = 0;
        p++;
    } while (p <= (u32 *)0x801EA7FF);
    gFile_RangeReadCallback = SD_FadeInStreamAudio;
    gFile_RangePauseCallback = SD_FadeOutStreamAudio;
    *SD_INIT_BLOCK_11_VIEW(g_SDValue->field_1619) =
        *SD_INIT_BLOCK_11_VIEW(gSD_szBgmFileName);
    *SD_INIT_BLOCK_10_VIEW(g_SDValue->field_1629) =
        *SD_INIT_BLOCK_10_VIEW(gSD_szSeFileName);
    *SD_INIT_BLOCK_10_VIEW(g_SDValue->field_1639) =
        *SD_INIT_BLOCK_10_VIEW(gSD_szMasterXaFileName);
    g_SDValue->flags_004A = 3;
    if (arg0 != 0) {
        g_SDValue->flags_004A |= 0xF0;
    }
    g_SDValue->cd_mix_default = 0xFF;
    g_SDValue->secondary_mix_default = 0xD2;
    g_SDValue->field_164B = 0xFF;
    st = g_SDValue;
    x = st->cd_mix_default;
    y = st->secondary_mix_default;
    st->cd_fade_target = 0xFF;
    q = g_SDValue;
    st->mix_scale = x;
    st->secondary_mix_scale = y;
    q->bgm_fade_target = 0xFF;
    SD_InitSecondaryWorkArea((s32 *)0x801E1670);
    SD_InitSpuCommonVolume();
    SD_InitBufferState();
    SD_InitSecondaryRuntime();
    SD_SetOutputType(0);
    gSD_pSecondaryState->field_0509 = 0;
    sec = gSD_pSecondaryState;
    st2 = g_SDValue;
    sec->field_050C = SD_UpdateVoiceSlots;
    st2->flags_0040 = 0;
}

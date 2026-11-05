#include "../types.h"
#include "sound.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"

void SD_InitVoiceState(void) {
    SDValue *voice_state;
    SDValue *lookup_state;
    SDValue *note_map_state;
    s32 init_index;
    s32 lookup_entry_index;
    s32 lookup_byte_offset;

    g_SDValue->s_bSD_VOICE_ACTIVE_MASK = 0;
    g_SDValue->s_bSD_VOICE_CURSOR = 0;

    for (init_index = 0; init_index < SD_VOICE_SLOT_COUNT; init_index++) {
        g_SDValue->s_abSD_VOICE_PRIORITY[init_index] = 0;
        g_SDValue->s_abSD_VOICE_FLAG[init_index] = 0;
        g_SDValue->s_awSD_VOICE_SE_ID[init_index] = 0;
        g_SDValue->s_awSD_VOICE_VOLUME_L[init_index] = 0;
        g_SDValue->s_awSD_VOICE_VOLUME_R[init_index] = 0;
        g_SDValue->s_abSD_VOICE_FADE_STEP[init_index] = 0;
        g_SDValue->s_awSD_VOICE_TIMER[init_index] = 0;
    }

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    /* The constant half of the staged voice attribute: unity volumex, unity
       pitch, middle-C sample note, linear attack and sustain with exponential
       release, and an all-bits attribute mask. */
    voice_state = g_SDValue;
    voice_state->s_stSD_VOICE_ATTR.volumex.left = 0x3FFF;
    voice_state->s_stSD_VOICE_ATTR.volumex.right = 0x3FFF;
    voice_state->s_stSD_VOICE_ATTR.pitch = 0x1000;
    voice_state->s_stSD_VOICE_ATTR.sample_note = 0x3C00;
    voice_state->s_stSD_VOICE_ATTR.a_mode = 1;
    voice_state->s_stSD_VOICE_ATTR.s_mode = 1;
    voice_state->s_stSD_VOICE_ATTR.r_mode = 3;
    voice_state->s_pstSD_SEGROUP_TABLE = (SDValueLink *)0x801E2800;
    voice_state->s_stSD_VOICE_ATTR.mask = 0xFFFF;
    voice_state->s_stSD_VOICE_ATTR.volmode.left = 0;
    voice_state->s_stSD_VOICE_ATTR.volmode.right = 0;
    voice_state->s_stSD_VOICE_ATTR.ar = 0;
    voice_state->s_stSD_VOICE_ATTR.dr = 0;
    voice_state->s_stSD_VOICE_ATTR.sr = 0;
    voice_state->s_stSD_VOICE_ATTR.rr = 0;
    voice_state->s_stSD_VOICE_ATTR.sl = 0;
    voice_state->s_pwSD_SE_INDEX = (u16 *)0x801E4000;
    voice_state->s_dwSD_SEGROUP_SPU_ADDR = 0x1010;
    voice_state->s_wSD_NEXT_SE_INDEX = 0;
    voice_state->s_wSD_SEGROUP_REQUEST = 0xFFFF;
    voice_state->s_pstSD_SE_TABLE =
        (SDNote *)(
            (((u32)&((u16 *)0)[voice_state->s_stGlobalHdrSE.s_wSD_NUM_SE] + 0xF) & 0xFFF0) +
            0x801E4000);

    init_index = 0;
    if (voice_state->s_stGlobalHdrSE.s_wSD_NUM_SE != 0) {
        note_map_state = voice_state;
        do {
            note_map_state->s_pwSD_SE_INDEX[init_index] = SD_PENDING_ENTRY_NONE;
            init_index++;
        } while (init_index < note_map_state->s_stGlobalHdrSE.s_wSD_NUM_SE);
    }

    init_index = 0;
    lookup_state = g_SDValue;
    for (; init_index < SD_VOICE_LOOKUP_BANK_COUNT; init_index++) {
        lookup_entry_index = 0;
        lookup_byte_offset = init_index << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        for (; lookup_entry_index < SD_VOICE_LOOKUP_BANK_ENTRY_COUNT; lookup_entry_index++) {
            /* The bank shift is already folded into lookup_byte_offset, and retail adds the
               record base to it in that order, so this keeps the byte form
               rather than becoming lookup_state->s_aawSD_INDIRECT_SE_ID[init_index][lookup_entry_index]. */
            *(u16 *)((u8 *)lookup_state + lookup_byte_offset + SD_VOICE_LOOKUP_BYTE_OFFSET) =
                SD_PENDING_ENTRY_NONE;
            lookup_byte_offset += 2;
        }
    }
}

#include "../types.h"
#define G_SDVALUE_IN_DATA
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

    g_SDValue->voice_active_mask = 0;
    g_SDValue->voice_allocation_cursor = 0;

    for (init_index = 0; init_index < SD_VOICE_SLOT_COUNT; init_index++) {
        g_SDValue->voice_priority[init_index] = 0;
        g_SDValue->voice_flags[init_index] = 0;
        g_SDValue->voice_ids[init_index] = 0;
        g_SDValue->voice_volume_left[init_index] = 0;
        g_SDValue->voice_volume_right[init_index] = 0;
        g_SDValue->voice_step[init_index] = 0;
        g_SDValue->voice_timer[init_index] = 0;
    }

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    /* The constant half of the staged voice attribute: unity volumex, unity
       pitch, middle-C sample note, linear attack and sustain with exponential
       release, and an all-bits attribute mask. */
    voice_state = g_SDValue;
    voice_state->voice_attr.volumex.left = 0x3FFF;
    voice_state->voice_attr.volumex.right = 0x3FFF;
    voice_state->voice_attr.pitch = 0x1000;
    voice_state->voice_attr.sample_note = 0x3C00;
    voice_state->voice_attr.a_mode = 1;
    voice_state->voice_attr.s_mode = 1;
    voice_state->voice_attr.r_mode = 3;
    voice_state->value_links = (SDValueLink *)0x801E2800;
    voice_state->voice_attr.mask = 0xFFFF;
    voice_state->voice_attr.volmode.left = 0;
    voice_state->voice_attr.volmode.right = 0;
    voice_state->voice_attr.ar = 0;
    voice_state->voice_attr.dr = 0;
    voice_state->voice_attr.sr = 0;
    voice_state->voice_attr.rr = 0;
    voice_state->voice_attr.sl = 0;
    voice_state->note_index_by_id = (u16 *)0x801E4000;
    voice_state->next_link_spu_address = 0x1010;
    voice_state->next_note_index = 0;
    voice_state->value_link_request_index = 0xFFFF;
    voice_state->note_table =
        (SDNote *)(
            (((u32)&((u16 *)0)[voice_state->note_id_count] + 0xF) & 0xFFF0) +
            0x801E4000);

    init_index = 0;
    if (voice_state->note_id_count != 0) {
        note_map_state = voice_state;
        do {
            note_map_state->note_index_by_id[init_index] = SD_PENDING_ENTRY_NONE;
            init_index++;
        } while (init_index < note_map_state->note_id_count);
    }

    init_index = 0;
    lookup_state = g_SDValue;
    for (; init_index < SD_VOICE_LOOKUP_BANK_COUNT; init_index++) {
        lookup_entry_index = 0;
        lookup_byte_offset = init_index << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        for (; lookup_entry_index < SD_VOICE_LOOKUP_BANK_ENTRY_COUNT; lookup_entry_index++) {
            /* The bank shift is already folded into lookup_byte_offset, and retail adds the
               record base to it in that order, so this keeps the byte form
               rather than becoming lookup_state->indirect_sound_ids[init_index][lookup_entry_index]. */
            *(u16 *)((u8 *)lookup_state + lookup_byte_offset + SD_VOICE_LOOKUP_BYTE_OFFSET) =
                SD_PENDING_ENTRY_NONE;
            lookup_byte_offset += 2;
        }
    }
}

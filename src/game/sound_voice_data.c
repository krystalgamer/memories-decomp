#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_mix.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "sound_voice_data.h"

void SD_CopySeqBlock(u32 *dst, u32 *src)
{
    u32 i;

    for (i = 0; i < SD_PENDING_INPUT_COPY_GROUP_COUNT; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}

void SD_LoadSequenceBankPair(s32 side, u32 *src)
{
    SDSeqBlock *blk;
    SDSeqBlock *other;
    SDSeqBlock *cur;
    SDNote *e;
    u32 addr_side;
    u32 addr_other;
    s32 rev;
    s32 i;
    s32 j;

    if ((u32)side >= 2) {
        return;
    }

    rev = 1 - side;
    addr_side = 0xD810 + (side % 2) * 0x19000;
    addr_other = 0xD810 + (rev % 2) * 0x19000;
    blk = (SDSeqBlock *)(0x801E7800 + (side << SD_PENDING_INPUT_BLOCK_SHIFT));
    other = (SDSeqBlock *)(0x801E7800 + (rev << SD_PENDING_INPUT_BLOCK_SHIFT));
    SD_CopySeqBlock((u32 *)blk, src);
    SD_ResetPendingEntries();

    for (i = 0; i < 2; i++) {
        cur = other;
        if (i != 0) {
            cur = blk;
        }
        for (j = 0; j < cur->count; j++) {
            u16 key = cur->keys[j];
            if (key != SD_PENDING_ENTRY_NONE) {
                u16 n = g_SDValue->next_note_index;
                u16 v;

                g_SDValue->note_index_by_id[key] = n;
                g_SDValue->note_table[n] = cur->data[j];
                e = (SDNote *)(
                    (u32)&((SDNote *)0)[n] +
                    (u32)g_SDValue->note_table
                );
                v = e->spu_address_div16;
                e->spu_address_div16 =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->next_note_index = g_SDValue->next_note_index + 1;
            }
            if (i != 0) {
                g_SDValue->field_044C[side][j] = key;
            }
        }
    }
}

#include "sound_init.h"

/* Sound driver initialisation after the SPU is up: enables reverb in studio-A
 * mode at full depth, clears and seeds the driver's level and track fields,
 * points the music track at its buffer at 0x801EA800 with an empty header,
 * and finishes through SD_SetSequenceTickMode, SD_SetSecondaryObjectCount and
 * SD_EnableSecondarySequenceUpdates.
 * sd_init_state.c calls it once. */
void SD_InitSecondaryRuntime(void)
{
    SpuReverbAttr packet;
    /* g_SDValue is reloaded three times, as retail does. The first load gets
       its own pointer so each lives only as long as its stores. */
    SDValue *a;
    SDValue *b;
    SDValue *c;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    packet.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    packet.mode = SPU_REV_MODE_STUDIO_A;
    packet.depth.left = 0x7FFF;
    packet.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&packet);
    b = g_SDValue;
    b->sequence_fade_level = 0;
    b->sequence_fade_step = 0;
    b->sequence_fade_target = 0;
    a = g_SDValue;
    a->bgm_fade_level = 0xFF;
    a->bgm_fade_target = 0xFF;
    c = g_SDValue;
    a->bgm_fade_step = 0;
    c->opened_music_package_id = -1;
    c->music_vab_open_result = -1;
    c->opened_sequence_command = -1;
    c->sequence_open_result = -1;
    c->music_track = (u16 *)0x801EA800;
    c->music_table = (SDMusicTableEntry *)0x801E2000;
    c->music_track[0] = 0xFFFF;
    c->music_track[1] = 0;
    *(s32 *)&c->music_track[2] = 0;
    *(s32 *)&c->music_track[4] = 0;
    *(s32 *)&c->music_track[6] = 0x40000;
    SD_SetSequenceTickMode(2);
    SD_SetSecondaryObjectCount(0x14);
    SD_EnableSecondarySequenceUpdates();
}

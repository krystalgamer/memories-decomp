#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_driver.h"
#include "sound_output_state.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"
#include "sound_effect.h"

void SD_InitVoiceState(void) {
    SDValue *voice_state;
    SDValue *lookup_state;
    SDValue *note_map_state;
    s32 init_index;
    s32 lookup_entry_index;

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
        (SDNote *)(((voice_state->s_stGlobalHdrSE.s_wSD_NUM_SE * sizeof(u16) + 0xF) & 0xFFF0) +
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
        for (lookup_entry_index = 0; lookup_entry_index < SD_VOICE_LOOKUP_BANK_ENTRY_COUNT; lookup_entry_index++) {
            lookup_state->s_aawSD_INDIRECT_SE_ID[init_index][lookup_entry_index] = SD_PENDING_ENTRY_NONE;
        }
    }
}

/* Same *g_SDValue struct as other siblings. Fills the u16 array at f43C
   from index f4 up to (but not including) f0 with 0xFFFF, always sets
   f442=0xFFFF regardless of whether the fill ran, then propagates f4 into
   f440 and recomputes f438 from f448's own f4 field plus 0x1010. The
   final field group is read via a fresh g_SDValue dereference rather
   than the cached local (see project_d8009b45c_reload_pointer_trick). */
void SD_ResetPendingEntries(void) {
    SDValue *state = g_SDValue;
    s32 i = state->s_stGlobalHdrSE.s_wSD_SE_DYNAMIC_START;

    state->s_wSD_SEGROUP_REQUEST = SD_VALUE_LINK_INDEX_NONE;
    if (i < state->s_stGlobalHdrSE.s_wSD_NUM_SE) {
        do {
            state->s_pwSD_SE_INDEX[i] = SD_PENDING_ENTRY_NONE;
            i++;
        } while (i < state->s_stGlobalHdrSE.s_wSD_NUM_SE);
    }
    g_SDValue->s_wSD_NEXT_SE_INDEX = g_SDValue->s_stGlobalHdrSE.s_wSD_SE_DYNAMIC_START;
    g_SDValue->s_dwSD_SEGROUP_SPU_ADDR =
        g_SDValue->s_pstSD_SEGROUP_TABLE->spu_size + 0x1010;
}

/* Imports one pending-input block: each live key is bound to the next free
   note index at f440, its note record is copied into f444's table and the
   copy's +6 SPU address (in 16-byte units) is rebased by spu_addr / 16.
   SD_StepActiveCommand calls it for command 0x51 with the SPU destination
   and RAM staging block that SD_QueueValueLinkTransfer queued. */
void SD_RegisterSeqBlockNotes(SDSeqBlock *input, u32 spu_addr) {
    s32 i;

    for (i = 0; i < input->count; i++) {
        if (input->keys[i] != SD_PENDING_ENTRY_NONE) {
            g_SDValue->s_pwSD_SE_INDEX[input->keys[i]] = g_SDValue->s_wSD_NEXT_SE_INDEX;
            g_SDValue->s_pstSD_SE_TABLE[g_SDValue->s_wSD_NEXT_SE_INDEX] = input->data[i];
            g_SDValue->s_pstSD_SE_TABLE[g_SDValue->s_wSD_NEXT_SE_INDEX].spu_address_div16 +=
                spu_addr >> 4;
            g_SDValue->s_wSD_NEXT_SE_INDEX++;
        }
    }
}

#define SD_VALUE_LINK_BLOCK_SIZE 0x800

void SD_QueueValueLinkTransfer(s32 link_index)
{
    SDCommand registration_command;
    SDValue *load_state;
    SDValue *registration_state;
    SDValue *cursor_state;
    s32 data_start_sector;
    s32 off;
    SDValueLink *entry;

    load_state = g_SDValue;
    data_start_sector = (s32)(load_state->s_stGlobalHdrSE.s_wSD_NUM_SEGROUP << 3);
    data_start_sector = (data_start_sector + SD_VALUE_LINK_BLOCK_SIZE - 1) /
           SD_VALUE_LINK_BLOCK_SIZE;
    data_start_sector = data_start_sector + 1;
    off = (link_index & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)load_state->s_pstSD_SEGROUP_TABLE);
    SD_EnqueueSpuRamTransfer(load_state->s_dwSD_SEGROUP_SPU_ADDR, 0x801E6800,
                  data_start_sector + entry->sector_offset, entry->spu_size,
                  SD_VALUE_LINK_BLOCK_SIZE, 0x10);
    registration_state = g_SDValue;
    registration_command.command = 0x51;
    registration_command.args.seq_block.spu_address = registration_state->s_dwSD_SEGROUP_SPU_ADDR;
    registration_command.args.seq_block.block_buffer = 0x801E6800;
    SD_EnqueueCommand(&registration_command);
    cursor_state = g_SDValue;
    off += (s32)cursor_state->s_pstSD_SEGROUP_TABLE;
    entry = (SDValueLink *)off;
    cursor_state->s_dwSD_SEGROUP_SPU_ADDR = cursor_state->s_dwSD_SEGROUP_SPU_ADDR + entry->spu_size;
}

/*
 * SD_ApplyVoiceSlotVolume: voice-slot attribute initialization order
 *
 * The 136-byte routine at `0x80047864` matches all 34 instructions with the
 * existing uniform `gcc_2_8_1_g0` profile. It uses the incoming index directly
 * and the shared `SDValue` voice-volume and gain arrays, replacing five
 * register bindings, byte-cursor accesses, and an explicit assembly move. Its
 * `void(s32)` contract and `SpuSetVoiceAttr` call are unchanged.
 *
 * Initializing the attribute-mask field at `+0x3C8` before the voice-mask field
 * at `+0x3C4` in C gives GCC the required register allocation. GCC schedules
 * the resulting machine stores in retail order, with the voice-mask store
 * first. Reversing those source assignments changes five instruction words. A
 * split profile alone leaves two wrong high-address register words, so the
 * accepted source does not add a compiler profile or change the global
 * declaration.
 *
 * The conditional voice-mask shift, both unsigned fixed-point volume products,
 * their eight-bit shifts and halfword stores, and the final SDK attribute
 * publication remain intact.
 */
void SD_ApplyVoiceSlotVolume(s32 index)
{
    u32 mask = SD_VOICE_SLOT_MASK_BASE;
    SDValue *state;
    u32 product;

    if (index != 0) {
        mask <<= index;
    }
    state = g_SDValue;
    state->s_stSD_VOICE_VOLUME_ATTR.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
    state->s_stSD_VOICE_VOLUME_ATTR.voice = mask;
    {
        u32 first = state->s_awSD_VOICE_VOLUME_L[index];
        product = first * (u32)state->s_abSD_VOICE_FADE_LEVEL[index];
    }
    state->s_stSD_VOICE_VOLUME_ATTR.volume.left = product >> 8;
    product = state->s_awSD_VOICE_VOLUME_R[index] * (u32)state->s_abSD_VOICE_FADE_LEVEL[index];
    state->s_stSD_VOICE_VOLUME_ATTR.volume.right = product >> 8;
    SpuSetVoiceAttr(&state->s_stSD_VOICE_VOLUME_ATTR);
}

void SD_UpdateVoiceSlots(void)
{
    s32 mask;
    s32 bit2;
    s32 accum;
    s32 i;
    s32 key_status;
    u16 timer;

    mask = SD_VOICE_SLOT_MASK_BASE;
    bit2 = 1;
    accum = 0;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->s_abSD_VOICE_FADE_STEP[i] != 0) {
            if (g_SDValue->s_abSD_VOICE_FADE_STEP[i] >= g_SDValue->s_abSD_VOICE_FADE_LEVEL[i]) {
                g_SDValue->s_abSD_VOICE_FADE_LEVEL[i] = 0;
            } else {
                g_SDValue->s_abSD_VOICE_FADE_LEVEL[i] =
                    g_SDValue->s_abSD_VOICE_FADE_LEVEL[i] - g_SDValue->s_abSD_VOICE_FADE_STEP[i];
            }
            if (g_SDValue->s_abSD_VOICE_FADE_LEVEL[i] != 0) {
                SD_ApplyVoiceSlotVolume(i);
            } else {
                g_SDValue->s_bSD_VOICE_ACTIVE_MASK &= ~bit2;
                g_SDValue->s_abSD_VOICE_FADE_STEP[i] = 0;
                SpuSetKey(SPU_OFF, mask);
                accum |= mask;
            }
        }
        timer = g_SDValue->s_awSD_VOICE_TIMER[i];
        if (timer != 0) {
            timer -= 1;
            g_SDValue->s_awSD_VOICE_TIMER[i] = timer;
            if (timer == 0) {
                key_status = SpuGetKeyStatus(mask);
                if (key_status != SPU_OFF) {
                    SpuSetKey(SPU_OFF, mask);
                    accum |= mask;
                }
            }
        }
        key_status = SpuGetKeyStatus(mask);
        if (key_status == SPU_ON_ENV_OFF && !(accum & mask)) {
            SpuSetKey(SPU_OFF, mask);
            accum |= mask;
        }
        bit2 <<= 1;
        mask <<= 1;
    }
    if (accum != 0)
        SpuSetKey(SPU_OFF, accum);
}

void SD_KeyOffDynamicVoices(void)
{
    s32 i;
    u32 mask = SD_VOICE_SLOT_MASK_BASE;
    u32 result = 0;
    u16 threshold = g_SDValue->s_stGlobalHdrSE.s_wSD_SE_DYNAMIC_START;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->s_awSD_VOICE_SE_ID[i] >= threshold)
            result |= mask;
        mask <<= 1;
    }
    SpuSetKey(SPU_OFF, result);
}

s32 SD_ReplaceValueLink(s32 value)
{
    u16 index = value;
    SDValue *state = g_SDValue;

    if (state->s_pstSD_SEGROUP_TABLE[index].spu_size == 0)
        return 1;
    if (state->s_wSD_SEGROUP_REQUEST == index)
        return 1;
    SD_KeyOffDynamicVoices();
    SD_ResetPendingEntries();
    {
        register SDValue *final = g_SDValue;
        register s32 call_value = index;
        final->s_wSD_SEGROUP_REQUEST = value;
        final->s_wSD_STATUS |= 2;
        SD_QueueValueLinkTransfer(call_value);
    }
    return 1;
}

s32 SD_RequestValueLink(u16 value)
{
    SDValue *state = g_SDValue;

    if (state->s_wSD_SEGROUP_REQUEST == value)
        return 1;
    state->s_wSD_SEGROUP_REQUEST = value;
    state->s_wSD_STATUS |= 2;
    SD_QueueValueLinkTransfer(value);
    return 1;
}

void SD_QueueValueLinkList(u16 *items, s32 count)
{
    s32 i;

    SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
    SD_ResetPendingEntries();
    g_SDValue->s_wSD_STATUS |= 2;
    for (i = 0; i < count; i++) {
        if (items[i] != 0xFFFF)
            SD_QueueValueLinkTransfer(items[i]);
    }
}

s32 SD_FilterCdFlaggedId(s32 value)
{
    u16 result = value;

    value &= 0x8000;
    if (value)
        return result;
    return 0xFFFF;
}

void SD_KeyOffVoice(s32 voice)
{
    s32 count;

    for (count = 0; count < SD_KEY_OFF_RETRY_LIMIT; count++) {
        SpuSetKey(SPU_OFF, voice);
        if (SpuGetKeyStatus(voice) == SPU_OFF)
            break;
    }
}

void SD_KeyOffVoiceById(s32 value)
{
    s32 normalized = SD_FilterCdFlaggedId((u16)value);
    SDValue *state = g_SDValue;
    s32 tries = 0;
    s32 slot = state->s_bSD_VOICE_CURSOR;
    s32 bit = 1 << (u8)slot;
    s32 command = 1 << ((u8)slot + SD_VOICE_SLOT_KEY_SHIFT);

    do {
        state = g_SDValue;
        if (state->s_bSD_VOICE_ACTIVE_MASK & bit) {
            if (state->s_awSD_VOICE_SE_ID[(u8)slot] == (u16)normalized) {
                SD_KeyOffVoice(command);
                g_SDValue->s_bSD_VOICE_CURSOR = slot;
                g_SDValue->s_bSD_VOICE_ACTIVE_MASK &= ~bit;
                break;
            }
        }
        bit <<= 1;
        command <<= 1;
        if (bit & (1 << SD_VOICE_SLOT_COUNT))
            bit = 1;
        slot++;
        tries++;
        slot &= SD_VOICE_SLOT_COUNT - 1;
    } while (tries < SD_VOICE_SLOT_COUNT);
}

void SD_SEStop(s32 id)
{
    u16 key = id;
    s32 value = id;
    s32 bits;
    s32 bit;
    s32 mask;
    s32 i;

    if ((value & 0x8000) != 0) {
        SD_DropQueuedCdPlayCommands();
        SD_EnqueueCdStop();
        return;
    }
    if ((value & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        u8 *table = (u8 *)g_SDValue->s_aawSD_INDIRECT_SE_ID[0];
        s32 offset = (value & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        s32 bank = 0;

        if ((value & SD_VOICE_LOOKUP_BANK_FLAG) != 0) {
            bank = 1;
        }
        bank *= SD_VOICE_LOOKUP_BANK_BYTE_STRIDE;
        offset += bank;
        key = *(u16 *)(table + offset);
        if (key == SD_PENDING_ENTRY_NONE) {
            return;
        }
    }
    mask = 1;
    bits = 0;
    bit = SD_VOICE_SLOT_MASK_BASE;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->s_awSD_VOICE_SE_ID[i] == key) {
            bits |= bit;
            g_SDValue->s_bSD_VOICE_ACTIVE_MASK &= ~mask;
            SD_KeyOffVoice(bit);
        }
        mask <<= 1;
        bit <<= 1;
    }
    if (bits != 0) {
        SpuSetKey(0, bits);
    }
}

void SD_KeyOffVoiceSlots(void)
{
    s32 count = 0;
    s32 total;

    do {
        SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
        SpuGetAllKeysStatus((char *)g_SDValue->s_abSD_VOICE_KEY_STATUS);
        total = g_SDValue->s_abSD_VOICE_KEY_STATUS[23] + g_SDValue->s_abSD_VOICE_KEY_STATUS[21] +
                g_SDValue->s_abSD_VOICE_KEY_STATUS[22] + g_SDValue->s_abSD_VOICE_KEY_STATUS[23];
        count++;
    } while (total != 0 && count < 24);
}

s32 SD_KeyOffVoiceGroup(u8 group)
{
    s32 i;
    s32 result = 0;
    s32 mask = 1;
    SDValue *state = g_SDValue;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        if ((state->s_abSD_VOICE_FLAG[i] & 0xF) == group)
            result |= mask;
    }
    if (result != 0)
        SpuSetKey(SPU_OFF, result << SD_VOICE_SLOT_KEY_SHIFT);
    return result;
}

/* The sound-effect voice slots: the active-voice count for one sound id
   (0x80047FAC) and the voice start that plays one (0x8004803C). The two are
   contiguous -- 0x80047FAC is 0x90 bytes and runs straight into 0x8004803C --
   and they follow the selection and key-off functions above and precede SD_AllocateSoundEffectVoice. They are
   the reader and the writer of the same four voice slots: the count walks
   g_SDValue->s_awSD_VOICE_SE_ID and the start assigns it, along with the flags,
   volumes and timer beside it. */

/* Counts how many of the four voice slots are playing this sound id: an id
   match in s_awSD_VOICE_SE_ID paired with a non-zero SPU envelope, which is what
   distinguishes a slot still sounding from one that has already decayed. */
s16 SD_CountActiveSoundEffectVoices(s32 sound_id) {
    u16 active_count = 0;
    s32 slot;
    u16 match_id = sound_id & 0xFFFF;

    for (slot = 0; slot < SD_VOICE_SLOT_COUNT; slot++) {
        s16 envelope;

        SpuGetVoiceEnvelope(slot + SD_VOICE_SLOT_FIRST_VOICE, &envelope);

        if (g_SDValue->s_awSD_VOICE_SE_ID[slot] == match_id) {
            if (envelope != 0) {
                active_count++;
            }
        }
    }

    return (s16)active_count;
}

/* Starts a sound-effect voice: looks the sound id up in the voice index table,
   derives the left and right volumes from the note table entry and the pan,
   keys the voice on and records the id, volumes and timer in the voice slots. */
void SD_StartSoundEffectVoice(u16 id, u8 voice, s32 pitch_add, u8 volume, s16 pan, u8 flags, u8 value) {
    u16 idx;

    idx = g_SDValue->s_pwSD_SE_INDEX[id];
    if (idx != 0xFFFF) {
        g_SDValue->s_stSD_VOICE_ATTR.volume.left = ((g_SDValue->s_pstSD_SE_TABLE[idx].volume * volume) >> 1) | 0xFF;
        g_SDValue->s_stSD_VOICE_ATTR.volume.right = ((g_SDValue->s_pstSD_SE_TABLE[idx].volume * volume) >> 1) | 0xFF;
        if (pan != 0) {
            if ((u16)(pan - 1) < 0x80) {
                g_SDValue->s_stSD_VOICE_ATTR.volume.left = g_SDValue->s_stSD_VOICE_ATTR.volume.left * (0x80 - pan) / 128;
            }
            if ((u16)(pan + 0x80) < 0x80) {
                g_SDValue->s_stSD_VOICE_ATTR.volume.right = g_SDValue->s_stSD_VOICE_ATTR.volume.right * (pan + 0x80) / 128;
            }
        }
        g_SDValue->s_stSD_VOICE_ATTR.note = g_SDValue->s_pstSD_SE_TABLE[idx].pitch + pitch_add;
        g_SDValue->s_stSD_VOICE_ATTR.voice = 1 << (voice + SD_VOICE_SLOT_KEY_SHIFT);
        g_SDValue->s_stSD_VOICE_ATTR.addr = g_SDValue->s_pstSD_SE_TABLE[idx].spu_address_div16 << 4;
        SpuSetKey(SPU_OFF, g_SDValue->s_stSD_VOICE_ATTR.voice);
        g_SDValue->s_bSD_VOICE_ACTIVE_MASK |= 1 << voice;
        g_SDValue->s_abSD_VOICE_PRIORITY[voice] = value;
        g_SDValue->s_abSD_VOICE_FLAG[voice] = flags;
        g_SDValue->s_awSD_VOICE_SE_ID[voice] = id;
        g_SDValue->s_awSD_VOICE_VOLUME_L[voice] = g_SDValue->s_stSD_VOICE_ATTR.volume.left;
        g_SDValue->s_awSD_VOICE_VOLUME_R[voice] = g_SDValue->s_stSD_VOICE_ATTR.volume.right;
        g_SDValue->s_abSD_VOICE_FADE_LEVEL[voice] = 0xFF;
        g_SDValue->s_abSD_VOICE_FADE_STEP[voice] = 0;
        g_SDValue->s_awSD_VOICE_TIMER[voice] = g_SDValue->s_pstSD_SE_TABLE[idx].timer << 2;
        SpuSetKeyOnWithAttr(&g_SDValue->s_stSD_VOICE_ATTR);
    }
}

void SD_AllocateSoundEffectVoice(s32 id, s16 pitch, u8 volume, s16 pan, u32 mode, u8 value)
{
    u16 sound_id = id;
    u8 slot;
    u8 count;
    u8 found;
    s32 index;
    s32 mask;
    s32 observed;
    u8 occurrence;
    s16 envelope;
    s16 mode_snapshot;

    mode_snapshot = mode;
    if (g_SDValue->s_pwSD_SE_INDEX[sound_id] == 0xFFFF) {
        return;
    }
    if ((id & 0x8000) != 0) {
        SD_EnqueueCdSePlay(sound_id, 0);
        return;
    }

    found = 0;
    if ((mode & 0xF) != 0) {
        mask = SD_KeyOffVoiceGroup(mode & 0xF);
        if (mask != 0) {
            slot = 0;
            do {
                if (mask & 1) {
                    g_SDValue->s_bSD_VOICE_CURSOR = slot;
                    SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume,
                                  pan, mode_snapshot, value);
                    return;
                }
                slot++;
                mask >>= 1;
            } while (slot < SD_VOICE_SLOT_COUNT);
        }
    }

    if (((u16)mode_snapshot & 0xF0) != 0) {
        slot = 0;
        count = 0;
        occurrence = ((u16)mode_snapshot & 0xF0) >> 4;
        for (index = 0; index < SD_VOICE_SLOT_COUNT; index++, slot++) {
            SpuGetVoiceEnvelope(
                index + SD_VOICE_SLOT_FIRST_VOICE, &envelope
            );
            /* Sharing the scratch keeps ID narrowing inside this loop. */
            observed = envelope;
            if (observed != 0) {
                observed = sound_id;
                if (g_SDValue->s_awSD_VOICE_SE_ID[slot] == observed) {
                    count++;
                    if (count == occurrence) {
                        g_SDValue->s_bSD_VOICE_CURSOR = slot;
                        SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume,
                                      pan, mode_snapshot, value);
                        return;
                    }
                }
            }
        }
    }

    mask = 1 << g_SDValue->s_bSD_VOICE_CURSOR;
    if (found == 0) {
        index = 0;
        do {
            SpuGetVoiceEnvelope(
                g_SDValue->s_bSD_VOICE_CURSOR + SD_VOICE_SLOT_FIRST_VOICE, &envelope
            );
            if (envelope == 0) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR + 1;
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }

    if (found == 0) {
        index = 0;
        mask = 1 << g_SDValue->s_bSD_VOICE_CURSOR;
        do {
            if (value >= g_SDValue->s_abSD_VOICE_PRIORITY[g_SDValue->s_bSD_VOICE_CURSOR]) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR + 1;
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }
}

void SD_SEPlay(s32 sound_id, s32 volume, s32 pan_value)
{
    SDSEPlayIdPair tags;
    s32 vol;
    s32 pan;
    u8 last_arg;
    u16 stop_value;
    s32 lo;
    s32 hi;
    s32 n;
    SDNote *e;
    s32 t2;

    /* The duplicate paths and consumed-id reuse preserve the retail copy
       chain: GCC must not forward sound_id into the second ID snapshot. */
    if (sound_id & 0x8000) {
        tags.words.original = sound_id;
        pan = pan_value;
        sound_id = volume;
        tags.words.copy = tags.words.original;
        vol = sound_id;
        stop_value = tags.words.original;
    } else {
        tags.words.original = sound_id;
        pan = pan_value;
        sound_id = volume;
        tags.words.copy = tags.words.original;
        vol = sound_id;
        stop_value = tags.words.original;
    }
    if (tags.words.original & 0x8000) {
        SD_EnqueueCdSePlay(stop_value & 0xFFFF, 0);
        return;
    }
    if ((tags.words.original & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;
        s32 n;

        lo = (tags.words.original & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        t2 = tags.words.original & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = t2;
        hi = (hi != 0) << 6;

        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        if (v == SD_PENDING_ENTRY_NONE) {
            return;
        }
        n = a->s_pwSD_SE_INDEX[v];
        if (n == SD_PENDING_ENTRY_NONE) {
            return;
        }
        /* Array indexing changes the direct-ID path's address registers. */
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)a->s_pstSD_SE_TABLE);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(v, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    } else {
        SDValue *b = g_SDValue;

        n = b->s_pwSD_SE_INDEX[tags.words.copy & 0xFFFF];
        if (n == SD_PENDING_ENTRY_NONE) {
            return;
        }
        e = (SDNote *)(n * SD_NOTE_RECORD_SIZE + (u32)b->s_pstSD_SE_TABLE);
        t2 = e->priority;
        last_arg = t2;
        vol &= 0xFF;
        SD_AllocateSoundEffectVoice(tags.words.copy & 0xFFFF, 0, vol, (s16)pan, e->mode, last_arg & 0xFF);
    }
}

void SD_SESetPan(s32 sound_id, s32 pan_value)
{
    s32 i;
    s32 pan;
    s32 lo;
    s32 hi;
    s32 near;
    u16 key;
    s16 env[4];
    SDValue *base;
    s32 v;
    s32 off;
    s16 pan2;
    u32 flag;
    s32 pt;
    s32 k;
    u32 nt;
    s32 prod;

    key = sound_id;
    pan2 = pan_value;
    if (sound_id & 0x8000) {
        SD_SetCdPan((s16)pan_value);
        return;
    }
    if ((sound_id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (sound_id & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = sound_id & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = 0 < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = g_SDValue;
        /* A byte offset from the state base: indexing s_aawSD_INDIRECT_SE_ID
           schedules the index shift after the bank test, and offsetting
           from the array instead of the base reverses the addu operands. */
        pt = *(u16 *)((u8 *)base + off + OFFSETOF(SDValue, s_aawSD_INDIRECT_SE_ID));
        sound_id = SD_PENDING_ENTRY_NONE;
        if (pt == sound_id) {
            return;
        }
        key = base->s_pwSD_SE_INDEX[pt];
        if (key == sound_id) {
            return;
        }
    }
    i = 0;
    nt = (u16)((u32)pan_value - 1);
    near = nt < 0x80;
    pan = (s16)pan_value;
    k = 0x80;
    lo = k - pan;
    hi = pan + k;
    /* Entered through a constant switch so loop.c leaves the voice loop
       alone; see SD_SESetVolumePan below. */
    switch (0) {
        for (;;) {
        }
        do {
    case 0:
            SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, env);
            v = g_SDValue->s_awSD_VOICE_SE_ID[i];
            if (v == key) {
                if (env[0] != 0) {
                    if (pan != 0) {
                        if (near) {
                            prod = g_SDValue->s_pstSD_SE_TABLE[v].volume;
                            prod *= lo;
                            g_SDValue->s_awSD_VOICE_VOLUME_L[i] = prod;
                        }
                        if ((u16)(pan2 + 0x80) < 0x80) {
                            prod = g_SDValue->s_pstSD_SE_TABLE[v].volume;
                            prod *= hi;
                            g_SDValue->s_awSD_VOICE_VOLUME_R[i] = prod;
                        }
                    }
                    SD_ApplyVoiceSlotVolume(i);
                }
            }
            i++;
        } while (i < SD_VOICE_SLOT_COUNT);
    }
}

void SD_SESetVolume(s32 sound_id, s32 volume)
{
    s32 lo;
    s32 hi;
    s32 i;
    u8 a1v = volume;
    s32 idm;
    u16 id;
    s32 ff;

    id = sound_id;
    if (sound_id & 0x8000) {
        SD_SetCdVolume(a1v);
        return;
    }
    if ((sound_id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;

        lo = (sound_id & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = sound_id & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        /* Keep the table base separate from the byte offset for allocation. */
        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->s_pwSD_SE_INDEX[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    idm = id & 0xFFFF;
    do {
        s16 local;
        SDValue *b;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        if (b->s_awSD_VOICE_SE_ID[i] == idm && local != 0) {
            b->s_abSD_VOICE_FADE_LEVEL[i] = a1v;
            SD_ApplyVoiceSlotVolume(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

/* Sets the level and pan of every live voice playing one sound id. Bit
 * 0x8000 in `id` sets the CD volume instead; an id carrying the lookup tag is
 * first translated through the bank table at s_aawSD_INDIRECT_SE_ID and s_pwSD_SE_INDEX.
 * `word` is a signed pan offset: left and right get 0x80 minus and plus it,
 * each only when it stays inside the byte range.
 *
 * Three spellings are load bearing, each measured against the retail words:
 *
 * - The definition is old style with u16/u8/u16 parameters. The callers and
 *   sound_effect_request.h still see three words, which old-style promotion keeps
 *   compatible, but inside the function GCC keeps each incoming register and
 *   a narrowed copy apart. That is retail's shape: the early tests read
 *   $a0/$a2 and the voice loop reads the copies in $s2/$s3/$s7.
 * - Retail recomputes i * 2 every pass of the voice loop, which loop.c would
 *   have strength-reduced in an ordinary structured loop. The constant switch
 *   enters the do/while at its case label; until flow deletes the dead empty
 *   loop that jump comes from outside the loop, so loop.c leaves it alone,
 *   and jump2 deletes the jump afterwards.
 * - Each level product goes through one local that first holds the note
 *   volume. That gives the pseudo register uses on both sides of the mult,
 *   so it is allocated $v0 (lbu/mult/mflo $v0) instead of LO plus a reload. */
void SD_SESetVolumePan(id, value, word)
    u16 id;
    u8 value;
    u16 word;
{
    s32 i;
    s32 pan;
    s32 right;
    s32 left;
    s32 leftOk;
    s32 lo;
    s32 hi;
    s32 ff;

    if (id & 0x8000) {
        g_SDValue->s_wSD_CD_VOLUME = value;
        SD_SetCdPan((s16)word);
        return;
    }
    if ((id & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;

        lo = (id & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = id & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        /* As in SD_SESetVolume, the table base stays separate from the
           byte offset so the add keeps retail's operand order. */
        table = (u8 *)&a->s_aawSD_INDIRECT_SE_ID;
        v = *(u16 *)(table + (lo + hi));
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->s_pwSD_SE_INDEX[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    leftOk = (u16)(word - 1) < 0x80;
    pan = (s16)word;
    left = 0x80 - pan;
    right = pan + 0x80;
    switch (0) {
        for (;;) {
        }
        do {
    case 0:
            {
                s16 local;
                SDValue *b;
                u16 vid;

                SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
                b = g_SDValue;
                vid = b->s_awSD_VOICE_SE_ID[i];
                if (vid == (id & 0xFFFF) && local != 0) {
                    b->s_abSD_VOICE_FADE_LEVEL[i] = value;
                    if (pan != 0) {
                        if (leftOk) {
                            SDValue *c = g_SDValue;
                            s32 v;

                            v = c->s_pstSD_SE_TABLE[vid].volume;
                            v *= left;
                            c->s_awSD_VOICE_VOLUME_L[i] = v;
                        }
                        if ((u16)(word + 0x80) < 0x80) {
                            SDValue *d = g_SDValue;
                            s32 v;

                            v = d->s_pstSD_SE_TABLE[vid].volume;
                            v *= right;
                            d->s_awSD_VOICE_VOLUME_R[i] = v;
                        }
                    }
                    SD_ApplyVoiceSlotVolume(i);
                }
            }
        } while (++i < SD_VOICE_SLOT_COUNT);
    }
}

void SD_FadeOutSoundEffectVoices(u16 id, u8 step)
{
    s32 i;
    s32 mask;

    if (step == 0)
        return;
    mask = 1;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        SDValue *state = g_SDValue;
        if (state->s_awSD_VOICE_SE_ID[i] == id &&
            (state->s_bSD_VOICE_ACTIVE_MASK & mask)) {
            state->s_abSD_VOICE_FADE_STEP[i] = step;
        }
    }
}

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
                u16 n = g_SDValue->s_wSD_NEXT_SE_INDEX;
                u16 v;

                g_SDValue->s_pwSD_SE_INDEX[key] = n;
                g_SDValue->s_pstSD_SE_TABLE[n] = cur->data[j];
                /* Integer sum: &table[n] reverses the addu operands. */
                e = (SDNote *)(n * sizeof(SDNote) + (u32)g_SDValue->s_pstSD_SE_TABLE);
                v = e->spu_address_div16;
                e->spu_address_div16 =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->s_wSD_NEXT_SE_INDEX = g_SDValue->s_wSD_NEXT_SE_INDEX + 1;
            }
            if (i != 0) {
                g_SDValue->s_aawSD_INDIRECT_SE_ID[side][j] = key;
            }
        }
    }
}

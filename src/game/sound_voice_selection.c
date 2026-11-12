#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

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

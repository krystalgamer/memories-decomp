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
    state->voice_volume_attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
    state->voice_volume_attr.voice = mask;
    {
        u32 first = state->voice_volume_left[index];
        product = first * (u32)state->voice_value[index];
    }
    state->voice_volume_attr.volume.left = product >> 8;
    product = state->voice_volume_right[index] * (u32)state->voice_value[index];
    state->voice_volume_attr.volume.right = product >> 8;
    SpuSetVoiceAttr(&state->voice_volume_attr);
}

void SD_UpdateVoiceSlots(void)
{
    s32 mask;
    s32 bit2;
    s32 accum;
    s32 i;
    s32 v0;
    u16 timer;

    mask = SD_VOICE_SLOT_MASK_BASE;
    bit2 = 1;
    accum = 0;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_step[i] != 0) {
            if (g_SDValue->voice_step[i] >= g_SDValue->voice_value[i]) {
                g_SDValue->voice_value[i] = 0;
            } else {
                g_SDValue->voice_value[i] =
                    g_SDValue->voice_value[i] - g_SDValue->voice_step[i];
            }
            if (g_SDValue->voice_value[i] != 0) {
                SD_ApplyVoiceSlotVolume(i);
                goto tail;
            }
            g_SDValue->voice_active_mask &= ~bit2;
            g_SDValue->voice_step[i] = 0;
            SpuSetKey(SPU_OFF, mask);
            accum |= mask;
        }
    tail:
        timer = g_SDValue->voice_timer[i];
        if (timer != 0) {
            timer -= 1;
            g_SDValue->voice_timer[i] = timer;
            if (timer == 0) {
                v0 = SpuGetKeyStatus(mask);
                if (v0 != SPU_OFF) {
                    SpuSetKey(SPU_OFF, mask);
                    accum |= mask;
                }
            }
        }
        v0 = SpuGetKeyStatus(mask);
        if (v0 == SPU_ON_ENV_OFF && !(accum & mask)) {
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
    u16 threshold = g_SDValue->dynamic_note_start;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        if (g_SDValue->voice_ids[i] >= threshold)
            result |= mask;
        mask <<= 1;
    }
    SpuSetKey(SPU_OFF, result);
}

s32 SD_ReplaceValueLink(s32 value)
{
    u16 index = value;
    SDValue *state = g_SDValue;

    if (state->value_links[index].spu_size == 0)
        return 1;
    if (state->value_link_request_index == index)
        return 1;
    SD_KeyOffDynamicVoices();
    SD_ResetPendingEntries();
    {
        register SDValue *final = g_SDValue;
        register s32 call_value = index;
        final->value_link_request_index = value;
        final->status_flags |= 2;
        SD_QueueValueLinkTransfer(call_value);
    }
    return 1;
}

s32 SD_RequestValueLink(u16 value)
{
    SDValue *state = g_SDValue;

    if (state->value_link_request_index == value)
        return 1;
    state->value_link_request_index = value;
    state->status_flags |= 2;
    SD_QueueValueLinkTransfer(value);
    return 1;
}

void SD_QueueValueLinkList(u16 *items, s32 count)
{
    s32 i;

    SpuSetKey(SPU_OFF, SD_VOICE_SLOT_MASK_ALL);
    SD_ResetPendingEntries();
    g_SDValue->status_flags |= 2;
    for (i = 0; i < count; i++) {
        if (items[i] != 0xFFFF)
            SD_QueueValueLinkTransfer(items[i]);
    }
}

s32 tent_SD_FilterCdFlaggedId(s32 value)
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

void tent_SD_KeyOffVoiceById(s32 value)
{
    s32 normalized = tent_SD_FilterCdFlaggedId((u16)value);
    SDValue *state = g_SDValue;
    s32 tries = 0;
    s32 slot = state->voice_allocation_cursor;
    s32 bit = 1 << (u8)slot;
    s32 command = 1 << ((u8)slot + SD_VOICE_SLOT_KEY_SHIFT);

    do {
        state = g_SDValue;
        if (state->voice_active_mask & bit) {
            if (state->voice_ids[(u8)slot] == (u16)normalized) {
                SD_KeyOffVoice(command);
                g_SDValue->voice_allocation_cursor = slot;
                g_SDValue->voice_active_mask &= ~bit;
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
        u8 *table = (u8 *)g_SDValue->indirect_sound_ids[0];
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
        if (g_SDValue->voice_ids[i] == key) {
            bits |= bit;
            g_SDValue->voice_active_mask &= ~mask;
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
        SpuGetAllKeysStatus((char *)g_SDValue->voice_key_status);
        total = g_SDValue->voice_key_status[23] + g_SDValue->voice_key_status[21] +
                g_SDValue->voice_key_status[22] + g_SDValue->voice_key_status[23];
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
        if ((state->voice_flags[i] & 0xF) == group)
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
   g_SDValue->voice_ids and the start assigns it, along with the flags,
   volumes and timer beside it. */

/* Counts how many of the four voice slots are playing this sound id: an id
   match in voice_ids paired with a non-zero SPU envelope, which is what
   distinguishes a slot still sounding from one that has already decayed. */
s16 SD_CountActiveSoundEffectVoices(s32 a0) {
    u16 s1 = 0;
    s32 s0;
    u16 s2 = a0 & 0xFFFF;

    for (s0 = 0; s0 < SD_VOICE_SLOT_COUNT; s0++) {
        s16 local;

        SpuGetVoiceEnvelope(s0 + SD_VOICE_SLOT_FIRST_VOICE, &local);

        if (g_SDValue->voice_ids[s0] == s2) {
            if (local != 0) {
                s1++;
            }
        }
    }

    return (s16)s1;
}

/* Starts a sound-effect voice: looks the sound id up in the voice index table,
   derives the left and right volumes from the note table entry and the pan,
   keys the voice on and records the id, volumes and timer in the voice slots. */
void SD_StartSoundEffectVoice(u16 id, u8 voice, s32 pitch_add, u8 volume, s16 pan, u8 flags, u8 value) {
    u16 idx;

    idx = g_SDValue->note_index_by_id[id];
    if (idx != 0xFFFF) {
        g_SDValue->voice_attr.volume.left = ((g_SDValue->note_table[idx].volume * volume) >> 1) | 0xFF;
        g_SDValue->voice_attr.volume.right = ((g_SDValue->note_table[idx].volume * volume) >> 1) | 0xFF;
        if (pan != 0) {
            if ((u16)(pan - 1) < 0x80) {
                g_SDValue->voice_attr.volume.left = g_SDValue->voice_attr.volume.left * (0x80 - pan) / 128;
            }
            if ((u16)(pan + 0x80) < 0x80) {
                g_SDValue->voice_attr.volume.right = g_SDValue->voice_attr.volume.right * (pan + 0x80) / 128;
            }
        }
        g_SDValue->voice_attr.note = g_SDValue->note_table[idx].pitch + pitch_add;
        g_SDValue->voice_attr.voice = 1 << (voice + SD_VOICE_SLOT_KEY_SHIFT);
        g_SDValue->voice_attr.addr = g_SDValue->note_table[idx].spu_address_div16 << 4;
        SpuSetKey(SPU_OFF, g_SDValue->voice_attr.voice);
        g_SDValue->voice_active_mask |= 1 << voice;
        g_SDValue->voice_priority[voice] = value;
        g_SDValue->voice_flags[voice] = flags;
        g_SDValue->voice_ids[voice] = id;
        g_SDValue->voice_volume_left[voice] = g_SDValue->voice_attr.volume.left;
        g_SDValue->voice_volume_right[voice] = g_SDValue->voice_attr.volume.right;
        g_SDValue->voice_value[voice] = 0xFF;
        g_SDValue->voice_step[voice] = 0;
        g_SDValue->voice_timer[voice] = g_SDValue->note_table[idx].timer << 2;
        SpuSetKeyOnWithAttr(&g_SDValue->voice_attr);
    }
}

#include "../types.h"
#include "../psyq/libspu.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

void SD_KeyOffVoice(s32 voice)
{
    s32 count;

    for (count = 0; count < SD_KEY_OFF_RETRY_LIMIT; count++) {
        SpuSetKey(SPU_OFF, voice);
        if (SpuGetKeyStatus(voice) == SPU_OFF)
            break;
    }
}

void func_80047CC4(s32 value)
{
    s32 normalized = func_80047C50((u16)value);
    SDValue *state = g_SDValue;
    s32 tries = 0;
    s32 slot = state->field_0435;
    s32 bit = 1 << (u8)slot;
    s32 command = 1 << ((u8)slot + SD_VOICE_SLOT_KEY_SHIFT);

    do {
        state = g_SDValue;
        if (state->voice_active_mask & bit) {
            if (state->voice_ids[(u8)slot] == (u16)normalized) {
                SD_KeyOffVoice(command);
                g_SDValue->field_0435 = slot;
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
        func_800464F0();
        func_80045114();
        return;
    }
    if ((value & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        u8 *table = (u8 *)g_SDValue->field_044C[0];
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
        SpuGetAllKeysStatus((char *)g_SDValue->field_15D8);
        total = g_SDValue->field_15EF + g_SDValue->field_15ED +
                g_SDValue->field_15EE + g_SDValue->field_15EF;
        count++;
    } while (total != 0 && count < 24);
}

s32 func_80047F38(u8 value)
{
    s32 i;
    s32 result = 0;
    s32 mask = 1;
    SDValue *state = g_SDValue;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        if ((state->voice_flags[i] & 0xF) == value)
            result |= mask;
    }
    if (result != 0)
        SpuSetKey(SPU_OFF, result << SD_VOICE_SLOT_KEY_SHIFT);
    return result;
}

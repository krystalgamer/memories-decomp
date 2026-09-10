#include "../types.h"
#include "../psyq/libspu.h"
#include "func_80047788.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

void func_80047C70(s32 value)
{
    s32 count;

    for (count = 0; count < SD_KEY_OFF_RETRY_LIMIT; count++) {
        SpuSetKey(SPU_OFF, value);
        if (SpuGetKeyStatus(value) == SPU_OFF)
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
                func_80047C70(command);
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


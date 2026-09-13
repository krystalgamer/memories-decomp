#include "../types.h"
#include "../unmatched.h"
#include "../psyq/libspu.h"
#include "func_80047788.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"

void func_80047DB0(s32 value)
{
    u16 key = value;
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
            func_80047C70(bit);
        }
        mask <<= 1;
        bit <<= 1;
    }
    if (bits != 0) {
        SpuSetKey(0, bits);
    }
}

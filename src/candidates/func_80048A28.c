/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 6 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_voice_data.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_mix.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"

void func_80048A28(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 i __asm__("$16");
    s32 pan;
    register s32 id __asm__("$18") = arg0;
    s32 a1v = arg1;
    s32 right;
    s32 left;
    s32 leftOk;
    register s32 a2v __asm__("$23") = arg2;
    s32 lo;
    s32 hi;
    register s32 ff __asm__("$4");

    if (arg0 & 0x8000) {
        g_SDValue->cd_volume = a1v & 0xFF;
        func_80044E90((s16)arg2);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u16 v;

        lo = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        v = *(u16 *)((u8 *)a + (lo + hi) + SD_VOICE_LOOKUP_BYTE_OFFSET);
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->field_043C[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    leftOk = (u16)(arg2 - 1) < 0x80;
    pan = (s16)arg2;
    left = 0x80 - pan;
    right = pan + 0x80;
    do {
        s16 local;
        SDValue *b;
        u16 vid;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        vid = b->voice_ids[i];
        if (vid == (id & 0xFFFF) && local != 0) {
            b->voice_value[i] = a1v;
            if (pan != 0) {
                if (leftOk) {
                    SDValue *c = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = c->field_0444[vid].volume * left;
                    c->voice_volume_left[i] = prod;
                }
                if ((u16)(a2v + 0x80) < 0x80) {
                    SDValue *d = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = d->field_0444[vid].volume * right;
                    d->voice_volume_right[i] = prod;
                }
            }
            func_80047864(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

#include "../game/sound_init.h"
#include "../game/sound_voice_data.h"


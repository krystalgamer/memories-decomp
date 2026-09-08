#include "../../../../src/types.h"
#include "../../../../src/psyq/libspu.h"
#include "../../../../src/game/sound.h"

extern s16 func_800451E0(u16, s32);
extern s32 func_80047F38(u8);
extern void func_8004803C(u16, u8, s32, u8, s16, u8, u8);

/* Starts a sound effect on one of the four voice slots. The low nibble of
   mode selects an already keyed group to steal, the high nibble asks for the
   n-th voice already playing this id, and otherwise the rotating slot cursor
   at field_0435 is walked twice: once looking for a silent voice, and once
   accepting any voice whose recorded value is no higher than this one. */
void func_800482B0(s32 id, s16 pitch, u8 volume, s16 pan, u32 mode, u8 value)
{
    u8 slot;
    u8 count;
    u8 found;
    s32 j;
    s32 m;
    s32 lim;
    s32 bit;
    s16 env;
    u32 md;

    md = mode;
    if (g_SDValue->field_043C[(u16)id] == 0xFFFF) {
        return;
    }
    if ((id & 0x8000) != 0) {
        func_800451E0(id, 0);
        return;
    }

    found = 0;
    if ((md & 0xF) != 0) {
        m = func_80047F38(md & 0xF);
        if (m != 0) {
            slot = 0;
            do {
                if (m & 1) {
                    g_SDValue->field_0435 = slot;
                    func_8004803C(id, g_SDValue->field_0435, pitch, volume,
                                  pan, md, value);
                    return;
                }
                slot++;
                m >>= 1;
            } while (slot < SD_VOICE_SLOT_COUNT);
        }
    }

    if ((md & 0xF0) != 0) {
        slot = 0;
        count = 0;
        lim = (md & 0xF0) >> 4;
        for (j = 0; j < SD_VOICE_SLOT_COUNT; j++, slot++) {
            SpuGetVoiceEnvelope(j + 0x14, &env);
            if (env != 0 && g_SDValue->voice_ids[slot] == id) {
                count++;
                if (count == lim) {
                    g_SDValue->field_0435 = slot;
                    func_8004803C(id, g_SDValue->field_0435, pitch, volume,
                                  pan, md, value);
                    return;
                }
            }
        }
    }

    bit = 1 << g_SDValue->field_0435;
    if (found == 0) {
        j = 0;
        do {
            SpuGetVoiceEnvelope(g_SDValue->field_0435 + 0x14, &env);
            if (env == 0) {
                found = 1;
                func_8004803C(id, g_SDValue->field_0435, pitch, volume, pan,
                              md, value);
            }
            bit <<= 1;
            if ((bit & 0x10) != 0) {
                bit = 1;
            }
            g_SDValue->field_0435 = g_SDValue->field_0435 + 1;
            g_SDValue->field_0435 = g_SDValue->field_0435 & 3;
            j++;
        } while (j < SD_VOICE_SLOT_COUNT && found == 0);
    }

    if (found == 0) {
        j = 0;
        bit = 1 << g_SDValue->field_0435;
        do {
            if (value >= g_SDValue->field_040C[g_SDValue->field_0435]) {
                found = 1;
                func_8004803C(id, g_SDValue->field_0435, pitch, volume, pan,
                              md, value);
            }
            bit <<= 1;
            if ((bit & 0x10) != 0) {
                bit = 1;
            }
            g_SDValue->field_0435 = g_SDValue->field_0435 + 1;
            g_SDValue->field_0435 = g_SDValue->field_0435 & 3;
            j++;
        } while (j < SD_VOICE_SLOT_COUNT && found == 0);
    }
}

#include "../types.h"
#include "sound.h"
#include "../psyq/libspu.h"
#include "sound_mix.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"
#include "sound_voice_selection.h"
#include "sound_voice_pan.h"

void func_80048768(s32 arg0, s32 arg1)
{
    s32 i;
    s32 pan;
    s32 lo;
    s32 hi;
    s32 near;
    u16 key;
    s16 env[4];
    u8 *base;
    s32 v;
    s32 off;
    s16 pan2;
    u32 flag;
    s32 pt;
    s32 k;
    u32 nt;
    s32 prod;

    key = arg0;
    pan2 = arg1;
    if (arg0 & 0x8000) {
        func_80044E90((s16)arg1);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = 0 < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = (u8 *)g_SDValue;
        pt = *(u16 *)(base + off + SD_VOICE_LOOKUP_BYTE_OFFSET);
        arg0 = SD_PENDING_ENTRY_NONE;
        if (pt == arg0) {
            return;
        }
        key = ((SDValue *)base)->field_043C[pt];
        if (key == arg0) {
            return;
        }
    }
    i = 0;
    nt = (u16)((u32)arg1 - 1);
    near = nt < 0x80;
    pan = (s16)arg1;
    k = 0x80;
    lo = k - pan;
    hi = pan + k;
loop:
    {
        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, env);
        v = g_SDValue->voice_ids[i];
        if (v == (u16)key) {
            if (env[0] != 0) {
                if (pan != 0) {
                    if (near) {
                        prod = g_SDValue->field_0444[v].volume;
                        prod *= lo;
                        g_SDValue->voice_volume_left[i] = prod;
                    }
                    if ((u16)(pan2 + 0x80) < 0x80) {
                        prod = g_SDValue->field_0444[v].volume;
                        prod *= hi;
                        g_SDValue->voice_volume_right[i] = prod;
                    }
                }
                func_80047864(i);
            }
        }
        i++;
    }
    if (i < SD_VOICE_SLOT_COUNT) {
        goto loop;
    }
}

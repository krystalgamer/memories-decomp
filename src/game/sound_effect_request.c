#include "../types.h"
#include "sound_output_state.h"
#include "sound.h"
#include "sound_voice_allocator.h"
#include "sound_effect_request.h"
#include "../psyq/libspu.h"
#include "sound_mix.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"
#include "sound_voice_selection.h"
#include "sound_voice_pan.h"

void SD_SEPlay(s32 arg0, s32 arg1, s32 arg2)
{
    SDSEPlayIdPair tags;
    s32 vol;
    s32 pan;
    u8 last_arg;
    u16 stop_value;
    s32 lo;
    s32 hi;
    s32 n;
    u8 *e;
    s32 t2;

    /* The duplicate paths and consumed-id reuse preserve the retail copy
       chain: GCC must not forward arg0 into the second ID snapshot. */
    if (arg0 & 0x8000) {
        tags.words.original = arg0;
        pan = arg2;
        arg0 = arg1;
        tags.words.copy = tags.words.original;
        vol = arg0;
        stop_value = tags.words.original;
    } else {
        tags.words.original = arg0;
        pan = arg2;
        arg0 = arg1;
        tags.words.copy = tags.words.original;
        vol = arg0;
        stop_value = tags.words.original;
    }
    if (tags.words.original & 0x8000) {
        func_800451E0(stop_value & 0xFFFF, 0);
        return;
    }
    if ((tags.words.original & 0xF000) == 0x4000) {
        SDValue *a = g_SDValue;
        u8 *table;
        u16 v;
        s32 n;

        lo = (tags.words.original & 0x1F) << 1;
        t2 = tags.words.original & 0x100;
        hi = t2;
        hi = (hi != 0) << 6;

        table = (u8 *)&a->field_044C;
        v = *(u16 *)(table + (lo + hi));
        if (v == 0xFFFF) {
            return;
        }
        n = a->field_043C[v];
        if (n == 0xFFFF) {
            return;
        }
        e = (u8 *)(n * 8 + (u32)a->field_0444);
        t2 = e[2];
        last_arg = t2;
        vol &= 0xFF;
        func_800482B0(v, 0, vol, (s16)pan, e[3], last_arg & 0xFF);
    } else {
        SDValue *b = g_SDValue;

        n = b->field_043C[tags.words.copy & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = (u8 *)(n * 8 + (u32)b->field_0444);
        t2 = e[2];
        last_arg = t2;
        vol &= 0xFF;
        func_800482B0(tags.words.copy & 0xFFFF, 0, vol, (s16)pan, e[3], last_arg & 0xFF);
    }
}

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
        SD_SetCdPan((s16)arg1);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = 0 < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = (u8 *)g_SDValue;
        /* Keep the state base first in the sum to preserve retail's addu order. */
        pt = *(u16 *)(base + off + (u32)&((SDValue *)0)->field_044C);
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
                SD_ApplyVoiceSlotVolume(i);
            }
        }
        i++;
    }
    if (i < SD_VOICE_SLOT_COUNT) {
        goto loop;
    }
}

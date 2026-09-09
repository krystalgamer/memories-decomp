#include "../types.h"
#include "sound.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"

extern void func_80044E90(s32);
extern void func_80047864(s32);

/* Prevent GCC from rewriting the masked-value boolean as a shift. */
register const u32 zero asm("$0");

void func_80048768(s32 arg0, s32 arg1)
{
    s32 i;
    s32 pan;
    register s32 lo asm("$20");
    register s32 hi asm("$19");
    register s32 near asm("$21");
    register s32 key asm("$18");
    s16 env[4];
    u8 *base;
    s32 v;
    s32 off;
    s16 pan2;
    u32 flag;
    register s32 pt asm("$2");
    register s32 k asm("$2");
    register u32 nt asm("$2");
    register s32 prod asm("$2");
    register u32 invalid_key asm("$4");

    key = arg0;
    pan2 = arg1;
    if (arg0 & 0x8000) {
        func_80044E90((s16)arg1);
        return;
    }
    i = 0;
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        off = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) * 2;
        flag = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        flag = zero < flag;
        off += flag << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        base = (u8 *)g_SDValue;
        pt = *(u16 *)(base + off + SD_VOICE_LOOKUP_BYTE_OFFSET);
        invalid_key = SD_PENDING_ENTRY_NONE;
        if (pt == invalid_key) {
            return;
        }
        key = ((SDValue *)base)->field_043C[pt];
        if (key == invalid_key) {
            return;
        }
    }
    nt = (u16)(arg1 - 1);
    near = nt < 0x80;
    pt = arg1 << 16;
    pan = pt >> 16;
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
                        prod = g_SDValue->field_0444[v].volume * lo;
                        g_SDValue->voice_volume_left[i] = prod;
                    }
                    if ((u16)(pan2 + 0x80) < 0x80) {
                        prod = g_SDValue->field_0444[v].volume * hi;
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

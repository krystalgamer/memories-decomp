#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_voice_constants.h"

extern u8 *g_SDValue;
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
    if ((arg0 & 0xF000) == 0x4000) {
        off = (arg0 & 0x1F) * 2;
        flag = arg0 & 0x100;
        flag = zero < flag;
        off += flag << 6;
        base = g_SDValue;
        pt = *(u16 *)(base + off + 0x44C);
        invalid_key = 0xFFFF;
        if (pt == invalid_key) {
            return;
        }
        key = *(u16 *)(*(u8 **)(base + 0x43C) + pt * 2);
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
        base = (u8 *)(i * 2);
        v = *(u16 *)(g_SDValue + (s32)base + 0x404);
        if (v == (u16)key) {
            if (env[0] != 0) {
                if (pan != 0) {
                    if (near) {
                        prod = *(u8 *)(*(u8 **)(g_SDValue + 0x444) + v * 8) * lo;
                        *(u16 *)(g_SDValue + (s32)base + 0x414) = prod;
                    }
                    if ((u16)(pan2 + 0x80) < 0x80) {
                        prod = *(u8 *)(*(u8 **)(g_SDValue + 0x444) + v * 8) * hi;
                        *(u16 *)(g_SDValue + (s32)base + 0x41C) = prod;
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

#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_voice_constants.h"

struct SoundState {
    u8 pad0[0x404];
    u16 ids[SD_VOICE_SLOT_COUNT];
    u8 pad1[0x424 - 0x40C];
    u8 arr424[SD_VOICE_SLOT_COUNT];
    u8 pad2[0x43C - 0x428];
    u16 *p43C;
    u8 pad3[SD_VOICE_LOOKUP_BYTE_OFFSET - 0x440];
    u16 tbl44C[SD_VOICE_LOOKUP_ENTRY_COUNT];
};

extern struct SoundState *g_SDValue;
extern void func_80044DC0(u8);
extern void func_80047864(s32);

void func_80048920(s32 arg0, s32 arg1)
{
    s32 lo;
    s32 hi;
    register s32 i __asm__("$16");
    register s32 a1v __asm__("$17") = arg1;
    register s32 idm __asm__("$18");
    register s32 id __asm__("$6");
    s32 ff;

    id = arg0;
    if (arg0 & 0x8000) {
        func_80044DC0((u8)a1v);
        return;
    }
    if ((arg0 & 0xF000) == 0x4000) {
        struct SoundState *a = g_SDValue;
        u16 v;

        lo = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        v = *(u16 *)((u8 *)a + (lo + hi) + SD_VOICE_LOOKUP_BYTE_OFFSET);
        ff = 0xFFFF;
        if (v == ff) {
            return;
        }
        id = a->p43C[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    idm = id & 0xFFFF;
    do {
        s16 local;
        struct SoundState *b;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        if (b->ids[i] == idm && local != 0) {
            b->arr424[i] = a1v;
            func_80047864(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

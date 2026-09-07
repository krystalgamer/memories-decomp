#include "../types.h"
#include "../psyq/libspu.h"

struct SoundNote {
    u8 volume;
    u8 pad0001[7];
};

struct SoundState {
    u8 pad0[0x404];
    u16 ids[4];
    u8 pad1[0x414 - 0x40C];
    u16 vol_l[4];
    u16 vol_r[4];
    u8 arr424[4];
    u8 pad2[0x43C - 0x428];
    u16 *p43C;
    u8 pad3[0x444 - 0x440];
    struct SoundNote *p444;
    u8 pad4[0x44C - 0x448];
    u16 tbl44C[64];
    u8 pad5[0x510 - 0x4CC];
    s16 cd_volume;
};

extern struct SoundState *g_SDValue;
extern void func_80044E90(s32);
extern void func_80047864(s32);

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
    if ((arg0 & 0xF000) == 0x4000) {
        struct SoundState *a = g_SDValue;
        u16 v;

        lo = (arg0 & 0x1F) << 1;
        hi = arg0 & 0x100;
        hi = (hi != 0) << 6;
        v = *(u16 *)((u8 *)a + (lo + hi) + 0x44C);
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
    leftOk = (u16)(arg2 - 1) < 0x80;
    pan = (s16)arg2;
    left = 0x80 - pan;
    right = pan + 0x80;
    do {
        s16 local;
        struct SoundState *b;
        u16 vid;

        SpuGetVoiceEnvelope(i + 0x14, &local);
        b = g_SDValue;
        vid = b->ids[i];
        if (vid == (id & 0xFFFF) && local != 0) {
            b->arr424[i] = a1v;
            if (pan != 0) {
                if (leftOk) {
                    struct SoundState *c = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = c->p444[vid].volume * left;
                    c->vol_l[i] = prod;
                }
                if ((u16)(a2v + 0x80) < 0x80) {
                    struct SoundState *d = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = d->p444[vid].volume * right;
                    d->vol_r[i] = prod;
                }
            }
            func_80047864(i);
        }
    } while (++i < 4);
}

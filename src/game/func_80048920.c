#include "../types.h"

struct SoundState {
    u8 pad0[0x404];
    u16 ids[4];
    u8 pad1[0x424 - 0x40C];
    u8 arr424[4];
    u8 pad2[0x43C - 0x428];
    u16 *p43C;
    u8 pad3[0x44C - 0x440];
    u16 tbl44C[64];
};

extern struct SoundState *g_SDValue;
extern void func_80044DC0(u8);
extern void func_80077C50(s32, void *);
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
    idm = id & 0xFFFF;
    do {
        s16 local;
        struct SoundState *b;

        func_80077C50(i + 0x14, &local);
        b = g_SDValue;
        if (b->ids[i] == idm && local != 0) {
            b->arr424[i] = a1v;
            func_80047864(i);
        }
    } while (++i < 4);
}

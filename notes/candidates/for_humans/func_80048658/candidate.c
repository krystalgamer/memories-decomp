#include "../../../../src/types.h"

struct SoundState {
    u8 pad0[0x43C];
    u16 *p43C;
    u8 pad1[0x444 - 0x440];
    u8 *p444;
    u8 pad2[0x44C - 0x448];
    u16 tbl44C[64];
};

extern struct SoundState *g_SDValue;
extern void func_800451E0(s32, s32);
extern void func_800482B0(s32, s32, u8, s32, s32, s32);

void SD_SEPlay(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 id asm("$8");
    register s32 idc asm("$9");
    register s32 vol asm("$6");
    register s32 pan asm("$7");
    register u16 stop_arg asm("$4");
    u8 last_arg;
    s32 lo;
    s32 hi;
    s32 n;
    register u8 *e asm("$2");
    s32 t2;

    id = arg0;
    pan = arg2;
    stop_arg = arg0;
    idc = id;
    vol = arg1;
    if (id & 0x8000) {
        func_800451E0(stop_arg & 0xFFFF, 0);
        return;
    }
    if ((id & 0xF000) == 0x4000) {
        struct SoundState *a = g_SDValue;
        u16 v;
        register s32 n asm("$2");

        lo = (id & 0x1F) << 1;
        t2 = id & 0x100;
        hi = t2;
        hi = (hi != 0) << 6;
        v = *(u16 *)((u8 *)a + (lo + hi) + 0x44C);
        if (v == 0xFFFF) {
            return;
        }
        n = a->p43C[v];
        if (n == 0xFFFF) {
            return;
        }
        e = (u8 *)(n * 8 + (u32)a->p444);
        t2 = e[2];
        last_arg = t2;
        vol &= 0xFF;
        func_800482B0(v, 0, vol, (s16)pan, e[3], last_arg & 0xFF);
    } else {
        struct SoundState *b = g_SDValue;

        n = b->p43C[idc & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = (u8 *)(n * 8 + (u32)b->p444);
        t2 = e[2];
        last_arg = t2;
        vol &= 0xFF;
        func_800482B0(idc & 0xFFFF, 0, vol, (s16)pan, e[3], last_arg & 0xFF);
    }
}

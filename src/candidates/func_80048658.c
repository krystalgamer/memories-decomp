/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 7 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sd_se_play.c.
 */
#include "../types.h"
#include "../unmatched.h"
#include "../game/sound_output_state.h"
#include "../game/sound.h"

void SD_SEPlay(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 id asm("$8");
    register s32 idc asm("$9");
    register s32 vol asm("$6");
    register s32 pan asm("$7");
    register u16 stop_arg asm("$4");
    u8 last_arg;
    u16 stop_value;
    s32 lo;
    s32 hi;
    s32 n;
    register u8 *e asm("$2");
    s32 t2;

    /* Keep both initialization blocks for GCC 2.8.1 instruction scheduling:
     * retail copies into $t1 before $a0. Flattening the blocks changes four
     * entry words under this profile. The stop-bit test uses the input. */
    if (arg0 & 0x8000) {
        id = arg0;
        pan = arg2;
        stop_arg = arg0;
        idc = id;
        vol = arg1;
        stop_value = stop_arg;
    } else {
        id = arg0;
        pan = arg2;
        stop_arg = arg0;
        idc = id;
        vol = arg1;
        stop_value = stop_arg;
    }
    if (id & 0x8000) {
        func_800451E0(stop_value & 0xFFFF, 0);
        return;
    }
    if ((id & 0xF000) == 0x4000) {
        SDValue *a = g_SDValue;
        u16 v;
        register s32 n asm("$2");

        lo = (id & 0x1F) << 1;
        t2 = id & 0x100;
        hi = t2;
        hi = (hi != 0) << 6;
        /* Naming field_044C in this address changes the word at +0x68. */
        v = *(u16 *)((u8 *)a + (lo + hi) + 0x44C);
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

        n = b->field_043C[idc & 0xFFFF];
        if (n == 0xFFFF) {
            return;
        }
        e = (u8 *)(n * 8 + (u32)b->field_0444);
        t2 = e[2];
        last_arg = t2;
        vol &= 0xFF;
        func_800482B0(idc & 0xFFFF, 0, vol, (s16)pan, e[3], last_arg & 0xFF);
    }
}

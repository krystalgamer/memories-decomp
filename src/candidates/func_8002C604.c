/* Reclassified from matching_c (#3859). This was src/game/func_8002C604.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g0_split, whose compiler and
 * assembler disagree about small data (GCC -G8, MASPSX -G0). Under
 * gcc_2_8_1_g0_split, a single threshold, it is 34 of 34 instructions with
 * 3 differing, opcode distance 0. The source below is the match, unchanged
 * apart from its include paths. */
#include "../types.h"
#include "../game/duel_effect_object_pool.h"
#include "../unmatched.h"
#include "../game/ordering_tables.h"

#include "../game/high_memory_addresses.h"

/* Allocates a request entry through func_8002C5CC and fills it: flag byte
 * 0x80, the id at +0x18, the buffer pointer D_80010000 + 0x3800 at +0x14,
 * two words copied from D_800E9D90, and the zeroed fields. Returns the entry
 * or 0 when none was free. */
u8 *func_8002C604(s32 arg0) {
    u8 *p = (u8 *)func_8002C5CC();

    if (p != 0) {
        u8 *q;
        GsOT **t;
        s32 b;

        q = D_80010000;
        p[0x1C] = 0x80;
        t = D_800E9D90;
        *(s16 *)(p + 0x18) = arg0;
        *(s16 *)(p + 0x1A) = 0;
        p[0x1D] = 0;
        *(s32 *)(p + 0x14) = (s32)(q + 0x3800);
        *(s32 *)(p + 8) = (s32)t[2];
        b = (s32)t[1];
        *(s16 *)(p + 0x10) = 8;
        *(s16 *)p = 0;
        *(s16 *)(p + 2) = 0;
        *(s16 *)(p + 4) = 0;
        *(s16 *)(p + 0x12) = 0;
        *(s32 *)(p + 0xC) = b;
    }

    return p;
}

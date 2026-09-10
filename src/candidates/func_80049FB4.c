/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 9 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_80049FB4.c.
 */
#include "../types.h"
#include "../game/func_80049FB4.h"

extern u8 D_80010834[];

s32 func_80049FB4(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    s16 t;
    register s32 n asm("$6");
    s16 r;
    register s32 col asm("$9");
    s32 sum;
    s32 diff;
    s32 d;
    register s32 positive_q asm("$4");
    register s32 positive_input asm("$7");
    register s32 positive_remainder asm("$2");
    register s32 negative_offset asm("$5");
    s32 q_seed;
    s32 negative_q;
    register u8 *positive_base asm("$5");
    register u8 *negative_base asm("$7");

    t = (s16)(arg1 + arg3);
    {
        register s32 row asm("$2");

        row = t / 128;
        sum = arg0 + row;
        row <<= 7;
        col = t - row;
    }
    diff = sum - arg2;
    d = (diff << 16) >> 16;
    if (d < 0)
        goto negative;
    positive_input = d;
    positive_base = D_80010834;
    positive_q = positive_input / 12;
    positive_remainder = positive_input - positive_q * 12;
    r = (s16)positive_remainder;
    sum = col << 16;
    sum >>= 15;
    sum += r << 8;
    sum += (s32)positive_base;
    return (*(u16 *)sum << positive_q) & 0xFFFF;
negative:
    negative_base = D_80010834;
    n = (s16)(-diff);
    r = (s16)(n - (n / 12) * 12);
    q_seed = 12;
    negative_q = q_seed;
    negative_q -= r;
    negative_q %= 12;
    negative_offset = (s16)col * 2;
    negative_offset += negative_q << 8;
    negative_offset += (s32)negative_base;
    return (*(u16 *)negative_offset >> ((n + 11) / 12)) & 0xFFFF;
}

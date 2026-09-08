#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libpress.h"
#include "../psyq/libcd.h"

extern u8 D_8009B060;
extern u8 D_8009B062;
extern u8 D_8009B067;
extern u8 *D_8009B498;
/* Four bytes, so at -G8 a plain declaration would land in sdata and both
   accesses would come out gp-relative; retail reaches this one through
   lui %hi / %lo. The other three globals are genuinely small and stay
   gp-relative. */
extern s32 D_800F5D44 __attribute__((section(".data")));

/* Feeds the MDEC one strip of the current movie frame. The strip header at
 * D_8009B498 + 0x42428 is copied into the ring slot for D_8009B067, that slot
 * and the matching decoded-output buffer go to LoadImage, the slot index
 * advances modulo four, and the accumulated strip height at +0x42428 grows by
 * the strip's own height. While the accumulation is still short of the frame
 * rect at +0x42420 the next strip is queued with DecDCTout; otherwise
 * D_8009B062 marks the frame complete.
 *
 * func_8005BE3C is the sibling to read for the conventions here: a fresh
 * pointer local is assigned D_8009B498 + 0x40000 for each group of accesses
 * rather than one being shared, which is what makes CSE collapse the repeated
 * global read into a register copy while still rebuilding the address
 * arithmetic. Writing dst off D_8009B498 with its own + 0x40000, rather than
 * off src, is what keeps retail's two separate additions of that constant.
 *
 * The modulo is written out because GCC's own expansion of % 4 is correct but
 * lands its three values in the wrong registers, and only one of the three can
 * be named while the operator is used. Written out, all three are nameable. */
void func_8005C1F4(void) {
    u8 *src;
    u8 *dst;
    s32 idx;
    s32 sum;
    /* rem holds the new slot index and nxt the un-wrapped one. A pinned local
       only takes its register for free when something can write it there
       directly - a load, or the store that consumes it - so the two operands
       of the subtraction are pinned and tmp, which is only ever copied, is
       left alone. */
    register s32 rem asm("$3");
    register s32 nxt asm("$5");
    s32 tmp;
    register s32 add asm("$4");
    u8 *out;

    if (D_8009B060 != 0) {
        if (D_800F5D44 != 0) {
            StCdInterrupt();
            D_800F5D44 = 0;
        }
    }
    dst = D_8009B498 + D_8009B067 * 8 + 0x40000;
    src = D_8009B498 + 0x40000;
    /* One eight-byte aggregate: two four-byte copies would issue load, store,
       load, store with a load-delay nop after each load, where retail pairs
       both lwl/lwr and then both swl/swr through two temporaries. */
    *(RECT *)(dst + 0x2400) = *(RECT *)(src + 0x2428);
    idx = D_8009B067;
    LoadImage((RECT *)(D_8009B498 + 0x42400 + idx * 8),
              (u32 *)(D_8009B498 + 0x37000 + idx * 0x2D00));
    nxt = D_8009B067 + 1;
    tmp = nxt;
    if (nxt < 0) {
        tmp = D_8009B067 + 4;
    }
    rem = nxt - (tmp & ~3);
    D_8009B067 = rem;
    out = D_8009B498 + 0x40000;
    /* Loaded in two statements so +0x2428 is read before +0x242C, which
       naming the second one for its pin would otherwise reverse. */
    sum = *(u16 *)(out + 0x2428);
    add = *(u16 *)(out + 0x242C);
    sum = sum + add;
    *(s16 *)(out + 0x2428) = sum;
    if ((s16)sum < *(s16 *)(out + 0x2420) + *(s16 *)(out + 0x2424)) {
        DecDCTout((u32 *)(D_8009B498 + 0x37000 + D_8009B067 * 0x2D00),
                  *(s16 *)(out + 0x242C) * *(s16 *)(out + 0x242E) / 2);
    } else {
        D_8009B062 = 1;
    }
}

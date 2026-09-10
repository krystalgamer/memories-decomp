#include "../types.h"
#include "func_80058DD8.h"
#include "camera_view.h"
#include "model_copy_slot_u16_values.h"
#include "model_update_view_metrics.h"
#include "view_state.h"
#include "../psyq/rand.h"
#include "model_slot_queries.h"
#include "../unmatched.h"
#include "func_80057AF4.h"
#include "func_8002BAB4.h"
#include "library_state_handlers.h"
#include "func_8002A788.h"

extern u8 D_800EA1E8[];
/* Overlay-resident halfwords, addressed %hi/%lo in retail under -G8. */
extern s16 D_80181002 __attribute__((section(".data")));
extern s16 D_80181012 __attribute__((section(".data")));

/* Library screen state dispatcher on the low nibble of D_800EA1E8's first
 * byte. States 0, 1 and 3 hand the record to func_8002BAA0, func_8002A788
 * and func_8002BAAC. State 2 runs func_8002ACA4 and, when model slot 0
 * reports 1 from func_80058DD8, tracks the view: if func_80058E68 disagrees
 * it copies the slot's values to 0x80181010 and steps the model's +0x20
 * value towards D_80181012 by one plus a twenty-fourth of the distance
 * between D_80181012 and D_80181002; otherwise it walks +0x20 towards
 * D_80181002 by eight, counts down the record's +0x20 and, at zero, reloads
 * it to 300 and plays effect 2 or 7 through func_80057AF4 on a coin flip.
 * Either way the model's +2 halfword advances by 12, func_8001352C runs,
 * the eight words at +0x10 are copied into func_800591FC's block, and
 * Model_UpdateViewMetrics(0) runs. */
void func_8002BAB4(void)
{
    u8 *state;
    u8 *model;
    s32 one;
    s32 r;
    /* Retail keeps the difference in $s0 (the same register as the slot
       result, so it is that variable reused) and takes its absolute value
       in a $v0 copy, testing the sign on $s0. A plain temporary cannot hold
       that: cse rewrites whichever register is not canonical, so either
       the compare moves to the copy or the negation moves to the source.
       Only a user-named hard register is exempt from that rewrite. */
    register s32 a __asm__("$2");
    s32 step;
    s32 cur;
    s32 target;
    s32 count;
    s32 mode;
    u8 *dst;
    u8 *p;

    state = D_800EA1E8;
    one = 1;
    /* Two statements: a single-set byte would be launched next to the
       switch compare, retail loads it before the record pointer. */
    mode = D_800EA1E8[0];
    mode &= 0xF;
    switch (mode) {
    case 0:
        func_8002BAA0(state);
        break;
    case 1:
        func_8002A788(state);
        break;
    case 2:
        func_8002ACA4(state);
        model = (u8 *)&D_800F2848;
        r = func_80058DD8(0);
        if (r == one) {
            if (func_80058E68(0) != r) {
                Model_CopySlotU16Values(0, (u16 *)0x80181010);
                r = D_80181012 - D_80181002;
                /* This spelling puts the divide's magic constant ahead of
                   the sign test, where retail's scheduler fills the second
                   load's delay slot with its low half. */
                a = r >= 0 ? r : -r;
                step = a / 24 + 1;
                cur = *(s32 *)(model + 0x20);
                if (D_80181012 < cur) {
                    *(s32 *)(model + 0x20) = cur - step;
                    /* Re-read rather than forwarded, as retail does. */
                    cur = *(volatile s32 *)(model + 0x20);
                }
                if (cur < D_80181012) {
                    *(s32 *)(model + 0x20) = cur + step;
                }
            } else {
                r = *(s32 *)(model + 0x20);
                target = D_80181002;
                if (r != target) {
                    if (target < r) {
                        r -= 8;
                        if (r < target) {
                            r = target;
                        }
                    } else {
                        r += 8;
                        if (target < r) {
                            r = target;
                        }
                    }
                    *(s32 *)(model + 0x20) = r;
                }
                count = *(s32 *)(state + 0x20) - 1;
                *(s32 *)(state + 0x20) = count;
                if (count <= 0) {
                    *(s32 *)(state + 0x20) = 0x12C;
                    r = 2;
                    if (rand() & 1) {
                        r = 7;
                    }
                    func_80057AF4(0, r, 1);
                }
            }
        }
        /* A fresh pointer: this block is a join, so the address is
           materialised again rather than taken from the model pointer. */
        p = (u8 *)&D_800F2848;
        *(u16 *)(p + 2) += 0xC;
        func_8001352C();
        dst = func_800591FC();
        /* Two 16-byte copies rather than one 32-byte struct: the extra
           reference is what ranks the model pointer into $s2 ahead of the
           constant 1 in $s3. */
        ((LibraryViewQuad *)dst)[0] = *(LibraryViewQuad *)(model + 0x10);
        ((LibraryViewQuad *)dst)[1] = *(LibraryViewQuad *)(model + 0x20);
        Model_UpdateViewMetrics(0);
        break;
    case 3:
        func_8002BAAC(state);
        break;
    }
}

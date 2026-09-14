/* Volatile scratchpad stores and source reloads preserve retail ordering
 * around the official Psy-Q RTPS macros. */
#include "../types.h"
#include "duel_side_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "duel_card.h"
#include "screen_projection.h"
#include "view_state.h"
#include "duel_screen_tables.h"

/* Projects the thirty coordinate pairs in D_800908A0 through the GTE, one per
   iteration, and writes the biased screen pairs to D_800EA070. Same scratchpad
   and rtps as display_projection.c; the per-iteration form of func_80015DFC.

   The setup after the three calls is written out one statement at a time, in
   the order the target materialises it, because sched1 otherwise emits the two
   table addresses ahead of the counter and the scratchpad address. */
void func_800177C4(void)
{
    ProjectedPair p;
    volatile u8 *pad;
    ProjectedPair *pp;
    ScreenPair *out;
    u16 *src;
    s32 i;

    SetGeomScreen(D_800F2848.projection);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    pad = (u8 *)0x1F8003E0;
    i = 0;
    pp = &p;
    out = D_800EA070;
    src = D_800908A0;
    do {
        s32 y;

        {
            u16 x = src[0];

            *(volatile s16 *)(pad + 2) = 0;
            *(volatile u16 *)pad = x;
            x = *(volatile u16 *)(src + 1);
            *(volatile u16 *)(pad + 4) = x;
            gte_ldv0((u8 *)pad);
            gte_rtps();
            gte_stsxy(pp);
        }
        out->x = p.x - 0x1A;
        y = p.y;
        out->y = y - 0x1E;
        if (D_8009B1D5 != 0) {
            if (i < DUEL_CARD_SIDE_RECORD_COUNT) {
                out->y = y - 0x1D;
            }
        } else {
            if (i >= DUEL_CARD_SIDE_RECORD_COUNT) {
                out->y = y - 0x1D;
            }
        }
        out++;
        src += 2;
        i++;
    } while (i < DUEL_CARD_RECORD_COUNT);
    SetGeomOffset(0, 0);
}

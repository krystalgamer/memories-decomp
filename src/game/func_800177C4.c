#include "../types.h"
#include "../game/duel_side_state.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../game/duel_card.h"
#include "../game/screen_projection.h"
#include "../game/view_state.h"
#include "../game/duel_screen_tables.h"

extern ScreenPair D_800EA070[];

/* Projects the thirty coordinate pairs in D_800908A0 through the GTE, one per
   iteration, and writes the biased screen pairs to D_800EA070. Same scratchpad
   and rtps as display_projection.c; the per-iteration form of func_80015DFC.

   The setup after the three calls is written out one statement at a time, in
   the order the target materialises it, because sched1 otherwise emits the two
   table addresses ahead of the counter and the scratchpad address. */
void func_800177C4(void)
{
    ProjectedPair p;
    volatile s16 *pad;
    ProjectedPair *pp;
    ScreenPair *out;
    volatile u16 *src;
    s32 i;

    SetGeomScreen(D_800F2848.projection);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    pad = (volatile s16 *)0x1F8003E0;
    i = 0;
    pp = &p;
    out = D_800EA070;
    src = D_800908A0;
    do {
        s32 y;

        u16 value;
        u16 next_value;

        value = src[0];
        pad[1] = 0;
        pad[0] = value;
        next_value = src[1];
        pad[2] = next_value;
        gte_ldv0((s16 *)pad);
        gte_rtps();
        gte_stsxy(pp);
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

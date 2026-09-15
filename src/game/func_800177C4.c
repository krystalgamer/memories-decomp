/* Typed scratchpad stores and official Psy-Q GTE macros reproduce the
 * per-coordinate projection block through the RTPS-normalizing profile. */
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

/* Projects all duel-slot coordinates into the biased screen-position table. */
void func_800177C4(void)
{
    ProjectedPair p;
    SVECTOR *scratch;
    ProjectedPair *projected;
    ScreenPair *out;
    u16 *source;
    s32 i;

    SetGeomScreen(D_800F2848.projection);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    scratch = (SVECTOR *)0x1F8003E0;
    i = 0;
    projected = &p;
    out = D_800EA070;
    source = D_800908A0;
    do {
        s32 y;
        u16 x = source[0];

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz = source[1];
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(projected);
        out->x = p.x - 0x1A;
        y = p.y;
        out->y = y - 0x1E;
        if (D_8009B1D5 != 0) {
            if (i < DUEL_CARD_SIDE_RECORD_COUNT) {
                out->y = y - 0x1D;
            }
        } else if (i >= DUEL_CARD_SIDE_RECORD_COUNT) {
            out->y = y - 0x1D;
        }
        out++;
        source += 2;
        i++;
    } while (i < DUEL_CARD_RECORD_COUNT);
    SetGeomOffset(0, 0);
}

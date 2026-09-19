#include "../types.h"
#include "view_state.h"
#include "duel_card_layout.h"
#include "duel_display.h"
#include "duel_grid.h"
#define SORTED_ENTRY_STATE_IN_DATA
#include "sorted_entry.h"
#include "duel_card.h"
#include "display_projection.h"
#include "func_80015EF4.h"
#include "../unmatched.h"
#include "../game/func_800540B4.h"

/* Primes the two scratchpad primitives func_80015EF4 draws every field card
   with -- a POLY_FT4 at 0x1F800140 (len 9, code 0x2C, then 0x2E for
   semi-transparency) and the texture fields of a POLY_GT4 at 0x1F800180 --
   and hands each occupied card of both sides to it, or to func_80015DFC for
   cards flagged 0x400. */
void func_800164FC(void) {
    DuelCardRecord *e;
    DisplayObject *a;
    POLY_FT4 *p1;
    POLY_GT4 *p3;
    s32 *p4;
    s32 n;
    s32 h;
    s32 c;
    s32 f;
    ViewState *q;
    DuelCardRecord *t;

    q = &D_800F2848;
    SetGeomScreen(q->projection);
    SetGeomOffset(0xA0, 0x6C);
    SetFarColor(0, 0, 0);
    do {
        SetFogNearFar(0x28A, 0x320, q->projection);
        func_800540B4(2);

        if ((D_8009B30C & 2) != 0) {
            D_8009B314 = D_8009B314 + 1;
            if (D_8009B314 >= D_8009B308) {
                D_8009B30C = D_8009B30C & ~3;
            }
            D_8009B310 = D_8009B304;
        }

        p4 = (s32 *)0x1F8000C0;
        p1 = (POLY_FT4 *)0x1F800140;
        p3 = (POLY_GT4 *)0x1F800180;
        c = DUEL_DISPLAY_COLOR_NORMAL;
        e = D_801A7B64;
        n = 0;

        setlen(p1, 9);
        p1->code = 0x2C;
        p1->r0 = 0xFF;
        p1->g0 = 0xFF;
        p1->b0 = 0xFF;
        p1->tpage = 0x5F;
        f = 0x80;
        p1->clut = 0x3C11;
    } while (0);
    p1->u3 = 0xAF;
    p1->u1 = 0xAF;
    p1->v3 = 0x37;
    p1->v2 = 0x37;
    p1->code = 0x2E;
    p1->u2 = f;
    p1->u0 = f;
    p1->v1 = 0;
    p1->v0 = 0;
    p3->tpage = 0x9E;
    p3->clut = 0x3C50;
    p3->v1 = f;
    p3->v0 = f;
    p3->v3 = 0xBC;
    p3->v2 = 0xBC;
    *p4 = c;

    do {
        h = e->flags;
        if ((h & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            a = e->object;
            if (a != 0) {
                if ((h & 0x400) != 0) {
                    func_80015DFC((DisplayProjectionTrackedObject *)e);
                    n++;
                    goto next;
                }
                *p4 = a->field_0C;
                func_80015EF4(e, p3, p1, p4);
            }
        }
        n++;
    next:
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    t = D_801A7B64;
    e = t + DUEL_CARD_SIDE_RECORD_COUNT;
    n = 0;
    do {
        h = e->flags;
        if ((h & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            a = e->object;
            if (a != 0) {
                if ((h & 0x400) != 0) {
                    func_80015DFC((DisplayProjectionTrackedObject *)e);
                    n++;
                    goto next2;
                }
                *p4 = a->field_0C;
                func_80015EF4(e, p3, p1, p4);
            }
        }
        n++;
    next2:
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    SetGeomOffset(0, 0);
}

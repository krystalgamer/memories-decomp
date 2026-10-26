#include "../types.h"
#include "view_state.h"
#include "duel_card_layout.h"
#include "duel_display.h"
#include "duel_grid.h"
#define SORTED_ENTRY_STATE_IN_DATA
#include "sorted_entry.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "display_projection.h"
#include "duel_field_card_render.h"
#include "../unmatched.h"
#include "../game/model_draw_slot.h"
#include "view_state_current.h"

/* Regional values: the two CLUTs (0x3C11 -> 0x3829, 0x3C50 -> 0x3868). The
 * European build (src/game/european/) defines its own. */
#ifndef DUEL_FIELD_CARDS_CLUT_1
#define DUEL_FIELD_CARDS_CLUT_1 0x3C11
#endif
#ifndef DUEL_FIELD_CARDS_CLUT_3
#define DUEL_FIELD_CARDS_CLUT_3 0x3C50
#endif

/* Primes the two scratchpad primitives Duel_RenderFieldCard3D draws every field card
   with -- a POLY_FT4 at 0x1F800140 (len 9, code 0x2C, then 0x2E for
   semi-transparency) and the texture fields of a POLY_GT4 at 0x1F800180 --
   and hands each occupied card of both sides to it, or to Duel_DrawProjectedFieldCardFrame for
   cards flagged 0x400. */
void Duel_DrawFieldCards(void) {
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

    q = &gViewState_Current;
    SetGeomScreen(q->projection);
    SetGeomOffset(0xA0, 0x6C);
    SetFarColor(0, 0, 0);
    do {
        SetFogNearFar(0x28A, 0x320, q->projection);
        Model_DrawSlot(2);

        if ((gModel_dwFogDrawFlags & 2) != 0) {
            gSortedEntry_nCount = gSortedEntry_nCount + 1;
            if (gSortedEntry_nCount >= gSortedEntry_nSortedCount) {
                gModel_dwFogDrawFlags = gModel_dwFogDrawFlags & ~3;
            }
            gSortedEntry_pCursor = gSortedEntry_pBase;
        }

        p4 = (s32 *)0x1F8000C0;
        p1 = (POLY_FT4 *)0x1F800140;
        p3 = (POLY_GT4 *)0x1F800180;
        c = DUEL_DISPLAY_COLOR_NORMAL;
        e = gDuel_aFieldCardRecords;
        n = 0;

        setlen(p1, 9);
        p1->code = 0x2C;
        p1->r0 = 0xFF;
        p1->g0 = 0xFF;
        p1->b0 = 0xFF;
        p1->tpage = 0x5F;
        f = 0x80;
        p1->clut = DUEL_FIELD_CARDS_CLUT_1;
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
    p3->clut = DUEL_FIELD_CARDS_CLUT_3;
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
                    Duel_DrawProjectedFieldCardFrame(e);
                    n++;
                    goto next;
                }
                *p4 = a->rgb;
                Duel_RenderFieldCard3D(e, p3, p1, p4);
            }
        }
        n++;
    next:
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    t = gDuel_aFieldCardRecords;
    e = t + DUEL_CARD_SIDE_RECORD_COUNT;
    n = 0;
    do {
        h = e->flags;
        if ((h & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            a = e->object;
            if (a != 0) {
                if ((h & 0x400) != 0) {
                    Duel_DrawProjectedFieldCardFrame(e);
                    n++;
                    goto next2;
                }
                *p4 = a->rgb;
                Duel_RenderFieldCard3D(e, p3, p1, p4);
            }
        }
        n++;
    next2:
        e++;
    } while (n < DUEL_FIELD_SIDE_ZONE_COUNT);

    SetGeomOffset(0, 0);
}

/* The duel field's per-card draw paths, in address order:
   Duel_DrawProjectedFieldCardFrame projects a card's ground position and
   draws its card frame at that screen point, and Duel_RenderFieldCard3D
   draws the card as a lit 3D quad with a projected ground sprite. Both are
   called from Duel_DrawFieldCards and build with the RTPS-normalizing
   profile. Both use ordering-table slot 2, but only
   Duel_DrawProjectedFieldCardFrame names its interior symbol gGraphics_pOrderingTable2
   (as gGraphics_pOrderingTable2[0]); Duel_RenderFieldCard3D reaches it as gGraphics_apOrderingTables[2]. */

#define ORDERING_TABLE_SLOT2_ARRAY

#include "../types.h"
#include "../ygo_types.h"
#include "duel_side_state.h"
#include "display_object.h"
#include "display_projection.h"
#include "screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "duel_draw_card_frame.h"
#include "ordering_tables.h"
#include "duel_field_card_render.h"
#include "graphics_ordering_table_views.h"

/* Volatile scratchpad stores and the second record reload preserve retail
 * ordering around the official Psy-Q RTPS macros. The scratchpad word is the
 * SVECTOR gte_ldv0 loads, and the qualifier lives on that pointer, so its
 * three stores are plain member stores.
 *
 * Immediately follows DisplayObject_ProjectLinkedGroundPosition in the image and shares its projection
 * setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
 * swc2 $14. Here the source coordinates come from the object's own target
 * record and the projected pair is biased into the object's screen fields. */
void Duel_DrawProjectedFieldCardFrame(DuelCardRecord *card)
{
    ProjectedPair p;
    s32 y;

    GsSetLsMatrix(&gGraphics_LightSourceMatrix);
    {
        u16 x = ((DisplayObject *)card->object)->field_30.h.field_30;
        volatile SVECTOR *scratch = (volatile SVECTOR *)0x1F8003E0;

        scratch->vy = 0;
        scratch->vx = x;
        scratch->vz =
            *(u16 *)&((DisplayObject *)((volatile DuelCardRecord *)card)
                         ->object)->field_34.h.field_34;
        gte_ldv0((u8 *)scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    card->screen_x = p.x - 0x1A;
    y = p.y;
    card->screen_y = y - 0x1E;
    if (gDuel_bActiveSide != 0) {
        if ((s8)card->table_index < 0xF) {
            card->screen_y = y - 0x1D;
        }
    } else {
        if ((s8)card->table_index >= 0xF) {
            card->screen_y = y - 0x1D;
        }
    }
    Duel_DrawCardFrame((DisplayObject *)card->object, gGraphics_pOrderingTable2[0], card->screen_x,
                  card->screen_y);
    SetGeomOffset(0xA0, 0x6C);
}

/* Regional values: the CLUT's y base and its x in 16-pixel units (0xF1 ->
 * 0xE1, 0x10 -> 0x28). The European build (src/game/european/) defines its
 * own. */
#ifndef DUEL_FIELD_CARD_CLUT_Y
#define DUEL_FIELD_CARD_CLUT_Y 0xF1
#endif
#ifndef DUEL_FIELD_CARD_CLUT_X16
#define DUEL_FIELD_CARD_CLUT_X16 0x10
#endif

#define CVECTOR_VIEW(color) ((CVECTOR *)(color))

/* Draws one 3D duel card. The display object's Euler bytes and position build
 * a local matrix in the scratchpad, four card corners are rotated, lit with
 * RotColorDpq into the POLY_GT4 and sorted at their mean depth, and the same
 * corners flattened to y = 0 are projected with RTPS into the POLY_FT4 that
 * is sorted at the back as the card's ground sprite.
 *
 * What the retail listing fixes about the source:
 * - The scale test reads the 16-bit flags word against GsROTOFF (1 << 27).
 *   cse cannot see that the mask misses every bit of a halfword; combine can,
 *   but only after the test is its own instruction, so retail keeps a literal
 *   zero, a branch that never goes, and the dead temporary's stack slot.
 * - The matrix, the first corner, the scale, the light normal, the rotation
 *   and the depth words are pointers set once after the guard, in address
 *   order. Corners 1-3 and copies 1-3 get their own pointers where they are
 *   first used; copy 0 is rot[4].
 * - Each chained store writes the right-hand corner first.
 * - The RTPS inputs are literal scratchpad addresses, which reload
 *   materialises in $t0-$t3.
 * - u0 and v0 are both read back before either is biased, and setPolyGT4
 *   follows the clut store.
 *
 * The GTE words come from the official Psy-Q inline_c.h macros; the profile
 * expands Psy-Q's RTPS placeholder to the real COP2 opcode. */
void Duel_RenderFieldCard3D(DuelCardRecord *record, POLY_GT4 *prim, POLY_FT4 *sprite, s32 *color)
{
    DuelCardRecord *holder = record;
    GsOT *G32 *tab;
    DisplayObject *obj;
    s32 a;
    s32 c;
    s32 d;
    s32 y;
    MATRIX *lm;
    SVECTOR *q;
    VECTOR *scale;
    SVECTOR *up;
    SVECTOR *rot;
    s32 *depth;
    SVECTOR *q1;
    SVECTOR *q2;
    SVECTOR *q3;
    SVECTOR *c1;
    SVECTOR *c2;
    SVECTOR *c3;

    obj = (DisplayObject *)holder->object;
    if ((obj->flags & 0x40) == 0) {
        return;
    }

    lm = (MATRIX *)0x1F8002C0;
    q = (SVECTOR *)0x1F800300;
    scale = (VECTOR *)0x1F800320;
    up = (SVECTOR *)0x1F800380;
    rot = (SVECTOR *)0x1F8003A0;
    depth = (s32 *)0x1F8003E0;
    up->vx = 0;
    up->vy = 0x1000;
    up->vz = 0;
    *color = obj->rgb;
    rot[0].vx = obj->field_20.b.field_20 << 4;
    y = obj->field_20.b.field_21 << 4;
    rot[0].vy = y;
    rot[0].vz = obj->field_20.b.field_22 << 4;
    if ((s8)holder->table_index >= 0xF) {
        rot[0].vy = y + 0x800;
    }
    lm->t[0] = (s16)obj->field_30.h.field_30;
    a = (s16)obj->field_30.h.field_32;
    lm->t[1] = a;
    d = obj->field_34.h.field_34;
    lm->t[2] = d;
    a += 0xFF;
    if (a < 0) {
        a = 0;
    }
    sprite->b0 = a;
    sprite->g0 = a;
    sprite->r0 = a;
    RotMatrixZYX_gte(rot, lm);
    if ((obj->flags & GsROTOFF) == 0) {
        scale->vx = obj->field_44.h.field_44;
        scale->vy = 0x1000;
        scale->vz = obj->field_44.h.field_46;
        ScaleMatrix(lm, scale);
    }

    q2 = (SVECTOR *)0x1F800310;
    q3 = (SVECTOR *)0x1F800318;
    q1 = (SVECTOR *)0x1F800308;
    q[0].vx = q2->vx = -0x19;
    q[1].vx = q3->vx = 0x1A;
    q[0].vz = q1->vz = 0x1D;
    q[2].vz = q3->vz = -0x1E;
    q[0].vy = q1->vy = q2->vy = q3->vy = 0;
    GsSetLsMatrix(lm);

    RotTransSV(&q[0], &rot[0], (PSXLONG *)depth);
    RotTransSV(&q[1], &rot[1], (PSXLONG *)depth);
    RotTransSV(&q[2], &rot[2], (PSXLONG *)depth);
    RotTransSV(&q[3], &rot[3], (PSXLONG *)depth);

    c1 = (SVECTOR *)0x1F8003C8;
    c2 = (SVECTOR *)0x1F8003D0;
    c3 = (SVECTOR *)0x1F8003D8;
    *(Bytes8 *)&rot[4] = *(Bytes8 *)&rot[0];
    *(Bytes8 *)c1 = *(Bytes8 *)&rot[1];
    *(Bytes8 *)c2 = *(Bytes8 *)&rot[2];
    *(Bytes8 *)c3 = *(Bytes8 *)&rot[3];

    GsSetLightMatrix(lm);
    GsSetLsMatrix(&gGraphics_LightSourceMatrix);

    depth[4] = RotColorDpq(&rot[0], up, CVECTOR_VIEW(color),
                (PSXLONG *)&prim->x0, CVECTOR_VIEW(&prim->r0),
                (PSXLONG *)&depth[0]);
    depth[5] = RotColorDpq(&rot[1], up, CVECTOR_VIEW(color),
                (PSXLONG *)&prim->x1, CVECTOR_VIEW(&prim->r1),
                (PSXLONG *)&depth[1]);
    depth[6] = RotColorDpq(&rot[2], up, CVECTOR_VIEW(color),
                (PSXLONG *)&prim->x2, CVECTOR_VIEW(&prim->r2),
                (PSXLONG *)&depth[2]);
    depth[7] = RotColorDpq(&rot[3], up, CVECTOR_VIEW(color),
                (PSXLONG *)&prim->x3, CVECTOR_VIEW(&prim->r3),
                (PSXLONG *)&depth[3]);

    if ((depth[0] | depth[1] | depth[2]
         | depth[3]) < 0) {
        return;
    }

    prim->u0 = prim->u2 = ((u8 *)&obj->field_5C)[0];
    prim->v0 = prim->v1 = ((u8 *)&obj->field_5C)[1];
    if (NormalClip(*(PSXLONG *)&prim->x0, *(PSXLONG *)&prim->x1,
                   *(PSXLONG *)&prim->x2) <= 0) {
        prim->u2 = 0x38;
        prim->u0 = 0x38;
        prim->v1 = 0x80;
        prim->v0 = 0x80;
    }
    c = prim->u0;
    d = prim->v0;
    c += 0x34;
    d += 0x3C;
    prim->u3 = c;
    prim->u1 = c;
    prim->v3 = d;
    prim->v2 = d;
    prim->clut = (((u16)obj->field_40.h.field_42 + DUEL_FIELD_CARD_CLUT_Y) << 6) | DUEL_FIELD_CARD_CLUT_X16;
    setPolyGT4(prim);
    SetSemiTrans(prim, 0);
    prim->tpage = prim->tpage & 0xFF9F;
    prim->tpage = prim->tpage | ((obj->attribute >> 23) & 0x60);
    if ((obj->attribute & GsALON) != 0) {
        SetSemiTrans(prim, 1);
        sprite->r0 = ((u8 *)&obj->rgb)[0];
        sprite->g0 = ((u8 *)&obj->rgb)[1];
        sprite->b0 = ((u8 *)&obj->rgb)[2];
    }

    depth[0] = (depth[4] + depth[5]
                          + depth[6] + depth[7]) / 4 >> 2;
    tab = gGraphics_apOrderingTables;
    GsSortPoly(prim, tab[2], *(u16 *)depth);

    c3->vy = 0;
    c2->vy = 0;
    c1->vy = 0;
    rot[4].vy = 0;
    gte_ldv0((SVECTOR *)0x1F8003C0);
    gte_rtps();
    gte_stsxy(&sprite->x0);
    gte_ldv0((SVECTOR *)0x1F8003C8);
    gte_rtps();
    gte_stsxy(&sprite->x1);
    gte_ldv0((SVECTOR *)0x1F8003D0);
    gte_rtps();
    gte_stsxy(&sprite->x2);
    gte_ldv0((SVECTOR *)0x1F8003D8);
    gte_rtps();
    gte_stsxy(&sprite->x3);
    GsSortPoly(sprite, tab[2], 0xFFF);
}

#include "../../types.h"
#include "variant388_strips.h"
#include "../spanish_model_variant/variant388_entry.h"

/* Projects five cross sections along the path from the matrix translation
 * along the aim direction, scaled by the progress half, and draws two
 * POLY_GT4 strips between them, then advances the grow and shrink timers
 * against the timing record.
 *
 * The second strip reads its first columns through col/next, and both are
 * then overwritten before the flag store and kept live after the loop.
 * GCC 2.8.1 CSE then keeps col/next as copies of the first strip's column
 * pointers, while the flag and sort code goes back to the originals, as in
 * the target.
 *
 * MODEL_VARIANT388_STRIPS_SHARED_COLUMN spells the second strip's reads as
 * col->sc[1] and col->sb[1] through the one column pointer instead of a
 * separate next pointer; both address sc[k + 1] and sb[k + 1]. The North
 * American header-371 images built with the CDK compiler use it. */
void FrenchModelVariant388_DrawStrips(u8 *ctx)
{
    Variant388EntryState *work;
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    GsOT *ot;
    Strip388Unit *unit;
    POLY_GT4 *poly;
    Strip388Timing *rec;
    Strip388Unit *col;
    Strip388Unit *next;
    s16 i;
    s16 k;
    s16 width;
    s32 angle;

    work = (Variant388EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (work->phase > 0) {
        unit = (Strip388Unit *)work;
        width = work->strip_width / 32;
        for (i = 0; i < 1; i++, unit++) {
            for (k = 0; k < 5; k++) {
                setVector(&unit->a[k], rcos(0x400) * width >> 12, rsin(0x400) * width >> 12, 0);
                setVector(&unit->b[k], 0, 0, 0);
                setVector(&unit->c[k], rcos(0xC00) * width >> 12, rsin(0xC00) * width >> 12, 0);
                rot.vx = 0;
                rot.vy = 0;
                rot.vz = angle;
                if (k == 0) {
                    m.t[0] = work->matrix.t[0];
                    m.t[1] = work->matrix.t[1];
                    m.t[2] = work->matrix.t[2];
                } else {
                    m.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->progress / 1024 * k / 4;
                    m.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->progress / 1024 * k / 4;
                    m.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->progress / 1024 * k / 4;
                }
                scale.vx = 0x1000;
                scale.vy = 0x1000;
                scale.vz = 0x1000;
                RotMatrix(&rot, &m);
                coord.coord = m;
                coord.super = 0;
                coord.flg = 0;
                GsGetLs(&coord, &ls);
                GsSetLsMatrix(&ls);
                ReadRotMatrix(&ls);
                RotMatrix(&rot, &ls);
                ScaleMatrix(&ls, &scale);
                SetRotMatrix(&ls);
                unit->otz[k] = RotTransPers3(&unit->a[k], &unit->b[k], &unit->c[k], &unit->sa[k],
                                             &unit->sb[k], &unit->sc[k], &p, &unit->flag[k]);
            }
        }
        unit = (Strip388Unit *)work;
        for (i = 0; i < 1; i++, unit++) {
            for (k = 0; k < 4; k++) {
                poly->x0 = unit->sa[k];
                poly->y0 = (unit->sa[k] >> 16);
                poly->x1 = unit->sa[k + 1];
                poly->y1 = (unit->sa[k + 1] >> 16);
                poly->x2 = unit->sb[k];
                poly->y2 = (unit->sb[k] >> 16);
                poly->x3 = unit->sb[k + 1];
                poly->y3 = (unit->sb[k + 1] >> 16);
                poly->r0 = 0x80;
                poly->g0 = 0;
                poly->b0 = 0x80;
                poly->r1 = 0x80;
                poly->g1 = 0;
                poly->b1 = 0x80;
                poly->r2 = 0xC0;
                poly->g2 = 0xC0;
                poly->b2 = 0xC0;
                poly->r3 = 0xC0;
                poly->g3 = 0xC0;
                poly->b3 = 0xC0;
                if (unit->otz[k] < 0) {
                    unit->otz[k] = 0;
                }
                unit->flag[k] = 0;
                if (unit->otz[k] >= 0) {
                    GsSortPoly(poly, ot, unit->otz[k + 1]);
                }
#ifdef MODEL_VARIANT388_STRIPS_SHARED_COLUMN
                col = (Strip388Unit *)((u8 *)unit + k * 4);
                poly->x0 = col->sc[0];
                poly->y0 = (col->sc[0] >> 16);
                poly->x1 = col->sc[1];
                poly->y1 = (col->sc[1] >> 16);
                poly->x2 = col->sb[0];
                poly->y2 = (col->sb[0] >> 16);
                poly->x3 = col->sb[1];
                poly->y3 = (col->sb[1] >> 16);
#else
                col = (Strip388Unit *)((u8 *)unit + k * 4);
                next = (Strip388Unit *)((u8 *)unit + (k + 1) * 4);
                poly->x0 = col->sc[0];
                poly->y0 = (col->sc[0] >> 16);
                poly->x1 = next->sc[0];
                poly->y1 = (next->sc[0] >> 16);
                poly->x2 = col->sb[0];
                poly->y2 = (col->sb[0] >> 16);
                poly->x3 = next->sb[0];
                poly->y3 = (next->sb[0] >> 16);
#endif
                poly->r0 = 0x80;
                poly->g0 = 0;
                poly->b0 = 0x80;
                poly->r1 = 0x80;
                poly->g1 = 0;
                poly->b1 = 0x80;
                poly->r2 = 0xC0;
                poly->g2 = 0xC0;
                poly->b2 = 0xC0;
                poly->r3 = 0xC0;
                poly->g3 = 0xC0;
                poly->b3 = 0xC0;
                if (col->otz[0] < 0) {
                    col->otz[0] = 0;
                }
                col = next = (Strip388Unit *)work;
                unit->flag[k] = 0;
                if (unit->otz[k] >= 0) {
                    GsSortPoly(poly, ot, unit->otz[k + 1]);
                }
            }
        }
        col = next = (Strip388Unit *)work;
        rec = (Strip388Timing *)work->config;
        if (work->index + 1 == rec->frame) {
            if (work->phase == 1 && work->progress <= 0x400) {
                if (rec->grow_start <= (u32)work->clock.frame) {
                    work->progress = ((u32)work->clock.frame - rec->grow_start << 10) / (rec->grow_end - rec->grow_start);
                }
                if (work->progress >= 0x400) {
                    work->progress = 0x400;
                    work->phase = 2;
                }
            }
            if (work->strip_width > 0) {
                rec = (Strip388Timing *)work->config;
                if ((u32)work->clock.frame >= rec->shrink_start) {
                    work->strip_width = 0x400 - ((u32)work->clock.frame - rec->shrink_start << 10) / (rec->shrink_end - rec->shrink_start);
                }
                if (work->strip_width <= 0) {
                    work->strip_width = 0;
                    if (work->phase == 3) {
                        work->phase = 4;
                    }
                }
            }
        }
    }
}

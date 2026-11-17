#include "../../types.h"

#include "../model_variant/variant405_entry.h"

/* Places each shard's two halves at origin + direction * scale / 1024 (scale
 * clamped at zero), turned by the ratan2 heading plus a half turn, and
 * projects them with RotTransPers3. A second pass sorts one white-to-grey
 * POLY_GT4 per shard whose second scale is below 0x800, then grows both
 * scales by step * 128 up to 0x800 while the shard state is 1, moving the
 * phase from 1 to 2 once the first scale reaches 0x400. The empty loops keep
 * the retail scheduling of the heading and the clamp. */
void FrenchModelVariant562_DrawShards(Variant405EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[3][2];
    long p;
    GsOT *ot;
    s32 angle;
    Variant405EntryState *work;
    POLY_GT4 *poly;
    Variant405EntryRing *rec;
    s16 j;
    s16 i;
    s32 v;
    s32 s;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    v = ratan2(work->screen_delta.vy, work->screen_delta.vx);
    poly = work->glyphs;
    do {
    } while (0);
    angle = v + 0x800;
    rec = work->rings;
    for (j = 0; j < work->config->count; j++, rec++) {
        for (i = 0; i < 2; i++) {
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (i == 0) {
                v = rec->progress;
            } else {
                v = rec->tail_progress;
            }
            s = 0;
            if (v >= 0) {
                do {
                    s = v;
                } while (0);
            }
            m.t[0] = work->origin.vx + work->direction.vx * s / 1024;
            m.t[1] = work->origin.vy + work->direction.vy * s / 1024;
            m.t[2] = work->origin.vz + work->direction.vz * s / 1024;
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
            rec->otz[i] = RotTransPers3(&rec->a[i], &rec->b[i], &rec->c[i],
                                        &rec->sa[i], &rec->sb[i], &rec->sc[i],
                                        &p, &flags[j][i]);
        }
    }

    rec = work->rings;
    for (j = 0; j < work->config->count; j++, rec++) {
        if (rec->tail_progress < 0x800) {
            poly->r0 = 0xFF;
            poly->g0 = 0xFF;
            poly->b0 = 0xFF;
            poly->r1 = 0x10;
            poly->g1 = 0x10;
            poly->b1 = 0x10;
            poly->r2 = 0xFF;
            poly->g2 = 0xFF;
            poly->b2 = 0xFF;
            poly->r3 = 0x10;
            poly->g3 = 0x10;
            poly->b3 = 0x10;
            for (i = 0; i < 1; i++) {
                poly->x0 = rec->sa[i];
                poly->y0 = rec->sa[i] >> 16;
                poly->x1 = rec->sa[i + 1];
                poly->y1 = rec->sa[i + 1] >> 16;
                poly->x2 = rec->sc[i];
                poly->y2 = rec->sc[i] >> 16;
                poly->x3 = rec->sc[i + 1];
                poly->y3 = rec->sc[i + 1] >> 16;
                if (rec->otz[i] >= 0 && flags[j][i] >= 0) {
                    GsSortPoly(poly, ot, rec->otz[i]);
                }
            }
        }
        if (rec->started == 1) {
            if (rec->progress < 0x800) {
                if (rec->progress >= 0x400 && work->phase == 1) {
                    work->phase = 2;
                }
                rec->progress += work->clock.step * 128;
                if (rec->progress >= 0x800) {
                    rec->progress = 0x800;
                }
            }
            if (rec->tail_progress < 0x800) {
                rec->tail_progress += work->clock.step * 128;
                if (rec->tail_progress >= 0x800) {
                    rec->tail_progress = 0x800;
                }
            }
        }
    }
}

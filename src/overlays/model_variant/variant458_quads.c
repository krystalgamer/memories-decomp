#include "../../types.h"
#include "../french_model_variant/variant475_entry.h"

/* Draws the visible quads, fading past scale 0x600, then advances the
 * timed quad count. The packet is reused for each submitted quad. */
void ModelVariant458_DrawQuads(u8 *ctx)
{
    SVECTOR rot;
    /* Retail reserves 16 bytes between rotation and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ModelVariantSheet *sheet;
    GsOT *ot;
    s16 i;
    s16 j;
    u8 r, g, b;
    Variant475EntryState *work;
    Variant475EntryQuads *quads;
    POLY_FT4 *poly;
    s32 fade;
    s32 otz;

    work = (Variant475EntryState *)ctx;
    sheet = (ModelVariantSheet *)work->sheets;
    quads = work->quads;
    poly = &work->extra_flat;
    ot = Graphics_GetDefaultOrderingTable();
    i = 0;
    ratan2(work->direction.vy, work->direction.vz);
    ratan2(work->screen_delta.vy, work->screen_delta.vx);
    do {
        j = 0;
        while (j < work->quad_count) {
            if (quads->level[j] > 0x600) {
                fade = 0x800 - quads->level[j];
                r = quads->color.r * fade / 512;
                g = quads->color.g * fade / 512;
                b = quads->color.b * fade / 512;
            } else {
                r = quads->color.r;
                g = quads->color.g;
                b = quads->color.b;
            }
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = work->starts[j].vx;
            m.t[1] = work->starts[j].vy;
            m.t[2] = work->starts[j].vz;
            if (work->tail.phase >= 4) {
                scale.vx = sheet->size;
                scale.vy = sheet->size;
                scale.vz = sheet->size;
            } else {
                scale.vx = 0x1000;
                scale.vy = 0x1000;
                scale.vz = 0x1000;
            }
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
            otz = RotTransPers4(&quads->a[j], &quads->b[j], &quads->c[j], &quads->d[j],
                (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                (long *)&poly->x3, &p, &flag);
            poly->r0 = r;
            poly->g0 = g;
            poly->b0 = b;
            if (otz >= 0 && flag >= 0 && quads->hidden[j] == 0) {
                GsSortPoly(poly, ot, otz);
            }
            /* Keeping the increment in the body preserves retail scheduling. */
            j++;
        }
        i++;
        quads++;
    } while (i < 1);
    if (work->quad_count < 8) {
        work->quad_count =
            ((work->clock.frame - work->config->start) << 3) /
            (work->config->field_24 - work->config->start);
        if (work->quad_count >= 8 && work->tail.phase == 0) {
            work->tail.phase = 1;
        }
    }
}

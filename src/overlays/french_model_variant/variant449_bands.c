#include "../../types.h"
#include "../model_variant/variant432_entry.h"

void FrenchModelVariant449_DrawBands(Variant432EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant432EntryState *work;
    GsOT *ot;
    s16 i;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBandTriple *band;
    s16 j;
    s16 sign;
    s16 bias;
    s32 lift;
    s32 progress;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    angle = ratan2(work->screen_delta.vy, work->screen_delta.vx) + 0x800;
    poly = &work->packets.textured[1];
    if (!(work->frame_count & 1)) {
        bias = 0;
    } else {
        bias = work->scale / 8;
    }
    if (work->tail.slot == 0) {
        sign = -1;
    } else {
        sign = 1;
    }
    band = work->records;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 3; j++) {
            lift = band->offset[j] * sign * work->scale / 4096;
            if (work->factor <= 0) {
                progress = 0;
            } else if (work->factor < 1024) {
                progress = work->factor;
            } else {
                progress = 1024;
            }
            if (j == 0) {
                progress += work->scale / 16 * work->spread / 1024;
            }
            setVector(&band->start[j], rcos(0x400) * 128 >> 12, rsin(0x400) * 128 >> 12, 0);
            setVector(&band->middle[j], 0, 0, 0);
            setVector(&band->end[j], rcos(0xC00) * 128 >> 12, rsin(0xC00) * 128 >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            m.t[0] = work->origin.vx + work->path.vx * progress / 1024;
            m.t[1] = work->origin.vy + work->path.vy * progress / 1024;
            m.t[2] = work->origin.vz + work->path.vz * progress / 1024 + lift;
            scale.vx = work->scale + bias;
            scale.vy = work->scale + bias;
            scale.vz = work->scale + bias;
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
            band->depth[j] = RotTransPers3(&band->start[j], &band->middle[j], &band->end[j],
                                         &band->start_screen[j], &band->middle_screen[j], &band->end_screen[j], &p, &flag);
        }
    }
    band = work->records;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                poly->x0 = band->start_screen[j];
                poly->y0 = band->start_screen[j] >> 16;
                poly->x1 = band->start_screen[j + 1];
                poly->y1 = band->start_screen[j + 1] >> 16;
                poly->x2 = band->middle_screen[j];
                poly->y2 = band->middle_screen[j] >> 16;
                poly->x3 = band->middle_screen[j + 1];
                poly->y3 = band->middle_screen[j + 1] >> 16;
            } else {
                poly->x0 = band->start_screen[j + 1];
                poly->y0 = band->start_screen[j + 1] >> 16;
                poly->x1 = band->start_screen[j];
                poly->y1 = band->start_screen[j] >> 16;
                poly->x2 = band->middle_screen[j + 1];
                poly->y2 = band->middle_screen[j + 1] >> 16;
                poly->x3 = band->middle_screen[j];
                poly->y3 = band->middle_screen[j] >> 16;
            }
            poly->r0 = band->inner[j].r;
            poly->g0 = band->inner[j].g;
            poly->b0 = band->inner[j].b;
            poly->r1 = band->inner[j + 1].r;
            poly->g1 = band->inner[j + 1].g;
            poly->b1 = band->inner[j + 1].b;
            poly->r2 = band->inner[j + 1].r;
            poly->g2 = band->inner[j + 1].g;
            poly->b2 = band->inner[j + 1].b;
            poly->r3 = band->outer[j].r;
            poly->g3 = band->outer[j].g;
            poly->b3 = band->outer[j].b;
            if (band->depth[j] > 0) {
                GsSortPoly(poly, ot, band->depth[j]);
            }
            if (j == 0) {
                poly->x0 = band->end_screen[j];
                poly->y0 = band->end_screen[j] >> 16;
                poly->x1 = band->end_screen[j + 1];
                poly->y1 = band->end_screen[j + 1] >> 16;
                poly->x2 = band->middle_screen[j];
                poly->y2 = band->middle_screen[j] >> 16;
                poly->x3 = band->middle_screen[j + 1];
                poly->y3 = band->middle_screen[j + 1] >> 16;
            } else {
                poly->x0 = band->end_screen[j + 1];
                poly->y0 = band->end_screen[j + 1] >> 16;
                poly->x1 = band->end_screen[j];
                poly->y1 = band->end_screen[j] >> 16;
                poly->x2 = band->middle_screen[j + 1];
                poly->y2 = band->middle_screen[j + 1] >> 16;
                poly->x3 = band->middle_screen[j];
                poly->y3 = band->middle_screen[j] >> 16;
            }
            poly->r0 = band->inner[j].r;
            poly->g0 = band->inner[j].g;
            poly->b0 = band->inner[j].b;
            poly->r1 = band->inner[j + 1].r;
            poly->g1 = band->inner[j + 1].g;
            poly->b1 = band->inner[j + 1].b;
            poly->r2 = band->inner[j + 1].r;
            poly->g2 = band->inner[j + 1].g;
            poly->b2 = band->inner[j + 1].b;
            poly->r3 = band->outer[j].r;
            poly->g3 = band->outer[j].g;
            poly->b3 = band->outer[j].b;
            if (band->depth[j] > 0) {
                GsSortPoly(poly, ot, band->depth[j]);
            }
        }
    }
}

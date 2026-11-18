#include "../../types.h"
#include "h433_entry.h"

void SpanishModelVariant450_DrawQuadGroups(Variant433EntryState *ctx)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long interpolation;
    long flag;
    Variant433EntryState *work;
    Variant433EntrySheet *group;
    Variant450Primary *primary;
    Variant450Primary *primary_base;
    POLY_GT4 *quad;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j;
    s32 extra;
    s32 depth;
    s32 primary_index;
    u8 r, g, b, r1, g1, b1;

    work = ctx;
    group = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->packets.textured[1];
    /* Retail addresses each ribbon from its phase words (+0x1D0), so the
     * primary records start there and keep their own view. */
    primary_base = (Variant450Primary *)work->ribbons[0].phase;
    primary_index = 0;
    for (i = 0; i < 7; i++, group++) {
        primary = primary_base + primary_index;
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->matrix.t[0];
            matrix.t[1] = work->matrix.t[1];
            matrix.t[2] = work->matrix.t[2];
            extra = 0;
            if (work->clock.frame_count & 1) {
                extra = group->size / 8;
            }
        } else {
            matrix.t[0] = primary->translation.vx;
            matrix.t[1] = primary->translation.vy;
            matrix.t[2] = primary->translation.vz;
            extra = 0;
            if (work->clock.frame_count & 1) {
                extra = group->size / 4;
            }
        }
        scale.vx = group->size + extra;
        scale.vy = group->size + extra;
        scale.vz = group->size + extra;
        RotMatrix(&rotation, &matrix);
        coord.coord = matrix;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rotation, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (j = 0, point = group->v0; j < 4; j++, point++) {
            depth = RotTransPers4(point, &group->v1[j], &group->v2[j],
                                 &group->v3[j], (long *)&quad->x0,
                                 (long *)&quad->x1, (long *)&quad->x2,
                                 (long *)&quad->x3, &interpolation, &flag);
            if (group->fading == 0) {
                setRGB0(quad, group->inner.r, group->inner.g, group->inner.b);
                setRGB1(quad, group->inner.r, group->inner.g, group->inner.b);
                setRGB2(quad, group->inner.r, group->inner.g, group->inner.b);
                setRGB3(quad, group->outer.r, group->outer.g, group->outer.b);
            } else {
                r = group->inner.r * group->brightness / 1024;
                g = group->inner.g * group->brightness / 1024;
                b = group->inner.b * group->brightness / 1024;
                r1 = group->outer.r * group->brightness / 1024;
                g1 = group->outer.g * group->brightness / 1024;
                b1 = group->outer.b * group->brightness / 1024;
                setRGB0(quad, r, g, b);
                setRGB1(quad, r, g, b);
                setRGB2(quad, r, g, b);
                setRGB3(quad, r1, g1, b1);
            }
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
        if (i == 0) {
            if (work->tail.phase == 0 && group->size < 4096) {
                group->size = (work->clock.frame-work->config->trigger) * 4096 /
                               (work->config->grow_end-work->config->trigger);
                if (group->size >= 4096) {
                    group->size = 4096;
                    work->tail.phase = 1;
                }
            }
            if (work->clock.frame >= work->config->fade_start) {
                group->size = 4096 - (work->clock.frame-work->config->fade_start) * 4096 /
                                     (work->config->fade_end-work->config->fade_start);
                if (group->size <= 0) {
                    group->size = 0;
                }
            }
        } else {
            if (group->size < 16384 && primary->threshold >= 1024) {
                group->size += work->clock.step * 4096;
                if (group->size >= 16384) {
                    group->size = 16384;
                    group->fading = 1;
                    if (i+1 == 7) {
                        work->tail.phase = 3;
                    }
                }
            } else if (group->brightness > 0 && group->fading == 1) {
                group->brightness -= work->clock.step * 64;
                if (group->brightness <= 0) {
                    group->brightness = 0;
                    if (i+1 == 7 && work->tail.phase == 3) {
                        work->tail.phase = 5;
                    }
                }
            }
            primary_index++;
        }
    }
}

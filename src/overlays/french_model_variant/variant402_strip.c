#include "../../types.h"
#include "variant402_strip.h"

/* The French 402 and 412 entries share this drawer; it reads the entry
 * state of whichever image builds it. */
#ifdef MODEL_VARIANT412_STRIP
#include "variant412_entry.h"
typedef Variant412EntryState Variant402EntryRecordOwner;
#else
#include "variant402_entry.h"
typedef Variant402EntryState Variant402EntryRecordOwner;
#endif

void FrenchModelVariant402_DrawStrip(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
#ifdef MODEL_VARIANT412_STRIP
    long interpolation;
    long flags;
#else
    long flags[1][2];
    long interpolation;
#endif
    Variant402EntryRecordOwner *work;
    Variant402EntryRecord *strip;
    POLY_GT4 *quad;
    GsOT *ot;
    s16 i, j;
    s32 angle;
    s32 size;
    s32 width;

    work = (Variant402EntryRecordOwner *)context;
    ot = Graphics_GetDefaultOrderingTable();
#ifdef MODEL_VARIANT412_STRIP
    strip = work->records;
#endif
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx);
    quad = work->packets.textured;
    angle += 2048;
    if (work->phase > 0) {
#ifndef MODEL_VARIANT412_STRIP
        strip = work->records;
#endif
        width = work->width;
        if (width < 0) {
            width += 31;
        }
        i = 0;
        size = width >> 5;
        for (; i < 1; i++, strip++) {
            for (j = 0; j < 2; j++) {
                j[strip->points[0]].vx = rcos(1024) * size >> 12;
                j[strip->points[0]].vy = rsin(1024) * size >> 12;
                j[strip->points[0]].vz = 0;
                setVector(&strip->points[1][j], 0, 0, 0);
                setVector(&strip->points[2][j], rcos(3072) * size >> 12,
                          rsin(3072) * size >> 12, 0);
                setVector(&rotation, 0, 0, angle);
                if (j == 0) {
                    matrix.t[0] = work->matrix.t[0];
                    matrix.t[1] = work->matrix.t[1];
                    matrix.t[2] = work->matrix.t[2];
                } else {
                    matrix.t[0] = work->matrix.t[0] +
                        work->aim.direction.vx * work->displacement / 1024;
                    matrix.t[1] = work->matrix.t[1] +
                        work->aim.direction.vy * work->displacement / 1024;
                    matrix.t[2] = work->matrix.t[2] +
                        work->aim.direction.vz * work->displacement / 1024;
                }
                scale.vz = scale.vy = scale.vx = 4096;
                RotMatrix(&rotation, &matrix);
                coordinate.coord = matrix;
                coordinate.super = 0;
                coordinate.flg = 0;
                GsGetLs(&coordinate, &light);
                GsSetLsMatrix(&light);
                ReadRotMatrix(&light);
                RotMatrix(&rotation, &light);
                ScaleMatrix(&light, &scale);
                SetRotMatrix(&light);
                strip->depth[j] = RotTransPers3(&strip->points[0][j], &strip->points[1][j],
                                                &strip->points[2][j], &strip->projected[0][j],
                                                &strip->projected[1][j],
                                                &strip->projected[2][j],
                                                &interpolation,
#ifdef MODEL_VARIANT412_STRIP
                                                &flags);
#else
                                                &flags[i][j]);
#endif
            }
        }
        strip = work->records;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 1; j++) {
                setXY4(quad, (u16)strip->projected[0][j], (strip->projected[0][j] >> 16),
                       (u16)strip->projected[0][j + 1], (strip->projected[0][j + 1] >> 16),
                       (u16)strip->projected[1][j], (strip->projected[1][j] >> 16),
                       (u16)strip->projected[1][j + 1], (strip->projected[1][j + 1] >> 16));
                setRGB0(quad, 0, 64, 192);
                setRGB1(quad, 0, 64, 192);
                setRGB2(quad, 128, 128, 128);
                setRGB3(quad, 128, 128, 128);
#ifdef MODEL_VARIANT412_STRIP
                if (strip->depth[j] > 0) {
                    if (strip->depth[j] < 2048) {
                        GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                    }
                }
#else
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
#endif
                setXY4(quad, (u16)strip->projected[2][j], (strip->projected[2][j] >> 16),
                       (u16)strip->projected[2][j + 1], (strip->projected[2][j + 1] >> 16),
                       (u16)strip->projected[1][j], (strip->projected[1][j] >> 16),
                       (u16)strip->projected[1][j + 1], (strip->projected[1][j + 1] >> 16));
                setRGB0(quad, 0, 64, 192);
                setRGB1(quad, 0, 64, 192);
                setRGB2(quad, 128, 128, 128);
                setRGB3(quad, 128, 128, 128);
#ifdef MODEL_VARIANT412_STRIP
                if (strip->depth[j] > 0) {
                    if (strip->depth[j] < 2048) {
                        GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                    }
                }
#else
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
#endif
            }
        }
        if (work->part_index + 1 ==
            ((Family402RingView *)work)->config->part_count) {
            if (work->phase == 1) {
                if (work->displacement <= 1024) {
#ifdef MODEL_VARIANT412_STRIP
                    work->displacement += work->step * 64;
#else
                    work->displacement += work->clock.step * 64;
#endif
                    if (work->displacement >= 1024) {
                        work->displacement = 1024;
                        work->phase = 2;
                    }
                }
            }
            if (work->phase == 5 && work->width > 0) {
#ifdef MODEL_VARIANT412_STRIP
                work->width -= work->step * 8;
#else
                work->width -= work->clock.step * 8;
#endif
                if (work->width <= 0) {
                    work->width = 0;
                }
            }
        }
    }
}

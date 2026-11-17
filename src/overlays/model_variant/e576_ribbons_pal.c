#include "../../types.h"
#include "e576_entry_pal.h"
#include "../../game/gpu_packets.h"

/* The North American header-393 copy, built with the CDK compiler, names the
 * closing point's screen fields with the literal index, reads the following
 * screen point as (sa + 1)[k] so the address folds into the current one, and
 * starts each quad through a pointer to its first point. */
#ifdef MODEL_VARIANT410_RIBBONS_FOLDED_NEXT
#define RIBBONS410_CLOSING(array) (ribbon->array[16])
#define RIBBONS410_NEXT(n) ((ribbon->screen + 1)[n])
#define RIBBONS410_START(n) (*(ribbon->screen + (n)))
#else
#define RIBBONS410_CLOSING(array) (ribbon->array[k])
#define RIBBONS410_NEXT(n) (ribbon->screen[(n) + 1])
#define RIBBONS410_START(n) (ribbon->screen[n])
#endif

void FrenchModelVariant410_DrawRibbons(Variant410EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long flags[9][17];
    long interpolation;
    long flag;
    Variant410EntryState *work;
    GsOT *ot;
    s16 i;
    s32 angle;
    s32 yaw;
    s32 pitch;
    s16 length;
    Variant410EntryRibbon *ribbon;
    POLY_G4 *quad;
    s16 k;
    s32 progress;
    s32 dx;
    s32 dy;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 3072;
    pitch = ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ribbon = work->ribbons;
    quad = &work->gradient;
    if (work->tail.phase >= 0) {
        for (i = 0; i < 9; i++, ribbon++) {
            if (i == 0) {
                angle = 1024;
            } else if ((s16)(i % 2) == 1) {
                angle = 1024 + (i + 1) * 200;
            } else {
                angle = 1024 - i * 200;
            }
            length = 8;
            for (k = 0; k < 17; k++) {
                progress = ribbon->progress[k];
                if (progress <= 0) {
                    progress = 0;
                }
                setVector(&ribbon->points[k],
                          -work->aim.direction.vx * progress / 1024
                              + (rcos(angle) * (-(rsin(progress * 2) * 128) >> 12) >> 12),
                          -work->aim.direction.vy * progress / 1024
                              + (rsin(angle) * (-(rsin(progress * 2) * 128) >> 12) >> 12),
                          -work->aim.direction.vz * progress / 1024);
                setVector(&ribbon->edges[k],
                          ribbon->points[k].vx + (rcos(yaw) * length >> 12),
                          ribbon->points[k].vy,
                          ribbon->points[k].vz + (rsin(yaw) * length >> 12));
                if (ribbon->progress[k] < 1024) {
                    ribbon->progress[k] += work->clock.step * 40;
                    if (ribbon->progress[k] >= 1024) {
                        ribbon->progress[k] = 1024;
                        if (k == 0 && work->tail.phase == 1) {
                            work->tail.phase = 2;
                        }
                    }
                }
            }
        }
        setVector(&rotation, 0, 0, 0);
        matrix.t[0] = work->aim.target.vx;
        matrix.t[1] = work->aim.target.vy;
        matrix.t[2] = work->aim.target.vz;
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rotation, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ribbon = work->ribbons;
        for (i = 0; i < 9; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->points[k - 1], &ribbon->points[k],
                                                   &ribbon->points[k - 1], &ribbon->points[k],
                                                   (long *)&ribbon->screen[k - 1], (long *)&ribbon->screen[k],
                                                   (long *)&ribbon->screen[k - 1], (long *)&ribbon->screen[k],
                                                   &interpolation, &flags[i][k]);
                    RotTransPers(&ribbon->edges[k], (long *)&ribbon->edge_screen[k], &interpolation, &flag);
                    dx = RIBBONS410_CLOSING(screen).point.vx - ribbon->screen[k - 1].point.vx;
                    dy = RIBBONS410_CLOSING(screen).point.vy - ribbon->screen[k - 1].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 3072;
                    ribbon->width[k] = RIBBONS410_CLOSING(edge_screen).point.vx - RIBBONS410_CLOSING(screen).point.vx;
                    ribbon->offsets.ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->points[k], &ribbon->points[k + 1],
                                                   &ribbon->points[k], &ribbon->points[k + 1],
                                                   (long *)&ribbon->screen[k], (long *)&ribbon->screen[k + 1],
                                                   (long *)&ribbon->screen[k], (long *)&ribbon->screen[k + 1],
                                                   &interpolation, &flags[i][k]);
                    RotTransPers(&ribbon->edges[k], (long *)&ribbon->edge_screen[k], &interpolation, &flag);
#ifdef MODEL_VARIANT410_RIBBONS_FOLDED_NEXT
                    /* Dead store, overwritten below: it only lengthens the CDK
                     * compiler's projection loop, which keeps the flag-row
                     * addresses inside the loop as in the header-393 copy. */
                    dx = ribbon->screen[k + 1].point.vx;
#endif
                    dx = RIBBONS410_NEXT(k).point.vx - ribbon->screen[k].point.vx;
                    dy = RIBBONS410_NEXT(k).point.vy - ribbon->screen[k].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 3072;
                    ribbon->width[k] = ribbon->edge_screen[k].point.vx - ribbon->screen[k].point.vx;
                    ribbon->offsets.ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
        ribbon = work->ribbons;
        for (i = 0; i < 9; i++, ribbon++) {
            for (k = 0; k < 16; k++) {
                quad->x0 = RIBBONS410_START(k).point.vx + ribbon->offsets.ox[k];
                quad->y0 = (ribbon->screen[k].packed >> 16) + ribbon->offsets.oy[k];
                quad->x1 = RIBBONS410_NEXT(k).point.vx + ribbon->offsets.ox[k + 1];
                quad->y1 = (RIBBONS410_NEXT(k).packed >> 16) + ribbon->offsets.oy[k + 1];
                quad->x2 = ribbon->screen[k].point.vx - ribbon->offsets.ox[k];
                quad->y2 = (ribbon->screen[k].packed >> 16) - ribbon->offsets.oy[k];
                quad->x3 = RIBBONS410_NEXT(k).point.vx - ribbon->offsets.ox[k + 1];
                quad->y3 = (RIBBONS410_NEXT(k).packed >> 16) - ribbon->offsets.oy[k + 1];
                if (k + 1 == 16) {
                    setRGB0(quad, ribbon->style.outer.r, ribbon->style.outer.g, ribbon->style.outer.b);
                    setRGB1(quad, 0, 0, 0);
                    setRGB2(quad, ribbon->style.inner.r, ribbon->style.inner.g, ribbon->style.inner.b);
                    setRGB3(quad, 0, 0, 0);
                } else {
                    setRGB0(quad, ribbon->style.outer.r, ribbon->style.outer.g, ribbon->style.outer.b);
                    setRGB1(quad, ribbon->style.outer.r, ribbon->style.outer.g, ribbon->style.outer.b);
                    setRGB2(quad, ribbon->style.inner.r, ribbon->style.inner.g, ribbon->style.inner.b);
                    setRGB3(quad, ribbon->style.inner.r, ribbon->style.inner.g, ribbon->style.inner.b);
                }
                if (ribbon->offsets.otz[k] >= 0 && flags[i][k] >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)ribbon->offsets.otz[k], 1);
                }
            }
            if (i + 1 == 9 && ribbon->progress[16] >= 1024 && work->tail.phase == 2) {
                work->tail.phase = 3;
            }
        }
    }
    work->ribbon_sway += work->clock.step * 384;
    work->ribbon_wave += work->clock.step << 6;
}

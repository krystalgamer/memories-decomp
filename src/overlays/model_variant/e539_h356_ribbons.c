#include "../../types.h"
#include "e539_h356_entry.h"
#include "../../game/gpu_packets.h"

void FrenchModelVariant373_DrawRibbons(EntryState373 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState373 *work;
    ModelVariantQuadSheet *colors;
    GsOT *ot;
    s16 i;
    s32 turn;
    s32 tilt;
    ModelVariantOffsetRay *ribbon;
    POLY_G3 *triangle;
    s16 j;
    s16 angle;
    s16 length;
    s32 flags;
    s16 radius;
    s32 dx;
    s32 dy;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 3072;
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ribbon = work->ribbons;
    tilt = ratan2(work->aim.direction.vz, work->aim.direction.vy) + 1024;
    ratan2(work->aim.direction.vz, work->aim.direction.vx);
    colors = work->quadrants;
    triangle = &work->triangle;
    if (work->slot == 1) {
        turn = -turn;
    }
    if (work->phase > 0) {
        length = 16;
        flags = work->clock.frame_count;
        for (i = 0, angle = work->angle; i < 8;
             i++, angle = work->angle + i * 512, ribbon++) {
            for (j = 0; j < 2; j++) {
                radius = 128;
                if (j == 0) {
                    radius = 40;
                }
                setVector(&ribbon->points[j], rcos(angle) * radius >> 12,
                          rsin(angle) * radius >> 12, j * ((flags & 1) * 32 + 192));
                setVector(&ribbon->offset_points[j],
                          ribbon->points[j].vx + (rcos(turn) * length >> 12),
                          ribbon->points[j].vy,
                          ribbon->points[j].vz + (rsin(turn) * length >> 12));
            }
        }
        setVector(&rotation, tilt, 0, 0);
        matrix.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->factor / 1024;
        matrix.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->factor / 1024;
        matrix.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->factor / 1024;
        if (work->scale < 4096) {
            scale.vx = work->scale;
            scale.vy = work->scale;
            scale.vz = work->scale;
        } else {
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
        }
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ribbon = work->ribbons;
        for (i = 0; i < 8; i++, ribbon++) {
            for (j = 0; j < 2; j++) {
                if (j == 0) {
                    ribbon->depth[0] = RotTransPers4(
                        &ribbon->points[0], &ribbon->points[1],
                        &ribbon->points[0], &ribbon->points[1],
                        &ribbon->screen[0], &ribbon->screen[1],
                        &ribbon->screen[0], &ribbon->screen[1], &interpolation, &flag);
                    dx = (s16)ribbon->screen[j + 1] - (s16)ribbon->screen[0];
                    dy = (ribbon->screen[j + 1] >> 16) - (ribbon->screen[0] >> 16);
                } else {
                    ribbon->depth[j] = RotTransPers4(
                        &ribbon->points[j - 1], &ribbon->points[j],
                        &ribbon->points[j - 1], &ribbon->points[j],
                        &ribbon->screen[j - 1], &ribbon->screen[j],
                        &ribbon->screen[j - 1], &ribbon->screen[j], &interpolation, &flag);
                    dx = (s16)ribbon->screen[j] - (s16)ribbon->screen[j - 1];
                    dy = (ribbon->screen[j] >> 16) - (ribbon->screen[j - 1] >> 16);
                }
                RotTransPers(&ribbon->offset_points[j], &ribbon->offset_screen[j], &interpolation, &flag);
                ribbon->angles[j] = ratan2(dy, dx) + 3072;
                ribbon->projected_width[j] = (s16)ribbon->offset_screen[j] - (s16)ribbon->screen[j];
                ribbon->width_x[j] = rcos(ribbon->angles[j]) * ribbon->projected_width[j] >> 12;
                ribbon->width_y[j] = rsin(ribbon->angles[j]) * ribbon->projected_width[j] >> 12;
            }
        }
        ribbon = work->ribbons;
        for (i = 0; i < 8; i++, ribbon++) {
            for (j = 0; j < 1; j++) {
                triangle->x0 = ribbon->screen[j] + ribbon->width_x[j];
                triangle->y0 = (ribbon->screen[j] >> 16) + ribbon->width_y[j];
                triangle->x1 = ribbon->screen[j + 1];
                triangle->y1 = ribbon->screen[j + 1] >> 16;
                triangle->x2 = ribbon->screen[j] - ribbon->width_x[j];
                triangle->y2 = (ribbon->screen[j] >> 16) - ribbon->width_y[j];
                triangle->r0 = colors->color[0].r;
                triangle->g0 = colors->color[0].r;
                triangle->b0 = colors->color[0].r;
                triangle->r1 = colors->color[1].r;
                triangle->g1 = colors->color[1].r;
                triangle->b1 = colors->color[1].r;
                triangle->r2 = colors->color[0].r;
                triangle->g2 = colors->color[0].r;
                triangle->b2 = colors->color[0].r;
                if (ribbon->depth[j] > 0) {
                    if (ribbon->depth[j] < 2048) {
                        Graphics_SubmitDrawModePacket((u32 *)triangle, ot, (u16)ribbon->depth[j], 1);
                    }
                }
            }
        }
    }
    if (work->index + 1 == work->config->count) {
        work->angle += work->clock.step * 50;
    }
}

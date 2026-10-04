#include "../../types.h"
#include "variant450_coils.h"

void func_8013E10C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Model450CoilView *work;
    Variant450QuadGroup *driver;
    POLY_FT4 *quad;
    GsOT *ot;
    s16 i;
    s16 size;
    s16 rx;
    s16 ry;
    s32 step;
    s32 base;
    s32 spin;
    s32 wave;
    s32 turn;
    s32 radius;
    Model450Coil *coil;
    s16 j;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx, dy;
    Model450Screen *previous;

    work = (Model450CoilView *)context;
    ot = func_80058F10();
    turn = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    quad = &work->packet;
    reach = 192;
    step = 1024;
    driver = &work->groups[1];
    if (!(work->flags & 1)) {
        size = 4096;
        rx = 512;
        ry = 0;
    } else {
        size = 4096;
        rx = 1536;
        ry = step;
    }
    if ((u32)(work->flags & 3) < 2) {
        phase = 0;
    } else {
        phase = 1500;
    }
    coil = work->coils;
    i = 0;
    base = work->rotation[1];
    spin = phase;
    radius = reach;
    for (; i < 2; i++, coil++, base += step) {
        twist = base * 2;
        for (j = 0, wave = 0; j < 17;
             j++, twist = base * 2 + j * 1500 / 16, spin += 1500, wave += 512) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->length / 64;
            setVector(&coil->points[j],
                bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&coil->edges[j],
                coil->points[j].vx + (rcos(turn) * (s16)length >> 12),
                coil->points[j].vy,
                coil->points[j].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rotation.vx = (s16)work->rotation[0] + rx;
    rotation.vy = (s16)work->rotation[1] + ry;
    rotation.vz = (s16)work->rotation[2];
    if ((u32)work->flags % 10 == 0) {
        work->rotation[1] += 1300;
    }
    work->rotation[1] += 32;
    matrix.t[0] = work->origin.vx;
    matrix.t[1] = work->origin.vy;
    matrix.t[2] = work->origin.vz;
    setVector(&scale, size, size, size);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    coil = work->coils;
    for (i = 0, previous = &coil->screen[15]; i < 2; i++,
         previous = (Model450Screen *)((u8 *)previous + sizeof(Model450Coil)), coil++) {
        for (j = 0; j < 17; j++) {
            if (j == 16) {
                coil->depth[j] = RotTransPers4(
                    &coil->points[15], &coil->points[16], &coil->points[15], &coil->points[16],
                    &previous->packed, &coil->screen[16].packed,
                    &previous->packed, &coil->screen[16].packed, &interpolation, &flag);
                RotTransPers(&coil->edges[16], &coil->edge_screen[16].packed, &interpolation, &flag);
                dx = coil->screen[j].point.vx - previous->point.vx;
                dy = coil->screen[j].point.vy - previous->point.vy;
                coil->angle[j] = ratan2(dy, dx) + 3072;
                if (work->length > 512) {
                    coil->width[j] = 1;
                } else {
                    coil->width[j] = coil->edge_screen[j].point.vx - coil->screen[j].point.vx;
                }
                coil->x_offset[j] = rcos(coil->angle[j]) * coil->width[j] >> 12;
                coil->y_offset[j] = rsin(coil->angle[j]) * coil->width[j] >> 12;
            } else {
                coil->depth[j] = RotTransPers4(
                    &coil->points[j], &coil->points[j + 1], &coil->points[j], &coil->points[j + 1],
                    &coil->screen[j].packed, &coil->screen[j + 1].packed,
                    &coil->screen[j].packed, &coil->screen[j + 1].packed, &interpolation, &flag);
                RotTransPers(&coil->edges[j], &coil->edge_screen[j].packed, &interpolation, &flag);
                dx = coil->screen[j + 1].point.vx - coil->screen[j].point.vx;
                dy = coil->screen[j + 1].point.vy - coil->screen[j].point.vy;
                coil->angle[j] = ratan2(dy, dx) + 3072;
                if (work->length > 512) {
                    coil->width[j] = 1;
                } else {
                    coil->width[j] = coil->edge_screen[j].point.vx - coil->screen[j].point.vx;
                }
                coil->x_offset[j] = rcos(coil->angle[j]) * coil->width[j] >> 12;
                coil->y_offset[j] = rsin(coil->angle[j]) * coil->width[j] >> 12;
            }
        }
    }
    coil = work->coils;
    for (i = 0; i < 2; i++, coil++) {
        for (j = 0; j < 16; j++) {
            quad->x0 = coil->screen[j].point.vx + coil->x_offset[j];
            quad->y0 = (coil->screen[j].packed >> 16) + coil->y_offset[j];
            quad->x1 = coil->screen[j + 1].point.vx + coil->x_offset[j + 1];
            quad->y1 = (coil->screen[j + 1].packed >> 16) + coil->y_offset[j + 1];
            quad->x2 = coil->screen[j].point.vx - coil->x_offset[j];
            quad->y2 = (coil->screen[j].packed >> 16) - coil->y_offset[j];
            quad->x3 = coil->screen[j + 1].point.vx - coil->x_offset[j + 1];
            quad->y3 = (coil->screen[j + 1].packed >> 16) - coil->y_offset[j + 1];
            setRGB0(quad, coil->color[j].r, coil->color[j].g, coil->color[j].b);
            if (coil->depth[j] > 0 && coil->field_2AC[j] > 0) {
                GsSortPoly(quad, ot, (u16)coil->depth[j]);
            }
        }
    }
    if (work->state < 5 && work->length > 0) {
        work->length = driver->scale / 8;
        if (work->length <= 0) {
            work->length = 0;
        }
    }
}

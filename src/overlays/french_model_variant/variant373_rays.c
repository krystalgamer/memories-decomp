#include "../../types.h"
#include "variant373_rays.h"

void func_8013E3EC(u8 *context)
{
    Family373RayView *work = (Family373RayView *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Family373Ray *ray;
    POLY_GT4 *quad;
    s16 i, j, width;
    s32 angle_a, angle_b;
    s32 radius;
    s32 level;
    s32 turn;
    s32 unused;
    s16 size;
    s32 dx, dy;

    ot = func_80058F10();
    turn = ratan2(work->view_direction[2], work->view_direction[0]);
    ray = work->rays;
    quad = &work->quad;
    turn += 3072;
    radius = work->radius * 768 / 1024;
    width = work->radius / 8;
    /* Both products are present in retail despite their unused results. */
    unused = radius * work->factor / 1024;
    unused = width * work->factor / 1024;
    level = work->scale;
    for (i = 0; i < 16; i++, ray++) {
        if (!(i & 1)) {
            angle_b = i * 256 + work->angle_b;
            angle_a = i * 512 + work->angle_a;
        } else {
            angle_b = i * 256 - work->angle_b;
            angle_a = i * 512 - work->angle_a;
        }
        for (j = 0; j < 2; j++) {
            if (work->phase < 8) {
                if (j == 0) {
                    width = level / 1024;
                } else {
                    width = level * 40 / 8192;
                }
            } else {
                if (j == 0) {
                    width = work->brightness * 8 / 255;
                } else {
                    width = work->brightness * 40 / 255;
                }
            }
            if (j == 0) {
                setVector(&ray->points[j], 0, 0, 0);
            } else {
                setVector(&ray->points[j],
                          rcos(angle_b) * (rsin(angle_a) * (radius * j) >> 12) >> 12,
                          rcos(angle_a) * (radius * j) >> 12,
                          rsin(angle_b) * (rsin(angle_a) * (radius * j) >> 12) >> 12);
            }
            setVector(&ray->edges[j],
                      ray->points[j].vx + (rcos(turn) * width >> 12),
                      ray->points[j].vy,
                      ray->points[j].vz + (rsin(turn) * width >> 12));
        }
    }
    size = 4608;
    if (!(work->flags & 1)) {
        size = 3584;
    }
    setVector(&rotation, 0, 0, 0);
    matrix.t[0] = work->origin[0] + work->delta[0] * work->factor / 1024;
    matrix.t[1] = work->origin[1] + work->delta[1] * work->factor / 1024;
    matrix.t[2] = work->origin[2] + work->delta[2] * work->factor / 1024;
    scale.vx = size;
    scale.vy = size;
    scale.vz = size;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    ray = work->rays;
    for (i = 0; i < 16; i++, ray++) {
        for (j = 0; j < 2; j++) {
            if (j == 1) {
                ray->depth[j] = RotTransPers4(
                    &ray->points[0], &ray->points[1], &ray->points[0], &ray->points[1],
                    &ray->projected[0], &ray->projected[1], &ray->projected[0], &ray->projected[1],
                    &interpolation, &flag);
                RotTransPers(&ray->edges[1], &ray->edge_projected[1], &interpolation, &flag);
                dx = (s16)ray->projected[j] - (s16)ray->projected[0];
                dy = (ray->projected[j] >> 16) - (ray->projected[0] >> 16);
                ray->angle[j] = ratan2(dy, dx) + 3072;
                ray->width[j] = (s16)ray->edge_projected[j] - (s16)ray->projected[j];
                ray->x_offset[j] = rcos(ray->angle[j]) * ray->width[j] >> 12;
                ray->y_offset[j] = rsin(ray->angle[j]) * ray->width[j] >> 12;
            } else {
                ray->depth[j] = RotTransPers4(
                    &ray->points[j], &ray->points[j + 1], &ray->points[j], &ray->points[j + 1],
                    &ray->projected[j], &ray->projected[j + 1],
                    &ray->projected[j], &ray->projected[j + 1], &interpolation, &flag);
                RotTransPers(&ray->edges[j], &ray->edge_projected[j], &interpolation, &flag);
                dx = (s16)ray->projected[j + 1] - (s16)ray->projected[j];
                dy = (ray->projected[j + 1] >> 16) - (ray->projected[j] >> 16);
                ray->angle[j] = ratan2(dy, dx) + 3072;
                ray->width[j] = (s16)ray->edge_projected[j] - (s16)ray->projected[j];
                ray->x_offset[j] = rcos(ray->angle[j]) * ray->width[j] >> 12;
                ray->y_offset[j] = rsin(ray->angle[j]) * ray->width[j] >> 12;
            }
        }
    }
    ray = work->rays;
    for (i = 0; i < 16; i++, ray++) {
        for (j = 0; j < 1; j++) {
            quad->x0 = ray->projected[j] + ray->x_offset[j];
            quad->y0 = (ray->projected[j] >> 16) + ray->y_offset[j];
            quad->x1 = ray->projected[j + 1] + ray->x_offset[j + 1];
            quad->y1 = (ray->projected[j + 1] >> 16) + ray->y_offset[j + 1];
            quad->x2 = ray->projected[j];
            quad->y2 = ray->projected[j] >> 16;
            quad->x3 = ray->projected[j + 1];
            quad->y3 = ray->projected[j + 1] >> 16;
            quad->r0 = ray->outer[j].r;
            quad->g0 = ray->outer[j].g;
            quad->b0 = ray->outer[j].b;
            quad->r1 = ray->outer[j + 1].r;
            quad->g1 = ray->outer[j + 1].g;
            quad->b1 = ray->outer[j + 1].b;
            quad->r2 = ray->inner[j].r;
            quad->g2 = ray->inner[j].g;
            quad->b2 = ray->inner[j].b;
            quad->r3 = ray->inner[j + 1].r;
            quad->g3 = ray->inner[j + 1].g;
            quad->b3 = ray->inner[j + 1].b;
            if (ray->depth[j] > 0) {
                if (ray->depth[j] < 2048) {
                    GsSortPoly(quad, ot, (u16)ray->depth[j]);
                }
            }
            quad->x0 = ray->projected[j] - ray->x_offset[j];
            quad->y0 = (ray->projected[j] >> 16) - ray->y_offset[j];
            quad->x1 = ray->projected[j + 1] - ray->x_offset[j + 1];
            quad->y1 = (ray->projected[j + 1] >> 16) - ray->y_offset[j + 1];
            quad->x2 = ray->projected[j];
            quad->y2 = ray->projected[j] >> 16;
            quad->x3 = ray->projected[j + 1];
            quad->y3 = ray->projected[j + 1] >> 16;
            quad->r0 = ray->outer[j].r;
            quad->g0 = ray->outer[j].g;
            quad->b0 = ray->outer[j].b;
            quad->r1 = ray->outer[j + 1].r;
            quad->g1 = ray->outer[j + 1].g;
            quad->b1 = ray->outer[j + 1].b;
            quad->r2 = ray->inner[j].r;
            quad->g2 = ray->inner[j].g;
            quad->b2 = ray->inner[j].b;
            quad->r3 = ray->inner[j + 1].r;
            quad->g3 = ray->inner[j + 1].g;
            quad->b3 = ray->inner[j + 1].b;
            if (ray->depth[j] > 0) {
                if (ray->depth[j] < 2048) {
                    GsSortPoly(quad, ot, (u16)ray->depth[j]);
                }
            }
        }
    }
    if (work->phase >= 7) {
        work->angle_a += 24;
        work->angle_b += 48;
    } else {
        work->angle_a += 16;
        work->angle_b += 32;
    }
    if (work->phase == 1) {
        if (work->radius < 768) {
            work->radius += work->step * 64;
            if (work->radius >= 768) {
                work->radius = 768;
                work->phase = 2;
            }
        }
    } else if (work->phase == 3) {
        work->radius = 384;
    } else if (work->phase == 4 || work->phase == 5) {
        if (work->radius < 1024) {
            work->radius += work->step * 64;
            if (work->radius >= 1024) {
                work->radius = 1024;
                work->phase = 6;
            }
        }
    }
}

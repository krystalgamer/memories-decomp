#include "../../types.h"
#include "variant408_update.h"

void func_8013B93C(u8 *context)
{
    ModelVariant408UpdateState *work = (ModelVariant408UpdateState *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    GsOT *ot;
    ModelVariant408Mesh *mesh;
    s16 i, j, k, angle, phase, radius;
    s32 unused;
    s32 tilt;
    POLY_G4 *quad;
    s32 depth;
    SVECTOR *point;

    ot = func_80058F10();
    unused = ratan2(work->view_delta.vz, work->view_delta.vx);
    unused = ratan2(work->view_delta.vy, work->view_delta.vx);
    quad = &work->quad;
    mesh = work->meshes;
    tilt = ratan2(work->delta.vz, work->delta.vy) + 3072;
    unused = ratan2(work->screen_delta.vy, work->screen_delta.vx);
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0, phase = work->angle; j < 17; j++, phase += 512) {
            if (mesh->phases[j] > 0) {
                radius = ((rcos(mesh->phases[j] + 2048) * 16) >> 12) + 20;
            } else {
                radius = rsin(1024) >> 10;
            }
            for (k = 0, angle = phase; k < 9; k++, angle = phase + k * 512) {
                (point = (SVECTOR *)(k * sizeof(*point)
                    + (j * sizeof(mesh->rows[0]) + (u32)mesh)))->vx =
                    work->delta.vx * work->extension / 1024 * j / 16
                    + ((rcos(angle) * radius) >> 12);
                point->vy = work->delta.vy * work->extension / 1024 * j / 16
                    + ((rsin(angle) * ((rcos(tilt) * radius) >> 12)) >> 12);
                point->vz = work->delta.vz * work->extension / 1024 * j / 16
                    + ((rsin(angle) * ((rsin(tilt) * radius) >> 12)) >> 12);
            }
            if (work->extension >= 1024 && work->elapsed > work->config->cycle_start) {
                if (mesh->phases[j] < 4096) {
                    mesh->phases[j] += 384;
                    if (mesh->phases[j] >= 4096) {
                        if (work->state >= 2 && work->repeat_limit <= mesh->repeats[j]) {
                            mesh->phases[j] = 4096;
                            if (j == 0 && work->state == 2) {
                                work->state = 3;
                            }
                        } else {
                            mesh->phases[j] -= 4096;
                            mesh->repeats[j]++;
                        }
                    }
                }
                if (work->state == 2 && j == 16) {
                    work->repeat_limit = mesh->repeats[16];
                }
            }
        }
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = work->matrix.t[0];
    matrix.t[1] = work->matrix.t[1];
    matrix.t[2] = work->matrix.t[2];
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    mesh = work->meshes;
    for (i = 0; i < 1; i++, mesh++) {
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 8; k++) {
                depth = RotTransPers4(
                    &mesh->rows[j].points[k], &mesh->rows[j].points[k + 1],
                    &mesh->rows[j + 1].points[k], &mesh->rows[j + 1].points[k + 1],
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                    (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    (PSXLONG *)&interpolation, (PSXLONG *)&flag);
                quad->r0 = mesh->colors[k].r;
                quad->g0 = mesh->colors[k].g;
                quad->b0 = mesh->colors[k].b;
                quad->r1 = mesh->colors[k + 1].r;
                quad->g1 = mesh->colors[k + 1].g;
                quad->b1 = mesh->colors[k + 1].b;
                quad->r2 = mesh->colors[k].r;
                quad->g2 = mesh->colors[k].g;
                quad->b2 = mesh->colors[k].b;
                quad->r3 = mesh->colors[k + 1].r;
                quad->g3 = mesh->colors[k + 1].g;
                quad->b3 = mesh->colors[k + 1].b;
                if (depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)quad, ot, depth & 0xFFFF, 1);
                }
            }
        }
    }
    if (work->state == 0) {
        if (work->extension < 1024) {
            work->extension = (work->elapsed - work->config->start) * 1024
                / (work->config->full - work->config->start);
            if (work->extension >= 1024) {
                work->extension = 1024;
                work->state = 1;
            }
        }
    } else if (work->state == 3) {
        if (work->extension > 0) {
            work->extension -= 64;
            if (work->extension <= 0) {
                work->extension = 0;
                work->state = 4;
            }
        }
    }
    work->angle += work->step * 32;
}

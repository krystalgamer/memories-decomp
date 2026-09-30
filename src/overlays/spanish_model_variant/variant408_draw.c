#include "../../types.h"
#include "variant408_draw.h"

void func_8013C02C(u8 *context)
{
    ModelVariant408State *work = (ModelVariant408State *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariant408Ring *ring;
    GsOT *ot;
    s16 i, j, k, angle, direction;
    s32 phase;
    u8 red, green, blue;
    GsGLINE *line;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    angle = ratan2(work->view_delta.vz, work->view_delta.vx);
    angle = ratan2(work->view_delta.vy, work->view_delta.vx);
    line = &work->line;
    ring = work->rings;
    angle = ratan2(work->delta.vz, work->delta.vy);
    angle = ratan2(work->screen_delta.vy, work->screen_delta.vx);
    direction = -1;
    if (work->slot == 0) {
        direction = 1;
    }
    for (i = 0; i < 4; i++, ring++) {
        if (ring->phase > 1536) {
            red = ring->color.r * (2048 - ring->phase) / 512;
            green = ring->color.g * (2048 - ring->phase) / 512;
            blue = ring->color.b * (2048 - ring->phase) / 512;
        } else {
            red = ring->color.r;
            green = ring->color.g;
            blue = ring->color.b;
        }
        if (ring->phase > 0) {
            phase = ring->phase;
            size = phase * 2;
        } else {
            phase = 0;
            size = phase;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = work->target.vx;
        matrix.t[1] = work->target.vy - ((rsin(phase) * 128) >> 12) + phase / 8;
        matrix.t[2] = work->target.vz + ((rsin(phase / 2) * 256) >> 12) * direction;
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
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                depth = RotTransPers4(
                    &ring->points[0][j][k], &ring->points[1][j][k],
                    &ring->points[0][j][k], &ring->points[1][j][k],
                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                    (PSXLONG *)&interpolation, (PSXLONG *)&flag);
                line->r0 = red;
                line->g0 = green;
                line->b0 = blue;
                line->r1 = 0;
                line->g1 = 0;
                line->b1 = 0;
                if (depth >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, depth);
                }
            }
        }
        if (ring->phase < 2048) {
            ring->phase += work->step * 32;
            if (ring->phase >= 2048) {
                ring->phase -= 2048;
                ring->repeats++;
                if (ring->repeats >= 6) {
                    ring->phase = 2048;
                    if (i + 1 == 4 && work->state == 1) {
                        work->state = 2;
                    }
                }
            }
        }
    }
}

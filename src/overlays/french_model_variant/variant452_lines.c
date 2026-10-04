#include "../../types.h"
#include "variant452_lines.h"

void func_8013D010(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX local;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant452LinesView *work = (Variant452LinesView *)ctx;
    Variant452Grid *grid = work->grid;
    GsGLINE *line = &work->line;
    GsOT *ot;
    s16 i, j, k;
    s32 active;
    s32 size;
    s32 fade;
    s32 depth;
    u8 r, g, b;

    ot = func_80058F10();
    active = 1;
    ratan2(work->delta.vz, work->delta.vx);
    ratan2(work->delta.vy, work->delta.vx);
    ratan2(work->delta_y, work->delta_x);
    ratan2(work->screen_delta.vy, work->screen_delta.vx);
    for (k = 0; k < 3; k++, grid++) {
        if (grid->count > 0) {
            fade = size = grid->count;
        } else {
            size = fade = 0;
        }
        if (fade > 2048) {
            r = grid->color[0] * (4096 - fade) / 2048;
            g = grid->color[1] * (4096 - fade) / 2048;
            b = grid->color[2] * (4096 - fade) / 2048;
        } else {
            r = grid->color[0];
            g = grid->color[1];
            b = grid->color[2];
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        matrix.t[0] = work->translation[0];
        matrix.t[1] = work->translation[1];
        matrix.t[2] = work->translation[2];
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rot, &matrix);
        ScaleMatrix(&matrix, &scale);
        coord.coord = matrix;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &local);
        GsSetLsMatrix(&local);
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 6; j++) {
                line->attribute = 0x50000000;
                depth = RotTransPers4(
                    &grid->inner[i][j], &grid->outer[i][j],
                    &grid->inner[i][j], &grid->outer[i][j],
                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                    &p, &flag);
                line->r0 = r;
                line->g0 = g;
                line->b0 = b;
                line->r1 = 0;
                line->g1 = 0;
                line->b1 = 0;
                if (depth >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, depth);
                }
            }
        }
        if (grid->count < 4096) {
            grid->count += work->step << 7;
            if (grid->count >= 4096) {
                if (work->phase < 4) {
                    grid->count -= 4096;
                } else {
                    grid->count = 4096;
                }
            } else {
                active = 0;
            }
        }
        if (k + 1 == 3 && active == 1 && work->phase == 4) {
            work->phase = 5;
        }
    }
}

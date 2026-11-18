#include "../../types.h"
#include "h435_entry_pal.h"

void FrenchModelVariant452_DrawLines(Variant452EntryState *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX local;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant452EntryState *work = ctx;
    ModelVariantWeb *grid = work->webs;
    GsGLINE *line = &work->line;
    GsOT *ot;
    s16 i, j, k;
    s32 active;
    s32 size;
    s32 fade;
    s32 depth;
    u8 r, g, b;

    ot = Graphics_GetDefaultOrderingTable();
    active = 1;
    ratan2(work->view_delta.vz, work->view_delta.vx);
    ratan2(work->view_delta.vy, work->view_delta.vx);
    ratan2(work->delta.vz, work->delta.vy);
    ratan2(work->screen_delta.vy, work->screen_delta.vx);
    for (k = 0; k < 3; k++, grid++) {
        if (grid->scale > 0) {
            fade = size = grid->scale;
        } else {
            size = fade = 0;
        }
        if (fade > 2048) {
            r = grid->color.r * (4096 - fade) / 2048;
            g = grid->color.g * (4096 - fade) / 2048;
            b = grid->color.b * (4096 - fade) / 2048;
        } else {
            r = grid->color.r;
            g = grid->color.g;
            b = grid->color.b;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        matrix.t[0] = work->position.vx;
        matrix.t[1] = work->position.vy;
        matrix.t[2] = work->position.vz;
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
                    &grid->near[i][j], &grid->far[i][j],
                    &grid->near[i][j], &grid->far[i][j],
                    (long *)&line->x0, (long *)&line->x1,
                    (long *)&line->x0, (long *)&line->x1,
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
        if (grid->scale < 4096) {
            grid->scale += work->clock.step << 7;
            if (grid->scale >= 4096) {
                if (work->phase < 4) {
                    grid->scale -= 4096;
                } else {
                    grid->scale = 4096;
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

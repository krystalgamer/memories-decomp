#include "../../types.h"
#include "beam.h"

void func_8017C760(u8 *context)
{
    ExodiaBeamState *work;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX local;
    MATRIX screen;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    GsOT *ot;
    s32 angle;
    s16 i, j;
    s16 width;
    ExodiaBeam *beam;
    POLY_GT4 *quad;

    work = (ExodiaBeamState *)context;
    ot = func_80058F10();
    quad = &work->quad;
    angle = ratan2(work->angles.vy, work->angles.vx) + 2048;
    if (work->phase > 0) {
        if (!(work->frame_count & 1)) {
            width = work->width / 2;
        } else {
            width = work->width * 640 / 1024;
        }
        beam = work->beams;
        for (i = 0; i < 1; i++, beam++) {
            for (j = 0; j < 17; j++) {
                (j + beam->outer)->vx = rcos(1024) * width >> 12;
                (j + beam->outer)->vy = rsin(1024) * width >> 12;
                (j + beam->outer)->vz = 0;
                setVector(&beam->center[j], 0, 0, 0);
                (j + beam->inner)->vx = rcos(3072) * width >> 12;
                (j + beam->inner)->vy = rsin(3072) * width >> 12;
                (j + beam->inner)->vz = 0;
                setVector(&rotation, 0, 0, angle);
                if (j == 0) {
                    local.t[0] = work->origin.vx;
                    local.t[1] = work->origin.vy;
                    local.t[2] = work->origin.vz;
                } else {
                    local.t[0] = work->origin.vx + work->direction.vx * j / 16 * work->distance / 1024;
                    local.t[1] = work->origin.vy + work->direction.vy * j / 16 * work->distance / 1024;
                    local.t[2] = work->origin.vz + work->direction.vz * j / 16 * work->distance / 1024;
                }
                setVector(&scale, 4096, 4096, 4096);
                RotMatrix(&rotation, &local);
                coordinate.coord = local;
                coordinate.super = 0;
                coordinate.flg = 0;
                GsGetLs(&coordinate, &screen);
                GsSetLsMatrix(&screen);
                ReadRotMatrix(&screen);
                RotMatrix(&rotation, &screen);
                ScaleMatrix(&screen, &scale);
                SetRotMatrix(&screen);
                beam->depth[j] = RotTransPers3(&beam->outer[j],
                    &beam->center[j], &beam->inner[j],
                    (PSXLONG *)&beam->projected[0][j], (PSXLONG *)&beam->projected[1][j],
                    (PSXLONG *)&beam->projected[2][j], (PSXLONG *)&interpolation,
                    (PSXLONG *)&beam->flags[j]);
            }
        }
        beam = work->beams;
        for (i = 0; i < 1; i++, beam++) {
            setRGB0(quad, 255, 255, 64);
            setRGB1(quad, 255, 255, 64);
            setRGB2(quad, 160, 160, 160);
            setRGB3(quad, 160, 160, 160);
            for (j = 0; j < 16; j++) {
                quad->x0 = beam->projected[0][j];
                quad->y0 = beam->projected[0][j] >> 16;
                quad->x1 = beam->projected[0][j + 1];
                quad->y1 = beam->projected[0][j + 1] >> 16;
                quad->x2 = beam->projected[1][j];
                quad->y2 = beam->projected[1][j] >> 16;
                quad->x3 = beam->projected[1][j + 1];
                quad->y3 = beam->projected[1][j + 1] >> 16;
                if (beam->depth[j] >= 0 && beam->flags[j] >= 0) {
                    GsSortPoly(quad, ot, beam->depth[j]);
                }
                quad->x0 = beam->projected[2][j];
                quad->y0 = beam->projected[2][j] >> 16;
                quad->x1 = beam->projected[2][j + 1];
                quad->y1 = beam->projected[2][j + 1] >> 16;
                quad->x2 = beam->projected[1][j];
                quad->y2 = beam->projected[1][j] >> 16;
                quad->x3 = beam->projected[1][j + 1];
                quad->y3 = beam->projected[1][j + 1] >> 16;
                if (beam->depth[j] >= 0 && beam->flags[j] >= 0) {
                    GsSortPoly(quad, ot, beam->depth[j]);
                }
            }
        }
        if (work->distance <= 1024) {
            work->distance += work->step * 128;
            if (work->distance >= 1024) {
                work->distance = 1024;
                if (work->phase == 2) {
                    work->phase = 3;
                }
            }
        }
    }
}

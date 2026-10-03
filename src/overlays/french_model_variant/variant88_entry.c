#include "../../types.h"
#include "variant88_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Family88State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR corners[4];
    s32 flag;
    s32 interpolation;
    s32 direction;
    GsOT *ot;
    Family88Config *config;
    s32 i, j;
    s32 depth;
    s32 step;
    SVECTOR *point;
    s32 target_y;

    memset(&rotation, 0, 8);
    work = (Family88State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &((Family88Config *)(D_8013B954 + 0x1C))[command];
        point = work->points;
        for (i = 0; i < config->groups * config->count; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            work->timers[i] = config->period;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->shared.textures[i] = func_80059A50(Model_GetActiveSlotIndex(), 3, &((GsIMAGE *)D_8013B954)[i]);
        }
        work->frame = -config->delay;
        work->shared.state.done = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->frame < 0) {
        work->frame += step;
        return 0;
    }
    SetPolyFT4(&quad);
    PushMatrix();
    setVector(&corners[0], -config->size, -config->size, 0);
    setVector(&corners[1], config->size, -config->size, 0);
    setVector(&corners[2], -config->size, config->size, 0);
    setVector(&corners[3], config->size, config->size, 0);
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    quad.tpage = work->shared.textures[0] >> 16;
    quad.clut = work->shared.textures[0];
    for (i = 0; i < config->groups; i++) {
        point = &work->points[i * config->count];
        for (j = 0; j < config->count; j++) {
            if ((work->frame - j * config->period / config->count) % config->period < step) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
                if (work->frame + j < config->duration) {
                    work->timers[i * config->count + j] = config->period;
                } else {
                    work->timers[i * config->count + j] = -1;
                }
            } else if (work->timers[i * config->count + j] != 0) {
                target_y = config->amplitude * ccos((j + i) * 4096 / config->count) / 4096 - 350;
                point->vy += (target_y - point->vy) / work->timers[i * config->count + j] * step;
                point->vz += (direction * 900 - point->vz) /
                              work->timers[i * config->count + j] * step;
                work->timers[i * config->count + j] -= step;
            }
            scale.vz = scale.vy = scale.vx =
                ((config->period - work->timers[i * config->count + j]) * 4096) / config->period;
            GsSetLsMatrix(&base);
            RotTrans(point, (VECTOR *)matrix.t, (PSXLONG *)&flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                                (PSXLONG *)&quad.x3, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
            if (i == 0) {
                setRGB0(&quad, config->r, config->g, config->b);
            } else {
                setRGB0(&quad, config->r1, config->g1, config->b1);
            }
            setUV4(&quad,
                   (s16)(work->timers[i * config->count + j] % 8) * 32, 0,
                   (s16)(work->timers[i * config->count + j] % 8) * 32 + 31, 0,
                   (s16)(work->timers[i * config->count + j] % 8) * 32, 31,
                   (s16)(work->timers[i * config->count + j] % 8) * 32 + 31, 31);
            SetSemiTrans(&quad, 0);
            if (j * config->period / config->count < work->frame &&
                work->timers[i * config->count + j] >= 0 && depth >= 0 && flag >= 0) {
                func_8005B260((u32 *)&quad, ot, depth & 0xFFFF, 1);
            }
            point++;
        }
    }
    PopMatrix();
    work->frame += step;
    if (work->frame < config->period) {
        return 0;
    } else if (work->frame >= config->duration + config->period) {
        if (work->shared.state.done) {
            return 2;
        } else {
            work->shared.state.done = 1;
            return 1;
        }
    } else {
        return 4;
    }
}

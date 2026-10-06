#include "../../types.h"
#include "variant82_entry.h"

static const VECTOR D_8013BDE4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model82State *work;
    Model82Config *config;
    MATRIX base, matrix;
    GsBOXF box;
    POLY_FT4 quad;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR vertices[3];
    PSXLONG interpolation, flag, depth;
    s32 direction;
    GsOT *ot;
    s32 i;
    SVECTOR *point, *velocity;

    memset(&rotation, 0, 8);
    scale = D_8013BDE4;
    work = (Model82State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BE10[command];
        point = work->positions;
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, 0, 0, 0);
            work->remaining[i] = 8;
        }
        work->texture = func_80059A50(Model_GetActiveSlotIndex(), 1, D_8013BDF4);
        work->elapsed = -config->delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        PushMatrix();
        /* Native delay path passes this stack matrix without initializing it. */
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        point = work->positions;
        velocity = work->velocities;
        for (i = 0; i <= config->count; point++, velocity++, i++) {
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->vx += rand() % config->spread * 2 - config->spread;
            point->vy -= rand() % config->spread * 2 - config->y_bias;
            point->vz += rand() % config->spread * 2 - config->spread;
            velocity->vx = (rand() % config->target_spread * 2 - config->target_spread - point->vx) / config->travel_duration;
            {
                s32 sample = rand();
                s32 destination_y = config->target_spread + 350;
                velocity->vy = (sample % config->target_spread * 2 -
                    destination_y - point->vy) / config->travel_duration;
            }
            velocity->vz = (direction * 350 + rand() % config->target_spread * 2 - config->target_spread - point->vz) / config->travel_duration;
        }
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    SetPolyFT4(&quad);
    quad.tpage = work->texture >> 16;
    quad.clut = work->texture;
    setRGB0(&quad, config->red / 2, config->green / 2, config->blue / 2);
    box.attribute = 0x50000000;
    box.w = box.h = 1;
    setVector(&vertices[0], 0, 0, 0);
    setVector(&vertices[1], -config->half_size, -config->half_size, 0);
    setVector(&vertices[2], config->half_size, config->half_size, 0);
    point = work->positions;
    velocity = work->velocities;
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < config->count; point++, velocity++, i++) {
        GsSetLsMatrix(&base);
        if (i / config->rate >= work->elapsed) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->vx += rand() % config->spread * 2 - config->spread;
            point->vy -= rand() % config->spread * 2 - config->y_bias;
            point->vz += rand() % config->spread * 2 - config->spread;
            velocity->vx = (rand() % config->target_spread * 2 - config->target_spread - point->vx) / config->travel_duration;
            {
                s32 sample = rand();
                s32 destination_y = config->target_spread + 350;
                velocity->vy = (sample % config->target_spread * 2 -
                    destination_y - point->vy) / config->travel_duration;
            }
            velocity->vz = ((rand() % config->target_spread + direction * 225) * 2 - config->target_spread - point->vz) / config->travel_duration;
        }
        if (i < work->elapsed * config->rate) {
            point->vx += velocity->vx * Model_GetFrameStep();
            point->vy += velocity->vy * Model_GetFrameStep();
            point->vz += velocity->vz * Model_GetFrameStep();
        }
        depth = RotTransPers(point, (PSXLONG *)&box.x, &interpolation, &flag);
        if (direction * point->vz < 450) {
            box.r = config->red;
            box.g = config->green;
            box.b = config->blue;
        } else {
            box.r = config->red * work->remaining[i] / 8;
            box.g = config->green * work->remaining[i] / 8;
            box.b = config->blue * work->remaining[i] / 8;
        }
        if (i < work->elapsed * config->rate && rand() % config->spark_divisor &&
            depth >= 0 && flag >= 0) {
            GsSortBoxFill(&box, ot, depth);
        }
        if (point->vz * direction > 450 && work->remaining[i]) {
            GsSetLsMatrix(&base);
            RotTrans(point, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotTransPers4(&vertices[1], &vertices[1], &vertices[2], &vertices[2],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.x1 = quad.x3;
            quad.x2 = quad.x0;
            setUVWH(&quad, (8 - work->remaining[i]) % 4 * 32,
                (8 - work->remaining[i]) / 4 * 32, 31, 31);
            SetSemiTrans(&quad, 1);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            work->remaining[i]--;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->rate + config->travel_duration) {
return_zero:
        return 0;
    }
    if (work->elapsed > config->end_time) {
        return 2;
    }
    if (work->elapsed < config->rate + config->travel_duration + config->count) {
        return 4;
    }
    if (work->completed) {
        goto return_zero;
    }
    work->completed = 1;
    return 1;
}

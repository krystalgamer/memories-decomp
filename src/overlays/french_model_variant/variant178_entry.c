#include "../../types.h"
#include "variant178_entry.h"

const VECTOR D_8013BD3C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model178State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model178Config *config;
    GsOT *ot;
    SVECTOR *point, *endpoint;
    s32 direction, step, i, j, time, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BD3C;
    work = (Model178State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BD84[command % 100];
        point = work->positions;
        for (i = 0; i < 64; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = 0;
        }
        point = work->endpoints;
        for (i = 0; i < 64; point++, i++) {
            Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vx += rand() % config->spread * 2 - config->spread;
            point->vy += rand() % config->spread;
            point->vz += direction * (rand() % config->spread);
            point->pad = 0;
        }
        point = work->velocities;
        for (i = 0; i < 4; point++, i++) {
            point->vx = rand() % config->velocity_spread * 2 - config->velocity_spread;
            point->vy = -config->rise;
            point->vz = rand() % config->velocity_spread * 2 - config->velocity_spread;
        }
        for (i = 1; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BD4C[i]);
        }
        work->texture[0] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BD4C[0]);
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    endpoint = work->endpoints;
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    for (i = 0; i < config->count; point++, endpoint++, i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else if (time < config->travel + config->fade) {
            value = time * 3584 / config->travel + 512;
            setVector(&scale, value, value, value);
            if (time < config->travel) {
                setRGB0(&quad, config->r, config->g, config->b);
            } else {
                s32 red, green, blue;
                value = config->travel + config->fade - time;
                red = config->r * value / config->fade;
                green = config->g * value / config->fade;
                blue = config->b * value / config->fade;
                setRGB0(&quad, red, green, blue);
            }
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            value = (time / 2 + i) % 8;
            setUV4(&quad, value * 32, 0, value * 32 + 31, 0,
                   value * 32, 63, value * 32 + 31, 63);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            GsSortPoly(&quad, ot, (u16)depth);
            if (time < config->travel) {
                point->vx += (endpoint->vx - point->vx) / (config->travel - time) * step;
                point->vy += (endpoint->vy - point->vy) / (config->travel - time) * step;
                point->vz += (endpoint->vz - point->vz) / (config->travel - time) * step;
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->burst_r, config->burst_g, config->burst_b);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    point = work->endpoints;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * config->spacing - config->travel;
        if (time >= 0 && time < config->burst_duration) {
            endpoint = work->velocities;
            for (j = 0; j < 4; endpoint++, j++) {
                setVector(&position, endpoint->vx, endpoint->vy, endpoint->vz);
                position.vx *= time;
                position.vy *= time;
                position.vz *= time;
                position.vx /= config->burst_duration;
                position.vy /= config->burst_duration;
                position.vz /= config->burst_duration;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                value = (time << 3) / config->burst_duration;
                setUV4(&quad, value * 32, 64, value * 32 + 31, 64,
                       value * 32, 95, value * 32 + 31, 95);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->travel;
    if (time < 0) {
        return 0;
    }
    if (time >= config->spacing * config->count + config->burst_duration) {
        return 2;
    }
    if (time < config->spacing * config->count) {
        return 4;
    }
    {
        u8 result;
        if (work->completed != 0) {
            result = 0;
        } else {
            work->completed++;
            result = 1;
        }
        return result;
    }
}

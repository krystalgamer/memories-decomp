#include "../../types.h"
#include "variant153_entry.h"

const VECTOR D_8013BE0C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model153State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model153Config *config;
    GsOT *ot;
    SVECTOR *point, *target;
    s32 step, i, j, time, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BE0C;
    work = (Model153State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BF18[command % 100];
        point = work->positions;
        for (i = 0; i < 64; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = 0;
        }
        point = work->targets;
        for (i = 0; i < 64; point++, i++) {
            Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vx += rand() % config->spread * 2 - config->spread;
            point->vy += rand() % config->spread;
            point->vz += direction * (rand() % config->spread);
            point->pad = 0;
        }
        point = work->burst_offsets;
        for (i = 0; i < 4; point++, i++) {
            point->vx = rand() % config->burst_spread * 2 - config->burst_spread;
            point->vy = -config->rise_height;
            point->vz = rand() % config->burst_spread * 2 - config->burst_spread;
        }
        work->texture[0] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BE1C[0]);
        for (i = 1; i < 9; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BE1C[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    target = work->targets;
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
    for (i = 0; i < config->count; point++, target++, i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else if (time < config->travel_duration + config->fade_duration) {
            if (time < config->travel_duration) {
                value = time * 3584 / config->travel_duration + 512;
            } else {
                value = (config->travel_duration + config->fade_duration - time) *
                    2048 / config->fade_duration + 2048;
            }
            setVector(&scale, value, value, value);
            if (time < config->travel_duration) {
                setRGB0(&quad, config->red, config->green, config->blue);
            } else {
                s32 red, green, blue;
                value = config->travel_duration + config->fade_duration - time;
                red = config->red * value / config->fade_duration;
                green = config->green * value / config->fade_duration;
                blue = config->blue * value / config->fade_duration;
                setRGB0(&quad, red, green, blue);
            }
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            value = (time / 2 + i) % 8;
            setUV4(&quad, value % 4 * 64, value / 4 * 87,
                   value % 4 * 64 + 63, value / 4 * 87,
                   value % 4 * 64, value / 4 * 87 + 86,
                   value % 4 * 64 + 63, value / 4 * 87 + 86);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (time < config->travel_duration) {
                point->vx += (target->vx - point->vx) / (config->travel_duration - time);
                point->vy += (target->vy - point->vy) / (config->travel_duration - time);
                point->vz += (target->vz - point->vz) / (config->travel_duration - time);
            } else {
                point->vy -= config->rise_height / config->fade_duration * step;
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->burst_red, config->burst_green, config->burst_blue);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    point = work->targets;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * config->spacing - config->travel_duration;
        if (time >= 0 && time < config->burst_duration) {
            target = work->burst_offsets;
            for (j = 0; j < 4; target++, j++) {
                setVector(&position, target->vx, target->vy, target->vz);
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
                value = time * 8 / config->burst_duration;
                setUV4(&quad, value * 32, 174, value * 32 + 31, 174,
                       value * 32, 205, value * 32 + 31, 205);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->travel_duration;
    if (time < 0) {
        return 0;
    }
    if (time >= config->spacing * config->count + config->burst_duration) {
        return 2;
    }
    if (time < config->spacing * config->count) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 0;
}

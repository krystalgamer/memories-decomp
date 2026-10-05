#include "../../types.h"
#include "variant129_entry.h"

const VECTOR D_8013BFE8 = {4096, 4096, 4096, 0};

#define DRAW_QUAD(a, b, c, d) \
    depth = RotAverage4(&vertices[a], &vertices[b], &vertices[c], &vertices[d], \
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, \
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag); \
    if (depth >= 0 && flag >= 0) { \
        GsSortPoly(&quad, ot, (u16)depth); \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model129State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 gradient;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    PSXLONG flag, interpolation;
    Model129Config *config;
    GsOT *ot;
    SVECTOR *point, *velocity;
    s32 scene_time;
    s32 step, i, time, burst_time, value;
    s16 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BFE8;
    work = (Model129State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C030[command];
        point = work->positions;
        velocity = work->velocities;
        for (i = 0; i < config->count; i++) {
            setVector(point, 0, 0, 0);
            setVector(velocity, 0, 0, 0);
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BFF8[i]);
        }
        work->elapsed = -config->delay;
        work->completed = 0;
        work->effect_started = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        point = work->positions;
        PushMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        PopMatrix();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&gradient);
    scene_time = work->elapsed - config->travel_duration;
    if (scene_time >= 0 && scene_time < config->flash_duration) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setUV4(&quad, 32, 0, 95, 0, 32, 63, 95, 63);
        setVector(&vertices[0], -config->half_size, -config->half_size, 0);
        setVector(&vertices[1], config->half_size, -config->half_size, 0);
        setVector(&vertices[2], -config->half_size, config->half_size, 0);
        setVector(&vertices[3], config->half_size, config->half_size, 0);
        setVector(&position, 0, -350, direction * 450);
        value = scene_time * 4096 / config->flash_duration + 4096;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = config->flash_red * (config->flash_duration - scene_time) / config->flash_duration;
        green = config->flash_green * (config->flash_duration - scene_time) / config->flash_duration;
        blue = config->flash_blue * (config->flash_duration - scene_time) / config->flash_duration;
        setRGB0(&quad, red, green, blue);
        DRAW_QUAD(0, 1, 2, 3);
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], 0, -config->half_size, 0);
    setVector(&vertices[2], config->half_size, -config->half_size, 0);
    setVector(&vertices[3], -config->half_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->half_size, 0, 0);
    setVector(&vertices[6], -config->half_size, config->half_size, 0);
    setVector(&vertices[7], 0, config->half_size, 0);
    setVector(&vertices[8], config->half_size, config->half_size, 0);
    scene_time = work->elapsed;
    if (work->effect_started == 0) {
        work->effect_started++;
        func_8005F7B0(30, config->count * config->spacing / 2);
    }
    point = work->positions;
    velocity = work->velocities;
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    for (i = 0; i < config->count; point++, velocity++, i++) {
        time = work->elapsed - config->spacing * i;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        value = 4096;
        burst_time = time - config->travel_duration;
        setVector(&position, point->vx, point->vy, point->vz);
        if (time >= 0 && time < config->travel_duration) {
            s32 duration = config->travel_duration;
            velocity->vx = -point->vx / (config->travel_duration - time) * step;
            velocity->vy = (-350 - point->vy) / (config->travel_duration - time) * step;
            velocity->vz = (direction * 450 - point->vz) / (config->travel_duration - time) * step;
            point->vx += velocity->vx;
            point->vy += velocity->vy;
            point->vz += velocity->vz;
            red = config->red * (config->travel_duration - time) / config->travel_duration;
            green = config->green * (config->travel_duration - time) / config->travel_duration;
            blue = config->blue * (config->travel_duration - time) / config->travel_duration;
            value = time * 4096 / duration;
            setRGB0(&quad, red, green, blue);
        }
        if (burst_time >= 0 && burst_time < (s16)(config->travel_duration / 3)) {
            s32 growth;
            growth = burst_time * 1365 / config->travel_duration;
            point->vx += velocity->vx;
            point->vy += velocity->vy;
            point->vz += velocity->vz;
            red = (config->red * (config->flash_duration - scene_time + config->travel_duration) /
                config->flash_duration) * ((s16)(config->travel_duration / 3) - burst_time) /
                config->travel_duration * 3;
            green = (config->green * (config->flash_duration - scene_time + config->travel_duration) /
                config->flash_duration) * ((s16)(config->travel_duration / 3) - burst_time) /
                config->travel_duration * 3;
            blue = (config->blue * (config->flash_duration - scene_time + config->travel_duration) /
                config->flash_duration) * ((s16)(config->travel_duration / 3) - burst_time) /
                config->travel_duration * 3;
            value = growth * 3 + 4096;
            setRGB0(&quad, red, green, blue);
        }
        if ((time >= 0 && time < config->travel_duration) ||
            (burst_time >= 0 && burst_time < (s16)(config->travel_duration / 3))) {
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            DRAW_QUAD(0, 1, 3, 4);
            DRAW_QUAD(2, 1, 5, 4);
            DRAW_QUAD(6, 7, 3, 4);
            DRAW_QUAD(8, 7, 5, 4);
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->travel_duration;
    burst_time = time - config->spacing * config->count;
    if (time < 0) {
        return 0;
    }
    if (burst_time >= (s16)(config->travel_duration / 3)) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}

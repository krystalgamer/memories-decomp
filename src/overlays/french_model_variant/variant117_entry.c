#include "../../types.h"
#include "variant117_entry.h"

static const VECTOR D_8013C08C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model117State *work;
    Model117Config *config;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation, depth;
    GsOT *ot;
    s32 direction, i, time, value;
    SVECTOR *point, *end, *target;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C08C;
    work = (Model117State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C144[command];
        point = work->positions;
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, 0, 0, 0);
            work->rows[i] = rand() % config->rows;
            work->frames[i] = rand() % config->frames;
            work->scales[i] = rand() % 4096 + 2048;
        }
        point = work->targets;
        for (i = 0; i < config->count; i++, point++) {
            setVector(point, rand() % (config->spread * 2) - config->spread,
                rand() % (config->spread * 2) - (s16)(config->spread + 350),
                direction * 450 + rand() % (config->spread * 2) - config->spread);
        }
        point = work->ends;
        for (i = 0; i < config->count; point++, i++) {
            value = rand() % 4096;
            setVector(point, config->radius * ccos(value) / 4096,
                config->radius * csin(value) / 4096 - 350,
                direction * (450 - config->radius));
        }
        for (i = 0; i < 6; i++) {
            work->textures[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C09C[i]);
        }
        work->completed = 0;
        work->elapsed = -config->delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        PushMatrix();
        /* Native delay path passes this stack matrix without initializing it. */
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&work->positions[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    point = work->positions;
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    target = work->targets;
    SetPolyFT4(&quad);
    quad.tpage = work->textures[0] >> 16;
    quad.clut = work->textures[0];
    setVector(&vertices[0], -config->size / 2, -config->size / 2, 0);
    setVector(&vertices[1], config->size / 2, -config->size / 2, 0);
    setVector(&vertices[2], -config->size / 2, config->size / 2, 0);
    setVector(&vertices[3], config->size / 2, config->size / 2, 0);
    end = work->ends;
    for (i = 0; i < config->count; point++, end++, target++, i++) {
        time = work->elapsed - i * config->spacing;
        if (time <= 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else {
            if (time < config->travel_duration) {
                setRGB0(&quad, config->red, config->green, config->blue);
                SetSemiTrans(&quad, 0);
                point->vx += (target->vx - point->vx) / (config->travel_duration - time) * 2;
                point->vy += (target->vy - point->vy) / (config->travel_duration - time) * 2;
                point->vz += (target->vz - point->vz) / (config->travel_duration - time) * 2;
            } else if (time < config->travel_duration + config->fade_duration) {
                red = (config->red / 2) * (config->travel_duration + config->fade_duration - time) / config->fade_duration;
                green = (config->green / 2) * (config->travel_duration + config->fade_duration - time) / config->fade_duration;
                blue = (config->blue / 2) * (config->travel_duration + config->fade_duration - time) / config->fade_duration;
                setRGB0(&quad, red, green, blue);
                SetSemiTrans(&quad, 1);
                point->vx += (end->vx - point->vx) / (config->travel_duration + config->fade_duration - time) * 2;
                point->vy += (end->vy - point->vy) / (config->travel_duration + config->fade_duration - time) * 2;
                point->vz += (end->vz - point->vz) / (config->travel_duration + config->fade_duration - time) * 2;
            }
            value = work->scales[i];
            setVector(&scale, value, value, value);
            copyVector(&position, point);
            if (time < config->travel_duration) {
                position.vy -= config->arc_height * csin(time * 2048 / config->travel_duration) / 4096;
            }
            setVector(&rotation, 0, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            value = (Model_GetActiveSlotIndex() * 2048 +
                ((s16 *)Model_GetViewMetricsBuffer())[1] + 1024) % 4096;
            if (value >= 0 && value < 2048) {
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x0,
                    (PSXLONG *)&quad.x3, (PSXLONG *)&quad.x2, &interpolation, &flag);
            } else {
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            }
            if (config->tile_size != 64) {
                setUVWH(&quad,
                    work->frames[i] * config->tile_size, work->rows[i] * config->tile_size,
                    config->tile_size - 1, config->tile_size - 1);
                quad.clut = work->textures[work->rows[i] + 1];
            } else {
                setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
                quad.clut = work->textures[1];
            }
            if (time >= 0 && time < config->travel_duration + config->fade_duration &&
                flag >= 0 && depth >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            work->frames[i] = (work->frames[i] + 1) % config->frames;
        }
    }
    setVector(&vertices[0], -config->flash_size / 2, -config->flash_size / 2, 0);
    setVector(&vertices[1], config->flash_size / 2, -config->flash_size / 2, 0);
    setVector(&vertices[2], -config->flash_size / 2, config->flash_size / 2, 0);
    setVector(&vertices[3], config->flash_size / 2, config->flash_size / 2, 0);
    setRGB0(&quad, config->red, config->green, config->blue);
    SetSemiTrans(&quad, 1);
    setVector(&rotation, 0, 0, 0);
    point = work->targets;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - (i * config->spacing + config->travel_duration);
        if (time >= 0 && time < 6) {
            value = work->scales[i];
            setVector(&scale, value, value, value);
            copyVector(&position, point);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.clut = work->textures[3];
            setUVWH(&quad, time / 2 * 64 + 64, 64, 63, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->travel_duration;
    if (time < 0) {
        return 0;
    }
    if (time > config->spacing * config->count + config->fade_duration) {
        if (work->completed) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

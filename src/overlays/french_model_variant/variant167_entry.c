#include "../../types.h"
#include "variant167_entry.h"

static const VECTOR D_8013BED8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model167State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, parameter;
    GsOT *ot;
    Model167Config *config;
    SVECTOR *point;
    s16 *offset, *increment;
    s32 i, time, value, angle0, angle1, step;
    u8 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BED8;
    work = (Model167State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        work->scroll = 0;
        config = work->config = &D_8013BF20[command % 100];
        offset = work->offsets;
        increment = work->increments;
        for (i = 0; i < 256; offset++, increment++, i++) {
            *offset = 0;
            *increment = config->amplitude
                + config->amplitude * csin(i * 32) / 4096;
        }
        work->phase = 0;
        point = work->points;
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        vertices[0].vy = -350;
        for (i = 0; i < config->count; point++, i++) {
            value = rand() % config->radius;
            angle0 = rand() % 4096;
            angle1 = rand() % 2048;
            point->vx = value * ccos(angle0) / 4096 * csin(angle1) / 4096;
            point->vy = value * ccos(angle1) / 4096;
            point->vz = value * csin(angle0) / 4096 * csin(angle1) / 4096;
            point->vx += vertices[0].vx;
            point->vy += vertices[0].vy;
            point->vz += vertices[0].vz;
        }
        work->texture[0] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BEE8[0]);
        for (i = 1; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEE8[i]);
        }
        work->elapsed = 0;
        work->started = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->fade_duration) {
        value = 2048 - time * 6144 / config->fade_duration;
        func_800595C8(2, value, value, value);
    } else if (time >= config->fade_duration) {
        func_800595C8(2, -4096, -4096, -4096);
    }
    time = work->elapsed - config->delay - config->particle_delay
        - config->count * config->stagger - config->particle_duration;
    if (time >= 0 && time < config->fade_duration) {
        value = -4096 + time * 6144 / config->fade_duration;
        func_800595C8(2, value, value, value);
    } else if (time >= config->fade_duration) {
        func_800595C8(2, 2048, 2048, 2048);
    }
    time = work->elapsed - config->delay;
    if (time < config->stagger * config->count + config->particle_duration
        + config->fade_duration + config->particle_delay && time >= 0) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        time = work->elapsed - config->delay;
        if (time >= 0 && time < config->fade_duration) {
            red = config->red * time / config->fade_duration;
            green = config->green * time / config->fade_duration;
            blue = config->blue * time / config->fade_duration;
            SetSemiTrans(&quad, 1);
        } else {
            time = work->elapsed - config->delay - config->particle_delay
                - config->stagger * config->count - config->particle_duration;
            if (time < 0) {
                red = config->red;
                green = config->green;
                blue = config->blue;
                SetSemiTrans(&quad, 0);
            } else {
                value = config->fade_duration - time;
                red = config->red * value / config->fade_duration;
                green = config->green * value / config->fade_duration;
                blue = config->blue * value / config->fade_duration;
                SetSemiTrans(&quad, 1);
            }
        }
        setRGB0(&quad, red, green, blue);
        offset = work->offsets;
        for (i = 0; i < 256; offset++, i++) {
            setUV4(&quad, 0, (work->scroll / 256 + i) % 64,
                192, (work->scroll / 256 + i) % 64,
                0, (work->scroll / 256 + i) % 64 + 1,
                192, (work->scroll / 256 + i) % 64 + 1);
            setXY4(&quad, *offset / 256 - 64, i, *offset / 256 + 128, i,
                *offset / 256 - 64, i + 1, *offset / 256 + 128, i + 1);
            Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 4095, 0, 0, 64, 64);
            setXY4(&quad, *offset / 256 + 128, i, *offset / 256 + 320, i,
                *offset / 256 + 128, i + 1, *offset / 256 + 320, i + 1);
            Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 4095, 0, 0, 64, 64);
        }
        offset = work->offsets;
        increment = &work->increments[work->phase];
        for (i = 0; i < 256 - work->phase; offset++, increment++, i++) {
            *offset += *increment;
            *offset %= 16384;
        }
        increment = work->increments;
        for (i = 0; i < work->phase; offset++, increment++, i++) {
            *offset += *increment;
            *offset %= 16384;
        }
        work->scroll = (work->scroll + (step << 8)) % 16384;
        work->phase = (work->phase + step) % 256;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setRGB0(&quad, config->particle_red, config->particle_green, config->particle_blue);
    point = work->points;
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - config->particle_delay - i * config->stagger;
        if (time >= 0 && time < config->particle_duration) {
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
            value = time * 8 / config->particle_duration;
            setUV4(&quad, 128 + value % 4 * 32, value / 4 * 32,
                128 + value % 4 * 32 + 31, value / 4 * 32,
                128 + value % 4 * 32, value / 4 * 32 + 31,
                128 + value % 4 * 32 + 31, value / 4 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->particle_delay;
    if (time < 0) {
        return 0;
    } else {
        time -= config->count * config->stagger + config->particle_duration;
        if (time >= config->fade_duration) {
            return 2;
        } else if (time < 0) {
            return 4;
        } else {
            if (!work->started) {
                work->started++;
                return 1;
            } else {
                return 0;
            }
        }
    }
}

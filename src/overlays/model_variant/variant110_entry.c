#include "../../types.h"
#include "variant110_entry.h"
#include "../../game/screen_projection.h"

/* Header 110's entry: after the delay two fans of triangles open and fade in
 * front of the slot, a sprite plays eight frames of the texture, a cloud of
 * dots grows out of the slot and a burst of sparks flies out, each over its
 * own duration. */

static const VECTOR D_8013C918 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model110State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G3 fan;
    GsBOXF dot;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[128];
    u16 depths[128], projection[128], flags[128];
    PSXLONG flag, interpolation;
    Model110Config *config;
    GsOT *ot;
    SVECTOR *point;
    DVECTOR *projected;
    u16 *depth_cursor, *flag_cursor;
    s32 i, t, fan_blue, sprite_blue, spark_blue, time, angle, elevation, radius, value, size, u, v;
    u8 step;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C918;
    work = (Model110State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C944[command];
        point = work->ring;
        setVector(point, 0, 0, 0);
        point++;
        for (i = 0; i < 16; point++, i++) {
            point->vx = config->ring_radius * ccos(i * 256 + 128) / 4096;
            point->vy = config->ring_radius * csin(i * 256 + 128) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 17; point++, i++) {
            value = rand() % config->ring_jitter + config->ring_radius * 2;
            point->vx = value * ccos(i * 256) / 4096;
            point->vy = value * csin(i * 256) / 4096;
            point->vz = 0;
        }
        point = work->dots;
        for (i = 0; i < config->dot_count; point++, i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = (rand() % config->dot_radius) / 2 + config->dot_radius / 2;
            point->vx = radius * ccos(value) / 4096 * csin(elevation) / 4096;
            point->vy = radius * ccos(elevation) / 4096;
            point->vz = radius * csin(value) / 4096 * csin(elevation) / 4096;
        }
        point = work->sparks;
        for (i = 0; i < config->spark_count; point++, i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = (rand() % config->spark_radius) / 2 + config->spark_radius / 2;
            point->vx = radius * ccos(value) / 4096 * csin(elevation) / 4096;
            point->vy = radius * ccos(elevation) / 4096;
            point->vz = radius * csin(value) / 4096 * csin(elevation) / 4096;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C928[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    SetPolyFT4(&quad);
    SetPolyG3(&fan);
    dot.attribute = 0x50000000;
    dot.w = 1;
    dot.h = 1;
    if (work->elapsed < config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->fan_duration) {
        SetSemiTrans(&fan, 1);
        v = config->fan_red * (config->fan_duration - time) / config->fan_duration;
        u = config->fan_green * (config->fan_duration - time) / config->fan_duration;
        fan_blue = config->fan_blue * (config->fan_duration - time) / config->fan_duration;
        setRGB0(&fan, v, u, fan_blue);
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = (time << 12) / config->fan_duration;
        size = value <= 4096;
        value = value >= 0 ? (size ? value : 4096) : 0;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 17);
        value = ((config->fan_duration - time) << 12) / config->fan_duration;
        setVector(&scale, value, value, value);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        elevation = 17;
        RotTransPersN(work->outer, &screen[17], &depths[17], &projection[17], &flags[17], elevation);
        for (i = 0; i < 16; i++) {
            /* The second triangle reads its far corner as t + 17, not i + 18: retail
             * gives that index an induction register of its own. */
            t = i + 1;
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 17].vx, screen[i + 17].vy,
                screen[i + 1].vx, screen[i + 1].vy);
            depth = AverageZ3(depths[0], depths[i + 17], depths[i + 1]);
            flag = (flags[0] | flags[i + 17] | flags[i + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 1].vx, screen[i + 1].vy,
                screen[t + 17].vx, screen[t + 17].vy);
            depth = AverageZ3(depths[0], depths[i + 1], depths[i + 18]);
            /* size also takes this flag; that is what gives the scale clamps their
             * registers. */
            flag = size = (flags[0] | flags[i + 1] | flags[i + 18]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
        angle = (config->fan_duration - time) * 160 / config->fan_duration;
        setRGB0(&fan, angle, angle, angle);
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        value = time * 3074 / config->fan_duration;
        size = value <= 4096;
        value = value >= 0 ? (size ? value : 4096) : 0;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 17);
        value = (config->fan_duration - time) * 3074 / config->fan_duration;
        setVector(&scale, value, value, value);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        elevation = 17;
        RotTransPersN(work->outer, &screen[17], &depths[17], &projection[17], &flags[17], elevation);
        for (i = 0; i < 16; i++) {
            t = i + 1;
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 17].vx, screen[i + 17].vy,
                screen[i + 1].vx, screen[i + 1].vy);
            depth = AverageZ3(depths[0], depths[i + 17], depths[i + 1]);
            flag = (flags[0] | flags[i + 17] | flags[i + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 1].vx, screen[i + 1].vy,
                screen[t + 17].vx, screen[t + 17].vy);
            depth = AverageZ3(depths[0], depths[i + 1], depths[i + 18]);
            flag = (flags[0] | flags[i + 1] | flags[i + 18]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->sprite_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        t = time * 8 / config->sprite_duration;
        value = t % 4 * 32;
        elevation = (time >= config->sprite_duration / 2) * 32;
        setUV4(&quad, value, elevation, value + 31, elevation, value, elevation + 31, value + 31, elevation + 31);
        SetSemiTrans(&quad, 1);
        v = config->sprite_red * (config->sprite_duration - time) / config->sprite_duration;
        u = config->sprite_green * (config->sprite_duration - time) / config->sprite_duration;
        sprite_blue = config->sprite_blue * (config->sprite_duration - time) / config->sprite_duration;
        setRGB0(&quad, v, u, sprite_blue);
        setVector(&vertices[0], -config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[1], config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[2], -config->sprite_size, config->sprite_size, 0);
        setVector(&vertices[3], config->sprite_size, config->sprite_size, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = ((config->sprite_duration - time) << 12) / config->sprite_duration;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->dot_duration) {
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = (time << 12) / config->dot_duration;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->dots, screen, depths, projection, flags, config->dot_count);
        projected = screen;
        depth_cursor = depths;
        flag_cursor = flags;
        dot.r = config->dot_red * (config->dot_duration - time) / config->dot_duration;
        dot.g = config->dot_green * (config->dot_duration - time) / config->dot_duration;
        dot.b = config->dot_blue * (config->dot_duration - time) / config->dot_duration;
        for (i = 0; i < config->dot_count; projected++, depth_cursor++, flag_cursor++, i++) {
            dot.x = projected->vx;
            dot.y = projected->vy;
            depth = *depth_cursor >> 2;
            flag = *flag_cursor & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortBoxFill(&dot, ot, depth);
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->spark_duration) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
        setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
        setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
        setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
        setVector(&vertices[3], config->spark_size, config->spark_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        v = config->spark_red * (config->spark_duration - time) / config->spark_duration;
        u = config->spark_green * (config->spark_duration - time) / config->spark_duration;
        spark_blue = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
        setRGB0(&quad, v, u, spark_blue);
        point = work->sparks;
        for (i = 0; i < config->spark_count; point++, i++) {
            setVector(&position, 0, -350, direction * 450);
            /* Retail sets vx instead of adding to it, so every spark starts at x = 0. */
            position.vx = point->vx * time / config->spark_duration;
            position.vy += point->vy * time / config->spark_duration;
            position.vz += point->vz * time / config->spark_duration;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    value = config->sprite_duration < config->fan_duration ? config->fan_duration : config->sprite_duration;
    value = value < config->spark_duration ? config->spark_duration : value;
    value = value < config->dot_duration ? config->dot_duration : value;
    time = work->elapsed - config->delay;
    if (time < 0) {
        return 0;
    }
    if (value < time) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed = 1;
    return 1;
}

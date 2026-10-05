#include "../../types.h"
#include "variant116_entry.h"

static const VECTOR D_8013C15C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model116State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    GsBOXF dot;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[128];
    u16 depths[128], projection[128], flags[128];
    PSXLONG flag, interpolation;
    Model116Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j, time, elevation, radius, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C15C;
    work = (Model116State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C1C0[command];
        point = work->ring;
        for (i = 0; i < 16; point++, i++) {
            point->vx = config->ring_radius * csin(i * 256) / 4096;
            point->vy = config->ring_radius * -ccos(i * 256) / 4096;
            point->vz = 0;
        }
        point = work->dots;
        for (i = 0; i < config->dot_count; point++, i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = (rand() % config->dot_radius) / 2 + config->dot_radius / 2;
            point->vx = (radius * ccos(value) / 4096) * csin(elevation) / 4096;
            point->vy = radius * ccos(elevation) / 4096;
            point->vz = (radius * csin(value) / 4096) * csin(elevation) / 4096;
        }
        point = work->sprites;
        for (i = 0; i < config->sprite_count; point++, i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = (rand() % config->sprite_radius) / 2 + config->sprite_radius / 2;
            point->vx = (radius * ccos(value) / 4096) * csin(elevation) / 4096;
            point->vy = radius * ccos(elevation) / 4096;
            point->vz = (radius * csin(value) / 4096) * csin(elevation) / 4096;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C16C[i]);
        }
        work->elapsed = -config->initial_delay;
        setVector(&vertices[0], 0, 0, 0);
        work->frame_toggle = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    SetPolyFT4(&quad);
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    dot.attribute = 0x50000000;
    dot.w = 1;
    dot.h = 1;
    setVector(&vertices[0], -config->ring_width / 2, -config->ring_height,
        -direction * config->ring_height);
    setVector(&vertices[1], config->ring_width / 2, -config->ring_height,
        -direction * config->ring_height);
    setVector(&vertices[2], -config->ring_width / 2, 0, 0);
    setVector(&vertices[3], config->ring_width / 2, 0, 0);
    setUV4(&quad, 0, 0, 31, 0, 0, 47, 31, 47);
    SetSemiTrans(&quad, 1);
    for (i = 0; i < config->ring_count; i++) {
        time = work->elapsed - i * config->ring_stagger;
        if (time >= 0 && time < config->ring_duration) {
            s32 red, green, blue;

            red = config->ring_r * (config->ring_duration - time) / config->ring_duration;
            green = config->ring_g * (config->ring_duration - time) / config->ring_duration;
            blue = config->ring_b * (config->ring_duration - time) / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            point = work->ring;
            for (j = 0; j < 16; point++, j++) {
                setVector(&position, 0, -350, direction * 450);
                position.vx = point->vx * time / config->ring_duration;
                position.vy += point->vy * time / config->ring_duration;
                position.vz += point->vz * time / config->ring_duration;
                setVector(&rotation, 0, 0, j * 256);
                value = time * 4096 / config->ring_duration;
                setVector(&scale, value, value, value);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    time = work->elapsed;
    if (time >= 0 && time < config->dot_duration) {
        DVECTOR *projected;
        u16 *depth_cursor, *flag_cursor;

        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = time * 4096 / config->dot_duration;
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
        dot.r = config->dot_r * (config->dot_duration - time) / config->dot_duration;
        dot.g = config->dot_g * (config->dot_duration - time) / config->dot_duration;
        dot.b = config->dot_b * (config->dot_duration - time) / config->dot_duration;
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
    time = work->elapsed;
    if (time >= 0 && time < config->sprite_duration) {
        s32 red, green, blue;

        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 48, 31, 48, 0, 79, 31, 79);
        setVector(&vertices[0], -config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[1], config->sprite_size, -config->sprite_size, 0);
        setVector(&vertices[2], -config->sprite_size, config->sprite_size, 0);
        setVector(&vertices[3], config->sprite_size, config->sprite_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        red = config->sprite_r * (config->sprite_duration - time) / config->sprite_duration;
        green = config->sprite_g * (config->sprite_duration - time) / config->sprite_duration;
        blue = config->sprite_b * (config->sprite_duration - time) / config->sprite_duration;
        setRGB0(&quad, red, green, blue);
        point = work->sprites;
        for (i = 0; i < config->sprite_count; point++, i++) {
            setVector(&position, 0, -350, direction * 450);
            position.vx = point->vx * time / config->sprite_duration;
            position.vy += point->vy * time / config->sprite_duration;
            position.vz += point->vz * time / config->sprite_duration;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    work->frame_toggle ^= 1;
    value = config->ring_stagger * config->ring_count + config->ring_duration < (s32)config->dot_duration
        ? (s32)config->dot_duration
        : config->ring_stagger * config->ring_count + config->ring_duration;
    value = value < (s32)config->sprite_duration ? (s32)config->sprite_duration : value;
    time = work->elapsed;
    if (time < 0) {
        return 0;
    }
    if (time > value) {
        return 2;
    }
    return time <= config->ring_stagger;
}

#include "../../types.h"
#include "variant141_entry.h"

static const VECTOR D_8013BF60 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model141State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flat;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    PSXLONG flag, interpolation;
    Model141Config *config;
    GsOT *ot;
    s32 i, j, time, value;
    SVECTOR *point, *velocity;
    s32 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BF60;
    work = (Model141State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013BFA8[command];
        config = work->config;
        point = work->positions;
        /* Native initialization does not advance this cursor. */
        for (i = 0; i < config->count; i++) {
            setVector(point, 0, 0, 0);
        }
        point = work->offsets;
        for (i = 0; i < 32; point++, i++) {
            setVector(point,
                rand() % (config->offset_spread * 2) - config->offset_spread,
                rand() % (config->offset_spread * 2) - config->offset_spread,
                (direction * rand()) % config->offset_spread);
        }
        point = work->velocities;
        for (i = 0; i < 32; point++, i++) {
            setVector(point,
                rand() % (config->point_spread * 2) - config->point_spread,
                rand() % (config->point_spread * 2) - config->point_spread,
                rand() % (config->point_spread * 2) - config->point_spread);
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BF70[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flat);
    SetSemiTrans(&quad, 1);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    setVector(&vertices[0], -config->grid_half_size, -config->grid_half_size, 0);
    setVector(&vertices[1], 0, -config->grid_half_size, 0);
    setVector(&vertices[2], config->grid_half_size, -config->grid_half_size, 0);
    setVector(&vertices[3], -config->grid_half_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->grid_half_size, 0, 0);
    setVector(&vertices[6], -config->grid_half_size, config->grid_half_size, 0);
    setVector(&vertices[7], 0, config->grid_half_size, 0);
    setVector(&vertices[8], config->grid_half_size, config->grid_half_size, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->travel_delay - config->spacing * i;
        if (time < 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        copyVector(&position, point);
        if (time >= 0 && time < config->travel_duration) {
            value = time * 4096 / config->travel_duration;
            setVector(&scale, value, value, value);
            point->vy += (-350 - point->vy) / (config->travel_duration - time);
            point->vz += (-direction * config->travel_depth - point->vz) / (config->travel_duration - time);
            red = config->red * (config->travel_duration - time) / config->travel_duration;
            green = config->green * (config->travel_duration - time) / config->travel_duration;
            blue = config->blue * (config->travel_duration - time) / config->travel_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            depth = RotAverage4(&vertices[2], &vertices[1], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            depth = RotAverage4(&vertices[6], &vertices[7], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            depth = RotAverage4(&vertices[8], &vertices[7], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    setVector(&vertices[0], -config->sprite_half_size, -config->sprite_half_size, 0);
    setVector(&vertices[1], config->sprite_half_size, -config->sprite_half_size, 0);
    setVector(&vertices[2], -config->sprite_half_size, config->sprite_half_size, 0);
    setVector(&vertices[3], config->sprite_half_size, config->sprite_half_size, 0);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    SetSemiTrans(&quad, 1);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->offsets;
    for (i = 0; i < 32; point++, i++) {
        setVector(&position, 0, -350, direction * 450);
        position.vx += point->vx;
        position.vy += point->vy;
        position.vz += point->vz;
        time = work->elapsed - config->burst_delay - i * config->spacing;
        if (time >= 0 && time < config->sprite_duration) {
            red = config->sprite_red * (config->sprite_duration - time) / config->sprite_duration;
            green = config->sprite_green * (config->sprite_duration - time) / config->sprite_duration;
            blue = config->sprite_blue * (config->sprite_duration - time) / config->sprite_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            value = time * 3 / config->sprite_duration;
            setUV4(&quad, 64 + value * 32, 0, 95 + value * 32, 0,
                64 + value * 32, 31, 95 + value * 32, 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        time = work->elapsed - config->burst_delay - i * config->spacing;
        velocity = work->velocities;
        if (time >= 0 && time < config->point_duration) {
            for (j = 0; j < 32; velocity++, j++) {
                box.r = config->point_gray * (config->point_duration - time) / config->point_duration;
                box.g = config->point_gray * (config->point_duration - time) / config->point_duration;
                box.b = config->point_gray * (config->point_duration - time) / config->point_duration;
                copyVector(&vertices[4], velocity);
                vertices[4].vx *= time;
                vertices[4].vy *= time;
                vertices[4].vz *= time;
                depth = RotTransPers(&vertices[4], (PSXLONG *)&box.x, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortBoxFill(&box, ot, depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        return 0;
    }
    time -= config->spacing * 32;
    if (time >= config->sprite_duration) {
        if (work->completed) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}

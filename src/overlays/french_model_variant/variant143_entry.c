#include "../../types.h"
#include "variant143_entry.h"

static const VECTOR D_8013BFF4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model143State *work;
    Model143Config *config;
    MATRIX base, matrix;
    POLY_F3 triangle;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[6];
    PSXLONG flag, interpolation, depth;
    GsOT *ot;
    s32 direction, step, i, j, time, clip, value;
    SVECTOR *point, *secondary, *angles;
    Model143Face *face;
    CVECTOR *color;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BFF4;
    work = (Model143State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C020[command];
        point = work->positions;
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = 0;
        }
        point = work->targets;
        for (i = 0; i < config->count; i++, point++) {
            setVector(point, rand() % (config->spread * 2) - config->spread,
                0, direction * (rand() % config->spread + 450));
        }
        point = work->rotations;
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, 0, 0, rand() % 4096);
        }
        color = work->colors;
        for (i = 0; i < 4; color++, i++) {
            color->r = config->red * (i + 4) / 8;
            color->g = config->green * (i + 4) / 8;
            color->b = config->blue * (i + 4) / 8;
        }
        for (i = 0; i < 2; i++, color++) {
            color->r = config->red;
            color->g = config->green;
            color->b = config->blue;
        }
        point = work->vertices;
        setVector(&point[0], 0, 0, direction * config->length / 2);
        setVector(&point[1], 0, -config->half_width, -direction * config->length / 2);
        setVector(&point[2], direction * config->half_width, 0, -direction * config->length / 2);
        setVector(&point[3], 0, config->half_width, -direction * config->length / 2);
        setVector(&point[4], -direction * config->half_width, 0, -direction * config->length / 2);
        work->faces[0].a = &point[1];
        work->faces[0].b = &point[0];
        work->faces[0].c = &point[2];
        work->faces[1].a = &point[2];
        work->faces[1].b = &point[0];
        work->faces[1].c = &point[3];
        work->faces[2].a = &point[3];
        work->faces[2].b = &point[0];
        work->faces[2].c = &point[4];
        work->faces[3].a = &point[4];
        work->faces[3].b = &point[0];
        work->faces[3].c = &point[1];
        work->faces[4].a = &point[1];
        work->faces[4].b = &point[2];
        work->faces[4].c = &point[3];
        work->faces[5].a = &point[3];
        work->faces[5].b = &point[4];
        work->faces[5].c = &point[1];
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C004[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyF3(&triangle);
    SetPolyFT4(&quad);
    SetSemiTrans(&triangle, 0);
    SetSemiTrans(&quad, 1);
    setVector(&scale, 4096, 4096, 4096);
    point = work->positions;
    secondary = work->targets;
    angles = work->rotations;
    for (i = 0; i < config->count; point++, secondary++, angles++, i++) {
        time = work->elapsed - (config->delay + i * config->spacing);
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->vy -= rand() % config->height_spread;
            secondary->vy = point->vy;
        } else if (time < config->travel_duration + config->hold_duration) {
            copyVector(&position, point);
            copyVector(&rotation, angles);
            if (time < config->travel_duration || point->pad) {
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                face = work->faces;
                color = work->colors;
                for (j = 0; j < 6; face++, color++, j++) {
                    clip = RotAverageNclip3(face->a, face->b, face->c,
                        (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                        (PSXLONG *)&triangle.x2, &interpolation, &depth, &flag);
                    setRGB0(&triangle, color->r, color->g, color->b);
                    if (clip > 0 && depth > 0 && flag >= 0) {
                        GsSortPoly(&triangle, ot, (u16)depth);
                    }
                }
                point->pad = 0;
            } else {
                point->pad = 1;
            }
            if (time < config->travel_duration - step) {
                copyVector(&vertices[0], point);
                applyVector(&vertices[0], -1, -1, -1, *=);
                addVector(&vertices[0], secondary);
                value = (config->travel_duration - time) / step;
                applyVector(&vertices[0], value, value, value, /=);
                addVector(point, &vertices[0]);
                angles->vx = (angles->vx + config->rotation_x) % 4096;
                angles->vy = (angles->vy + config->rotation_y) % 4096;
                angles->vz = (angles->vz + config->rotation_z) % 4096;
            } else {
                copyVector(point, secondary);
            }
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], 0, -config->sprite_half_size, 0);
    setVector(&vertices[1], config->sprite_half_size, -config->sprite_half_size, 0);
    setVector(&vertices[2], 0, config->sprite_half_size, 0);
    setVector(&vertices[3], config->sprite_half_size, config->sprite_half_size, 0);
    setVector(&vertices[4], -config->sprite_half_size, -config->sprite_half_size, 0);
    setVector(&vertices[5], -config->sprite_half_size, config->sprite_half_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->targets;
    secondary = work->rotations;
    for (i = 0; i < config->count; point++, secondary++, i++) {
        time = work->elapsed - (config->delay + i * config->spacing + config->travel_duration);
        if (time >= 0 && time < config->sprite_duration) {
            copyVector(&position, point);
            setVector(&rotation, 0, 0, secondary->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            red = config->sprite_red * time / config->sprite_duration;
            green = config->sprite_green * time / config->sprite_duration;
            blue = config->sprite_blue * time / config->sprite_duration;
            setRGB0(&quad, red, green, blue);
            value = time * 4 / config->sprite_duration;
            setUV4(&quad, value * 32, 0, value * 32 + 31, 0,
                value * 32, 63, value * 32 + 31, 63);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth *= 2;
            depth += RotTransPers(&vertices[4], (PSXLONG *)&quad.x1, &interpolation, &flag);
            depth += RotTransPers(&vertices[5], (PSXLONG *)&quad.x3, &interpolation, &flag);
            depth /= 4;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay;
    if (time < config->travel_duration) {
        return 0;
    }
    if (time >= config->count * config->spacing + config->travel_duration + config->hold_duration) {
        return 2;
    }
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - (config->delay + i * config->spacing);
        if (time > config->travel_duration && work->completed == i) {
            work->completed = i + 1;
            if (i == config->count - 1) {
                return 1;
            }
            return 4;
        }
    }
    value = Model_GetSlotAnimationIndex(Model_GetActiveSlotIndex() ^ 1);
    if (value == 5 || value == 8) {
        return 4;
    }
    return 0;
}

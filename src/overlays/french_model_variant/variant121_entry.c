#include "../../types.h"
#include "variant121_entry.h"

static const VECTOR D_8013C0A8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model121State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT3 triangle;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    DVECTOR screen[64];
    u16 depths[64], projection[64], flags[64];
    PSXLONG flag, interpolation;
    Model121Config *config;
    GsOT *ot;
    SVECTOR *point, *destination;
    s32 i, time, angle, value;
    s32 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C0A8;
    work = (Model121State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C0F0[command % 100];
        point = work->positions;
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, 0, 0, 0);
        }
        point = work->destinations;
        for (i = 0; i < config->count; point++, i++) {
            point->vx = rand() % config->spread * 2 - config->spread;
            point->vy = rand() % config->spread * 2 - config->spread - 350;
            point->vz = direction * 900;
        }
        point = work->ring;
        setVector(point, 0, 0, 0);
        point++;
        for (i = 0; i < 25; point++, i++) {
            angle = i * 4096 / 24;
            point->vx = config->ring_radius * ccos(angle) / 4096;
            point->vy = config->ring_radius * csin(angle) / 4096;
            point->vz = direction * config->ring_depth
                + (config->ring_depth / 4) * csin(i * 8192 / 24) / 4096;
        }
        if (command < 100) {
            for (i = 0; i < 24; i++) {
                if (i < 8) {
                    work->palette.colors[i].r = 0;
                    work->palette.colors[i].g = i * 3 * 255 / 24;
                    work->palette.colors[i].b = (24 - i * 3) * 255 / 24;
                } else if (i < 16) {
                    work->palette.colors[i].r = (i - 8) * 3 * 255 / 24;
                    work->palette.colors[i].g = (24 - (i - 8) * 3) * 255 / 24;
                    work->palette.colors[i].b = 0;
                } else {
                    work->palette.colors[i].r = (24 - (i - 16) * 3) * 255 / 24;
                    work->palette.colors[i].g = 0;
                    work->palette.colors[i].b = (i - 16) * 3 * 255 / 24;
                }
            }
        } else {
            for (i = 0; i < 24; i++) {
                work->palette.colors[i].r = config->red;
                work->palette.colors[i].g = config->green;
                work->palette.colors[i].b = config->blue;
            }
        }
        for (i = 0; i < 2; i++) {
            work->palette.storage.texture[i] = func_80059A50(
                Model_GetActiveSlotIndex(), 1, &D_8013C0B8[i]);
        }
        work->elapsed = -config->initial_delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        PushMatrix();
        point = work->positions;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        for (i = 0; i < 24; point++, i++) {
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        work->elapsed += Model_GetFrameStep();
        PopMatrix();
        return 0;
    }
    PushMatrix();
    point = work->positions;
    SetPolyFT4(&quad);
    SetPolyGT3(&triangle);
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->sprite_size, -config->sprite_size, 0);
    setVector(&vertices[1], 0, -config->sprite_size, 0);
    setVector(&vertices[2], config->sprite_size, -config->sprite_size, 0);
    setVector(&vertices[3], -config->sprite_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->sprite_size, 0, 0);
    setVector(&vertices[6], -config->sprite_size, config->sprite_size, 0);
    setVector(&vertices[7], 0, config->sprite_size, 0);
    setVector(&vertices[8], config->sprite_size, config->sprite_size, 0);
    destination = work->destinations;
    for (i = 0; i < config->count; point++, destination++, i++) {
        time = work->elapsed - i * config->particle_stagger;
        if (time >= 0 && time < config->travel_duration) {
            quad.tpage = work->palette.storage.texture[0] >> 16;
            quad.clut = work->palette.storage.texture[0];
            setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
            if (direction * point->vz <= 450) {
                red = work->palette.colors[(time / 2) % 24].r;
                green = work->palette.colors[(time / 2) % 24].g;
                blue = work->palette.colors[(time / 2) % 24].b;
            } else {
                red = work->palette.colors[(time / 2) % 24].r * (900 - direction * point->vz) / 450;
                green = work->palette.colors[(time / 2) % 24].g * (900 - direction * point->vz) / 450;
                blue = work->palette.colors[(time / 2) % 24].b * (900 - direction * point->vz) / 450;
            }
            setRGB0(&quad, red, green, blue);
            setVector(&position, point->vx, point->vy, point->vz);
            setVector(&rotation, 0, 0, 0);
            value = 512 + time * 3584 / config->travel_duration;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[2], &vertices[1], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[6], &vertices[7], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[8], &vertices[7], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (destination->vx - point->vx) / (config->travel_duration - time) * 2;
            point->vy += (destination->vy - point->vy) / (config->travel_duration - time) * 2;
            point->vz += (destination->vz - point->vz) / (config->travel_duration - time) * 2;
        } else {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
    }
    time = work->elapsed;
    if (time >= 0 && time < config->ring_duration) {
        triangle.tpage = work->palette.storage.texture[1] >> 16;
        triangle.clut = work->palette.storage.texture[1];
        setUV3(&triangle, 0, 96, 255, 64, 255, 127);
        setRGB0(&triangle, 0, 0, 0);
        SetSemiTrans(&triangle, 1);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        setVector(&rotation, 0, 0, time * 2048 / 24);
        if (time < config->ring_duration - config->fade_duration
            && time >= config->fade_duration) {
            value = 4096;
        } else if (time < config->fade_duration) {
            value = time * 4096 / config->fade_duration;
        } else {
            value = (config->ring_duration - time) * 4096 / config->fade_duration;
        }
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 26);
        for (i = 0; i < 24; i++) {
            setXY3(&triangle, screen[0].vx, screen[0].vy,
                screen[i + 1].vx, screen[i + 1].vy,
                screen[i + 2].vx, screen[i + 2].vy);
            setRGB1(&triangle, work->palette.colors[i].r,
                work->palette.colors[i].g, work->palette.colors[i].b);
            setRGB2(&triangle, work->palette.colors[i + 1].r,
                work->palette.colors[i + 1].g, work->palette.colors[i + 1].b);
            triangle.v1 = (i * 128 / 24) % 64 + 64;
            triangle.v2 = ((i + 1) * 128 / 24) % 64 + 64;
            depth = AverageZ3(depths[0], depths[i], depths[i + 1]);
            flag = (flags[0] | flags[i] | flags[i + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&triangle, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->travel_duration * 2 / 3;
    if (time < 0) {
        return 0;
    }
    if (time > config->count * config->particle_stagger + (s16)(config->travel_duration / 3)) {
        if (work->completed) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}

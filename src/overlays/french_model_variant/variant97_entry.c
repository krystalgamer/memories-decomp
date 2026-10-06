#include "../../types.h"
#include "variant97_entry.h"

static const VECTOR D_8013C158 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model97State *work;
    Model97Config *config;
    MATRIX base, matrix, ring_matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation, depth;
    GsOT *ot;
    SVECTOR *point, *other;
    s32 i, value, time, frame;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C158;
    work = (Model97State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        config = work->config = &D_8013C1BC[command];
        point = work->ring;
        for (i = 0; i < config->ring_count; point++, i++) {
            point->vx = config->radius * ccos((i << 12) / config->ring_count) / 4096;
            point->vy = 0;
            point->vz = config->radius * csin((i << 12) / config->ring_count) / 4096;
        }
        point = work->offsets;
        for (i = 0; i < config->particle_count; point++, i++) {
            s32 angle_b, magnitude;
            value = rand() % 4096;
            angle_b = rand() % 1536 + 512;
            magnitude = rand() % config->spread;
            point->vx = magnitude * ccos(value) / 4096 * csin(angle_b) / 4096;
            point->vy = magnitude * ccos(angle_b) / 4096;
            point->vz = magnitude * csin(value) / 4096 * csin(angle_b) / 4096;
        }
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
        work->target.vy = 0;
        work->initialized = 0;
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C168[i]);
        }
        work->texture[2] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013C168[2]);
        work->elapsed = -config->delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (!work->initialized) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        point = work->path;
        other = &work->path[config->path_count];
        for (i = 0; i < config->path_count; point++, other++, i++) {
            point->vx = matrix.t[0] - matrix.t[0] * i / config->path_count;
            point->vy = 0;
            point->vz = matrix.t[2] + (work->target.vz - matrix.t[2]) * i / config->path_count;
            copyVector(other, point);
            other->vy = -config->height;
        }
        work->initialized = 1;
    }
    SetPolyFT4(&quad);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setRGB0(&quad, config->red, config->green, config->blue);
    setVector(&vertices[0], -config->height / 4, -config->height, 0);
    setVector(&vertices[1], config->height / 4, -config->height, 0);
    setVector(&vertices[2], -config->height / 4, 0, 0);
    setVector(&vertices[3], config->height / 4, 0, 0);
    SetSemiTrans(&quad, 1);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->path;
    for (i = 0; i < config->path_count; point++, i++) {
        time = work->elapsed - i * config->stagger;
        if (time >= 0 && time < config->beam_duration) {
            copyVector(&position, point);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            frame = time * 16 / config->beam_duration;
            if (frame >= 8) {
                frame = 15 - frame;
            }
            setUV4(&quad, frame * 32, 64, frame * 32 + 31, 64,
                frame * 32, 191, frame * 32 + 31, 191);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth & 0xffff);
            }
        }
    }
    time = work->elapsed - config->stagger * config->path_count;
    if (time >= 0 && time < config->ring_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        red = config->red * (config->ring_duration - time) / config->ring_duration;
        green = config->green * (config->ring_duration - time) / config->ring_duration;
        blue = config->blue * (config->ring_duration - time) / config->ring_duration;
        setRGB0(&quad, red, green, blue);
        {
            s32 growth_time = time * 3;
            value = config->height + config->height * growth_time / config->ring_duration;
        }
        setVector(&vertices[0], -config->height / 4, -value, 0);
        setVector(&vertices[1], config->height / 4, -value, 0);
        setVector(&vertices[2], -config->height / 4, 0, 0);
        setVector(&vertices[3], config->height / 4, 0, 0);
        SetSemiTrans(&quad, 1);
        setVector(&rotation, 0, config->spin * time / config->ring_duration, 0);
        value = (time << 12) / config->ring_duration;
        setVector(&scale, value, 4096, value);
        RotMatrix(&rotation, &ring_matrix);
        ScaleMatrix(&ring_matrix, &scale);
        ring_matrix.t[0] = ring_matrix.t[1] = ring_matrix.t[2] = 0;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        point = work->ring;
        for (i = 0; i < config->ring_count; point++, i++) {
            GsSetLsMatrix(&ring_matrix);
            RotTransSV(point, &position, &flag);
            addVector(&position, &work->target);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, (time / 2 % 8) * 32, 64, (time / 2 % 8) * 32 + 31, 64,
                (time / 2 % 8) * 32, 191, (time / 2 % 8) * 32 + 31, 191);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth & 0xffff);
            }
        }
    }
    time = work->elapsed - config->stagger * config->path_count;
    if (time >= 0 && time < config->particle_duration) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        red = config->particle_red * time / config->particle_duration;
        green = config->particle_green * time / config->particle_duration;
        blue = config->particle_blue * time / config->particle_duration;
        setRGB0(&quad, red, green, blue);
        setVector(&vertices[0], -config->half_size, -config->half_size, 0);
        setVector(&vertices[1], config->half_size, -config->half_size, 0);
        setVector(&vertices[2], -config->half_size, config->half_size, 0);
        setVector(&vertices[3], config->half_size, config->half_size, 0);
        SetSemiTrans(&quad, 1);
        value = time * 8 / config->particle_duration;
        setUV4(&quad, value % 4 * 32, 192 + value / 4 * 32,
            value % 4 * 32 + 31, 192 + value / 4 * 32,
            value % 4 * 32, 192 + value / 4 * 32 + 31,
            value % 4 * 32 + 31, 192 + value / 4 * 32 + 31);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        point = work->offsets;
        for (i = 0; i < config->particle_count; point++, i++) {
            Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&position);
            addVector(&position, point);
            position.vy -= config->lift * time / config->particle_duration;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth & 0xffff);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->stagger * config->path_count;
    if (time < 0) {
        return 0;
    }
    if (time > config->particle_duration) {
        Model_SetSlotTintTarget((~Model_GetActiveSlotIndex()) & 1, 0, 128, 128, 128);
        return 2;
    }
    if (time > config->ring_duration) {
        if (work->completed) {
            return 0;
        } else {
            work->completed++;
            return 1;
        }
    }
    return 4;
}

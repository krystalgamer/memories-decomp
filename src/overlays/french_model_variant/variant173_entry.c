#include "../../types.h"
#include "variant173_entry.h"

const VECTOR D_8013BEB0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model173State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model173Config *config;
    GsOT *ot;
    SVECTOR *point, *offset;
    s32 step, i, j, time, value;
    s32 particle_index;
    u8 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BEB0;
    work = (Model173State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BFD8[command % 100];
        point = work->offsets;
        for (i = 0; i < config->count; point++, i++) {
            value = rand() % 4096;
            j = rand() % 2048;
            time = rand() % config->spread;
            point->vx = time * csin(value) / 4096 * csin(j) / 4096;
            point->vy = time * ccos(j) / 4096;
            point->vz = time * ccos(value) / 4096 * csin(j) / 4096;
        }
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->center);
        work->center.vy = -350;
        if (command < 100) {
            for (i = 0; i < 10; i++) {
                work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BEC0[i]);
            }
        } else {
            for (i = 0; i < 10; i++) {
                work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEC0[i]);
            }
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
        work->mode = command / 100;
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    offset = work->offsets;
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    for (i = 0; i < config->count / config->group_size; i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            for (particle_index = 0; particle_index < config->group_size; point++, offset++, particle_index++) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
                setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            }
        } else if (time < config->gather_duration + config->travel_duration) {
            quad.tpage = work->texture[1] >> 16;
            quad.clut = work->texture[1];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, 0, 128, 63, 128, 0, 191, 63, 191);
            if (time < config->gather_duration) {
                value = (i + work->frames) * 128 % 3 + 1024;
            } else {
                s32 growth, base_scale;
                j = time - config->gather_duration;
                growth = j * 2048 / config->travel_duration;
                base_scale = (i + work->frames) * 128 % 3 + 2048;
                value = growth + base_scale;
            }
            setVector(&scale, value, value, value);
            for (particle_index = 0; particle_index < config->group_size; point++, offset++, particle_index++) {
                setVector(&rotation, 0, 0, rand() % 4096);
                setVector(&position, point->vx, point->vy, point->vz);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                if (time < config->gather_duration && work->mode == 0) {
                    red = config->red * time / config->gather_duration;
                    green = config->green * time / config->gather_duration;
                    blue = config->blue * time / config->gather_duration;
                } else {
                    red = config->red;
                    green = config->green;
                    blue = config->blue;
                }
                setRGB0(&quad, red, green, blue);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                if (time < config->gather_duration) {
                    point->vx += (offset->vx / 4 + work->center.vx - point->vx) /
                        (config->gather_duration - time) * step;
                    point->vy += (offset->vy / 4 + work->center.vy - point->vy) /
                        (config->gather_duration - time) * step;
                    point->vz += (direction * 450 - point->vz) /
                        (config->gather_duration - time) * step;
                } else {
                    value = time - config->gather_duration;
                    point->vx += (offset->vx + work->center.vx - point->vx) /
                        (config->travel_duration - value) * step;
                    point->vy += (offset->vy + work->center.vy - point->vy) /
                        (config->travel_duration - value) * step;
                    point->vz += (offset->vz + work->center.vz - point->vz) /
                        (config->travel_duration - value) * step;
                }
            }
        } else if (time < config->gather_duration + config->travel_duration + config->burst_duration) {
            quad.tpage = work->texture[2] >> 16;
            quad.clut = work->texture[2];
            SetSemiTrans(&quad, 1);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            for (particle_index = 0; particle_index < config->group_size; point++, offset++, particle_index++) {
                setVector(&position, offset->vx, offset->vy, offset->vz);
                position.vx += work->center.vx;
                position.vy += work->center.vy;
                position.vz += work->center.vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                red = config->red;
                green = config->green;
                blue = config->blue;
                setRGB0(&quad, red, green, blue);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                value = ((time - config->gather_duration - config->travel_duration) * 8) /
                    config->burst_duration;
                setUV4(&quad, value % 4 * 64, value / 4 * 64,
                       value % 4 * 64 + 63, value / 4 * 64,
                       value % 4 * 64, value / 4 * 64 + 63,
                       value % 4 * 64 + 63, value / 4 * 64 + 63);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        } else {
            point += config->group_size;
            offset += config->group_size;
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->gather_duration;
    if (time < 0) {
        return 0;
    }
    time -= config->count / config->group_size * config->spacing +
        config->travel_duration + config->burst_duration;
    if (time < 0) {
        return 4;
    }
    if (work->completed != 0) {
        return 2;
    }
    work->completed++;
    return 1;
}

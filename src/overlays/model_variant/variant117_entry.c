#include "../../types.h"
#include "variant117_entry.h"
#include "../../game/screen_projection.h"

/* Header 117's entry: eight textured sprites fly out from a model part on
 * looping timers, a beam of quads then grows from the part towards the
 * camera axis, and two spinning squares pulse at the end. */

static const VECTOR D_8013CBF0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model117State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad, unused_quad;
    POLY_FT3 tri;
    POLY_G3 unused_gouraud;
    POLY_GT4 unused_strip;
    SVECTOR saved, rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    SVECTOR unused_points[3];
    DVECTOR screen[18];
    u16 depths[18], projection[18], flags[18];
    PSXLONG flag, interpolation;
    Model117Config *config;
    GsOT *ot;
    SVECTOR *point;
    u8 step;
    s32 i, time, size, tilt, half, length, dy, offset, apex, near, far;
    PSXLONG depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CBF0;
    work = (Model117State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CC8C[command];
        point = work->directions;
        for (i = 0; i < 8; point++, i++) {
            size = rand() % 4096;
            length = rand() % 2048 + (Model_GetActiveSlotIndex() ^ 1) * 2048;
            point->vx = config->spread / config->sprite_period * ccos(size) / 4096 * ccos(length) / 4096;
            point->vy = config->spread / config->sprite_period * csin(size) / 4096 * ccos(length) / 4096;
            point->vz = config->spread / config->sprite_period * csin(length) / 4096;
        }
        for (i = 7; i >= 0; i--) {
            work->timers[i] = i * -2;
        }
        point = work->ring;
        for (i = 0; i < 9; point++, i++) {
            point->vx = config->beam_radius * ccos(i * 512) / 4096;
            point->vy = config->beam_radius * csin(i * 512) / 4096;
            point->vz = 0;
        }
        work->squares[0].vx = 0;
        point = &work->squares[0];
        point->vy = -config->square_size / 2;
        point->vz = 0;
        work->squares[1].vx = config->square_size;
        point = &work->squares[1];
        point->vy = -config->square_size / 2;
        point->vz = 0;
        work->squares[2].vx = 0;
        point = &work->squares[2];
        point->vy = config->square_size / 2;
        point->vz = 0;
        work->squares[3].vx = config->square_size / 2;
        point = &work->squares[3];
        point->vy = config->square_size / 2;
        point->vz = 0;
        work->squares[4].vx = 0;
        point = &work->squares[4];
        point->vy = -config->square_size / 2;
        point->vz = 0;
        work->squares[5].vx = config->square_size;
        point = &work->squares[5];
        point->vy = -config->square_size / 2;
        point->vz = -direction * config->square_size;
        work->squares[6].vx = 0;
        point = &work->squares[6];
        point->vy = config->square_size / 2;
        point->vz = 0;
        work->squares[7].vx = config->square_size / 2;
        point = &work->squares[7];
        point->vy = config->square_size / 2;
        point->vz = -direction * config->square_size;
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CC00[i]);
        }
        work->elapsed = -config->start_delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    SetPolyFT4(&quad);
    SetPolyFT4(&unused_quad);
    SetPolyFT3(&tri);
    SetPolyG3(&unused_gouraud);
    SetPolyGT4(&unused_strip);
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    setVector(&vertices[0], -config->sprite_size, -config->sprite_size, 0);
    setVector(&vertices[1], config->sprite_size, -config->sprite_size, 0);
    setVector(&vertices[2], -config->sprite_size, config->sprite_size, 0);
    setVector(&vertices[3], config->sprite_size, config->sprite_size, 0);
    setVector(&origin, 0, 0, 0);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    tilt = -catan((-350 - matrix.t[1]) * 4096 / (direction * 450 - matrix.t[2]));
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration - config->delay) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        size = csin(tilt);
        length = ccos(tilt);
        point = work->directions;
        for (i = 0; i < 8; point++, i++) {
            if (work->timers[i] >= 0) {
                setUV4(&quad, work->timers[i] / 8 * 32, 0, work->timers[i] / 8 * 32 + 31, 0,
                    work->timers[i] / 8 * 32, 31, work->timers[i] / 8 * 32 + 31, 31);
                red = config->sprite_red * (config->sprite_period - work->timers[i]) / config->sprite_period;
                green = config->sprite_green * (config->sprite_period - work->timers[i]) / config->sprite_period;
                blue = config->sprite_blue * (config->sprite_period - work->timers[i]) / config->sprite_period;
                setRGB0(&quad, red, green, blue);
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                position.vx = position.vx * work->timers[i];
                position.vy = position.vy * work->timers[i] * size / 4096;
                position.vz = position.vz * work->timers[i] * length / 4096;
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
                position.vx += matrix.t[0];
                position.vy += matrix.t[1];
                position.vz += matrix.t[2];
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
                work->timers[i] = (work->timers[i] + step) % config->sprite_period;
            } else {
                work->timers[i] += step;
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration - config->delay + config->rise_duration) {
        SetSemiTrans(&quad, 1);
        SetSemiTrans(&tri, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        tri.tpage = work->texture[1] >> 16;
        tri.clut = work->texture[1];
        setRGB0(&quad, config->beam_red, config->beam_green, config->beam_blue);
        setRGB0(&tri, config->beam_red, config->beam_green, config->beam_blue);
        setVector(&rotation, tilt, 0, 0);
        size = time < config->rise_duration ? time * 4096 / config->rise_duration
            : time < config->duration - config->delay ? 4096
            : (config->rise_duration - time + (config->duration - config->delay)) * 4096 / config->rise_duration;
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        if (time < config->rise_duration) {
            size = (direction * 450 - matrix.t[2]) * time / config->rise_duration;
            dy = (-350 - matrix.t[1]) * time / config->rise_duration;
        } else {
            size = direction * 450 - matrix.t[2];
            dy = -350 - matrix.t[1];
        }
        saved.vx = position.vx;
        saved.vy = position.vy;
        saved.vz = position.vz;
        offset = time * (direction * 450 - matrix.t[2]) / config->rise_duration * 32 / (direction * 96) % 32;
        if (direction * size < direction * 96) {
            length = size;
        } else {
            length = direction * 96;
        }
        if (config->duration - config->delay < time) {
            length = size - size * (config->rise_duration + config->duration - config->delay - time) / config->rise_duration;
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        apex = RotTransPers(&origin, (PSXLONG *)&tri.x0, &interpolation, &flag);
        if (size != 0) {
            position.vy += dy * length / size;
        }
        position.vz += length;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 9);
        if (time <= config->duration - config->delay) {
            for (i = 0; i < 8; i++) {
                tri.x1 = screen[i].vx;
                tri.y1 = screen[i].vy;
                tri.x2 = screen[i + 1].vx;
                tri.y2 = screen[i + 1].vy;
                setUV3(&tri, i * 32 + 16, 64 - offset, i * 32, 95 - offset, (i + 1) * 32 - 1, 95 - offset);
                depth = (depths[i] + depths[i + 1]) / 8;
                if (flag >= 0) {
                    flag = (flags[i] | flags[i + 1]) & 0x20;
                }
                if (depth >= 0 && flag == 0 && apex >= 0) {
                    GsSortPoly(&tri, ot, (u16)depth);
                }
            }
        }
        half = 1;
        while (direction * length < direction * size) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, &screen[half * 9], &depths[half * 9], &projection[half * 9],
                &flags[half * 9], 9);
            for (i = 0; i < 8; i++) {
                far = i + (half ^ 1) * 9;
                near = i;
                near += half * 9;
                quad.x0 = screen[near + 1].vx;
                quad.y0 = screen[near + 1].vy;
                quad.x1 = screen[near].vx;
                quad.y1 = screen[near].vy;
                quad.x2 = screen[far + 1].vx;
                quad.y2 = screen[far + 1].vy;
                quad.x3 = screen[far].vx;
                quad.y3 = screen[far].vy;
                depth = AverageZ4(depths[far], depths[far + 1], depths[near], depths[near + 1]);
                flag = (flags[far] | flags[far + 1] | flags[near] | flags[near + 1]) & 0x20;
                setUV4(&quad, i * 32, offset + 32, i * 32 + 31, offset + 32, i * 32, offset + 63,
                    i * 32 + 31, offset + 63);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            half ^= 1;
            if (direction * (length + direction * 96) >= direction * size) {
                position.vz = saved.vz + size;
                position.vy = saved.vy + dy;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->ring, &screen[half * 9], &depths[half * 9], &projection[half * 9],
                    &flags[half * 9], 9);
                for (i = 0; i < 8; i++) {
                    far = i + (half ^ 1) * 9;
                    near = i;
                    near += half * 9;
                    quad.x0 = screen[near + 1].vx;
                    quad.y0 = screen[near + 1].vy;
                    quad.x1 = screen[near].vx;
                    quad.y1 = screen[near].vy;
                    quad.x2 = screen[far + 1].vx;
                    quad.y2 = screen[far + 1].vy;
                    quad.x3 = screen[far].vx;
                    quad.y3 = screen[far].vy;
                    depth = AverageZ4(depths[far], depths[far + 1], depths[near], depths[near + 1]);
                    flag = (flags[far] | flags[far + 1] | flags[near] | flags[near + 1]) & 0x20;
                    setUV4(&quad, i * 32, offset + 32, i * 32 + 31, offset + 32, i * 32,
                        offset + 31 + (size - length) * 32 / (direction * 96), i * 32 + 31,
                        offset + 31 + (size - length) * 32 / (direction * 96));
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            }
            length += direction * 96;
            position.vz = saved.vz + length;
            if (size != 0) {
                position.vy += dy * (direction * 96) / size;
            }
        }
    }
    time = work->elapsed - config->delay - config->rise_duration;
    if (time >= 0 && time < config->duration - config->delay) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        point = work->squares;
        setUV4(&quad, 64, 96, 127, 96, 64, 159, 127, 159);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], -350, direction * 450);
        for (i = 0; i < 8; i++) {
            setVector(&rotation, tilt, 0, i * 512);
            size = time % config->spin_period * 4096 / config->spin_period;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&point[0], &point[1], &point[2], &point[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            red = config->square_red * (4096 - size) / 4096;
            green = config->square_green * (4096 - size) / 4096;
            blue = config->square_blue * (4096 - size) / 4096;
            setRGB0(&quad, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            size = (size + 2048) % 4096;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&point[4], &point[5], &point[6], &point[7],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            red = config->square_red * (4096 - size) / 4096;
            green = config->square_green * (4096 - size) / 4096;
            blue = config->square_blue * (4096 - size) / 4096;
            setRGB0(&quad, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->delay;
    if (time < config->rise_duration) {
        return 0;
    }
    if (config->duration < time) {
        return 2;
    }
    if (time <= config->duration - config->rise_duration) {
        return 4;
    }
    return 1;
}

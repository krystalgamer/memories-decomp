#include "../../types.h"
#include "variant107us_entry.h"
#include "../../game/screen_projection.h"

/* The US header-107 entry: a star burst flashes and fades at the model
 * part, eight textured sprites fly out from the part on looping timers, a
 * beam of quads then grows from the part towards the camera axis, and two
 * spinning squares pulse at the end. */

static const VECTOR D_8013D2D4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model107UsState *work;
    Model107UsConfig *config;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_FT3 tri;
    POLY_G3 star;
    SVECTOR saved, rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    DVECTOR screen[18];
    u16 depths[18], projection[18], flags[18];
    PSXLONG flag, interpolation;
    GsOT *ot;
    SVECTOR *point;
    u8 step;
    s32 i, time, size, tilt, half, length, dy, offset, near, far;
    PSXLONG depth;
    u8 red, green, blue;
    u8 star_red, star_green, star_blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D2D4;
    work = (Model107UsState *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013D338[command];
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
        work->star[0].vx = 0;
        point = work->star;
        point[0].vy = 0;
        point[0].vz = 0;
        point[1].vx = 0;
        point[1].vy = config->star_height;
        point[1].vz = 0;
        point[2].vx = config->star_width;
        point[2].vy = 0;
        point[2].vz = 0;
        point[3].vx = 0;
        point[3].vy = -config->star_height;
        point[3].vz = 0;
        point[4].vx = -config->star_width;
        point[4].vy = 0;
        point[4].vz = 0;
        point[5].vx = 0;
        point[5].vy = config->star_width;
        point[5].vz = 0;
        point[6].vx = config->star_height;
        point[6].vy = 0;
        point[6].vz = 0;
        point[7].vx = 0;
        point[7].vy = -config->star_width;
        point[7].vz = 0;
        point[8].vx = -config->star_height;
        point[8].vy = 0;
        point[8].vz = 0;
        point = &work->star[1];
        for (i = 0; i < 8; point++, i++) {
            point[8].vx = point->vx;
            point[8].vy = point->vy;
            point[8].vz = point->vz;
            point[8].vx = point[8].vx / 2;
            point[8].vy = point[8].vy / 2;
            point[8].vz = point[8].vz / 2;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D2E4[i]);
        }
        work->elapsed = -config->start_delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyFT3(&tri);
    SetPolyG3(&star);
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
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        saved.vx = position.vx;
        saved.vy = position.vy;
        saved.vz = position.vz;
        if (time < config->rise_duration) {
            size = (direction * 450 - matrix.t[2]) * time / config->rise_duration;
            dy = (-350 - matrix.t[1]) * time / config->rise_duration;
        } else {
            size = direction * 450 - matrix.t[2];
            dy = -350 - matrix.t[1];
        }
        offset = time * (direction * 450 - matrix.t[2]) / config->rise_duration * 32 / (direction * 128) % 32;
        if (direction * size < direction * 128) {
            length = size;
        } else {
            length = direction * 128;
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
        depth = RotTransPers(&origin, (PSXLONG *)&tri.x0, &interpolation, &flag);
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
                if (flag >= 0) {
                    flag = (flags[i] | flags[i + 1]) & 0x20;
                }
                if (depth >= 0 && flag == 0 && (depths[i] + depths[i + 1]) / 8 >= 0) {
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
                setUV4(&quad, i * 32, offset + 32, i * 32 + 31, offset + 32, i * 32, offset + 63,
                    i * 32 + 31, offset + 63);
                flag = (flags[far] | flags[far + 1] | flags[near] | flags[near + 1]) & 0x20;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            half ^= 1;
            if (direction * (length + direction * 128) >= direction * size) {
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
                    setUV4(&quad, i * 32, offset + 32, i * 32 + 31, offset + 32, i * 32,
                        offset + 31 + (size - length) * 32 / (direction * 128), i * 32 + 31,
                        offset + 31 + (size - length) * 32 / (direction * 128));
                    flag = (flags[far] | flags[far + 1] | flags[near] | flags[near + 1]) & 0x20;
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            }
            length += direction * 128;
            position.vz = saved.vz + length;
            if (size != 0) {
                position.vy += dy * (direction * 128) / size;
            }
        }
    }
    time = work->elapsed - config->delay - config->rise_duration;
    if (time >= 0 && time < config->duration - config->delay) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        point = work->squares;
        setUV4(&quad, 0, 96, 63, 96, 0, 159, 63, 159);
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
    time = work->elapsed;
    if (time >= 0 && time < config->star_duration) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        size = time * 4096 / config->star_duration;
        setVector(&rotation, 0, 0, size / 3);
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->star, screen, depths, projection, flags, 17);
        star_red = config->star_red * (config->star_duration - time) / config->star_duration;
        star_green = config->star_green * (config->star_duration - time) / config->star_duration;
        star_blue = config->star_blue * (config->star_duration - time) / config->star_duration;
        size = (config->star_duration - time) * 255 / config->star_duration;
        setRGB1(&star, 0, 0, 0);
        setRGB2(&star, 0, 0, 0);
        for (i = 1; i < 5; i++) {
            setRGB0(&star, star_red, star_green, star_blue);
            star.x0 = screen[0].vx;
            star.y0 = screen[0].vy;
            star.x1 = screen[i].vx;
            star.y1 = screen[i].vy;
            star.x2 = screen[i % 4 + 1].vx;
            star.y2 = screen[i % 4 + 1].vy;
            depth = AverageZ3(depths[0], depths[i], depths[i % 4 + 2]);
            flag = (flags[0] | flags[i] | flags[i % 4 + 1]) & 0x20;
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&star, ot, (u16)depth, 1);
            }
            star.x0 = screen[0].vx;
            star.y0 = screen[0].vy;
            star.x1 = screen[i + 4].vx;
            star.y1 = screen[i + 4].vy;
            star.x2 = screen[i % 4 + 5].vx;
            star.y2 = screen[i % 4 + 5].vy;
            depth = AverageZ3(depths[0], depths[i + 4], depths[i % 4 + 5]);
            flag = (flags[0] | flags[i + 4] | flags[i % 4 + 5]) & 0x20;
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&star, ot, (u16)depth, 1);
            }
            setRGB0(&star, size, size, size);
            star.x0 = screen[0].vx;
            star.y0 = screen[0].vy;
            star.x1 = screen[i + 8].vx;
            star.y1 = screen[i + 8].vy;
            star.x2 = screen[i % 4 + 9].vx;
            star.y2 = screen[i % 4 + 9].vy;
            depth = AverageZ3(depths[0], depths[i + 8], depths[i % 4 + 9]);
            flag = (flags[0] | flags[i + 8] | flags[i % 4 + 9]) & 0x20;
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&star, ot, (u16)depth, 1);
            }
            star.x0 = screen[0].vx;
            star.y0 = screen[0].vy;
            star.x1 = screen[i + 12].vx;
            star.y1 = screen[i + 12].vy;
            star.x2 = screen[i % 4 + 13].vx;
            star.y2 = screen[i % 4 + 13].vy;
            depth = AverageZ3(depths[0], depths[i + 12], depths[i % 4 + 13]);
            flag = (flags[0] | flags[i + 12] | flags[i % 4 + 13]) & 0x20;
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&star, ot, (u16)depth, 1);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->delay;
    if (time < 0) {
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

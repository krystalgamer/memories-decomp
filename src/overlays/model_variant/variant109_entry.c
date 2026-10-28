#include "../../types.h"
#include "variant109_entry.h"

/* Header 109's entry: a 13x13 Gouraud grid waves under the slot while the
 * model's ambient light dims, then sprites fly out from a model part, a beam
 * of quads grows towards the camera axis, two squares pulse, and a fading
 * double-sided strip of flashes follows the beam. */

static const VECTOR D_8013E520 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model109State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad, flash;
    POLY_FT3 tri;
    POLY_G3 unused_gouraud;
    POLY_GT4 strip;
    SVECTOR saved, rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    /* Only three entries are declared; the fourth overruns into screen[0]
     * and screen[1], as retail's frame shows. */
    SVECTOR edges[3];
    DVECTOR screen[169];
    u16 depths[169], projection[169], flags[169];
    PSXLONG flag, interpolation;
    Model109Config *config;
    GsOT *ot;
    SVECTOR *point;
    Model109Colour *colour;
    s32 tilt;
    u8 step;
    s32 i, half, size, length, dy, offset, time, fade, center;
    PSXLONG depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013E520;
    work = (Model109State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013E5BC[command];
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
        point = work->grid;
        colour = work->colours;
        size = 0;
        for (i = 0; i < 13; i++) {
            for (half = 0; half < 13; half++) {
                point->vx = config->grid_size * half / 12 - config->grid_size / 2;
                point->vy = config->amplitude * csin(size) / 4096;
                point->vz = config->grid_size * i / 12 - config->grid_size / 2;
                colour->red = config->low[0] + rand() % (config->high[0] - config->low[0] + 1);
                /* Green and blue draw from crossed ends of the ranges. */
                colour->green = config->low[2] + rand() % (config->high[1] - config->low[2] + 1);
                colour->blue = config->low[1] + rand() % (config->high[2] - config->low[1] + 1);
                point++;
                colour++;
                size = (size + 372) % 4096;
            }
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E530[i]);
        }
        work->elapsed = -config->lead;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    SetPolyFT4(&quad);
    SetPolyFT4(&flash);
    SetPolyFT3(&tri);
    SetPolyG3(&unused_gouraud);
    SetPolyGT4(&strip);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    SetSemiTrans(&strip, 1);
    strip.tpage = work->texture[4] >> 16;
    strip.clut = work->texture[4];
    setUV4(&strip, 64, 160, 127, 160, 64, 223, 127, 223);
    setVector(&position, 0, config->base, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    RotTransPersN(work->grid, screen, depths, projection, flags, 169);
    time = work->elapsed + config->lead;
    colour = work->colours;
    for (i = 0; i < 12; i++) {
        for (half = 0; half < 12; half++) {
            size = i * 13 + half;
            length = (i + 1) * 13 + half;
            setXY4(&strip, screen[size].vx, screen[size].vy, screen[size + 1].vx, screen[size + 1].vy,
                screen[length].vx, screen[length].vy, screen[length + 1].vx, screen[length + 1].vy);
            /* Each corner's channels are computed before any is stored. */
            if (work->elapsed < 0) {
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r0 = red;
                strip.g0 = green;
                strip.b0 = blue;
                colour++;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r1 = red;
                strip.g1 = green;
                strip.b1 = blue;
                colour += 12;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r2 = red;
                strip.g2 = green;
                strip.b2 = blue;
                colour++;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r3 = red;
                strip.g3 = green;
                strip.b3 = blue;
                colour -= 13;
            } else if ((fade = config->fade - (work->elapsed - config->delay - config->duration)) < config->fade) {
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r0 = red;
                strip.g0 = green;
                strip.b0 = blue;
                colour++;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r1 = red;
                strip.g1 = green;
                strip.b1 = blue;
                colour += 12;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r2 = red;
                strip.g2 = green;
                strip.b2 = blue;
                colour++;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r3 = red;
                strip.g3 = green;
                strip.b3 = blue;
                colour -= 13;
            } else {
                strip.r0 = colour->red;
                strip.g0 = colour->green;
                strip.b0 = colour->blue;
                colour++;
                strip.r1 = colour->red;
                strip.g1 = colour->green;
                strip.b1 = colour->blue;
                colour += 12;
                strip.r2 = colour->red;
                strip.g2 = colour->green;
                strip.b2 = colour->blue;
                colour++;
                strip.r3 = colour->red;
                strip.g3 = colour->green;
                strip.b3 = colour->blue;
                colour -= 13;
            }
            depth = AverageZ4(depths[size], depths[size + 1], depths[length], depths[length + 1]);
            flag = (flags[size] | flags[size + 1] | flags[length] | flags[length + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&strip, ot, (u16)depth, 1);
            }
        }
        colour++;
    }
    if (Model_GetActiveSlotIndex() != 0) {
        point = &work->grid[155];
        vertices[0].vx = point->vx;
        vertices[0].vy = point->vy;
        vertices[0].vz = point->vz;
        for (i = 0; i < 155; i++) {
            point->vy = (point - 1)->vy;
            point--;
        }
    } else {
        point = work->grid;
        vertices[0].vx = point->vx;
        vertices[0].vy = point->vy;
        vertices[0].vz = point->vz;
        for (i = 0; i < 155; i++) {
            point->vy = (point + 1)->vy;
            point++;
        }
    }
    point->vy = vertices[0].vy;
    PopMatrix();
    if (work->elapsed < 0) {
        time = config->lead + work->elapsed;
        size = 2048 - time * 6144 / config->lead;
        Model_SetSlotAmbient(2, size, size, size);
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    time = work->elapsed - config->delay - config->duration;
    if (time >= 0) {
        size = time * 6144 / config->fade - 4096;
        Model_SetSlotAmbient(2, size, size, size);
    } else {
        Model_SetSlotAmbient(2, -4096, -4096, -4096);
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
                if (depth >= 0 && flag >= 0) {
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
                fade = i + (half ^ 1) * 9;
                center = i;
                center += half * 9;
                quad.x0 = screen[center + 1].vx;
                quad.y0 = screen[center + 1].vy;
                quad.x1 = screen[center].vx;
                quad.y1 = screen[center].vy;
                quad.x2 = screen[fade + 1].vx;
                quad.y2 = screen[fade + 1].vy;
                quad.x3 = screen[fade].vx;
                quad.y3 = screen[fade].vy;
                depth = AverageZ4(depths[fade], depths[fade + 1], depths[center], depths[center + 1]);
                flag = (flags[fade] | flags[fade + 1] | flags[center] | flags[center + 1]) & 0x20;
                setUV4(&quad, i * 32, offset + 32, i * 32 + 31, offset + 32, i * 32, offset + 63,
                    i * 32 + 31, offset + 63);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            half ^= 1;
            if (length + direction * 96 >= direction * size) {
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
                    fade = i + (half ^ 1) * 9;
                    center = i;
                    center += half * 9;
                    quad.x0 = screen[center + 1].vx;
                    quad.y0 = screen[center + 1].vy;
                    quad.x1 = screen[center].vx;
                    quad.y1 = screen[center].vy;
                    quad.x2 = screen[fade + 1].vx;
                    quad.y2 = screen[fade + 1].vy;
                    quad.x3 = screen[fade].vx;
                    quad.y3 = screen[fade].vy;
                    depth = AverageZ4(depths[fade], depths[fade + 1], depths[center], depths[center + 1]);
                    flag = (flags[fade] | flags[fade + 1] | flags[center] | flags[center + 1]) & 0x20;
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
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->rise_duration + config->flash_time) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        setRGB0(&quad, config->flash_red, config->flash_green, config->flash_blue);
        SetSemiTrans(&flash, 1);
        flash.tpage = work->texture[0] >> 16;
        flash.clut = work->texture[0];
        setUV4(&quad, 0, 96, 63, 96, 0, 223, 63, 223);
        setVector(&vertices[0], -config->flash_width / 4, -config->flash_width, 0);
        setVector(&vertices[1], config->flash_width / 4, -config->flash_width, 0);
        setVector(&vertices[2], -config->flash_width / 4, 0, 0);
        setVector(&vertices[3], config->flash_width / 4, 0, 0);
        setVector(&edges[0], -config->flash_width / 2, -config->flash_width / 2, 0);
        setVector(&edges[1], config->flash_width / 2, -config->flash_width / 2, 0);
        setVector(&edges[2], -config->flash_width / 2, config->flash_width / 2, 0);
        setVector(&edges[3], config->flash_width / 2, config->flash_width / 2, 0);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        position.vy = config->base + config->amplitude;
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
        length = direction * 96;
        if (direction * size < length) {
            length = size;
        }
        position.vz += length;
        while (direction * length < direction * size) {
            fade = (config->flash_time - time + config->rise_duration * length / size) * 4096 / config->flash_time;
            fade = fade < 0 ? 0 : fade > 4096 ? 4096 : fade;
            setVector(&scale, fade, fade, fade);
            setRGB0(&flash, config->sprite_red, config->sprite_green, config->sprite_blue);
            fade = (time + config->rise_duration * length / size) / 8 * 32;
            setUV4(&flash, fade, 0, fade + 31, 0, fade, 31, fade + 31, 31);
            center = position.vx;
            position.vx = center + (config->beam_radius + config->flash_width / 4);
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
            depth = RotAverage4(&edges[0], &edges[1], &edges[2], &edges[3],
                (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                (PSXLONG *)&flash.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&flash, ot, (u16)depth);
            }
            position.vx = center - (config->beam_radius + config->flash_width / 4);
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
            depth = RotAverage4(&edges[0], &edges[1], &edges[2], &edges[3],
                (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                (PSXLONG *)&flash.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&flash, ot, (u16)depth);
            }
            position.vx = center;
            if (length + direction * 96 >= direction * size) {
                position.vx = center + (config->beam_radius + config->flash_width / 4);
                position.vz = saved.vz + size;
                position.vy = saved.vy + dy;
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
                depth = RotAverage4(&edges[0], &edges[1], &edges[2], &edges[3],
                    (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                    (PSXLONG *)&flash.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&flash, ot, (u16)depth);
                }
                position.vx = center - (config->beam_radius + config->flash_width / 4);
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
                depth = RotAverage4(&edges[0], &edges[1], &edges[2], &edges[3],
                    (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1, (PSXLONG *)&flash.x2,
                    (PSXLONG *)&flash.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&flash, ot, (u16)depth);
                }
            }
            length += direction * 96;
            position.vz = saved.vz + length;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->delay;
    if (time < config->rise_duration) {
        return 0;
    }
    if (config->duration < time && time < config->duration + config->fade) {
        return 0;
    }
    if (config->duration + config->fade < time) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (time <= config->duration - config->rise_duration) {
        return 4;
    }
    return 1;
}

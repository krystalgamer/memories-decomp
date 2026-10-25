#include "../../types.h"
#include "variant72_entry.h"
#include "../../game/screen_projection.h"

/* Header 72's entry: a ribbon trails behind the slot's parts, one row recorded
 * a frame and shaded from the start to the end colour; two fans of triangles
 * open and fade in front of the slot, a sprite plays eight frames of the
 * texture, and, when the command asks for them, 48 sparks fly out. During the
 * lead time the entry only records the parts' positions. */

static const VECTOR D_8013CD68 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_G4 ribbon;
    POLY_FT4 quad;
    POLY_G3 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    u16 depths[192], projection[192], flags[192];
    DVECTOR screen[192];
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model72State *work;
    s32 direction;
    GsOT *ot;
    u8 step;
    s32 time;
    Model72Config *config;
    SVECTOR *point;
    s32 i, j, half, t, elevation, swing, right, grey, below_right, pick, fan_blue;
    s32 value, size, u, v, sprite_blue, spark_blue;
    Model72Colour *colour;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CD68;
    work = (Model72State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CD94[command % 100];
        point = work->points;
        for (i = 0; i < config->columns * config->rows; point++, i++) {
            setVector(point, 0, 0, 0);
        }
        colour = work->colours;
        for (i = 0; i < config->rows; colour++, i++) {
            colour->red = config->start_red + (config->end_red - config->start_red) * i / (config->rows - 1);
            colour->green = config->start_green + (config->end_green - config->start_green) * i / (config->rows - 1);
            colour->blue = config->start_blue + (config->end_blue - config->start_blue) * i / (config->rows - 1);
        }
        work->row = 0;
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
        point = work->sparks;
        for (i = 0; i < 48; point++, i++) {
            swing = csin(rand() % 2048);
            pick = rand();
            point->vx = (config->spark_spread / 4 + config->spark_spread * swing / 4096 * 3 / 4) * (pick % 2 * 2 - 1);
            point->vy = -config->spark_lift + config->spark_size * csin(rand() % 4096) / 8192;
            point->vz = rand() % config->spark_spread * 2 - config->spark_spread;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CD78[i]);
        }
        work->elapsed = -config->lead * 2;
        work->completed = 0;
        work->sparks_on = command / 100;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        PushMatrix();
        point = work->points;
        for (i = 0; i < config->rows; i++) {
            for (j = 0; j < config->columns; j++) {
                GsSetLsMatrix(&base);
                GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[j]),
                    &matrix);
                setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
                point++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        return 0;
    }
    SetPolyG4(&ribbon);
    SetPolyFT4(&quad);
    SetPolyG3(&fan);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    point = &work->points[work->row * config->columns];
    for (i = 0; i < config->columns; i++) {
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
        setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        point++;
    }
    GsSetLsMatrix(&base);
    elevation = work->row * config->columns;
    value = (config->rows - work->row) * config->columns;
    RotTransPersN(&work->points[elevation], screen, depths, projection, flags, value);
    RotTransPersN(work->points, &screen[value], &depths[value], &projection[value], &flags[value], elevation);
    for (i = 0; i < config->rows - 1; i++) {
        for (j = 0; j < config->columns - 1; j++) {
            value = i * config->columns + j;
            right = value + 1;
            elevation = (i + 1) * config->columns + j;
            below_right = elevation + 1;
            setXY4(&ribbon, screen[value].vx, screen[value].vy, screen[right].vx, screen[right].vy, screen[elevation].vx,
                screen[elevation].vy, screen[below_right].vx, screen[below_right].vy);
            colour = &work->colours[i];
            ribbon.r0 = colour->red - j * colour->red / config->columns;
            ribbon.g0 = colour->green - j * colour->green / config->columns;
            ribbon.b0 = colour->blue - j * colour->blue / config->columns;
            ribbon.r1 = colour->red - (j + 1) * colour->red / config->columns;
            ribbon.g1 = colour->green - (j + 1) * colour->green / config->columns;
            ribbon.b1 = colour->blue - (j + 1) * colour->blue / config->columns;
            colour++;
            ribbon.r2 = colour->red - j * colour->red / config->columns;
            ribbon.g2 = colour->green - j * colour->green / config->columns;
            ribbon.b2 = colour->blue - j * colour->blue / config->columns;
            ribbon.r3 = colour->red - (j + 1) * colour->red / config->columns;
            ribbon.g3 = colour->green - (j + 1) * colour->green / config->columns;
            ribbon.b3 = colour->blue - (j + 1) * colour->blue / config->columns;
            depth = AverageZ4(depths[value], depths[right], depths[elevation], depths[below_right]);
            flag = (flags[value] | flags[right] | flags[elevation] | flags[below_right]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ribbon, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->start * 2;
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
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 17);
        value = ((config->fan_duration - time) << 12) / config->fan_duration;
        size = value <= 4096;
        value = value >= 0 ? (size ? value : 4096) : 0;
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
            flag = (flags[0] | flags[i + 1] | flags[i + 18]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
        grey = (config->fan_duration - time) * 160 / config->fan_duration;
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        setRGB0(&fan, grey, grey, grey);
        value = time * 3074 / config->fan_duration;
        size = 4096;
        size = value <= size;
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
    time = work->elapsed - config->start * 2;
    if (time >= 0 && time < config->sprite_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        pick = time * 8 / config->sprite_duration;
        value = pick % 4 * 32;
        elevation = pick / 4 * 32;
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
    time = work->elapsed - config->start * 2;
    if (time >= 0 && time < config->spark_duration && work->sparks_on != 0) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
        SetSemiTrans(&quad, 1);
        v = config->spark_red * (config->spark_duration - time) / config->spark_duration;
        u = config->spark_green * (config->spark_duration - time) / config->spark_duration;
        spark_blue = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
        setRGB0(&quad, v, u, spark_blue);
        setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
        setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
        setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
        setVector(&vertices[3], config->spark_size, config->spark_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        point = work->sparks;
        for (i = 0; i < 48; point++, i++) {
            position.vx = point->vx * time / config->spark_duration;
            position.vy = point->vy * time / config->spark_duration;
            position.vz = point->vz + direction * 450;
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
    work->row = (work->row + config->rows - 1) % config->rows;
    PopMatrix();
    work->elapsed += step;
    half = work->elapsed / 2;
    if (half < config->start) {
        return 0;
    }
    if (config->end < half) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}

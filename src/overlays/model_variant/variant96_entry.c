#include "../../types.h"
#include "variant96_entry.h"

/* Header 96's entry: a flare glows on the attacking slot's joint, then
 * swings out in front of the slot dragging a six-image trail, a fan of
 * triangles opens and closes there and a burst sprite plays eight frames
 * of the second texture while it shrinks. */

static const VECTOR D_8013C9CC = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model96State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G3 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG flag, interpolation;
    Model96Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 i, t, time, step, value, u, v;
    u8 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C9CC;
    work = (Model96State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CA14[command];
        point = work->trail;
        for (i = 0; i < 6; point++, i++) {
            setVector(point, 0, 0, 0);
        }
        point = work->rings.points;
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
        work->head = 0;
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C9DC[i]);
        }
        work->elapsed = -config->delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        PushMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
        point = work->trail;
        for (i = 0; i < 6; point++, i++) {
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
        }
        PopMatrix();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG3(&fan);
    time = work->elapsed;
    if (time >= 0 && time < config->flare_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[1];
        setVector(&position, work->trail[0].vx, work->trail[0].vy, work->trail[0].vz);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setVector(&vertices[0], 0, -config->flare_size, -config->flare_size);
        setVector(&vertices[1], 0, -config->flare_size, config->flare_size);
        setVector(&vertices[2], 0, config->flare_size, -config->flare_size);
        setVector(&vertices[3], 0, config->flare_size, config->flare_size);
        depth = RotTransPers4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        red = config->flare_red * time / config->flare_duration;
        green = config->flare_green * time / config->flare_duration;
        blue = config->flare_blue * time / config->flare_duration;
        setUV4(&quad, 0, 0, 127, 0, 0, 87, 127, 87);
        setRGB0(&quad, red, green, blue);
        SetSemiTrans(&quad, 1);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    time = work->elapsed - config->flight_offset;
    if (time >= 0 && time < config->flight_duration + config->fade_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setVector(&vertices[0], 0, -config->flare_size, -config->flare_size);
        setVector(&vertices[1], 0, -config->flare_size, config->flare_size);
        setVector(&vertices[2], 0, config->flare_size, -config->flare_size);
        setVector(&vertices[3], 0, config->flare_size, config->flare_size);
        setUV4(&quad, 0, 0, 127, 0, 0, 87, 127, 87);
        point = &work->trail[work->head];
        if (time <= config->flight_duration) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
            point->vx = config->swing * csin((time << 12) / config->flight_duration) / 4096;
            point->vy = matrix.t[1] + (-350 - matrix.t[1]) * time / config->flight_duration;
            point->vz = matrix.t[2] + (direction * 450 - matrix.t[2]) * time / config->flight_duration;
        } else {
            point->vx = config->swing * csin((time << 12) / config->flight_duration) / 4096;
            point->vy = -350;
            point->vz = direction * 450;
        }
        for (i = 0; i < 6; i++) {
            position.vx = (work->trail + (work->head + i) % 6)->vx;
            position.vy = (work->trail + (work->head + i) % 6)->vy;
            position.vz = (work->trail + (work->head + i) % 6)->vz;
            setVector(&rotation, 0, 0, 0);
            if (time - i * 2 < config->flight_duration / 2) {
                rotation.vy = 1024 - ((time - i * 2) << 12) / config->flight_duration;
            } else if (time - i * 2 < config->flight_duration) {
                rotation.vy = ((time - i * 2 - config->flight_duration / 2) << 12) / config->flight_duration - 1024;
            } else {
                rotation.vy = 1024;
            }
            value = 4096 - (i << 11) / 6;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotTransPers4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            red = config->flare_red - (config->flare_red - config->tail_red) * i / 6;
            green = config->flare_green - (config->flare_green - config->tail_green) * i / 6;
            blue = config->flare_blue - (config->flare_blue - config->tail_blue) * i / 6;
            if (config->flight_duration < time) {
                red = red * (config->fade_duration - time + config->flight_duration) / config->fade_duration;
                /* Retail fades green from the blue channel. */
                green = blue * (config->fade_duration - time + config->flight_duration) / config->fade_duration;
                blue = blue * (config->fade_duration - time + config->flight_duration) / config->fade_duration;
            }
            setRGB0(&quad, red, green, blue);
            if (i != 0 || config->flight_duration < time) {
                SetSemiTrans(&quad, 1);
            } else {
                SetSemiTrans(&quad, 0);
            }
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        work->head = (work->head + 5) % 6;
    }
    time = work->elapsed - config->flight_offset - config->flight_duration;
    if (time >= 0 && time < config->fan_duration) {
        SetSemiTrans(&fan, 1);
        red = config->fan_red * (config->fan_duration - time) / config->fan_duration;
        green = config->fan_green * (config->fan_duration - time) / config->fan_duration;
        blue = config->fan_blue * (config->fan_duration - time) / config->fan_duration;
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        setRGB0(&fan, red, green, blue);
        value = (time << 12) / config->fan_duration;
        value = value < 0 ? 0 : (value > 4096 ? 4096 : value);
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->rings.fan.ring, screen, depths, projection, flags, 17);
        value = ((config->fan_duration - time) << 12) / config->fan_duration;
        setVector(&scale, value, value, value);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        v = 17;
        RotTransPersN(work->rings.fan.outer, &screen[17], &depths[17], &projection[17], &flags[17], v);
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
            depth = AverageZ3(depths[0], depths[i + 1], depths[t + 17]);
            /* Retail tests the first triangle's corner flags again. */
            flag = (flags[0] | flags[i + 1] | flags[i + 17]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
        red = (config->fan_duration - time) * 160 / config->fan_duration;
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        setRGB0(&fan, red, red, red);
        value = time * 3074 / config->fan_duration;
        value = value < 0 ? 0 : (value > 4096 ? 4096 : value);
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->rings.fan.ring, screen, depths, projection, flags, 17);
        value = (config->fan_duration - time) * 3074 / config->fan_duration;
        setVector(&scale, value, value, value);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        v = 17;
        RotTransPersN(work->rings.fan.outer, &screen[17], &depths[17], &projection[17], &flags[17], v);
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
            depth = AverageZ3(depths[0], depths[i + 1], depths[t + 17]);
            flag = (flags[0] | flags[i + 1] | flags[i + 17]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->flight_offset - config->flight_duration;
    if (time >= 0 && time < config->burst_duration) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        u = time * 8 / config->burst_duration;
        value = u % 4 * 32;
        v = u / 4 * 32 + 88;
        setUV4(&quad, value, v, value + 31, v, value, v + 31, value + 31, v + 31);
        SetSemiTrans(&quad, 1);
        red = config->burst_red * (config->burst_duration - time) / config->burst_duration;
        green = config->burst_green * (config->burst_duration - time) / config->burst_duration;
        blue = config->burst_blue * (config->burst_duration - time) / config->burst_duration;
        setRGB0(&quad, red, green, blue);
        setVector(&vertices[0], -config->burst_size, -config->burst_size, 0);
        setVector(&vertices[1], config->burst_size, -config->burst_size, 0);
        setVector(&vertices[2], -config->burst_size, config->burst_size, 0);
        setVector(&vertices[3], config->burst_size, config->burst_size, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = ((config->burst_duration - time) << 12) / config->burst_duration;
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
    PopMatrix();
    work->elapsed += step;
    if (work->elapsed < config->flight_offset + config->flight_duration) {
        return 0;
    }
    if (config->fan_duration < time) {
        return 2;
    }
    return 1;
}

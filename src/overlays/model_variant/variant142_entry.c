#include "../../types.h"
#include "variant142_entry.h"
#include "../../game/screen_projection.h"

/* Header 142's entry: up to 64 textured quads leave the command's slot part
 * and fly to points spread around the other slot, each bursting into four
 * smaller quads on arrival, while two spinning triangle fans fade on the
 * slot's fan parts and the third slot's ambient light dims and recovers.
 * The return value reports the hit (1), the end (2) or a still-moving entry. */

static const VECTOR D_8013C390 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model142State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G3 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    GsOT *ot;
    s32 step;
    s32 side;
    Model142Config *config;
    SVECTOR *point, *target;
    s32 i, j, time, value;
    PSXLONG depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C390;
    work = (Model142State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C4AC[command % 100];
        point = work->points;
        for (i = 0; i < 64; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point->pad = 0;
            point++;
        }
        point = work->targets;
        for (i = 0; i < 64; i++) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vx += side * config->width * csin(i << 6) / 4096;
            point->vy += config->width / 2 * ccos(i << 6) / 4096;
            point->vz += side * (rand() % config->width);
            point->pad = 0;
            point++;
        }
        point = work->offsets;
        for (i = 0; i < 4; i++) {
            point->vx = rand() % config->burst_spread * 2 - config->burst_spread;
            point->vy = -config->burst_lift;
            point->vz = rand() % config->burst_spread * 2 - config->burst_spread;
            point++;
        }
        point = work->inner;
        for (i = 0; i < 4; i++) {
            point->vx = config->inner_radius * csin(i << 10) / 4096;
            point->vy = config->inner_radius * ccos(i << 10) / 4096;
            point->vz = 0;
            point[4].vx = config->outer_radius * ccos(i << 10) / 4096;
            point[4].vy = config->outer_radius * csin(i << 10) / 4096;
            point[4].vz = 0;
            point++;
        }
        point += 4;
        point->vx = config->outer_radius * ccos(0) / 4096;
        point->vy = config->outer_radius * csin(0) / 4096;
        point->vz = 0;
        point++;
        point->vx = config->outer_radius * ccos(1024) / 4096;
        point->vy = config->outer_radius * csin(1024) / 4096;
        point->vz = 0;
        point++;
        point->vx = 0;
        point->vy = 0;
        point->vz = 0;
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &D_8013C3B0[0]);
        for (i = 1; i < 9; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C3B0[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model142State), sizeof(Model142State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyG3(&fan);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    point = work->points;
    target = work->targets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->move_delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
        } else if (time < config->duration + config->fade) {
            value = time * 3 * 1024 / config->duration + 1024;
            setVector(&scale, value, value, value);
            if (time < config->duration) {
                setRGB0(&quad, config->red, config->green, config->blue);
            } else {
                value = config->duration + config->fade - time;
                red = config->red * value / config->fade;
                green = config->green * value / config->fade;
                blue = config->blue * value / config->fade;
                setRGB0(&quad, red, green, blue);
            }
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            value = (time / 2 + i) % 8;
            setUV4(&quad, value % 4 * 64, value / 4 * 87, value % 4 * 64 + 63, value / 4 * 87,
                   value % 4 * 64, value / 4 * 87 + 86, value % 4 * 64 + 63, value / 4 * 87 + 86);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (time < config->duration) {
                point->vx += (target->vx - point->vx) / (config->duration - time) * step;
                point->vy += (target->vy - point->vy) / (config->duration - time) * step;
                point->vz += (target->vz - point->vz) / (config->duration - time) * step;
            }
        }
        point++;
        target++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->burst_red, config->burst_green, config->burst_blue);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    point = work->targets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->move_delay - i * config->spacing - config->duration;
        if (time >= 0 && time < config->burst_duration) {
            target = work->offsets;
            for (j = 0; j < 4; j++) {
                setVector(&position, target->vx, target->vy, target->vz);
                position.vx *= time;
                position.vy *= time;
                position.vz *= time;
                position.vx /= config->burst_duration;
                position.vy /= config->burst_duration;
                position.vz /= config->burst_duration;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                value = time * 8 / config->burst_duration;
                setUV4(&quad, value * 64, 174, value * 64 + 31, 174, value * 64, 205, value * 64 + 31, 205);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                target++;
            }
        }
        point++;
    }
    time = work->elapsed - config->fan_delay;
    if (time >= 0 && time < config->fan_duration) {
        red = config->fan_red * (config->fan_duration - time) / config->fan_duration;
        green = config->fan_green * (config->fan_duration - time) / config->fan_duration;
        blue = config->fan_blue * (config->fan_duration - time) / config->fan_duration;
        setRGB0(&fan, red, green, blue);
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, red / 2, green / 2, blue / 2);
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->fan_parts[i]), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            setVector(&rotation, 0, 0, (time << 10) / config->fan_duration);
            value = (i % 2 * 2 - 1) * (time << 12) / config->fan_duration;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            target = &work->center;
            point = work->inner;
            for (j = 0; j < 4; j++) {
                depth = RotAverage3(target, point, point + 4,
                    (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
                }
                depth = RotAverage3(target, point, point + 6,
                    (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
                }
                point++;
            }
        }
    }
    time = work->elapsed;
    if (time < config->ambient_duration) {
        value = 2048 - (time << 12) / config->ambient_duration;
        Model_SetSlotAmbient(2, value, value, value);
    } else {
        time -= config->ambient_delay;
        if (time >= 0 && time < config->ambient_duration) {
            value = (time << 12) / config->ambient_duration - 2048;
            Model_SetSlotAmbient(2, value, value, value);
        } else if (time >= config->ambient_duration) {
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
        } else {
            Model_SetSlotAmbient(2, -2048, -2048, -2048);
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->move_delay - config->duration;
    if (time < 0) {
        return 0;
    }
    if (work->elapsed >= config->ambient_delay + config->ambient_duration) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (time < config->spacing * config->count) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 0;
}

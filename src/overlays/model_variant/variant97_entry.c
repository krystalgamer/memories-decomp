#include "../../types.h"
#include "variant97_entry.h"
#include "../../game/graphics_constants.h"

/* Header 97's entry: a full-screen flash fades in, a flickering glow fan
 * pulses around a model part, eight shards fade in one after another around
 * that part and then fly in front of the slot while spinning, and a textured
 * fan grows there last. The return value reports the hit (1) once, the end
 * of the fan (2) or neither (0). */

static const VECTOR gModelVariant97_D12E4 = {4096, 4096, 4096, 0};

s32 ModelVariant97_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT3 fan;
    POLY_G3 glow;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model97State *work;
    Model97Config *config;
    s32 side;
    GsOT *ot;
    s32 i, time, size;
    PSXLONG depth;
    u8 red, green, blue;
    SVECTOR *point;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant97_D12E4;
    work = (Model97State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &gModelVariant97_D1364[command];
        point = work->shards;
        for (i = 0; i < 8; point++, i++) {
            point->vx = config->shard_radius * ccos(i << 9) / 4096;
            point->vy = config->shard_radius * csin(i << 9) / 4096;
            point->vz = 0;
        }
        point = &work->glow_centre;
        setVector(point, 0, 0, 0);
        point = work->glow_ring;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->glow_radius * ccos(i << 8) / 4096;
            point->vy = config->glow_radius * csin(i << 8) / 4096;
            point->vz = 0;
        }
        point = work->fan_ring;
        for (i = 0; i < 16; point++, i++) {
            point->vx = config->fan_radius * ccos(i << 8) / 3064;
            point->vy = config->fan_radius * csin(i << 8) / 3064;
            point->vz = 0;
        }
        point = &work->fan_centre;
        setVector(point, 0, 0, 0);
        point = work->fan_outer;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->fan_radius * ccos(i << 8) / 4096;
            point->vy = config->fan_radius * csin(i << 8) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant97_D12F4[i]);
        }
        work->elapsed = -config->delay;
        work->hit = 0;
        work->frame = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyGT3(&fan);
    SetPolyG3(&glow);
    SetPolyF4(&flash);
    time = work->elapsed - config->gap * 8 - config->fade_in - config->fly_time;
    if (time >= 0 && time < config->flash_time) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->fan_time - time) * 255 / config->fan_time;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed;
    if (time >= 0 && time < config->glow_time) {
        setRGB1(&glow, 0, 0, 0);
        setRGB2(&glow, 0, 0, 0);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&rotation, 0, 0, 0);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        size = 2048;
        if (time < config->glow_ramp) {
            size = (time << 11) / config->glow_ramp;
        } else if (time >= config->glow_time - config->glow_ramp) {
            size = ((config->glow_time - time) << 11) / config->glow_ramp;
        }
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        point = work->glow_ring;
        for (i = 0; i < 16; point++, i++) {
            depth = RotAverage3(&work->glow_centre, &point[0], &point[1], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, &interpolation, &flag);
            red = 128 / (work->frame + 1);
            setRGB0(&glow, red, red, red);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
        }
        size *= 2;
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        point = work->glow_ring;
        for (i = 0; i < 16; point++, i++) {
            depth = RotAverage3(&work->glow_centre, &point[0], &point[1], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, &interpolation, &flag);
            red = config->glow_red / (work->frame + 1);
            green = config->glow_green / (work->frame + 1);
            blue = config->glow_blue / (work->frame + 1);
            setRGB0(&glow, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
        }
    }
    work->frame ^= 1;
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 0);
    point = work->shards;
    for (i = 0; i < 8; point++, i++) {
        time = work->elapsed - i * config->gap;
        if (time >= 0 && work->elapsed < config->gap * 8 + config->fade_in) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            work->anchor.vx = position.vx;
            work->anchor.vy = position.vy;
            work->anchor.vz = position.vz;
            position.vx += point->vx;
            position.vy += point->vy;
            position.vz += point->vz;
            size = (work->frame << 8) + 4096;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3 + 1] >> 16;
            quad.clut = work->texture[i % 3 + 1];
            setUV4(&quad, i % 3 * 64, 0, i % 3 * 64 + 63, 0, i % 3 * 64, 63, i % 3 * 64 + 63, 63);
           if (time < config->fade_in) {
                red = config->red * time / config->fade_in;
                green = config->green * time / config->fade_in;
                blue = config->blue * time / config->fade_in;
                setSemiTrans(&quad, 1);
            } else {
                red = config->red;
                green = config->green;
                blue = config->blue;
                setSemiTrans(&quad, 0);
            }
            setRGB0(&quad, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - (config->gap * 8 + config->fade_in);
    if (time >= 0 && time < config->fly_time) {
        setSemiTrans(&quad, 0);
        setRGB0(&quad, config->red, config->green, config->blue);
        point = work->shards;
        for (i = 0; i < 8; point++, i++) {
            setVector(&position, work->anchor.vx, work->anchor.vy, work->anchor.vz);
            setVector(&rotation, 0, 0, time << 7);
            size = ((config->fly_time - time) << 12) / config->fly_time;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&position, point->vx, point->vy, point->vz);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3 + 1] >> 16;
            quad.clut = work->texture[i % 3 + 1];
            setUV4(&quad, i % 3 * 64, 0, i % 3 * 64 + 63, 0, i % 3 * 64, 63, i % 3 * 64 + 63, 63);
           if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        work->anchor.vx += -work->anchor.vx / (config->fly_time - time) * 2;
        work->anchor.vy += -(work->anchor.vy + 350) / (config->fly_time - time) * 2;
        work->anchor.vz += (side * 450 - work->anchor.vz) / (config->fly_time - time) * 2;
    }
    fan.tpage = work->texture[0] >> 16;
    fan.clut = work->texture[0];
    SetSemiTrans(&fan, 1);
    setUV3(&fan, 208, 0, 192, 127, 223, 127);
    time = work->elapsed - config->gap * 8 - config->fade_in - config->fly_time;
    if (time >= 0 && time < config->fan_time) {
        setVector(&position, 0, -350, side * 450);
        setVector(&rotation, 0, 0, time << 6);
        size = (time << 12) / config->fan_time;
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setRGB0(&fan, 0, 0, 0);
        red = config->fan_red * (config->fan_time - time) / config->fan_time;
        green = config->fan_green * (config->fan_time - time) / config->fan_time;
        blue = config->fan_blue * (config->fan_time - time) / config->fan_time;
        setRGB1(&fan, red, green, blue);
        setRGB2(&fan, red, green, blue);
        point = work->fan_ring;
        for (i = 0; i < 8; point += 2, i++) {
            depth = RotAverage3(&work->fan_centre, &point[0], &point[1], (PSXLONG *)&fan.x0,
                (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&fan, ot, (u16)depth);
            }
        }
        point = work->fan_outer;
        for (i = 0; i < 16; point++, i++) {
            depth = RotAverage3(&work->fan_centre, &point[0], &point[1], (PSXLONG *)&fan.x0,
                (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&fan, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (time < 0) {
        return 0;
    }
    if (time > config->fan_time) {
        return 2;
    }
    if (work->hit) {
        return 0;
    }
    work->hit++;
    return 1;
}

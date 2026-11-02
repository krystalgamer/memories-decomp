#include "../../types.h"
#include "variant120_entry.h"
#include "../../game/graphics_constants.h"

/* Header 120's entry: the slot ambient dims while a flickering sprite follows
 * the camera part and 16 spinning blades of shaded strips turn around it;
 * both then grow in front of the slot and fade while a full-screen flash
 * fades out and the ambient recovers. Commands from 100 up also queue a tint
 * during the first half of the slot animation window; once the animation
 * passes the middle of the window, the entry moves the camera and restarts
 * both slot animations. */

static const VECTOR gModelVariant120_D1264 = {4096, 4096, 4096, 0};

s32 ModelVariant120_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G4 strip;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    ModelTintColor tint_start, tint_end;
    PSXLONG flag, interpolation;
    Model120State *work;
    Model120Config *config;
    GsOT *ot;
    s32 side;
    s32 i, j, time, other, size;
    u16 red, green, blue, fade;
    s32 step;
    u8 r, g, b;
    PSXLONG depth;
    SVECTOR *point, *speed;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant120_D1264;
    work = (Model120State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant120_D1290[command % 100];
        point = work->angles;
        for (i = 0; i < 16; point++, i++) {
            point->vx = 0;
            point->vy = rand() % 4096;
            point->vz = i << 8;
        }
        point = work->speeds;
        for (i = 0; i < 16; point++, i++) {
            point->vx = 0;
            point->vy = rand() % 64;
            point->vz = rand() % 64;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant120_D1274[i]);
        }
        work->camera_steps = 2;
        work->elapsed = 0;
        work->started = 0;
        if (command >= 100) {
            work->tint = 1;
        } else {
            work->tint = 0;
        }
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    time = work->elapsed;
    if (time < config->delay) {
        size = ((config->delay - time) << 11) / config->delay;
    } else {
        time = time - config->start - config->duration;
        if (time >= 0 && time < config->recovery) {
            size = (time << 11) / config->recovery;
        } else {
            size = 0;
        }
    }
    Model_SetSlotAmbient(2, size, size, size);
    if (work->elapsed < config->delay) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    time = Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) - config->anim_start;
    if (time >= 0 && time < (s16)config->anim_span / 2 && time % 3 < 2 && work->tint == 1) {
        Model_SetFrameStepOverride(1);
        tint_start.r = 96;
        tint_start.g = 96;
        tint_start.b = 96;
        tint_end.r = 0;
        tint_end.g = 0;
        tint_end.b = 0;
        Model_QueueTintRequestForAllParts(Model_GetActiveSlotIndex(), 2, tint_start, tint_end,
            (s16)config->anim_span / 2 - time);
    }
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyG4(&strip);
    setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->start - config->delay + config->duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
        if (time < config->start - config->delay) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            setRGB0(&quad, config->red >> 1, config->green >> 1, config->blue >> 1);
            size = rand() % 256 + 0xF80;
            setVector(&scale, size, size, size);
        } else {
            size = time - (config->start - config->delay);
            r = config->red * (config->duration - size) / config->duration;
            g = config->green * (config->duration - size) / config->duration;
            b = config->blue * (config->duration - size) / config->duration;
            setRGB0(&quad, r, g, b);
            time = (size << 13) / config->duration + 4096;
            setVector(&position, 0, -350, side * 450);
            setVector(&scale, time, time, time);
        }
        setVector(&vertices[0], -config->half_size, -config->half_size, 0);
        setVector(&vertices[1], config->half_size, -config->half_size, 0);
        setVector(&vertices[2], -config->half_size, config->half_size, 0);
        setVector(&vertices[3], config->half_size, config->half_size, 0);
        setVector(&rotation, 0, 0, 0);
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
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->start - config->delay + config->duration) {
        if (time < config->start - config->delay) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            size = rand() % 256 + 0x780;
            setVector(&scale, size, size, size);
            fade = 64;
            red = config->blade_red >> 2;
            green = config->blade_green >> 2;
            blue = config->blade_blue >> 2;
        } else {
            size = time - (config->start - config->delay);
            time = (size << 13) / config->duration + 4096;
            setVector(&position, 0, -350, side * 450);
            setVector(&scale, time, time, time);
            red = config->blade_red * (config->duration - size) / config->duration / 2;
            green = config->blade_green * (config->duration - size) / config->duration / 2;
            fade = (config->duration - size) * 255 / config->duration / 2;
            blue = config->blade_blue * (config->duration - size) / config->duration / 2;
        }
        point = work->angles;
        speed = work->speeds;
        for (i = 0; i < 16; point++, speed++, i++) {
            GsSetLsMatrix(&base);
            rotation.vx = speed->vx;
            rotation.vy = speed->vy;
            rotation.vz = speed->vz;
            rotation.vx = rotation.vx * work->elapsed;
            rotation.vy = rotation.vy * work->elapsed;
            rotation.vz = rotation.vz * work->elapsed;
            rotation.vx += point->vx;
            rotation.vy += point->vy;
            rotation.vz += point->vz;
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (j = 0; j < 8; j++) {
                setVector(&vertices[0], config->width * j / 8, -config->height * j / 8, 0);
                setVector(&vertices[1], config->width * j / 8, config->height * j / 8, 0);
                setVector(&vertices[2], config->width * (j + 1) / 8, -config->height * (j + 1) / 8, 0);
                setVector(&vertices[3], config->width * (j + 1) / 8, config->height * (j + 1) / 8, 0);
                strip.r0 = r = red * (8 - j) / 8;
                strip.g0 = g = green * (8 - j) / 8;
                strip.b0 = b = blue * (8 - j) / 8;
                setRGB1(&strip, r, g, b);
                setRGB2(&strip, red * (7 - j) / 8, green * (7 - j) / 8, blue * (7 - j) / 8);
                setRGB3(&strip, r, g, b);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&strip.x0, (PSXLONG *)&strip.x1, (PSXLONG *)&strip.x2,
                    (PSXLONG *)&strip.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&strip, ot, (u16)depth, 1);
                }
                if (j < 4) {
                    size = fade * (4 - j) / 16;
                    time = fade * (3 - j) / 16;
                    setRGB0(&strip, size, size, size);
                    setRGB1(&strip, size, size, size);
                    setRGB2(&strip, time, time, time);
                    setRGB3(&strip, time, time, time);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&strip.x0, (PSXLONG *)&strip.x1, (PSXLONG *)&strip.x2,
                        (PSXLONG *)&strip.x3, &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&strip, ot, (u16)depth, 1);
                    }
                }
            }
        }
    }
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->duration) {
        r = config->flash_red * (config->duration - time) / config->duration;
        g = config->flash_green * (config->duration - time) / config->duration;
        b = config->flash_blue * (config->duration - time) / config->duration;
        setRGB0(&flash, r, g, b);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    if (Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) >= config->anim_start + (s16)config->anim_span / 2) {
        size = Model_GetActiveSlotIndex();
        other = Model_GetActiveSlotIndex() ^ 1;
        setVector(&vertices[0], 0, 0, 0);
        switch (work->camera_steps) {
        case 0:
            Model_RequestCameraMoveToSlots(1, -1, -1, 0);
            break;
        case 1:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 2, &vertices[0]);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(size, Model_GetSlotAnimationIndex(size), config->anim_start);
            Model_SeekSlotAnimation(other, 1, 0);
            work->started = 0;
            work->camera_steps--;
            break;
        case 2:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &vertices[0]);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(size, Model_GetSlotAnimationIndex(size), config->anim_start);
            Model_SeekSlotAnimation(other, 1, 0);
            work->started = 0;
            work->camera_steps--;
            break;
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->start;
    if (time < 0) {
        return 0;
    }
    if (time >= config->duration + config->recovery) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (work->started) {
        return 0;
    } else {
        work->started = 1;
        return 1;
    }
}

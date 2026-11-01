#include "../../types.h"
#include "variant121_entry.h"
#include "../../game/graphics_constants.h"

/* Header 121's entry: six textured quads stepped along the camera part's
 * last three positions trail it, then 64 particle boxes and 64 shaded lines
 * burst from a point in front of the slot while a glow quad pair cycles
 * through its texture frames and a full-screen flash fades out; the entry
 * resets the slot ambient when the burst ends. Commands from 100 up also
 * queue a tint during the first half of the slot animation window; once the
 * animation passes the middle of the window, the entry moves the camera and
 * restarts both slot animations. */

s32 ModelVariant121_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G4 strip; /* initialised but never drawn */
    GsBOXF box;
    GsGLINE line;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    ModelTintColor tint_start, tint_end;
    PSXLONG flag, interpolation;
    Model121State *work;
    Model121Config *config;
    GsOT *ot;
    s32 side;
    s32 i, j, time, other, size;
    s32 step;
    PSXLONG depth;
    SVECTOR *point, *source;
    u8 r, g, b;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant121_D1840;
    work = (Model121State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant121_D1888[command % 100];
        point = work->points;
        for (i = 0; i < 128; point++, i++) {
            point->vx = rand() % (config->spread * 2) - config->spread;
            point->vy = rand() % (config->spread * 2) - config->spread;
            point->vz = rand() % (config->spread * 2) - config->spread;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant121_D1850[i]);
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
    if (work->elapsed < config->delay) {
        PushMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        point = &work->points[128];
        for (i = 0; i < 3; point++, i++) {
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        PopMatrix();
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
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    line.attribute = 0x50000000;
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->duration) {
        box.r = config->box_red * (config->duration - time) / config->duration;
        box.g = config->box_green * (config->duration - time) / config->duration;
        box.b = config->box_blue * (config->duration - time) / config->duration;
        line.r0 = box.r;
        line.g0 = box.g;
        line.r1 = 0;
        line.g1 = 0;
        line.b1 = 0;
        setVector(&position, 0, -350, side * 450);
        line.b0 = box.b;
        size = (time << 12) / config->duration;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setVector(&vertices[0], 0, 0, 0);
        RotTransPers(&vertices[0], (PSXLONG *)&box.x, &interpolation, &flag);
        size = box.x;
        other = box.y;
        point = work->points;
        for (i = 0; i < 64; point++, i++) {
            depth = RotTransPers(point, (PSXLONG *)&box.x, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortBoxFill(&box, ot, (u16)depth);
            }
        }
        /* Each line also re-sorts the box at the last particle's position. */
        for (i = 0; i < 64; point++, i++) {
            depth = RotTransPers(point, (PSXLONG *)&line.x0, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortBoxFill(&box, ot, (u16)depth);
            }
            line.x1 = size + (line.x0 - size) * time / config->duration;
            line.y1 = other + (line.y0 - other) * time / config->duration;
            if (depth >= 0 && flag >= 0) {
                GsSortGLine(&line, ot, (u16)depth);
            }
        }
        setRGB0(&quad, config->glow_red, config->glow_green, config->glow_blue);
        setVector(&vertices[0], -config->glow_half_size, -config->glow_half_size, 0);
        setVector(&vertices[1], config->glow_half_size, -config->glow_half_size, 0);
        setVector(&vertices[2], -config->glow_half_size, config->glow_half_size, 0);
        setVector(&vertices[3], config->glow_half_size, config->glow_half_size, 0);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setVector(&position, 0, -350, side * 450);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        setUV4(&quad, time * 8 / config->duration % 4 * 32, time * 8 / config->duration / 4 * 32 + 64,
            time * 8 / config->duration % 4 * 32 + 31, time * 8 / config->duration / 4 * 32 + 64,
            time * 8 / config->duration % 4 * 32, time * 8 / config->duration / 4 * 32 + 95,
            time * 8 / config->duration % 4 * 32 + 31, time * 8 / config->duration / 4 * 32 + 95);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
        setRGB0(&quad, 128, 128, 128);
        setVector(&scale, 2048, 2048, 2048);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        setUV4(&quad, time * 8 / config->duration % 4 * 32, time * 8 / config->duration / 4 * 32 + 64,
            time * 8 / config->duration % 4 * 32 + 31, time * 8 / config->duration / 4 * 32 + 64,
            time * 8 / config->duration % 4 * 32, time * 8 / config->duration / 4 * 32 + 95,
            time * 8 / config->duration % 4 * 32 + 31, time * 8 / config->duration / 4 * 32 + 95);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->trail_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->trail_half_size, -config->trail_half_size, 0);
        setVector(&vertices[1], config->trail_half_size, -config->trail_half_size, 0);
        setVector(&vertices[2], -config->trail_half_size, config->trail_half_size, 0);
        setVector(&vertices[3], config->trail_half_size, config->trail_half_size, 0);
        setRGB0(&quad, config->trail_red, config->trail_green, config->trail_blue);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        /* The shift runs one step too far, copying the last particle into
         * the newest slot before the camera part overwrites it. */
        point = &work->points[130];
        source = &work->points[129];
        for (i = 0; i < 3; point--, source--, i++) {
            setVector(point, source->vx, source->vy, source->vz);
        }
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&work->points[128], matrix.t[0], matrix.t[1], matrix.t[2]);
        /* The y and z steps extrapolate from the older position. */
        point = &work->points[128];
        source = &work->points[129];
        for (j = 0; j < 2; point++, source++, j++) {
            for (i = 1; i < 4; i++) {
                setVector(&position, point->vx + (source->vx - point->vx) * i / 3,
                    source->vy + (source->vy - point->vy) * i / 3,
                    source->vz + (source->vz - point->vz) * i / 3);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                size = j * 3 + i;
                setUV4(&quad, size / 2 * 64, 0, size / 2 * 64 + 63, 0, size / 2 * 64, 63, size / 2 * 64 + 63, 63);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
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
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point = &work->points[128];
            for (i = 0; i < 3; point++, i++) {
                setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            }
            break;
        case 2:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &vertices[0]);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(size, Model_GetSlotAnimationIndex(size), config->anim_start);
            Model_SeekSlotAnimation(other, 1, 0);
            work->started = 0;
            work->camera_steps--;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point = &work->points[128];
            for (i = 0; i < 3; point++, i++) {
                setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            }
            break;
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->start;
    if (time < 0) {
        return 0;
    }
    if (time >= config->duration) {
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

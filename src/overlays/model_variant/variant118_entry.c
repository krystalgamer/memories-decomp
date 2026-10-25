#include "../../types.h"
#include "variant118_entry.h"

/* Header 118's entry: three textured quads trail the camera part's last three
 * positions, then two rings of 64 shaded triangles fan out from a point in
 * front of the slot and fade, then a shell of 64 random dots grows from the
 * same point as 2x2 boxes and fades. Commands from 100 up also queue a tint
 * during the first half of the slot animation window; once the animation
 * passes the middle of the window, the entry moves the camera and restarts
 * both slot animations. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model118State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G3 fan;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[64];
    u16 depths[64], projection[64], flags[64];
    ModelTintColor tint_start, tint_end;
    PSXLONG flag, interpolation;
    s32 direction;
    GsOT *ot;
    Model118Config *config;
    s32 i, j, time, step, other;
    s32 side;
    s32 elevation, size;
    s32 n0, n1;
    u16 red, green, blue;
    SVECTOR *point;
    DVECTOR *projected;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C044;
    work = (Model118State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C08C[command % 100];
        point = work->trail;
        for (i = 0; i < 3; point++, i++) {
            setVector(point, 0, 0, 0);
        }
        for (i = 0; i < 64; i++) {
            work->scales[i] = rand() % 4096 + 2048;
        }
        point = work->dots;
        for (i = 0; i < 64; point++, i++) {
            size = config->radius + rand() % config->height / 2;
            other = rand() % 4096;
            elevation = rand() % 4096;
            point->vx = size * ccos(other) / 4096 * ccos(elevation) / 4096;
            point->vy = size * csin(other) / 4096 * ccos(elevation) / 4096;
            point->vz = size * csin(elevation) / 4096;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C054[i]);
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
    SetPolyG3(&fan);
    box.attribute = 0x50000000;
    box.w = 2;
    box.h = 2;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setRGB0(&quad, config->quad_red, config->quad_green, config->quad_blue);
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&work->trail[2], work->trail[1].vx, work->trail[1].vy, work->trail[1].vz);
    setVector(&work->trail[1], work->trail[0].vx, work->trail[0].vy, work->trail[0].vz);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&work->trail[0], matrix.t[0], matrix.t[1], matrix.t[2]);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->trail;
    for (i = 0; i < 3; point++, i++) {
        time = work->elapsed - config->start - i * 2;
        if (time >= 0 && time < config->quad_duration) {
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, i * 64, 0, i * 64 + 63, 0, i * 64, 63, i * 64 + 63, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->fan_duration) {
        setVector(&vertices[0], 0, -config->height, 0);
        setVector(&vertices[1], -config->width / 2, 0, 0);
        setVector(&vertices[2], config->width / 2, 0, 0);
        setVector(&vertices[3], 0, -350, direction * 450);
        red = config->fan_red * (config->fan_duration - time) / config->fan_duration;
        green = config->fan_green * (config->fan_duration - time) / config->fan_duration;
        blue = config->fan_blue * (config->fan_duration - time) / config->fan_duration;
        setRGB0(&fan, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setRGB1(&fan, red >> 3, green >> 3, blue >> 3);
        setRGB2(&fan, red, green, blue);
        GsSetLsMatrix(&base);
        RotTransPers(&vertices[3], (PSXLONG *)&fan.x2, &interpolation, &flag);
        for (i = 0; i < 64; i++) {
            size = i * 64;
            setVector(&rotation, 0, 0, size);
            size = work->scales[i];
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&fan.x0,
                (PSXLONG *)&fan.x1, (PSXLONG *)&quad.x0, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
                fan.x1 = quad.x0;
                fan.y1 = quad.y0;
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
        blue = (config->fan_duration - time) * 128 / config->fan_duration;
        setRGB0(&fan, 0, 0, 0);
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, blue, blue, blue);
        for (i = 0; i < 64; i++) {
            size = i * 64;
            setVector(&rotation, 0, 0, size);
            size = work->scales[i] * 2 / 3;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&fan.x0,
                (PSXLONG *)&fan.x1, (PSXLONG *)&quad.x0, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
                fan.x1 = quad.x0;
                fan.y1 = quad.y0;
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->box_duration) {
        box.r = config->box_red * (config->box_duration - time) / config->box_duration;
        box.g = config->box_green * (config->box_duration - time) / config->box_duration;
        box.b = config->box_blue * (config->box_duration - time) / config->box_duration;
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        size = (time << 12) / config->box_duration;
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->dots, screen, depths, projection, flags, 64);
        projected = screen;
        i = 0;
    box_loop:
        box.x = projected->vx;
        box.y = projected->vy;
        depth = depths[i] >> 2;
        if (depth >= 0 && !(flags[i] & 0x20)) {
            GsSortBoxFill(&box, ot, depth);
        }
        projected++;
        i++;
        if (i < 64) {
            goto box_loop;
        }
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
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 1, &vertices[0]);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(size, Model_GetSlotAnimationIndex(size), config->anim_start);
            Model_SeekSlotAnimation(other, 1, 0);
            work->started = 0;
            work->camera_steps--;
            break;
        case 2:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 0, &vertices[0]);
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
    if (time >= config->box_duration) {
        return 2;
    }
    if (work->started) {
        return 0;
    } else {
        work->started = 1;
        return 1;
    }
}

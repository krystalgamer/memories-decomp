#include "../../types.h"
#include "variant129_entry.h"
#include "../../game/screen_projection.h"

/* The entry is a relative of the header-118 entry. Up to sixteen points each
 * follow one of the slot's parts until their start time, then draw a growing,
 * fading fan of triangles and, after it, a growing, fading shell of dots. The
 * slot is tinted while its animation runs, and the camera moves between the
 * slots once the animation passes its midpoint. A negative command draws a
 * frame; any other command sets the work area up. */

s32 ModelVariant129_HandleRequest(u8 *context, s32 command)
{
    Model129State *work;
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
    Model129Config *config;
    s32 i, j, time, step, other;
    s32 elevation, size;
    u16 red, green, blue;
    SVECTOR *point;
    DVECTOR *projected;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant129_D0FA0;
    work = (Model129State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant129_D0FE8[command % 100];
        point = work->positions;
        for (i = 0; i < 16; point++, i++) {
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
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant129_D0FB0[i]);
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
    j = 0;
    box.w = 2;
    box.h = 2;
    point = work->positions;
    if (config->starts[0] >= 0) {
        for (; config->starts[j] >= 0; j++, point++) {
            time = work->elapsed - config->starts[j];
            if (time < 0) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[j]), &matrix);
                setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            } else if (time < config->fan_duration) {
                setVector(&vertices[0], 0, -config->height, -config->width);
                setVector(&vertices[1], -config->width / 2, 0, 0);
                setVector(&vertices[2], config->width / 2, 0, 0);
                red = config->red * (config->fan_duration - time) / config->fan_duration;
                green = config->green * (config->fan_duration - time) / config->fan_duration;
                blue = config->blue * (config->fan_duration - time) / config->fan_duration;
                setRGB0(&fan, 0, 0, 0);
                setRGB1(&fan, red >> 3, green >> 3, blue >> 3);
                setRGB2(&fan, red, green, blue);
                setVector(&position, point->vx, point->vy, point->vz);
                GsSetLsMatrix(&base);
                RotTransPers(point, (PSXLONG *)&fan.x2, &interpolation, &flag);
                for (i = 0; i < 64; i++) {
                    size = i * 64;
                    setVector(&rotation, 0, 0, size);
                    size = work->scales[i] * time / config->fan_duration;
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
                    size = work->scales[i] * 2 / 3 * time / config->fan_duration;
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
        }
        point = work->positions;
        other = 0;
        if (config->starts[0] >= 0) {
            MATRIX *m = &matrix;
            DVECTOR *start = screen;

            for (; config->starts[other] >= 0; point++, other++) {
                time = work->elapsed - config->starts[other];
                if (time >= 0 && time < config->box_duration) {
                    box.r = config->box_red * (config->box_duration - time) / config->box_duration;
                    box.g = config->box_green * (config->box_duration - time) / config->box_duration;
                    box.b = config->box_blue * (config->box_duration - time) / config->box_duration;
                    setVector(&position, point->vx, point->vy, point->vz);
                    setVector(&rotation, 0, 0, 0);
                    size = (time << 12) / config->box_duration;
                    setVector(&scale, size, size, size);
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, m);
                    MulMatrix2(&base, m);
                    ScaleMatrix(m, &scale);
                    GsSetLsMatrix(m);
                    RotTransPersN(work->dots, screen, depths, projection, flags, 64);
                    projected = start;
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
            }
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
    time = work->elapsed - config->starts[0];
    if (time < 0) {
        return 0;
    }
    for (i = 0; config->starts[i] >= 0; i++) {
        time = work->elapsed - config->starts[i];
    }
    if (time >= config->box_duration) {
        return 2;
    }
    for (i = 0; config->starts[i] >= 0; i++) {
        time = work->elapsed - config->starts[i];
        if (time >= 0 && work->started == i) {
            work->started = i + 1;
            if (config->starts[work->started] < 0) {
                return 1;
            }
            return 4;
        }
    }
    size = Model_GetSlotAnimationIndex(Model_GetActiveSlotIndex() ^ 1);
    if (size == 5 || size == 8) {
        return 4;
    }
    return 0;
}

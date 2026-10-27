#include "../../types.h"
#include "variant128_entry.h"
#include "../../game/screen_projection.h"

/* Header 128's entry: the command's slot parts flash grey and recover, a
 * gradient fills the screen around the slot, 64 random points around the
 * slot grow as one-pixel boxes, and lines through each of up to eight trailing
 * parts sweep across the screen with fading triangle fans. The return value
 * reports the hit (1) once, the end of the boxes (2) or neither (0). */

static const VECTOR D_8013C598 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model128State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 flash;
    POLY_G3 trail;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[64];
    u16 depths[64], projection[64], flags[64];
    ModelTintColor tint_start, tint_end;
    PSXLONG interpolation, flag;
    s32 step;
    GsOT *ot;
    Model128Config *config;
    s32 i, time, radius, angle;
    s32 side;
    SVECTOR *dot;
    DVECTOR *projected;
    PSXLONG depth;
    u16 red, green, blue;
    s16 left_x, left_y, right_x, right_y;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C598;
    work = (Model128State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C5C4[command % 100];
        dot = work->dots;
        for (i = 0; i < 64; i++) {
            radius = config->radius + rand() % config->radius / 2;
            angle = rand() % 4096;
            /* The elevation shares depth's variable, as the allocation requires. */
            depth = rand() % 4096;
            dot->vx = radius * ccos(angle) / 4096 * ccos(depth) / 4096;
            dot->vy = radius * csin(angle) / 4096 * ccos(depth) / 4096;
            dot->vz = radius * csin(depth) / 4096;
            dot++;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C5A8[i]);
        }
        work->elapsed = 0;
        if (command >= 100) {
            work->tinted = 1;
        } else {
            work->tinted = 0;
        }
        for (i = 0; i < 64; i++) {
            work->latch[i] = 0;
        }
        work->hit = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&flash);
    SetPolyG3(&trail);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    for (i = 0; config->tint_delay[i] >= 0; i++) {
        time = Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) - config->tint_delay[i];
        if (time >= 0 && time < config->tint_window[i]) {
            if (work->tinted) {
                Model_SetFrameStepOverride(1);
            }
            tint_start.r = 96;
            tint_start.g = 96;
            tint_start.b = 96;
            tint_end.r = 0;
            tint_end.g = 0;
            tint_end.b = 0;
            Model_QueueTintRequestForParts(Model_GetActiveSlotIndex(), 2, tint_start, tint_end,
                config->tint_duration[i], config->parts[0], config->parts[1], config->parts[2],
                config->parts[3], config->parts[4], config->parts[5], config->parts[6], config->parts[7]);
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->flash_duration) {
        red = config->flash_red * (config->flash_duration - time) / config->flash_duration;
        green = config->flash_green * (config->flash_duration - time) / config->flash_duration;
        blue = config->flash_blue * (config->flash_duration - time) / config->flash_duration;
        setVector(&vertices[0], 0, -350, side * 450);
        setRGB0(&flash, red / 2, green / 2, blue / 2);
        setRGB1(&flash, red * 2 / 3, green * 2 / 3, blue * 2 / 3);
        setRGB2(&flash, red * 2 / 3, green * 2 / 3, blue * 2 / 3);
        setRGB3(&flash, red, green, blue);
        GsSetLsMatrix(&base);
        RotTransPers(&vertices[0], (PSXLONG *)&flash.x3, &interpolation, &flag);
        flash.x0 = 0;
        flash.y0 = 0;
        flash.x1 = flash.x3 - 1;
        flash.y1 = 0;
        flash.x2 = 0;
        flash.y2 = flash.y3 - 1;
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        flash.x0 = GRAPHICS_DEFAULT_WIDTH - 1;
        flash.y0 = 0;
        flash.x1 = flash.x3;
        flash.y1 = 0;
        flash.x2 = GRAPHICS_DEFAULT_WIDTH - 1;
        flash.y2 = flash.y3 - 1;
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        flash.x0 = 0;
        flash.y0 = GRAPHICS_DEFAULT_HEIGHT - 1;
        flash.x1 = flash.x3 - 1;
        flash.y1 = GRAPHICS_DEFAULT_HEIGHT - 1;
        flash.x2 = 0;
        flash.y2 = flash.y3;
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        flash.x0 = GRAPHICS_DEFAULT_WIDTH - 1;
        flash.y0 = GRAPHICS_DEFAULT_HEIGHT - 1;
        flash.x1 = flash.x3;
        flash.y1 = GRAPHICS_DEFAULT_HEIGHT - 1;
        flash.x2 = GRAPHICS_DEFAULT_WIDTH - 1;
        flash.y2 = flash.y3;
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->box_duration) {
        box.r = config->box_red * (config->box_duration - time) / config->box_duration;
        box.g = config->box_green * (config->box_duration - time) / config->box_duration;
        box.b = config->box_blue * (config->box_duration - time) / config->box_duration;
        setVector(&position, 0, -350, side * 450);
        setVector(&rotation, 0, 0, 0);
        time = (time << 12) / config->box_duration;
        setVector(&scale, time, time, time);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->dots, screen, depths, projection, flags, 64);
        for (i = 0; i < 64; i++) {
            projected = &screen[i];
            box.x = projected->vx;
            box.y = projected->vy;
            depth = depths[i] >> 2;
            if (depth >= 0 && !(flags[i] & 0x20)) {
                GsSortBoxFill(&box, ot, depth);
            }
        }
    }
    setVector(&vertices[0], 0, -config->height, 0);
    setVector(&vertices[1], 0, config->height, 0);
    setVector(&vertices[2], 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    time = work->elapsed - config->delay + 8;
    if (time < 0) {
        dot = &work->trails[0].tail;
        for (i = 0; config->parts[i] >= 0; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
            dot->vx = matrix.t[0];
            dot->vy = matrix.t[1];
            dot->vz = matrix.t[2];
            dot += 2;
        }
    }
    if (time >= 0 && time < config->trail_duration) {
        red = config->trail_red * (config->trail_duration - time) / config->trail_duration;
        green = config->trail_green * (config->trail_duration - time) / config->trail_duration;
        blue = config->trail_blue * (config->trail_duration - time) / config->trail_duration;
        setRGB0(&trail, 0, 0, 0);
        setRGB1(&trail, 0, 0, 0);
        setRGB2(&trail, red, green, blue);
        for (i = 0, dot = &work->trails[0].head; config->parts[i] >= 0; i++) {
            if (work->latch[i] < 4) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
                dot->vx = matrix.t[0];
                dot->vy = matrix.t[1];
                dot->vz = matrix.t[2];
                work->latch[i]++;
            } else {
                GsSetLsMatrix(&base);
                depth = RotTransPers(dot, (PSXLONG *)&flash.x0, &interpolation, &flag);
                RotTransPers(dot + 1, (PSXLONG *)&flash.x1, &interpolation, &flag);
                if (flash.x0 < flash.x1) {
                    left_x = flash.x0;
                    left_y = flash.y0;
                    right_y = flash.y1;
                    right_x = flash.x1;
                } else {
                    left_x = flash.x1;
                    right_x = flash.x0;
                    left_y = flash.y1;
                    right_y = flash.y0;
                }
                if (left_x == right_x) {
                    flash.x0 = left_x;
                    flash.y0 = 0;
                    flash.x3 = right_x;
                    flash.y3 = GRAPHICS_DEFAULT_HEIGHT - 1;
                } else if (left_y == right_y) {
                    flash.x0 = 0;
                    flash.y0 = left_y;
                    flash.x3 = GRAPHICS_DEFAULT_WIDTH - 1;
                    flash.y3 = right_y;
                } else if (left_y < right_y) {
                    flash.y0 = (right_x * left_y - left_x * right_y) / (right_x - left_x);
                    if (flash.y0 < 0) {
                        flash.y0 = 0;
                        flash.x0 = (left_x * right_y - right_x * left_y) / (right_y - left_y);
                    } else {
                        flash.x0 = 0;
                    }
                    flash.y3 = ((GRAPHICS_DEFAULT_WIDTH - 1 - left_x) * right_y
                        - (GRAPHICS_DEFAULT_WIDTH - 1 - right_x) * left_y) / (right_x - left_x);
                    if (flash.y3 >= GRAPHICS_DEFAULT_HEIGHT) {
                        flash.x3 = ((GRAPHICS_DEFAULT_HEIGHT - 1 - left_y) * right_x
                            - (GRAPHICS_DEFAULT_HEIGHT - 1 - right_y) * left_x) / (right_y - left_y);
                        flash.y3 = GRAPHICS_DEFAULT_HEIGHT - 1;
                    } else {
                        flash.x3 = GRAPHICS_DEFAULT_WIDTH - 1;
                    }
                } else {
                    flash.y0 = (right_x * left_y - left_x * right_y) / (right_x - left_x);
                    if (flash.y0 >= GRAPHICS_DEFAULT_HEIGHT) {
                        flash.x0 = ((GRAPHICS_DEFAULT_HEIGHT - 1 - left_y) * right_x
                            - (GRAPHICS_DEFAULT_HEIGHT - 1 - right_y) * left_x) / (right_y - left_y);
                        flash.y0 = GRAPHICS_DEFAULT_HEIGHT - 1;
                    } else {
                        flash.x0 = 0;
                    }
                    flash.y3 = ((GRAPHICS_DEFAULT_WIDTH - 1 - left_x) * right_y
                        - (GRAPHICS_DEFAULT_WIDTH - 1 - right_x) * left_y) / (right_x - left_x);
                    if (flash.y3 < 0) {
                        flash.y3 = 0;
                        flash.x3 = (left_x * right_y - right_x * left_y) / (right_y - left_y);
                    } else {
                        flash.x3 = GRAPHICS_DEFAULT_WIDTH - 1;
                    }
                }
                position.vx = dot->vx;
                position.vy = dot->vy;
                position.vz = dot->vz;
                angle = ratan2(flash.y0 - quad.y0, flash.x0 - quad.x0);
                setVector(&rotation, 0, 0, angle);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPers3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&flash.x1,
                    (PSXLONG *)&flash.x2, (PSXLONG *)&trail.x2, &interpolation, &flag);
                trail.x0 = flash.x0;
                trail.y0 = flash.y0;
                trail.x1 = flash.x1;
                trail.y1 = flash.y1;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = flash.x3;
                trail.y0 = flash.y3;
                trail.x1 = flash.x1;
                trail.y1 = flash.y1;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = flash.x3;
                trail.y0 = flash.y3;
                trail.x1 = flash.x2;
                trail.y1 = flash.y2;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = flash.x0;
                trail.y0 = flash.y0;
                trail.x1 = flash.x2;
                trail.y1 = flash.y2;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                red = (config->trail_duration - time) * 255 / config->trail_duration;
                setRGB0(&trail, 0, 0, 0);
                setRGB1(&trail, 0, 0, 0);
                trail.x0 = trail.x2 + (flash.x0 - trail.x2) / 2;
                trail.y0 = trail.y2 + (flash.y0 - trail.y2) / 2;
                trail.x1 = trail.x2 + (flash.x1 - trail.x2) / 2;
                trail.y1 = trail.y2 + (flash.y1 - trail.y2) / 2;
                setRGB2(&trail, red, red, red);
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = trail.x2 + (flash.x3 - trail.x2) / 2;
                trail.y0 = trail.y2 + (flash.y3 - trail.y2) / 2;
                trail.x1 = trail.x2 + (flash.x1 - trail.x2) / 2;
                trail.y1 = trail.y2 + (flash.y1 - trail.y2) / 2;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = trail.x2 + (flash.x3 - trail.x2) / 2;
                trail.y0 = trail.y2 + (flash.y3 - trail.y2) / 2;
                trail.x1 = trail.x2 + (flash.x2 - trail.x2) / 2;
                trail.y1 = trail.y2 + (flash.y2 - trail.y2) / 2;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                trail.x0 = trail.x2 + (flash.x0 - trail.x2) / 2;
                trail.y0 = trail.y2 + (flash.y0 - trail.y2) / 2;
                trail.x1 = trail.x2 + (flash.x2 - trail.x2) / 2;
                trail.y1 = trail.y2 + (flash.y2 - trail.y2) / 2;
                Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
            }
            dot += 2;
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay;
    if (time >= 0) {
        if (time < config->box_duration) {
            if (work->hit) {
                return 0;
            }
            work->hit = 1;
            return 1;
        }
        return 2;
    }
    return 0;
}

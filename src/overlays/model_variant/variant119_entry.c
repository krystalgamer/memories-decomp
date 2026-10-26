#include "../../types.h"
#include "variant119_entry.h"
#include "../../game/graphics_constants.h"

/* Header 119's entry: waves of 16 textured blades turning around the camera
 * part, each wave growing and fading over the row's fade time, with a
 * full-screen flash at the start. Commands from 100 up also queue a tint
 * during the first half of the slot animation window; once the animation
 * passes the middle of the window, the entry moves the camera and restarts
 * both slot animations. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model119State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR offset;
    /* Never used; retail's frame has room for it. */
    SVECTOR spare[3];
    ModelTintColor tint_start, tint_end;
    PSXLONG flag, interpolation;
    s32 direction;
    GsOT *ot;
    Model119Config *config;
    s32 i, j, time, step, other;
    s32 side;
    u8 red, green, blue;
    SVECTOR *point;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BA1C;
    work = (Model119State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BA48[command % 100];
        point = work->vertices;
        setVector(&point[0], config->length + config->half_width, -config->half_width,
            -side * config->depth);
        setVector(&point[1], config->length + config->half_width, config->half_width,
            -side * config->depth);
        setVector(&point[2], config->length, -config->half_width, 0);
        setVector(&point[3], config->length, config->half_width, 0);
        for (j = 0; j < 1; j++) {
            work->texture[j] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BA2C[j]);
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
    SetPolyF4(&flash);
    setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 0, 0, 63, 0, 0, 31, 63, 31);
    if (config->part == 0xFF) {
        setVector(&position, 0, -350, side * 450);
    } else {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
    }
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->start - i * config->spacing;
        if (time >= 0 && time < config->fade) {
            red = config->red * (config->fade - time) / config->fade;
            green = config->green * (config->fade - time) / config->fade;
            blue = config->blue * (config->fade - time) / config->fade;
            setRGB0(&quad, red, green, blue);
            direction = (time << 12) / config->fade;
            setVector(&scale, direction, direction, direction);
            other = 256;
            point = work->vertices;
            for (j = 0, direction = time * 300; j < 16; j++) {
                setVector(&rotation, 0, 0, direction);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&point[0], &point[1], &point[2], &point[3], (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                direction += other;
            }
        }
    }
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->fade) {
        red = config->flash_red * (config->fade - time) / config->fade;
        green = config->flash_green * (config->fade - time) / config->fade;
        blue = config->flash_blue * (config->fade - time) / config->fade;
        setRGB0(&flash, red, green, blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    if (Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) >= config->anim_start + (s16)config->anim_span / 2) {
        direction = Model_GetActiveSlotIndex();
        other = Model_GetActiveSlotIndex() ^ 1;
        setVector(&offset, 0, 0, 0);
        switch (work->camera_steps) {
        case 0:
            Model_RequestCameraMoveToSlots(1, -1, -1, 0);
            break;
        case 1:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &offset);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(direction, Model_GetSlotAnimationIndex(direction), config->anim_start);
            Model_SeekSlotAnimation(other, 1, 0);
            work->started = 0;
            work->camera_steps--;
            break;
        case 2:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 2, &offset);
            work->elapsed = config->anim_start * 2;
            Model_SeekSlotAnimation(direction, Model_GetSlotAnimationIndex(direction), config->anim_start);
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
    if (time >= config->fade + config->spacing * config->count) {
        return 2;
    }
    if (work->started) {
        return 0;
    } else {
        work->started = 1;
        return 1;
    }
}

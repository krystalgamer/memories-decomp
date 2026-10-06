#include "../../types.h"
#include "variant144_entry.h"

static const VECTOR D_8013BEB0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model144State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 gradient;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR projected[64];
    u16 depths[64], interpolation[64], flags[64];
    ModelTintColor tint_start, tint_end;
    PSXLONG parameter, flag;
    s32 direction;
    GsOT *ot;
    Model144Config *config;
    SVECTOR *point;
    s32 i, value, angle0, angle1, time, step;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BEB0;
    work = (Model144State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BEDC[command % 100];
        point = work->points;
        for (i = 0; i < 64; point++, i++) {
            value = config->radius + (rand() % config->radius) / 2;
            angle0 = rand() % 4096;
            angle1 = rand() % 4096;
            point->vx = value * ccos(angle0) / 4096 * ccos(angle1) / 4096;
            point->vy = value * csin(angle0) / 4096 * ccos(angle1) / 4096;
            point->vz = value * csin(angle1) / 4096;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEC0[i]);
        }
        work->elapsed = 0;
        if (command >= 100) {
            work->frame_step_override = 1;
        } else {
            work->frame_step_override = 0;
        }
        work->started = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&gradient);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    for (i = 0; config->tint_starts[i] >= 0; i++) {
        time = Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) - config->tint_starts[i];
        if (time >= 0 && time < config->tint_durations[i]) {
            if (work->frame_step_override) {
                Model_SetFrameStepOverride(1);
            }
            /* Native code leaves both fourth color bytes uninitialized. */
            tint_start.b0 = 96;
            tint_start.b1 = 96;
            tint_start.b2 = 96;
            tint_end.b0 = 0;
            tint_end.b1 = 0;
            tint_end.b2 = 0;
            Model_QueueTintRequestForParts(Model_GetActiveSlotIndex(), 2, tint_start, tint_end,
                config->tint_ramp_durations[i], config->parts[0], config->parts[1], config->parts[2],
                config->parts[3], config->parts[4], config->parts[5], config->parts[6], config->parts[7]);
        }
    }
    time = work->elapsed - config->main_delay;
    if (time >= 0 && time < config->gradient_duration) {
        u16 red, green, blue;
        red = config->red * (config->gradient_duration - time) / config->gradient_duration;
        green = config->green * (config->gradient_duration - time) / config->gradient_duration;
        blue = config->blue * (config->gradient_duration - time) / config->gradient_duration;
        setVector(&vertices[0], 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTransPers(&vertices[0], (PSXLONG *)&gradient.x3, &parameter, &flag);
        if (flag >= 0) {
            setRGB0(&gradient, red / 2, green / 2, blue / 2);
            setRGB1(&gradient, red * 2 / 3, green * 2 / 3, blue * 2 / 3);
            setRGB2(&gradient, red * 2 / 3, green * 2 / 3, blue * 2 / 3);
            setRGB3(&gradient, red, green, blue);
            setXY3(&gradient, 0, 0, gradient.x3 - 1, 0, 0, gradient.y3 - 1);
            func_8005B260((u32 *)&gradient, ot, 1, 1);
            setXY3(&gradient, 319, 0, gradient.x3, 0, 319, gradient.y3 - 1);
            func_8005B260((u32 *)&gradient, ot, 1, 1);
            setXY3(&gradient, 0, 255, gradient.x3 - 1, 255, 0, gradient.y3);
            func_8005B260((u32 *)&gradient, ot, 1, 1);
            setXY3(&gradient, 319, 255, gradient.x3, 255, 319, gradient.y3);
            func_8005B260((u32 *)&gradient, ot, 1, 1);
        } else {
            setRGB0(&gradient, red, green, blue);
            setRGB1(&gradient, red, green, blue);
            setRGB2(&gradient, red, green, blue);
            setRGB3(&gradient, red, green, blue);
            setXY4(&gradient, 0, 0, 320, 0, 0, 256, 320, 256);
            func_8005B260((u32 *)&gradient, ot, 1, 1);
        }
    }
    time = work->elapsed - config->main_delay;
    if (time >= 0 && time < config->particle_duration) {
        box.r = config->particle_red * (config->particle_duration - time) / config->particle_duration;
        box.g = config->particle_green * (config->particle_duration - time) / config->particle_duration;
        box.b = config->particle_blue * (config->particle_duration - time) / config->particle_duration;
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, 0);
        value = (time << 12) / config->particle_duration;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->points, projected, depths, interpolation, flags, 64);
        for (i = 0; i < 64; i++) {
            box.x = projected[i].vx;
            box.y = projected[i].vy;
            depth = depths[i] >> 2;
            if (depth >= 0) {
                GsSortBoxFill(&box, ot, depth);
            }
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->half_size, -config->half_size, -config->half_size / 2);
    setVector(&vertices[1], config->half_size, -config->half_size, -config->half_size / 2);
    setVector(&vertices[2], -config->half_size, config->half_size, -config->half_size / 2);
    setVector(&vertices[3], config->half_size, config->half_size, -config->half_size / 2);
    time = work->elapsed - config->glow_delay;
    if (time >= 0 && time < config->glow_duration) {
        s32 u;
        u = (time / 2 % 3) * 32;
        setUV4(&quad, u, 0, u + 31, 0, u, 31, u + 31, 31);
        if (time < config->glow_fade_in) {
            s32 red, green, blue;
            red = config->glow_red * time / config->glow_fade_in;
            green = config->glow_green * time / config->glow_fade_in;
            blue = config->glow_blue * time / config->glow_fade_in;
            setRGB0(&quad, red, green, blue);
        } else {
            setRGB0(&quad, config->glow_red, config->glow_green, config->glow_blue);
        }
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, time * 100);
        for (i = 0; config->parts[i] >= 0; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &parameter, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->main_delay;
    if (time < 0) {
        return 0;
    } else if (time >= config->particle_duration) {
        return 2;
    } else {
        if (work->started) {
            return 0;
        } else {
            work->started = 1;
            return 1;
        }
    }
}

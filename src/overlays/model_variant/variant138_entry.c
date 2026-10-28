#include "../../types.h"
#include "variant138_entry.h"

/* Header 138's entry: a 13x13 rainbow grid of Gouraud-shaded quads waves and
 * scrolls in front of the slot while the model's ambient light dims; a
 * rainbow-shaded ring then sways and bounces with 34 colour-cycling sprites
 * orbiting it, 32 sprites rise from around the opposing slot, and the light
 * is restored at the end. */

static const VECTOR D_8013E304 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT4 strip;
    POLY_G4 unused_poly;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR points[169];
    DVECTOR screen[169];
    u16 depths[169], projection[169], flags[169];
    SVECTOR saved, origin;
    VECTOR unused; /* Never used; it keeps retail's frame size. */
    Model138Colour saved_colour;
    PSXLONG flag, interpolation;
    Model138State *work;
    Model138Config *config;
    s32 direction;
    GsOT *ot;
    SVECTOR *point, *edge, *other;
    Model138Colour *colour;
    s32 i, j, time, value, fade, far, angle;
    u8 red, green, blue;
    u8 r0, g0, b0, r1, g1, b1, r2, g2, b2, r3, g3, b3;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013E304;
    work = (Model138State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013E37C[command];
        point = work->lower;
        other = work->upper;
        for (i = 0; i < 17; i++) {
            point->vx = config->sprite_size * i / 16 - config->sprite_size / 2;
            point->vy = config->base;
            point->vz = 0;
            other->vx = config->sprite_size * i / 16 - config->sprite_size / 2;
            other->vy = config->base;
            other->vz = 0;
            point++;
            other++;
        }
        work->spin = -direction * config->speed;
        point = work->orbit;
        for (i = 0; i < 34; i++) {
            work->frames[i] = rand() % 8;
            point->vx = config->sprite_size * i / 34 - config->sprite_size / 2;
            point->vy = config->base;
            point->vz = 0;
            point++;
        }
        point = work->grid;
        colour = work->colours;
        value = 0;
        for (i = 0; i < 13; i++) {
            for (j = 0; j < 13; j++) {
                point->vx = config->grid_size * j / 12 - config->grid_size / 2;
                point->vy = config->amplitude * csin(value) / 4096;
                angle = (i + j) * 4096 / 12;
                point->vz = config->grid_size * i / 12 - config->grid_size / 2;
                colour->red = csin(angle) / 32 + 127;
                colour->green = csin(angle + 0x555) / 32 + 127;
                colour->blue = csin(angle + 0xAAA) / 32 + 127;
                point++;
                colour++;
                value = (value + 372) % 4096;
            }
        }
        Model_GetSlotDimensionsWithMinimums(Model_GetActiveSlotIndex() ^ 1, (ModelEffectAdjustment *)&saved);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&origin);
        point = work->rising;
        for (i = 0; i < 32; i++) {
            point->vx = origin.vx;
            if (saved.vx != 0) {
                point->vx += rand() % (saved.vx * 2) - saved.vx;
            }
            point->vy = origin.vy;
            if (saved.vy != 0) {
                point->vy += rand() % (saved.vy * 2) - saved.vy;
            }
            point->vz = 0;
            if (saved.vz != 0) {
                point->vz = direction * rand() % saved.vz;
            }
            point++;
        }
        /* All three words are packed from the first image. */
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, D_8013E328);
        }
        work->elapsed = -config->lead;
        work->completed = 0;
        colour = work->ring;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                value = (j + i * 2) % 17 * 4096 / 17;
                colour->red = csin(value) / 32 + 127;
                colour->green = csin(value + 0x555) / 32 + 127;
                colour->blue = csin(value + 0xAAA) / 32 + 127;
                colour++;
            }
        }
#ifndef VERSION_FRENCH
        printf(D_8013E314, sizeof(Model138State), sizeof(Model138State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    SetPolyGT4(&strip);
    SetPolyG4(&unused_poly);
    setRGB0(&quad, config->red, config->green, config->blue);
    time = work->elapsed + config->lead;
    SetSemiTrans(&strip, 1);
    strip.tpage = work->texture[2] >> 16;
    strip.clut = work->texture[2];
    setUV4(&strip, 128, 32, 191, 32, 128, 95, 191, 95);
    setVector(&position, 0, config->base, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    RotTransPersN(work->grid, screen, depths, projection, flags, 169);
    colour = work->colours;
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 12; j++) {
            value = i * 13 + j;
            far = (i + 1) * 13 + j;
            setXY4(&strip, screen[value].vx, screen[value].vy, screen[value + 1].vx, screen[value + 1].vy,
                screen[far].vx, screen[far].vy, screen[far + 1].vx, screen[far + 1].vy);
            /* Each corner's channels are computed before any is stored. */
            if (work->elapsed < 0) {
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r0 = red;
                strip.g0 = green;
                strip.b0 = blue;
                colour++;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r1 = red;
                strip.g1 = green;
                strip.b1 = blue;
                colour += 12;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r2 = red;
                strip.g2 = green;
                strip.b2 = blue;
                colour++;
                red = colour->red * time / config->lead;
                green = colour->green * time / config->lead;
                blue = colour->blue * time / config->lead;
                strip.r3 = red;
                strip.g3 = green;
                strip.b3 = blue;
                colour -= 13;
            } else if ((fade = config->fade - (work->elapsed - config->start)) < config->fade) {
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r0 = red;
                strip.g0 = green;
                strip.b0 = blue;
                colour++;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r1 = red;
                strip.g1 = green;
                strip.b1 = blue;
                colour += 12;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r2 = red;
                strip.g2 = green;
                strip.b2 = blue;
                colour++;
                red = colour->red * fade / config->lead;
                green = colour->green * fade / config->lead;
                blue = colour->blue * fade / config->lead;
                strip.r3 = red;
                strip.g3 = green;
                strip.b3 = blue;
                colour -= 13;
            } else {
                strip.r0 = colour->red;
                strip.g0 = colour->green;
                strip.b0 = colour->blue;
                colour++;
                strip.r1 = colour->red;
                strip.g1 = colour->green;
                strip.b1 = colour->blue;
                colour += 12;
                strip.r2 = colour->red;
                strip.g2 = colour->green;
                strip.b2 = colour->blue;
                colour++;
                strip.r3 = colour->red;
                strip.g3 = colour->green;
                strip.b3 = colour->blue;
                colour -= 13;
            }
            /* The grid's border corners are black. */
            if (i == 0) {
                strip.r0 = 0;
                strip.g0 = 0;
                strip.b0 = 0;
                strip.r1 = 0;
                strip.g1 = 0;
                strip.b1 = 0;
            } else if (i == 11) {
                strip.r2 = 0;
                strip.g2 = 0;
                strip.b2 = 0;
                strip.r3 = 0;
                strip.g3 = 0;
                strip.b3 = 0;
            }
            if (j == 0) {
                strip.r0 = 0;
                strip.g0 = 0;
                strip.b0 = 0;
                strip.r2 = 0;
                strip.g2 = 0;
                strip.b2 = 0;
            } else if (j == 11) {
                strip.r1 = 0;
                strip.g1 = 0;
                strip.b1 = 0;
                strip.r3 = 0;
                strip.g3 = 0;
                strip.b3 = 0;
            }
            depth = AverageZ4(depths[value], depths[value + 1], depths[far], depths[far + 1]);
            flag = (flags[value] | flags[value + 1] | flags[far] | flags[far + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&strip, ot, (u16)depth, 1);
            }
        }
        colour++;
    }
    if (Model_GetActiveSlotIndex() != 0) {
        point = &work->grid[155];
        saved.vx = point->vx;
        saved.vy = point->vy;
        saved.vz = point->vz;
        colour = &work->colours[155];
        saved_colour.red = colour->red;
        saved_colour.green = colour->green;
        saved_colour.blue = colour->blue;
        for (i = 0; i < 155; i++) {
            point->vy = (point - 1)->vy;
            colour->red = (colour - 1)->red;
            colour->green = (colour - 1)->green;
            colour->blue = (colour - 1)->blue;
            point--;
            colour--;
        }
        point->vy = saved.vy;
    } else {
        point = work->grid;
        saved.vx = point->vx;
        saved.vy = point->vy;
        saved.vz = point->vz;
        colour = work->colours;
        saved_colour.red = colour->red;
        saved_colour.green = colour->green;
        saved_colour.blue = colour->blue;
        for (i = 0; i < 155; i++) {
            point->vy = (point + 1)->vy;
            colour->red = (colour + 1)->red;
            colour->green = (colour + 1)->green;
            colour->blue = (colour + 1)->blue;
            point++;
            colour++;
        }
        point->vy = saved.vy;
    }
    colour->red = saved_colour.red;
    colour->green = saved_colour.green;
    colour->blue = saved_colour.blue;
    PopMatrix();
    if (work->elapsed < 0) {
        time = config->lead + work->elapsed;
        value = 2048 - time * 6144 / config->lead;
        Model_SetSlotAmbient(2, value, value, value);
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    time = work->elapsed - config->start;
    if (time >= 0) {
        value = time * 6144 / config->fade - 4096;
        Model_SetSlotAmbient(2, value, value, value);
    } else {
        Model_SetSlotAmbient(2, -4096, -4096, -4096);
    }
    setVector(&position, 0, 0, work->spin);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    point = work->lower;
    edge = points;
    for (j = 0; j < 17; j++) {
        edge->vx = point->vx;
        edge->vy = point->vy;
        edge->vz = point->vz;
        point++;
        edge++;
    }
    for (i = 1; i < 9; i++) {
        point = work->lower;
        other = work->upper;
        for (j = 0; j < 17; j++) {
            edge->vx = point->vx + (other->vx - point->vx) * i / 9;
            edge->vy = point->vy + (other->vy - point->vy) / (9 - i);
            edge->vz = point->vz + (other->vz - point->vz) * i / 9;
            point++;
            other++;
            edge++;
        }
    }
    fade = config->fade - (work->elapsed - config->start);
    fade = fade < 0 ? 0 : fade > config->lead ? config->lead : fade;
    RotTransPersN(points, screen, depths, projection, flags, 153);
    SetSemiTrans(&strip, 1);
    strip.tpage = work->texture[1] >> 16;
    strip.clut = work->texture[1];
    point = work->lower;
    colour = work->ring;
    for (j = 0; j < 16; point++, j++) {
        if (point->vy != config->base || point[1].vy != point->vy) {
            value = j;
            far = j + 17;
            setXY4(&strip, screen[value].vx, screen[value].vy, screen[value + 1].vx, screen[value + 1].vy,
                screen[far].vx, screen[far].vy, screen[far + 1].vx, screen[far + 1].vy);
            depth = AverageZ4(depths[value], depths[value + 1], depths[far], depths[far + 1]);
            flag = (flags[value] | flags[value + 1] | flags[far] | flags[far + 1]) & 0x20;
            setUV4(&strip, j * 16 % 128, 32, j * 16 % 128 + 15, 32, j * 16 % 128, 39, j * 16 % 128 + 15, 39);
            r0 = colour->red * fade / config->lead;
            g0 = colour->green * fade / config->lead;
            b0 = colour->blue * fade / config->lead;
            colour++;
            r1 = colour->red * fade / config->lead;
            g1 = colour->green * fade / config->lead;
            b1 = colour->blue * fade / config->lead;
            colour += 16;
            r2 = colour->red * fade / config->lead;
            g2 = colour->green * fade / config->lead;
            b2 = colour->blue * fade / config->lead;
            colour++;
            r3 = colour->red * fade / config->lead;
            g3 = colour->green * fade / config->lead;
            b3 = colour->blue * fade / config->lead;
            colour -= 18;
            strip.r0 = r0;
            strip.g0 = g0;
            strip.b0 = b0;
            strip.r1 = r1;
            strip.g1 = g1;
            strip.b1 = b1;
            strip.r2 = r2;
            strip.g2 = g2;
            strip.b2 = b2;
            strip.r3 = r3;
            strip.g3 = g3;
            strip.b3 = b3;
            if (j == 0) {
                strip.r0 = 0;
                strip.g0 = 0;
                strip.b0 = 0;
                strip.r2 = 0;
                strip.g2 = 0;
                strip.b2 = 0;
            } else if (j == 15) {
                strip.r1 = 0;
                strip.g1 = 0;
                strip.b1 = 0;
                strip.r3 = 0;
                strip.g3 = 0;
                strip.b3 = 0;
            }
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&strip, ot, (u16)depth);
            }
        }
        colour++;
    }
    colour = &work->ring[17];
    for (i = 1; i < 8; i++) {
        point = work->lower;
        for (j = 0; j < 16; point++, j++) {
            if (point->vy != config->base || point[1].vy != point->vy) {
                r0 = colour->red * fade / config->lead;
                g0 = colour->green * fade / config->lead;
                b0 = colour->blue * fade / config->lead;
                colour++;
                r1 = colour->red * fade / config->lead;
                g1 = colour->green * fade / config->lead;
                b1 = colour->blue * fade / config->lead;
                colour += 16;
                r2 = colour->red * fade / config->lead;
                g2 = colour->green * fade / config->lead;
                b2 = colour->blue * fade / config->lead;
                colour++;
                r3 = colour->red * fade / config->lead;
                g3 = colour->green * fade / config->lead;
                b3 = colour->blue * fade / config->lead;
                colour -= 18;
                strip.r0 = r0;
                strip.g0 = g0;
                strip.b0 = b0;
                strip.r1 = r1;
                strip.g1 = g1;
                strip.b1 = b1;
                strip.r2 = r2;
                strip.g2 = g2;
                strip.b2 = b2;
                strip.r3 = r3;
                strip.g3 = g3;
                strip.b3 = b3;
                if (j == 0) {
                    strip.r0 = 0;
                    strip.g0 = 0;
                    strip.b0 = 0;
                    strip.r2 = 0;
                    strip.g2 = 0;
                    strip.b2 = 0;
                } else if (j == 15) {
                    strip.r1 = 0;
                    strip.g1 = 0;
                    strip.b1 = 0;
                    strip.r3 = 0;
                    strip.g3 = 0;
                    strip.b3 = 0;
                }
                value = i * 17 + j;
                far = (i + 1) * 17 + j;
                setXY4(&strip, screen[value].vx, screen[value].vy, screen[value + 1].vx, screen[value + 1].vy,
                    screen[far].vx, screen[far].vy, screen[far + 1].vx, screen[far + 1].vy);
                depth = AverageZ4(depths[value], depths[value + 1], depths[far], depths[far + 1]);
                flag = (flags[value] | flags[value + 1] | flags[far] | flags[far + 1]) & 0x20;
                setUV4(&strip, j * 16 % 128, 64 / (9 - i) + 32, j * 16 % 128 + 15, 64 / (9 - i) + 32,
                    j * 16 % 128, 64 / (9 - i) + 31 + 64 / ((8 - i) * (9 - i)), j * 16 % 128 + 15,
                    64 / (9 - i) + 31 + 64 / ((8 - i) * (9 - i)));
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&strip, ot, (u16)depth);
                }
            }
            colour++;
        }
        colour++;
    }
    value = config->sprite_size / 16;
    /* A dead guard: only its reload of config survives in retail. */
    if (config->fade == 0) {
        fade = 0;
    }
    setVector(&scale, 4096, 4096, 4096);
    setVector(&points[0], -value, -value, 0);
    setVector(&points[1], value, -value, 0);
    setVector(&points[2], -value, value, 0);
    setVector(&points[3], value, value, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    point = work->orbit;
    for (i = 0; i < 34; point++, i++) {
        value = (i - work->elapsed / 2) % 17 * 8192 / 34;
        red = config->sprite_red * csin(value) / 4096;
        green = config->sprite_green * csin(value + 0x555) / 4096;
        blue = config->sprite_blue * csin(value + 0xAAA) / 4096;
        fade = config->fade - (work->elapsed - config->start);
        if (fade < config->fade) {
            red = red * fade / config->lead;
            green = green * fade / config->lead;
            blue = blue * fade / config->lead;
        }
        setRGB0(&quad, red, green, blue);
        position.vx = point->vx;
        position.vy = point->vy;
        position.vz = point->vz;
        position.vz += work->spin;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&points[0], &points[1], &points[2], &points[3], (PSXLONG *)&quad.x0,
            (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
        setUV4(&quad, (work->frames[i] & 7) << 5, 0, ((work->frames[i] & 7) << 5) + 31, 0,
            (work->frames[i] & 7) << 5, 31, ((work->frames[i] & 7) << 5) + 31, 31);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
        work->frames[i] = (work->frames[i] + 1) % 8;
    }
    value = config->period * 450 / (config->speed * 4);
    point = work->lower;
    for (i = 0; i < 17; point++, i++) {
        point->vz = direction * config->sway * csin(work->elapsed * 1024 / config->period - i * 1024 / 17) / 4096;
        time = work->elapsed - config->period / 2 - i * value / 16 * 2;
        if (time >= 0) {
            point->vz += direction * config->drift * time / config->period * 2;
        }
        if (time >= 0 && time < value) {
            point->vy = -config->bounce * csin(time * 1024 / value) / 4096;
        } else if (time >= value && time < value * 2) {
            point->vy = config->bounce * -ccos((time - value) * 1024 / value) / 4096;
        } else {
            point->vy = 0;
        }
        point->vy += config->base;
    }
    value = config->period * 450 / (config->speed * 4);
    point = work->orbit;
    for (i = 0; i < 34; i++, point++) {
        time = work->elapsed - config->period / 2 - i * value / 17;
        point->vz = direction * config->sway * csin(work->elapsed * 1024 / config->period - i * 512 / 17) / 4096;
        if (time >= 0) {
            point->vz += direction * config->drift * time / config->period * 2;
        }
        if (time >= 0 && time < value) {
            point->vy = -config->bounce * csin(time * 1024 / value) / 4096;
        } else if (time >= value && time < value * 2) {
            point->vy = config->bounce * -ccos((time - value) * 1024 / value) / 4096;
        } else {
            point->vy = 0;
        }
        point->vy += config->base;
    }
    work->spin += (direction << 1) * config->speed / config->period * Model_GetFrameStep();
    value = config->sprite_size / 16;
    setVector(&points[0], -value, -value, 0);
    setVector(&points[1], value, -value, 0);
    setVector(&points[2], -value, value, 0);
    setVector(&points[3], value, value, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setRGB0(&quad, config->sprite_red, config->sprite_green, config->sprite_blue);
    point = work->rising;
    for (i = 0; i < 32; point++, i++) {
        time = work->elapsed - config->period / 2 - i * 2 - config->period * 450 / (config->speed * 4);
        if (time >= 0 && time < config->period * 450 / (config->speed * 4)) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            position.vz = work->spin;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&points[0], &points[1], &points[2], &points[3], (PSXLONG *)&quad.x0,
                (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            value = time * 8 / (config->period * 450 / (config->speed * 4));
            setUV4(&quad, (7 - value) << 5, 0, ((7 - value) << 5) + 31, 0, (7 - value) << 5, 31,
                ((7 - value) << 5) + 31, 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed - config->period / 2 - config->period * 450 / (config->speed * 4);
    if (time < 0) {
        return 0;
    }
    if (config->start + config->fade < work->elapsed) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (config->period * 450 / (config->speed * 4) < time) {
        if (work->completed != 0) {
            return 0;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

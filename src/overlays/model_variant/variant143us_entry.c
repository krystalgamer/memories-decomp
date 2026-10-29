#include "../../types.h"
#include "variant143us_entry.h"

/* Header 143's entry: a 13x13 Gouraud-shaded wave scrolls under the slot
 * while the model's ambient light dims and recovers, a ring of textured quads
 * spreads out from one of the model's joints, 96 sparks fly from another
 * joint to points around the slot, 48 shaded domes open in turn and bursts of
 * four sparks scatter around the opposing slot. A negative command draws a
 * frame; any other command sets the work area up and prints its size. */

static const VECTOR D_8013DC7C = {4096, 4096, 4096, 0};
#ifndef VERSION_FRENCH
/* The work-size format keeps the three stale bytes that pad it; they differ
 * between the two load slots. */
#ifndef MODEL143US_STRING_PAD
#define MODEL143US_STRING_PAD "\0ry<"
#endif
static const char D_8013DC8C[20] = "%d ( %x ) Byte.\n" MODEL143US_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT4 strip;
    POLY_G4 panel;
    POLY_G3 cap;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[169];
    u16 depths[169], projection[169], flags[169];
    SVECTOR corner[4];
    SVECTOR origin;
    Model143UsColour saved;
    PSXLONG flag, interpolation;
    Model143UsState *work;
    s32 direction;
    GsOT *ot;
    s32 step, i, k, time;
    SVECTOR *target;
    Model143UsConfig *config;
    /* Separate setup and drawing pointers keep the original registers. */
    SVECTOR *point, *vertex;
    Model143UsColour *colour, *other;
    /* a, b and c are shared scratch values (wave angle, ring radius, light
     * level, lift, vertex indices, fade amount, dome scale and sprite frame in
     * turn); keeping three variables reproduces the original registers. */
    s32 j, a, c, b;
    u8 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013DC7C;
    work = (Model143UsState *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013DD10[command];
        a = 0;
        point = work->grid;
        colour = work->colours;
        for (i = 0; i < 13; i++) {
            for (j = 0; j < 13; j++) {
                point->vx = config->grid_size * j / 12 - config->grid_size / 2;
                point->vy = config->wave_height * csin(a) / 4096;
                point->vz = config->grid_size * i / 12 - config->grid_size / 2;
                a = (a + 372) % 4096;
                colour->red = config->base[0] + config->swing[0] * csin(a) / 4096;
                colour->green = config->base[1] + config->swing[1] * csin(a) / 4096;
                colour->blue = config->base[2] + config->swing[2] * csin(a) / 4096;
                point++;
                colour++;
            }
        }
        point = work->ring;
        for (i = 0; i < 32; i++) {
            a = config->ring_spread / config->ring_time * 8;
            point->vx = a * ccos(i << 7) / 4096;
            point->vy = 0;
            point->vz = a * csin(i << 7) / 4096;
            point++;
        }
        point = work->targets;
        for (i = 0; i < 96; i++) {
            b = rand() % 4096;
            point->vx = config->target_radius * ccos(b) / 4096;
            point->vy = 0;
            point->vz = config->target_radius * csin(b) / 4096;
            point++;
        }
        point = work->dome;
        for (i = 0; i < 6; i++) {
            /* The spark target pointer doubles as the mirrored vertex. */
            target = point + 8;
            for (j = 0; j < 4; j++) {
                point->vx = (-config->dome_radius / 2 + config->dome_radius * ccos(j * 683 / 4) / 4096) * (6 - i) / 6;
                point->vy = -config->dome_height * i / 6;
                point->vz = direction * config->dome_radius * csin(j * 683 / 4) / 4096 * (6 - i) / 6 +
                    direction * config->dome_radius * csin(i * 1024 / 6) / 4096;
                target->vx = point->vx;
                target->vy = point->vy;
                target->vz = point->vz;
                target->vx = -point->vx;
                point++;
                target--;
            }
            point->vx = 0;
            point->vy = -config->dome_height * i / 6;
            point->vz = direction * config->dome_radius * (6 - i) / 6 +
                direction * config->dome_radius * csin(i * 1024 / 6) / 4096;
            point += 5;
        }
        point->vx = 0;
        point->vy = -config->dome_height;
        point->vz = direction * config->dome_radius;
        red = config->dome_outer[0] - config->dome_inner[0];
        green = config->dome_outer[1] - config->dome_inner[1];
        blue = config->dome_outer[2] - config->dome_inner[2];
        colour = work->dome_colours;
        for (i = 0; i < 6; i++) {
            other = colour + 8;
            for (j = 0; j < 4; j++) {
                colour->red = other->red = config->dome_inner[0] + j * red / 4 * i / 6;
                colour->green = other->green = config->dome_inner[1] + j * green / 4 * i / 6;
                colour->blue = other->blue = config->dome_inner[2] + j * blue / 4 * i / 6;
                colour++;
                other--;
            }
            colour->red = config->dome_inner[0] + i * red / 6;
            colour->green = config->dome_inner[1] + i * green / 6;
            colour->blue = config->dome_inner[2] + i * blue / 6;
            colour += 5;
        }
        colour->red = config->dome_outer[0];
        colour->green = config->dome_outer[1];
        colour->blue = config->dome_outer[2];
        for (i = 47; i >= 0; i--) {
            work->spin[i] = 0;
        }
        point = work->burst;
        for (i = 0; i < 192; i++) {
            point->vx = rand() % config->scatter * 2 - config->scatter;
            point->vy = rand() % config->scatter * 2 - config->scatter;
            point->vz = rand() % config->scatter * 2 - config->scatter;
            point++;
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DCA0[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf(D_8013DC8C, sizeof(Model143UsState), sizeof(Model143UsState));
#endif
    } else {
        config = work->config;
        ot = func_80058F10();
        PushMatrix();
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        SetPolyFT4(&quad);
        SetPolyGT4(&strip);
        SetPolyG4(&panel);
        SetPolyG3(&cap);
        time = work->elapsed - config->wave_start;
        if (time < 0 || work->elapsed >= config->fade_start + config->wave_fade) {
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
        } else if (time < config->wave_fade) {
            a = 2048 - time * 6144 / config->wave_fade;
            Model_SetSlotAmbient(2, a, a, a);
        } else if ((time = work->elapsed - config->fade_start) >= 0 && time <= config->wave_fade) {
            a = time * 6144 / config->wave_fade - 4096;
            Model_SetSlotAmbient(2, a, a, a);
        } else {
            Model_SetSlotAmbient(2, -4096, -4096, -4096);
        }
        time = work->elapsed - config->wave_start;
        if (time >= 0 && work->elapsed < config->fade_start + config->wave_fade) {
            SetSemiTrans(&strip, 1);
            strip.tpage = work->texture[3] >> 16;
            strip.clut = work->texture[3];
            if (Model_GetActiveSlotIndex() != 0) {
                setUV4(&strip, 64 - time % 64, 64 - time % 64, 127 - time % 64, 64 - time % 64, 64 - time % 64,
                    127 - time % 64, 127 - time % 64, 127 - time % 64);
            } else {
                setUV4(&strip, time % 64, time % 64, time % 64 + 63, time % 64, time % 64, time % 64 + 63,
                    time % 64 + 63, time % 64 + 63);
            }
            a = work->elapsed - config->spark_start - config->flight_time;
            if (a < 0) {
                setVector(&position, 0, config->grid_lift, 0);
            } else {
                b = config->grid_lift * (config->interval * 96 - a) / (config->interval * 96);
                setVector(&position, 0, b, 0);
            }
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
                    a = i * 13 + j;
                    b = (i + 1) * 13 + j;
                    setXY4(&strip, screen[a].vx, screen[a].vy, screen[a + 1].vx, screen[a + 1].vy,
                        screen[b].vx, screen[b].vy, screen[b + 1].vx, screen[b + 1].vy);
                    /* Each corner's channels are computed before any is stored. */
                    if (time < config->wave_fade) {
                        red = colour->red * time / config->wave_fade;
                        green = colour->green * time / config->wave_fade;
                        blue = colour->blue * time / config->wave_fade;
                        strip.r0 = red;
                        strip.g0 = green;
                        strip.b0 = blue;
                        colour++;
                        red = colour->red * time / config->wave_fade;
                        green = colour->green * time / config->wave_fade;
                        blue = colour->blue * time / config->wave_fade;
                        strip.r1 = red;
                        strip.g1 = green;
                        strip.b1 = blue;
                        colour += 12;
                        red = colour->red * time / config->wave_fade;
                        green = colour->green * time / config->wave_fade;
                        blue = colour->blue * time / config->wave_fade;
                        strip.r2 = red;
                        strip.g2 = green;
                        strip.b2 = blue;
                        colour++;
                        red = colour->red * time / config->wave_fade;
                        green = colour->green * time / config->wave_fade;
                        blue = colour->blue * time / config->wave_fade;
                        strip.r3 = red;
                        strip.g3 = green;
                        strip.b3 = blue;
                        colour -= 13;
                    } else if (work->elapsed - config->fade_start >= 0) {
                        c = config->wave_fade - (work->elapsed - config->fade_start);
                        red = colour->red * c / config->wave_fade;
                        green = colour->green * c / config->wave_fade;
                        blue = colour->blue * c / config->wave_fade;
                        strip.r0 = red;
                        strip.g0 = green;
                        strip.b0 = blue;
                        colour++;
                        red = colour->red * c / config->wave_fade;
                        green = colour->green * c / config->wave_fade;
                        blue = colour->blue * c / config->wave_fade;
                        strip.r1 = red;
                        strip.g1 = green;
                        strip.b1 = blue;
                        colour += 12;
                        red = colour->red * c / config->wave_fade;
                        green = colour->green * c / config->wave_fade;
                        blue = colour->blue * c / config->wave_fade;
                        strip.r2 = red;
                        strip.g2 = green;
                        strip.b2 = blue;
                        colour++;
                        red = colour->red * c / config->wave_fade;
                        green = colour->green * c / config->wave_fade;
                        blue = colour->blue * c / config->wave_fade;
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
                    /* The outer rows and columns fade to black at the edges. */
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
                    depth = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                    flag = (flags[a] | flags[a + 1] | flags[b] | flags[b + 1]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&strip, ot, (u16)depth);
                    }
                }
                colour++;
            }
            if (Model_GetActiveSlotIndex() != 0) {
                vertex = &work->grid[155];
                corner[0].vx = vertex->vx;
                corner[0].vy = vertex->vy;
                corner[0].vz = vertex->vz;
                colour = &work->colours[155];
                saved.red = colour->red;
                saved.green = colour->green;
                saved.blue = colour->blue;
                for (i = 0; i < 155; i++) {
                    vertex->vy = (vertex - 1)->vy;
                    vertex--;
                    colour->red = (colour - 1)->red;
                    colour->green = (colour - 1)->green;
                    colour->blue = (colour - 1)->blue;
                    colour--;
                }
            } else {
                vertex = work->grid;
                corner[0].vx = vertex->vx;
                corner[0].vy = vertex->vy;
                corner[0].vz = vertex->vz;
                colour = work->colours;
                saved.red = colour->red;
                saved.green = colour->green;
                saved.blue = colour->blue;
                for (i = 0; i < 155; i++) {
                    vertex->vy = (vertex + 1)->vy;
                    vertex++;
                    colour->red = (colour + 1)->red;
                    colour->green = (colour + 1)->green;
                    colour->blue = (colour + 1)->blue;
                    colour++;
                }
            }
            vertex->vy = corner[0].vy;
            colour->red = saved.red;
            colour->green = saved.green;
            colour->blue = saved.blue;
        }
        time = work->elapsed - config->ring_start;
        if (time >= 0 && time < config->ring_time) {
            SetSemiTrans(&quad, 1);
            quad.tpage = work->texture[2] >> 16;
            quad.clut = work->texture[2];
            setUV4(&quad, 128, 0, 191, 0, 128, 79, 191, 79);
            red = config->ring_colour[0] * (config->ring_time - time) / config->ring_time;
            green = config->ring_colour[1] * (config->ring_time - time) / config->ring_time;
            blue = config->ring_colour[2] * (config->ring_time - time) / config->ring_time;
            setRGB0(&quad, red, green, blue);
            setVector(&corner[0], config->ring_width, -config->ring_height, -config->ring_width / 2);
            setVector(&corner[1], config->ring_width, -config->ring_height, config->ring_width / 2);
            setVector(&corner[2], 0, 0, -config->ring_width / 2);
            setVector(&corner[3], 0, 0, config->ring_width / 2);
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->ring_joint), &matrix);
            origin.vx = matrix.t[0];
            origin.vy = 0;
            origin.vz = matrix.t[2];
            vertex = work->ring;
            for (i = 0; i < 32; i++) {
                position.vx = origin.vx;
                position.vy = origin.vy;
                position.vz = origin.vz;
                position.vx += vertex->vx;
                position.vy += vertex->vy;
                position.vz += vertex->vz;
                a = (time << 12) / config->ring_time;
                setVector(&scale, a + 4096, a + 4096, a + 4096);
                setVector(&rotation, 0, -(i << 12) / 32, 0);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3], (PSXLONG *)&quad.x0,
                    (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                vertex->vx += vertex->vx / (time + 1);
                vertex->vz += vertex->vz / (time + 1);
                vertex++;
            }
        }
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setVector(&corner[0], -config->spark_size, -config->spark_size, 0);
        setVector(&corner[1], config->spark_size, -config->spark_size, 0);
        setVector(&corner[2], -config->spark_size, config->spark_size, 0);
        setVector(&corner[3], config->spark_size, config->spark_size, 0);
        setRGB0(&quad, config->spark_colour[0], config->spark_colour[1], config->spark_colour[2]);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        vertex = work->sparks;
        /* Setting target last keeps the original instruction order. */
        target = work->targets;
        for (i = 0; i < 96; i++) {
            time = work->elapsed - config->spark_start - config->interval * i / 2;
            if (time < 0) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->spark_joint), &matrix);
                vertex->vx = matrix.t[0];
                vertex->vy = matrix.t[1];
                vertex->vz = matrix.t[2];
            } else if (time < config->flight_time) {
                position.vx = vertex->vx;
                position.vy = vertex->vy;
                position.vz = vertex->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3], (PSXLONG *)&quad.x0,
                    (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                a = time * 8 / config->flight_time;
                setUV4(&quad, a << 5, 128, (a << 5) + 31, 128, a << 5, 159, (a << 5) + 31, 159);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                vertex->vx += (target->vx - vertex->vx) / (config->flight_time - time) * step;
                vertex->vy += (target->vy - vertex->vy) / (config->flight_time - time) * step *
                    ccos((time << 10) / config->flight_time) / 4096;
                vertex->vz += (target->vz - vertex->vz) / (config->flight_time - time) * step;
            }
            vertex++;
            target++;
        }
        setVector(&rotation, 0, 0, 0);
        for (k = 0; k < 48; k++) {
            time = work->elapsed - config->spark_start - config->flight_time - k * config->interval;
            if (time >= 0 && time < config->dome_time) {
                b = (time << 11) / config->dome_time * 2 + 2048;
                b = b < 0 ? 0 : b > 4096 ? 4096 : b;
                setVector(&scale, b, b, b);
                setVector(&position, 0, 0, work->spin[k]);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->dome, screen, depths, projection, flags, 55);
                colour = work->dome_colours;
                for (i = 0; i < 5; i++) {
                    for (j = 0; j < 8; j++) {
                        b = i * 9 + j;
                        c = (i + 1) * 9 + j;
                        setXY4(&panel, screen[b].vx, screen[b].vy, screen[b + 1].vx, screen[b + 1].vy,
                            screen[c].vx, screen[c].vy, screen[c + 1].vx, screen[c + 1].vy);
                        panel.r0 = colour->red;
                        panel.g0 = colour->green;
                        panel.b0 = colour->blue;
                        colour++;
                        panel.r1 = colour->red;
                        panel.g1 = colour->green;
                        panel.b1 = colour->blue;
                        colour += 8;
                        panel.r2 = colour->red;
                        panel.g2 = colour->green;
                        panel.b2 = colour->blue;
                        colour++;
                        panel.r3 = colour->red;
                        panel.g3 = colour->green;
                        panel.b3 = colour->blue;
                        /* Stepping back ten, then on by one after the sort,
                         * matches the original pointer updates. */
                        colour -= 10;
                        depth = AverageZ4(depths[b], depths[b + 1], depths[c], depths[c + 1]);
                        flag = (flags[b] | flags[b + 1] | flags[c] | flags[c + 1]) & 0x20;
                        if (depth >= 0 && flag == 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&panel, ot, (u16)depth, 1);
                        }
                        colour++;
                    }
                    colour++;
                }
                colour = &work->dome_colours[54];
                setRGB2(&cap, colour->red, colour->green, colour->blue);
                colour = &work->dome_colours[45];
                /* The apex index is kept in a variable, so its address is formed
                 * from the hoisted array bases. */
                c = 54;
                for (j = 0; j < 8; j++) {
                    a = 45 + j;
                    setXY3(&cap, screen[a].vx, screen[a].vy, screen[a + 1].vx, screen[a + 1].vy,
                        screen[c].vx, screen[c].vy);
                    cap.r0 = colour->red;
                    cap.g0 = colour->green;
                    cap.b0 = colour->blue;
                    colour++;
                    cap.r1 = colour->red;
                    cap.g1 = colour->green;
                    cap.b1 = colour->blue;
                    colour--;
                    depth = AverageZ3(depths[a], depths[a + 1], depths[c]);
                    flag = (flags[a] | flags[a + 1] | flags[c]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&cap, ot, (u16)depth, 1);
                    }
                    colour++;
                }
                work->spin[k] += (direction * 1350 - work->spin[k]) / (config->dome_time - time) * step;
            }
        }
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setVector(&corner[0], -config->spark_size, -config->spark_size, 0);
        setVector(&corner[1], config->spark_size, -config->spark_size, 0);
        setVector(&corner[2], -config->spark_size, config->spark_size, 0);
        setVector(&corner[3], config->spark_size, config->spark_size, 0);
        setRGB0(&quad, config->spark_colour[0], config->spark_colour[1], config->spark_colour[2]);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&origin);
        origin.vy = -350;
        vertex = work->burst;
        for (i = 0; i < 48; i++) {
            time = work->elapsed - config->spark_start - config->interval * i - config->flight_time -
                config->dome_time / 3;
            if (time >= 0 && time < config->burst_time) {
                for (j = 0; j < 4; j++) {
                    position.vx = vertex->vx * time / config->burst_time;
                    position.vy = vertex->vy * time / config->burst_time;
                    position.vz = vertex->vz * time / config->burst_time;
                    position.vx += origin.vx;
                    position.vy += origin.vy;
                    position.vz += origin.vz;
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3], (PSXLONG *)&quad.x0,
                        (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                    a = time * 8 / config->burst_time;
                    setUV4(&quad, a << 5, 128, (a << 5) + 31, 128, a << 5, 159, (a << 5) + 31, 159);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    vertex++;
                }
            } else {
                vertex += 4;
            }
        }
        PopMatrix();
        work->elapsed += step;
        time = work->elapsed - config->spark_start - config->flight_time - config->dome_time / 3;
        if (time < 0) {
            return 0;
        }
        time = work->elapsed - config->spark_start - config->flight_time - config->interval * 48 -
            config->dome_time / 3;
        if (time < 0) {
            return 4;
        }
        if (config->fade_start + config->wave_fade < work->elapsed) {
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
            return 2;
        }
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}

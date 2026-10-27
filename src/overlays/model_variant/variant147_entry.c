#include "../../types.h"
#include "variant147_entry.h"

/* The entry raises a rippling, colour-faded grid under the slot, grows a
 * tube of strips with spinning spokes and fading sparks towards the other
 * slot, sweeps two beams along a spiral path and ends on a fading screen
 * flash. A negative command draws a frame; any other command sets the work
 * area up and prints its size. */

static const VECTOR D_8013E530 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT4 strip;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[169];
    DVECTOR screen[169];
    u16 depths[169], projection[169], flags[169];
    SVECTOR corner[4];
    PSXLONG flag, interpolation;
    Model147State *work;
    Model147Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, k, t, a, b, angle, otz, scroll, fade, count, frame;
    s32 next, below, corner_index;
    u8 red, green, blue;
    SVECTOR *point, *other, *inner, *vertex;
    CVECTOR *color;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013E530;
    work = (Model147State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013E5DC[command];
        point = work->ring;
        other = work->far_ring;
        for (i = 0; i < 17; i++) {
            point->vx = config->width * i / 16 - config->width / 2;
            point->vy = config->height;
            point->vz = 0;
            other->vx = config->width * i / 16 - config->width / 2;
            other->vy = config->height;
            other->vz = 0;
            point++;
            other++;
        }
        work->offset = -direction * config->speed;
        point = work->spokes;
        for (i = 0; i < 34; i++) {
            work->phase[i] = rand() % 8;
            point->vx = config->width * i / 34 - config->width / 2;
            point->vy = config->height;
            point->vz = 0;
            point++;
        }
        point = work->grid;
        color = work->colors;
        angle = 0;
        for (i = 0; i < 13; i++) {
            for (j = 0; j < 13; j++) {
                point->vx = config->grid_size * j / 12 - config->grid_size / 2;
                point->vy = config->ripple * csin(angle) / 4096;
                point->vz = config->grid_size * i / 12 - config->grid_size / 2;
                color->r = rand() % (config->high[0] - config->low[0] + 1) + config->low[0];
                color->g = rand() % (config->high[1] - config->low[2] + 1) + config->low[2];
                color->b = rand() % (config->high[2] - config->low[1] + 1) + config->low[1];
                point++;
                color++;
                angle = (angle + 372) % 4096;
            }
        }
        Model_GetSlotDimensionsWithMinimums(Model_GetActiveSlotIndex() ^ 1, (ModelEffectAdjustment *)&corner[0]);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corner[1]);
        point = work->sparks;
        for (i = 0; i < 32; i++) {
            point->vx = corner[1].vx;
            if (corner[0].vx != 0) {
                point->vx += rand() % (corner[0].vx * 2) - corner[0].vx;
            }
            point->vy = corner[1].vy;
            if (corner[0].vy != 0) {
                point->vy += rand() % (corner[0].vy * 2) - corner[0].vy;
            }
            point->vz = 0;
            if (corner[0].vz != 0) {
                point->vz = direction * rand() % corner[0].vz;
            }
            point++;
        }
        point = work->path;
        for (i = 0; i < 256; i++) {
            point->vx = config->path_radius * ccos(i << 6) / 4096;
            point->vy = -config->path_height * (i * 2 - 256) / 256;
            point->vz = config->path_radius * csin(i << 6) / 4096;
            point++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E550[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x)Byte.\n", sizeof(Model147State), sizeof(Model147State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    SetPolyGT4(&strip);
    SetPolyF4(&flash);
    quad.r0 = config->color[0];
    quad.g0 = config->color[1];
    quad.b0 = config->color[2];
    t = work->elapsed - config->grid_start;
    if (t >= 0 && work->elapsed - config->fade_out < config->fade) {
        SetSemiTrans(&strip, 1);
        strip.tpage = work->texture[4] >> 16;
        strip.clut = work->texture[4];
        if (Model_GetActiveSlotIndex()) {
            setUV4(&strip, 192 - t % 64, 96 - t % 64, 255 - t % 64, 96 - t % 64,
                192 - t % 64, 159 - t % 64, 255 - t % 64, 159 - t % 64);
        } else {
            setUV4(&strip, t % 64 + 128, t % 64 + 32, t % 64 + 191, t % 64 + 32,
                t % 64 + 128, t % 64 + 95, t % 64 + 191, t % 64 + 95);
        }
        setVector(&position, 0, config->height, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->grid, screen, depths, projection, flags, 169);
        color = work->colors;
        for (i = 0; i < 12; i++) {
            for (j = 0; j < 12; j++) {
                a = i * 13 + j;
                b = (i + 1) * 13 + j;
                strip.x0 = screen[a].vx;
                strip.y0 = screen[a].vy;
                strip.x1 = screen[a + 1].vx;
                strip.y1 = screen[a + 1].vy;
                strip.x2 = screen[b].vx;
                strip.y2 = screen[b].vy;
                strip.x3 = screen[b + 1].vx;
                strip.y3 = screen[b + 1].vy;
                if (t < config->fade) {
                    red = color->r * t / config->fade;
                    green = color->g * t / config->fade;
                    blue = color->b * t / config->fade;
                    color++;
                    setRGB0(&strip, red, green, blue);
                    red = color->r * t / config->fade;
                    green = color->g * t / config->fade;
                    blue = color->b * t / config->fade;
                    color += 12;
                    setRGB1(&strip, red, green, blue);
                    red = color->r * t / config->fade;
                    green = color->g * t / config->fade;
                    blue = color->b * t / config->fade;
                    color++;
                    setRGB2(&strip, red, green, blue);
                    red = color->r * t / config->fade;
                    green = color->g * t / config->fade;
                    blue = color->b * t / config->fade;
                    color -= 13;
                    setRGB3(&strip, red, green, blue);
                } else if ((fade = config->fade - (work->elapsed - config->fade_out)) < config->fade) {
                    red = color->r * fade / config->fade;
                    green = color->g * fade / config->fade;
                    blue = color->b * fade / config->fade;
                    color++;
                    setRGB0(&strip, red, green, blue);
                    red = color->r * fade / config->fade;
                    green = color->g * fade / config->fade;
                    blue = color->b * fade / config->fade;
                    color += 12;
                    setRGB1(&strip, red, green, blue);
                    red = color->r * fade / config->fade;
                    green = color->g * fade / config->fade;
                    blue = color->b * fade / config->fade;
                    color++;
                    setRGB2(&strip, red, green, blue);
                    red = color->r * fade / config->fade;
                    green = color->g * fade / config->fade;
                    blue = color->b * fade / config->fade;
                    color -= 13;
                    setRGB3(&strip, red, green, blue);
                } else {
                    setRGB0(&strip, color->r, color->g, color->b);
                    color++;
                    setRGB1(&strip, color->r, color->g, color->b);
                    color += 12;
                    setRGB2(&strip, color->r, color->g, color->b);
                    color++;
                    setRGB3(&strip, color->r, color->g, color->b);
                    color -= 13;
                }
                if (i == 0) {
                    setRGB0(&strip, 0, 0, 0);
                    setRGB1(&strip, 0, 0, 0);
                } else if (i == 11) {
                    setRGB2(&strip, 0, 0, 0);
                    setRGB3(&strip, 0, 0, 0);
                }
                if (j == 0) {
                    setRGB0(&strip, 0, 0, 0);
                    setRGB2(&strip, 0, 0, 0);
                } else if (j == 11) {
                    setRGB1(&strip, 0, 0, 0);
                    setRGB3(&strip, 0, 0, 0);
                }
                otz = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                flag = (flags[a] | flags[a + 1] | flags[b] | flags[b + 1]) & 0x20;
                if (otz >= 0 && flag == 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&strip, ot, (u16)otz, 1);
                }
            }
            color++;
        }
        if (Model_GetActiveSlotIndex()) {
            point = &work->grid[155];
            setVector(&corner[0], point->vx, point->vy, point->vz);
            for (i = 0; i < 155; i++) {
                point->vy = point[-1].vy;
                point--;
            }
        } else {
            point = work->grid;
            setVector(&corner[0], point->vx, point->vy, point->vz);
            for (i = 0; i < 155; i++) {
                point->vy = point[1].vy;
                point++;
            }
        }
        point->vy = corner[0].vy;
    }
    t = work->elapsed - config->grid_start;
    if (t < 0) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
    } else if (t < config->fade) {
        angle = 2048 - t * 6144 / config->fade;
        Model_SetSlotAmbient(2, angle, angle, angle);
    } else if ((t = work->elapsed - config->fade_out) >= 0 && t < config->fade) {
        angle = t * 6144 / config->fade - 4096;
        Model_SetSlotAmbient(2, angle, angle, angle);
    } else {
        Model_SetSlotAmbient(2, -4096, -4096, -4096);
    }
    t = work->elapsed - config->tube_start;
    if (t >= 0) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setVector(&position, 0, 0, work->offset);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = config->color[0];
        green = config->color[1];
        blue = config->color[2];
        if (t < config->fade) {
            red = red * t / config->fade;
            green = green * t / config->fade;
            blue = blue * t / config->fade;
        }
        fade = config->fade - (work->elapsed - config->fade_out);
        if (fade >= 0 && fade < config->fade) {
            red = red * fade / config->fade;
            green = green * fade / config->fade;
            blue = blue * fade / config->fade;
        }
        setRGB0(&quad, red, green, blue);
        point = work->ring;
        vertex = vertices;
        for (j = 0; j < 17; j++) {
            vertex->vx = point->vx;
            vertex->vy = point->vy;
            vertex->vz = point->vz;
            point++;
            vertex++;
        }
        for (i = 1; i < 9; i++) {
            point = work->ring;
            other = work->far_ring;
            for (j = 0; j < 17; j++) {
                vertex->vx = point->vx + (other->vx - point->vx) * i / 9;
                vertex->vy = point->vy + (other->vy - point->vy) / (9 - i);
                vertex->vz = point->vz + (other->vz - point->vz) * i / 9;
                point++;
                other++;
                vertex++;
            }
        }
        RotTransPersN(vertices, screen, depths, projection, flags, 153);
        inner = work->ring;
        for (j = 0; j < 16; j++) {
            if (inner[0].vy != config->height || inner[1].vy != config->height) {
                a = j;
                b = j + 17;
                quad.x0 = screen[a].vx;
                quad.y0 = screen[a].vy;
                quad.x1 = screen[a + 1].vx;
                quad.y1 = screen[a + 1].vy;
                quad.x2 = screen[b].vx;
                quad.y2 = screen[b].vy;
                quad.x3 = screen[b + 1].vx;
                quad.y3 = screen[b + 1].vy;
                otz = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                flag = (flags[a] | flags[a + 1] | flags[b] | flags[b + 1]) & 0x20;
                setUV4(&quad, j * 16 % 128, 32, j * 16 % 128 + 15, 32, j * 16 % 128, 39, j * 16 % 128 + 15, 39);
                if (otz >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, otz);
                }
            }
            inner++;
        }
        for (i = 1; i < 8; i++) {
            inner = work->ring;
            for (j = 0; j < 16; j++) {
                if (inner[0].vy != config->height || inner[1].vy != config->height) {
                    a = i * 17 + j;
                    b = (i + 1) * 17 + j;
                    quad.x0 = screen[a].vx;
                    quad.y0 = screen[a].vy;
                    quad.x1 = screen[a + 1].vx;
                    quad.y1 = screen[a + 1].vy;
                    quad.x2 = screen[b].vx;
                    quad.y2 = screen[b].vy;
                    quad.x3 = screen[b + 1].vx;
                    quad.y3 = screen[b + 1].vy;
                    otz = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                    flag = (flags[a] | flags[a + 1] | flags[b] | flags[b + 1]) & 0x20;
                    setUV4(&quad, j * 16 % 128, 64 / (9 - i) + 32, j * 16 % 128 + 15, 64 / (9 - i) + 32,
                        j * 16 % 128, 64 / (9 - i) + 31 + 64 / ((8 - i) * (9 - i)), j * 16 % 128 + 15, 64 / (9 - i) + 31 + 64 / ((8 - i) * (9 - i)));
                    if (otz >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, otz);
                    }
                }
                inner++;
            }
        }
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setVector(&scale, 4096, 4096, 4096);
        a = config->width / 16;
        /* A dead guard: only its reload of config survives in retail. */
        if (config->fade == 0) {
            fade = 0;
        }
        setVector(&vertices[0], -a, -a, 0);
        setVector(&vertices[1], a, -a, 0);
        setVector(&vertices[2], -a, a, 0);
        setVector(&vertices[3], a, a, 0);
        red = config->spark_color[0];
        green = config->spark_color[1];
        blue = config->spark_color[2];
        if (t < config->fade) {
            red = red * t / config->fade;
            green = green * t / config->fade;
            blue = blue * t / config->fade;
        }
        fade = config->fade - (work->elapsed - config->fade_out);
        if (fade >= 0 && fade < config->fade) {
            red = red * fade / config->fade;
            green = green * fade / config->fade;
            blue = blue * fade / config->fade;
        }
        setRGB0(&quad, red, green, blue);
        point = work->spokes;
        for (i = 0; i < 34; i++) {
            setVector(&position, point->vx, point->vy, point->vz);
            position.vz += work->offset;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, (work->phase[i] & 7) * 32, 0, (work->phase[i] & 7) * 32 + 31, 0,
                (work->phase[i] & 7) * 32, 31, (work->phase[i] & 7) * 32 + 31, 31);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, otz);
            }
            work->phase[i] = (work->phase[i] + 1) % 8;
            point++;
        }
        a = config->period * 450 / (config->speed * 4);
        point = work->ring;
        for (i = 0; i < 17; i++) {
            point->vz = direction * config->sway * csin(t * 1024 / config->period - (i << 10) / 17) / 4096;
            b = t - config->period / 2 - i * a / 16 * 2;
            if (b >= 0) {
                point->vz += direction * config->drift * b / config->period * 2;
            }
            if (b >= 0 && b < a) {
                point->vy = -config->wave_height * csin((b << 10) / a) / 4096;
            } else if (b >= a && b < a * 2) {
                point->vy = config->wave_height * -ccos(((b - a) << 10) / a) / 4096;
            } else {
                point->vy = 0;
            }
            point->vy += config->height;
            point++;
        }
        a = config->period * 450 / (config->speed * 4);
        point = work->spokes;
        for (i = 0; i < 34; i++) {
            b = t - config->period / 2 - i * a / 17;
            point->vz = direction * config->sway * csin((t << 10) / config->period - (i << 9) / 17) / 4096;
            if (b >= 0) {
                point->vz += direction * config->drift * b / config->period * 2;
            }
            if (b >= 0 && b < a) {
                point->vy = -config->wave_height * csin((b << 10) / a) / 4096;
            } else if (b >= a && b < a * 2) {
                point->vy = config->wave_height * -ccos(((b - a) << 10) / a) / 4096;
            } else {
                point->vy = 0;
            }
            point->vy += config->height;
            point++;
        }
        work->offset += direction * (config->speed * 2) / config->period * step;
    }
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    SetSemiTrans(&quad, 1);
    a = config->width / 16;
    setVector(&vertices[0], -a, -a, 0);
    setVector(&vertices[1], a, -a, 0);
    setVector(&vertices[2], -a, a, 0);
    setVector(&vertices[3], a, a, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->spark_color[0], config->spark_color[1], config->spark_color[2]);
    point = work->sparks;
    for (i = 0; i < 32; i++) {
        t = work->elapsed - config->tube_start - config->period / 2 - i * 2 - config->period * 450 / (config->speed * 4);
        if (t < config->period * 450 / (config->speed * 4) && t >= 0) {
            setVector(&position, point->vx, point->vy, point->vz);
            position.vz = work->offset;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            a = t * 8 / (config->period * 450 / (config->speed * 4));
            setUV4(&quad, (7 - a) * 32, 0, (7 - a) * 32 + 31, 0, (7 - a) * 32, 31, (7 - a) * 32 + 31, 31);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, otz);
            }
        }
        point++;
    }
    t = work->elapsed - config->beam_start;
    if (t >= 0 && t < config->beam_time) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 96, 63, 96, 0, 192, 63, 192);
        red = config->beam_color[0] * t / config->beam_time;
        green = config->beam_color[1] * t / config->beam_time;
        blue = config->beam_color[2] * t / config->beam_time;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setRGB0(&quad, red, green, blue);
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&position);
        position.vy = -350;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        k = (t - 4) * 128 / config->beam_time;
        point = &work->path[t * 128 / config->beam_time + 128];
        setVector(&corner[0], point->vx, point->vy, point->vz);
        setVector(&corner[2], point->vx, point->vy, point->vz);
        other = &work->path[k + 128];
        setVector(&corner[1], other->vx, other->vy, other->vz);
        setVector(&corner[3], other->vx, other->vy, other->vz);
        corner[0].vy -= config->beam_width;
        corner[1].vy -= config->beam_width;
        corner[2].vy += config->beam_width;
        corner[3].vy += config->beam_width;
        otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
            &interpolation, &flag);
        if (otz >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, otz);
        }
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        red = config->beam_color2[0] * t / config->beam_time;
        green = config->beam_color2[1] * t / config->beam_time;
        blue = config->beam_color2[2] * t / config->beam_time;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setRGB0(&quad, red, green, blue);
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&position);
        position.vy = -350;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        setUV4(&quad, 64, 96, 95, 96, 64, 192, 95, 192);
        a = 512 / config->beam_time;
        frame = (t - 4) * 128 / config->beam_time;
        b = 80 / a;
        point = &work->path[frame + 128];
        for (i = 0; i < b; i++) {
            setVector(&corner[0], point->vx, point->vy, point->vz);
            setVector(&corner[2], point->vx, point->vy, point->vz);
            other = point - a;
            setVector(&corner[1], other->vx, other->vy, other->vz);
            setVector(&corner[3], other->vx, other->vy, other->vz);
            corner[0].vy -= config->beam_width;
            corner[1].vy -= config->beam_width;
            corner[2].vy += config->beam_width;
            corner[3].vy += config->beam_width;
            otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, otz);
            }
            point -= a;
        }
        setUV4(&quad, 96, 96, 127, 96, 96, 192, 127, 192);
        setVector(&corner[0], point->vx, point->vy, point->vz);
        setVector(&corner[2], point->vx, point->vy, point->vz);
        other = point - a;
        setVector(&corner[1], other->vx, other->vy, other->vz);
        setVector(&corner[3], other->vx, other->vy, other->vz);
        corner[0].vy -= config->beam_width;
        corner[1].vy -= config->beam_width;
        corner[2].vy += config->beam_width;
        corner[3].vy += config->beam_width;
        otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
            &interpolation, &flag);
        if (otz >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, otz);
        }
    }
    t = work->elapsed - config->beam_start - config->beam_time;
    if (t >= 0 && t < config->flash_time) {
#ifdef VERSION_FRENCH
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
#else
        setXY4(&flash, 0, 0, 320, 0, 0, 240, 320, 240);
#endif
        red = (config->flash_time - t) * 255 / config->flash_time;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    count = config->period * 450 / (config->speed * 4);
    t = work->elapsed - config->tube_start - config->period / 2 - count;
    if (t < 0) {
        return 0;
    }
    if (config->fade_out + config->fade < work->elapsed) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (count < t) {
        if (work->completed == 0) {
            work->completed = 1;
            return 1;
        }
        return 0;
    }
    return 4;
}

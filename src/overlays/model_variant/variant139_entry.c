#include "../../types.h"
#include "variant139_entry.h"
#include "../../game/screen_projection.h"

/* The entry spins two textured tubes that swirl around a centre sliding
 * towards the other slot, trails a column of beam sprites behind each swirl
 * and releases 96 rising sparks that burst into rings of dots. A negative
 * command draws a frame; any other command sets the work area up and prints
 * its size. */

static const VECTOR gModelVariant139_D1AAC = {4096, 4096, 4096, 0};

s32 ModelVariant139_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    GsBOXF dot;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR offset;
    SVECTOR corner[4];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG flag, interpolation;
    Model139State *work;
    Model139Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    /* r is a shared scratch value (radius, angle, mesh row, fade or frame);
     * keeping one variable reproduces the original register allocation. */
    s32 i, j, k, t, r, a, b, v, w, otz;
    u8 red, green, blue;
    SVECTOR *point, *trail;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant139_D1AAC;
    work = (Model139State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->mesh;
        config = work->config = &gModelVariant139_D1B74[command % 100];
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                r = config->inner_radius + (config->outer_radius - config->inner_radius) * csin(i << 7) / 4096;
                point->vx = r * ccos(j << 8) / 4096;
                point->vy = -config->height * i / 8;
                point->vz = r * csin(j << 8) / 4096;
                point->pad = 0;
                point++;
            }
        }
        work->centre.vx = 0;
        work->centre.vy = 0;
        work->centre.vz = -direction * 450;
        work->angle = 0;
        for (i = 1, v = 2048; i >= 0; i--) {
            work->phase[i] = v;
            v -= 2048;
        }
        point = work->trail;
        for (i = 0; i < 2; i++) {
            for (j = 1; j < 9; j++) {
                r = config->swirl_radius;
                a = (work->phase[i] + j * config->spacing) % 4096;
                point->vx = r * ccos(a) / 4096;
                point->vy = 0;
                point->vz = r * csin(a) / 4096;
                point->vx += work->centre.vx;
                point->vy += work->centre.vy;
                point->vz += work->centre.vz;
                point->pad = 0;
                point++;
            }
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corner[0]);
        point = work->sparks;
        for (i = 0; i < 96; i++) {
            point->vx = corner[0].vx + rand() % config->swirl_radius * 2 - config->swirl_radius;
            point->vy = rand() % config->swirl_radius * 2 - (s16)(config->swirl_radius + 350);
            point->vz = corner[0].vz + rand() % config->swirl_radius * 2 - config->swirl_radius;
            point++;
        }
        point = work->burst;
        point->vx = 0;
        point->vy = 0;
        point->vz = 0;
        point = &work->burst[1];
        for (i = 0; i < 32; i++) {
            r = rand() % 4096;
            a = rand() % 2048;
            b = rand() % config->burst_radius / 2 + config->burst_radius;
            point->vx = b * ccos(r) / 4096 * csin(a) / 4096;
            point->vy = b * ccos(a) / 4096;
            point->vz = b * csin(r) / 4096 * csin(a) / 4096;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant139_D1ACC[i]);
        }
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, gModelVariant139_D1B04);
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model139State), sizeof(Model139State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    t = work->elapsed - config->delay;
    if (t >= 0 && t < config->fade_in + config->hold + config->fade_out) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setVector(&scale, 4096, 4096, 4096);
        if (t < config->fade_in) {
            red = config->tube_color[0] * t / config->fade_in;
            green = config->tube_color[1] * t / config->fade_in;
            blue = config->tube_color[2] * t / config->fade_in;
            setRGB0(&quad, red, green, blue);
        } else if (t < config->fade_in + config->hold) {
            setRGB0(&quad, config->tube_color[0], config->tube_color[1], config->tube_color[2]);
        } else {
            r = config->fade_in + config->hold + config->fade_out - t;
            red = config->tube_color[0] * r / config->fade_out;
            green = config->tube_color[1] * r / config->fade_out;
            blue = config->tube_color[2] * r / config->fade_out;
            setRGB0(&quad, red, green, blue);
        }
        for (i = 0; i < 2; i++) {
            setVector(&rotation, 0, work->angle + (i << 10), 0);
            offset.vx = work->centre.vx;
            offset.vy = work->centre.vy;
            offset.vz = work->centre.vz;
            if (t < config->fade_in + config->hold) {
                r = config->swirl_radius;
            } else {
                r = config->swirl_radius * (config->fade_in + config->hold + config->fade_out - t) / config->fade_out;
            }
            offset.vx += r * ccos(work->phase[i]) / 4096;
            offset.vz += r * csin(work->phase[i]) / 4096;
            setVector(&position, offset.vx, offset.vy, offset.vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->mesh, screen, depths, projection, flags, 17);
            /* r alternates between the two screen rows of the current strip. */
            r = 1;
            point = &work->mesh[17];
            trail = work->trail + i * 8;
            for (j = 1; j < 9; j++) {
                setVector(&position, trail->vx, trail->vy, trail->vz);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                a = r * 17;
                RotTransPersN(point, &screen[a], &depths[a], &projection[a], &flags[a], 17);
                w = t * 2 % 64 + (8 - j) % 2 * 32;
                for (k = 0; k < 16; k++) {
                    a = k + r * 17;
                    b = k + (r ^ 1) * 17;
                    quad.x0 = screen[a].vx;
                    quad.y0 = screen[a].vy;
                    quad.x1 = screen[a + 1].vx;
                    quad.y1 = screen[a + 1].vy;
                    quad.x2 = screen[b].vx;
                    quad.y2 = screen[b].vy;
                    quad.x3 = screen[b + 1].vx;
                    quad.y3 = screen[b + 1].vy;
                    otz = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                    setUV4(&quad, k * 2 % 16 * 8, w, k * 2 % 16 * 8 + 15, w, k * 2 % 16 * 8, w + 31, k * 2 % 16 * 8 + 15, w + 31);
                    if (!((flags[a] | flags[a + 1] | flags[b] | flags[b + 1]) & 0x20)) {
                        GsSortPoly(&quad, ot, (u16)otz);
                    }
                }
                point += 17;
                r ^= 1;
                trail++;
            }
            work->phase[i] = (work->phase[i] + config->swirl * step) % 4096;
            /* Shift the trail back by one point, starting from its last one. */
            point = work->trail + (i + 1) * 8 - 1;
            for (j = 1; j < 8; j++) {
                point->vx = point[-1].vx;
                point->vy = point[-1].vy;
                point->vz = point[-1].vz;
                point--;
            }
            point->vx = offset.vx;
            point->vy = offset.vy;
            point->vz = offset.vz;
        }
        work->angle = (work->angle + config->spin * step) % 4096;
        r = t - config->fade_in;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corner[0]);
        if (r >= 0 && r < config->hold) {
            work->centre.vx = config->sway * csin((r << 12) / config->hold) / 4096;
            work->centre.vz = -direction * 450 + (corner[0].vz + direction * 450) * r / config->hold;
        }
    }
    t = work->elapsed - config->delay;
    if (t >= 0 && t < config->fade_in + config->hold + config->fade_out) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        setVector(&corner[0], -config->beam_size, -config->beam_size, 0);
        setVector(&corner[1], config->beam_size, -config->beam_size, 0);
        setVector(&corner[2], -config->beam_size, config->beam_size, 0);
        setVector(&corner[3], config->beam_size, config->beam_size, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        if (t < config->fade_in) {
            red = config->beam_color[0] * t / config->fade_in;
            green = config->beam_color[1] * t / config->fade_in;
            blue = config->beam_color[2] * t / config->fade_in;
            setRGB0(&quad, red, green, blue);
        } else if (t < config->fade_in + config->hold) {
            setRGB0(&quad, config->beam_color[0], config->beam_color[1], config->beam_color[2]);
        } else {
            r = config->fade_in + config->hold + config->fade_out - t;
            red = config->beam_color[0] * r / config->fade_out;
            green = config->beam_color[1] * r / config->fade_out;
            blue = config->beam_color[2] * r / config->fade_out;
            setRGB0(&quad, red, green, blue);
        }
        point = work->trail;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&position, point->vx, point->vy, point->vz);
                position.vy = -config->rise * j / 8;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                setUV4(&quad, j / 2 * 64, 128, j / 2 * 64 + 63, 128, j / 2 * 64, 191, j / 2 * 64 + 63, 191);
                if (flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)otz);
                }
                point++;
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    dot.attribute = 0x50000000;
    dot.w = 1;
    dot.h = 1;
    setVector(&corner[0], -config->spark_size, -config->spark_size, 0);
    setVector(&corner[1], config->spark_size, -config->spark_size, 0);
    setVector(&corner[2], -config->spark_size, config->spark_size, 0);
    setVector(&corner[3], config->spark_size, config->spark_size, 0);
    point = work->sparks;
    for (i = 0; i < 96; i++) {
        t = work->elapsed - config->delay - config->fade_in - config->hold - i * config->spark_spacing;
        if (t >= 0 && t < config->spark_time) {
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            quad.r0 = config->spark_color[0];
            quad.g0 = config->spark_color[1];
            quad.b0 = config->spark_color[2];
            setVector(&position, point->vx, point->vy, point->vz);
            position.vy -= config->rise * t / config->spark_time;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            otz = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            r = t * 8 / config->spark_time;
            setUV4(&quad, r % 4 * 32, r / 4 * 32 + 192, r % 4 * 32 + 31, r / 4 * 32 + 192,
                r % 4 * 32, r / 4 * 32 + 223, r % 4 * 32 + 31, r / 4 * 32 + 223);
            if (flag >= 0) {
                GsSortPoly(&quad, ot, (u16)otz);
            }
            dot.r = config->spark_color[0] * (config->spark_time - t) / config->spark_time;
            dot.g = config->spark_color[1] * (config->spark_time - t) / config->spark_time;
            dot.b = config->spark_color[2] * (config->spark_time - t) / config->spark_time;
            setVector(&position, point->vx, point->vy, point->vz);
            r = (t << 12) / config->spark_time;
            setVector(&scale, r, r, r);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->burst, screen, depths, projection, flags, 33);
            for (j = 1; j < 33; j++) {
                dot.x = screen[j].vx;
                dot.y = screen[j].vy;
                if (!(flags[j] & 0x20)) {
                    GsSortBoxFill(&dot, ot, depths[j] >> 2);
                }
            }
        }
        point++;
    }
    PopMatrix();
    work->elapsed += step;
    t = work->elapsed - config->delay - config->fade_in - config->hold;
    if (t < 0) {
        return 0;
    }
    if (t >= config->spark_spacing * 96 + config->spark_time) {
        return 2;
    }
    if (t < config->spark_spacing * 96) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 0;
}

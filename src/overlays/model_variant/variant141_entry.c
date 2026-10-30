#include "../../types.h"
#include "variant141_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry scrolls a wavy background in and out, grows a tube from one of
 * the slot's parts, throws sparks that burst at their targets, spins beams
 * between two parts joined by a bolt, flares at the bolt's middle, flashes
 * the screen and spirals ribbons and a cylinder around the other slot. A
 * negative command draws a frame; any other command sets the work area up
 * and prints its size. */

static const VECTOR D_8013EBE0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad, glow;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corner[4];
    SVECTOR from, to;
    Model141Scratch u;
    u16 depths[34], projection[34], flags[34];
    PSXLONG interpolation, flag;
    Model141State *work;
    Model141Config *config;
    GsOT *ot;
    s32 step;
    s32 side, other;
    s32 i, j, k, t, a, b, c, value, depth;
    u8 red, green, blue;
    s32 direction;
    u8 *wave, *delta;
    SVECTOR *point, *target;
    SVECTOR *G32 *q;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013EBE0;
    work = (Model141State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        wave = work->wave;
        delta = work->wave_step;
        work->scroll = 0;
        config = work->config = &D_8013ECE0[command % 100];
        for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT; i++) {
            *wave = 0;
            *delta = config->wave + config->wave * csin(i * 8192 / GRAPHICS_DEFAULT_HEIGHT) / 4096;
            wave++;
            delta++;
        }
        work->wave_offset = 0;
        point = work->ring;
        for (i = 0; i < 17; i++) {
            point->vx = config->ring_radius * ccos(i << 8) / 4096;
            point->vy = config->ring_radius * csin(i << 8) / 4096;
            point->vz = 0;
            point++;
        }
        point = work->tube_targets;
        for (i = 0; i < 28; i++) {
            point->vx = 0;
            point->vy = -350;
            point->vz = direction * 450;
            point->vx += config->tube_radius * ccos(i << 8) / 4096;
            point->vy += config->tube_radius * csin(i << 8) / 4096;
            point->vz += direction * 450;
            point++;
        }
        point = work->spark_targets;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corner[0]);
        for (i = 0; i < 72; i++) {
            value = rand() % 4096;
            side = rand() % config->spark_spread;
            point->vx = side * ccos(value) / 4096;
            point->vy = 0;
            point->vz = corner[0].vz * i / 72 + side * csin(value) / 4096;
            point++;
        }
        point = work->beams;
        for (i = 0; i < 48; i++) {
            point->vx = 0;
            point->vy = rand() % 4096;
            point->vz = rand() % 4096;
            point++;
        }
        point = work->flare;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                point->vx = j * config->flare_size;
                point->vy = i * config->flare_size;
                point->vz = 0;
                point++;
            }
        }
        work->flare_quads[0] = &work->flare[0];
        work->flare_quads[1] = &work->flare[1];
        work->flare_quads[2] = &work->flare[3];
        work->flare_quads[3] = &work->flare[4];
        work->flare_quads[4] = &work->flare[2];
        work->flare_quads[5] = &work->flare[1];
        work->flare_quads[6] = &work->flare[5];
        work->flare_quads[7] = &work->flare[4];
        work->flare_quads[8] = &work->flare[8];
        work->flare_quads[9] = &work->flare[7];
        work->flare_quads[10] = &work->flare[5];
        work->flare_quads[11] = &work->flare[4];
        work->flare_quads[12] = &work->flare[6];
        work->flare_quads[13] = &work->flare[7];
        work->flare_quads[14] = &work->flare[3];
        work->flare_quads[15] = &work->flare[4];
        point = work->spiral;
        for (i = 0; i < 17; i++) {
            point->vx = config->spiral_radius * csin(i << 9) / 4096 + config->spiral_wobble * ccos(i << 10) / 4096;
            point->vy = config->spiral_radius * ccos(i << 8) / 4096 + config->spiral_wobble * csin(i << 10) / 4096;
            point->vz = config->spiral_wobble * csin(i << 10) / 4096;
            point++;
        }
        point = work->cylinder;
        target = &work->cylinder[17];
        for (i = 0; i < 17; i++) {
            point->vx = config->cylinder_radius * ccos(i << 8) / 4096;
            point->vy = -config->cylinder_height;
            point->vz = config->cylinder_radius * csin(i << 8) / 4096;
            target->vx = point->vx;
            target->vy = point->vy;
            target->vz = point->vz;
            target->vy = config->cylinder_height;
            point++;
            target++;
        }
        for (i = 0; i < 8; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013EC00[i]);
        }
        work->elapsed = 0;
        work->frame = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model141State), sizeof(Model141State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyFT4(&glow);
    SetPolyF4(&flash);
    t = work->elapsed - config->fade_in;
    if (t >= 0 && t < config->fade) {
        value = 0x800 - t * 0x1800 / config->fade;
        Model_SetSlotAmbient(2, value, value, value);
    } else if (t >= config->fade) {
        Model_SetSlotAmbient(2, -0x1000, -0x1000, -0x1000);
    }
    t = work->elapsed - config->fade_out;
    if (t >= 0 && t < config->fade) {
        value = t * 0x1800 / config->fade - 0x1000;
        Model_SetSlotAmbient(2, value, value, value);
    } else if (t >= config->fade) {
        Model_SetSlotAmbient(2, 0x800, 0x800, 0x800);
    }
    if (work->elapsed >= config->fade_in && work->elapsed < config->fade_out + config->fade) {
        quad.tpage = work->texture[7] >> 16;
        quad.clut = work->texture[7];
        t = work->elapsed - config->fade_in;
        if (t >= 0 && t < config->fade) {
            red = config->fade_color[0] * t / config->fade;
            green = config->fade_color[1] * t / config->fade;
            blue = config->fade_color[2] * t / config->fade;
            SetSemiTrans(&quad, 1);
        } else {
            t = work->elapsed - config->fade_out;
            if (t < 0) {
                red = config->fade_color[0];
                green = config->fade_color[1];
                blue = config->fade_color[2];
                SetSemiTrans(&quad, 0);
            } else {
                value = config->fade - t;
                red = config->fade_color[0] * value / config->fade;
                green = config->fade_color[1] * value / config->fade;
                blue = config->fade_color[2] * value / config->fade;
                SetSemiTrans(&quad, 1);
            }
        }
        setRGB0(&quad, red, green, blue);
        wave = work->wave;
        for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT; i += 2) {
            setUV4(&quad, 0, (work->scroll + i) % 64, 192, (work->scroll + i) % 64,
                0, (work->scroll + i) % 64 + 1, 192, (work->scroll + i) % 64 + 1);
            setXY4(&quad, *wave - 64, i, *wave + 128, i, *wave - 64, i + 1, *wave + 128, i + 1);
            Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 4095, 0, 0, 64, 64);
            setXY4(&quad, *wave + 128, i, *wave + 320, i, *wave + 128, i + 1, *wave + 320, i + 1);
            Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 4095, 0, 0, 64, 64);
            wave += 2;
        }
        wave = work->wave;
        delta = &work->wave_step[work->wave_offset];
        for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT - work->wave_offset; i++) {
            *wave += *delta++;
            *wave %= 64;
            wave++;
        }
        delta = work->wave_step;
        for (i = 0; i < work->wave_offset; i++) {
            *wave += *delta++;
            *wave %= 64;
            wave++;
        }
        work->scroll = (work->scroll + step) % 64;
        work->wave_offset = (work->wave_offset + step) % GRAPHICS_DEFAULT_HEIGHT;
    }
    quad.tpage = work->texture[3] >> 16;
    quad.clut = work->texture[3];
    SetSemiTrans(&quad, 1);
    setRGB0(&quad, config->tube_color[0], config->tube_color[1], config->tube_color[2]);
    /* value doubles as the "previous ring drawn" flag; sharing it with the
     * other value uses gives retail's register choice. */
    value = 0;
    side = 0;
    point = work->tube;
    target = work->tube_targets;
    for (i = 0; i < 28; i++) {
        t = work->elapsed - config->start - i * config->tube_spacing;
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->tube_part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            if (value) {
                GsSetLsMatrix(&base);
                RotTransPers(point, (PSXLONG *)&quad.x0, &interpolation, &flag);
                /* Clearing j before other lengthens its live range, which ranks
                 * it below point for a saved register as in retail. */
                j = 0;
                other = (side ^ 1) * 17;
                for (; j < 16; j++) {
                    quad.x1 = quad.x0;
                    quad.y1 = quad.y0;
                    quad.x2 = u.screen[other + j].vx;
                    quad.y2 = u.screen[other + j].vy;
                    quad.x3 = u.screen[other + j + 1].vx;
                    quad.y3 = u.screen[other + j + 1].vy;
                    setUV4(&quad, 64, j * 16 % 128, 95, j * 16 % 128, 64, j * 16 % 128 + 15, 95, j * 16 % 128 + 15);
                    depth = (depths[other + j] + depths[other + j + 1]) / 8;
                    if (flag >= 0) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
                value = 0;
            }
        } else if (t < config->tube_time) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            c = (s32)((u32)t << 12) / config->tube_time;
            setVector(&scale, c, c, c);
            c = (s32)((u32)t << 10) / config->tube_time;
            setVector(&rotation, 0, 0, c);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            c = side * 17;
            other = (side ^ 1) * 17;
            RotTransPersN(work->ring, &u.screen[side * 17], &depths[side * 17], &projection[side * 17],
                &flags[side * 17], 17);
            side ^= 1;
            if (value) {
                for (j = 0; j < 16; j++) {
                    a = j + other;
                    b = j + c;
                    setXY4(&quad, u.screen[a].vx, u.screen[a].vy, u.screen[b].vx, u.screen[b].vy,
                        u.screen[a + 1].vx, u.screen[a + 1].vy, u.screen[b + 1].vx, u.screen[b + 1].vy);
                    setUV4(&quad, 64, j * 16 % 128, 95, j * 16 % 128, 64, j * 16 % 128 + 15, 95, j * 16 % 128 + 15);
                    depth = AverageZ4(depths[a], depths[b], depths[a + 1], depths[b + 1]);
                    if (!((flags[a] | flags[b] | flags[a + 1] | flags[b + 1]) & 0x20)) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
            } else {
                value = 1;
            }
            point->vx += (target->vx - point->vx) / (config->tube_time - t) * step;
            point->vy += (target->vy - point->vy) / (config->tube_time - t) * step;
            point->vz += (target->vz - point->vz) / (config->tube_time - t) * step;
        }
        point++;
        target++;
    }
    quad.tpage = work->texture[6] >> 16;
    quad.clut = work->texture[6];
    SetSemiTrans(&quad, 0);
    setUV4(&quad, 224, 0, 239, 0, 224, 15, 239, 15);
    setVector(&corner[0], -config->spark_size, -config->spark_size, 0);
    setVector(&corner[1], config->spark_size, -config->spark_size, 0);
    setVector(&corner[2], -config->spark_size, config->spark_size, 0);
    setVector(&corner[3], config->spark_size, config->spark_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    setRGB0(&quad, config->spark_color[0], config->spark_color[1], config->spark_color[2]);
    point = work->sparks;
    target = work->spark_targets;
    for (i = 0; i < 24; i++) {
        t = work->elapsed - config->start - i * config->spark_spacing;
        if (t < 0) {
            for (j = 0; j < 3; j++) {
                GsSetLsMatrix(&base);
                GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->spark_part), &matrix);
                target++;
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
                point->vy += 70;
                point++;
            }
        } else if (t < config->spark_time) {
            for (j = 0; j < 3; j++) {
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
                point->vx += (target->vx - point->vx) / (config->spark_time - t) * step;
                point->vy += (target->vy - point->vy) / (config->spark_time - t) * step;
                point->vz += (target->vz - point->vz) / (config->spark_time - t) * step;
                point++;
                target++;
            }
        } else {
            point += 3;
            target += 3;
        }
    }
    quad.tpage = work->texture[4] >> 16;
    quad.clut = work->texture[4];
    SetSemiTrans(&quad, 1);
    setVector(&corner[0], -config->burst_size, -config->burst_size, 0);
    setVector(&corner[1], config->burst_size, -config->burst_size, 0);
    setVector(&corner[2], -config->burst_size, config->burst_size, 0);
    setVector(&corner[3], config->burst_size, config->burst_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    setRGB0(&quad, config->burst_color[0], config->burst_color[1], config->burst_color[2]);
    point = work->spark_targets;
    for (i = 0; i < 24; i++) {
        t = work->elapsed - config->start - i * config->spark_spacing - config->spark_time;
        if (t >= 0 && t < config->burst_time) {
            for (j = 0; j < 3; j++) {
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                position.vy -= config->burst_rise * t / config->burst_time;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                value = t * 8 / config->burst_time;
                setUV4(&quad, value % 4 * 32 + 96, value / 4 * 32, value % 4 * 32 + 127, value / 4 * 32,
                    value % 4 * 32 + 96, value / 4 * 32 + 31, value % 4 * 32 + 127, value / 4 * 32 + 31);
                if (flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
                point++;
            }
        } else {
            point += 3;
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setVector(&corner[0], 0, -config->beam_width / 2, 0);
    setVector(&corner[1], config->beam_length, -config->beam_width / 2, 0);
    setVector(&corner[2], 0, config->beam_width / 2, 0);
    setVector(&corner[3], config->beam_length, config->beam_width / 2, 0);
    setVector(&scale, 4096, 4096, 4096);
    GsSetLsMatrix(&base);
    GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->beam_from), &matrix);
    from.vx = matrix.t[0];
    from.vy = matrix.t[1];
    from.vz = matrix.t[2];
    GsSetLsMatrix(&base);
    GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->beam_to), &matrix);
    to.vx = matrix.t[0];
    to.vy = matrix.t[1];
    to.vz = matrix.t[2];
    side = work->elapsed - config->start - config->beam_time;
    value = config->bolt_time - side;
    u.middle.vx = (from.vx * value + to.vx * side) / config->bolt_time;
    u.middle.vy = (from.vy * value + to.vy * side) / config->bolt_time - csin((s32)((u32)side << 11) / config->bolt_time) * 150 / 4096;
    u.middle.vz = (from.vz * value + to.vz * side) / config->bolt_time;
    point = work->beams;
    for (i = 0; i < 48; i++) {
        t = work->elapsed - config->start - i * config->beam_spacing;
        if (t >= 0 && t < config->beam_time) {
            position.vx = from.vx;
            position.vy = from.vy;
            position.vz = from.vz;
            rotation.vx = point->vx;
            rotation.vy = point->vy;
            rotation.vz = point->vz;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrixZYX(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (t < config->beam_time / 2) {
                setRGB0(&quad, config->beam_color[0], config->beam_color[1], config->beam_color[2]);
            } else {
                s32 r, g, bl;

                r = config->beam_color[0] * (config->beam_time - t) * 2 / config->beam_time;
                g = config->beam_color[1] * (config->beam_time - t) * 2 / config->beam_time;
                bl = config->beam_color[2] * (config->beam_time - t) * 2 / config->beam_time;
                setRGB0(&quad, r, g, bl);
            }
            setUV4(&quad, 96, t * 2 / config->beam_time * 32 + 64, 223, t * 2 / config->beam_time * 32 + 64,
                96, t * 2 / config->beam_time * 32 + 95, 223, t * 2 / config->beam_time * 32 + 95);
            if (flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            position.vx = to.vx;
            position.vy = to.vy;
            position.vz = to.vz;
            rotation.vz += 0x800;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrixZYX(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
        point++;
    }
    t = work->elapsed - config->start - config->beam_time;
    if (t >= 0 && t < config->beam_spacing * 48) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, work->frame % 4 * 16 + 64, 63, work->frame % 4 * 16 + 64,
            0, work->frame % 4 * 16 + 127, 63, work->frame % 4 * 16 + 127);
        red = config->beam_color[0] * t / (config->beam_spacing * 48);
        green = config->beam_color[1] * t / (config->beam_spacing * 48);
        blue = config->beam_color[2] * t / (config->beam_spacing * 48);
        setRGB0(&quad, red, green, blue);
        value = u.middle.vx - from.vx;
        side = u.middle.vy - from.vy;
        c = u.middle.vz - from.vz;
        other = SquareRoot0(value * value + side * side + c * c);
        k = ratan2(c, side);
        corner[0].vx = other * ccos(k + 0x400) / 4096;
        corner[0].vy = other * csin(k + 0x400) / 16384;
        corner[0].vz = 0;
        corner[1].vx = other * ccos(k - 0x400) / 4096;
        corner[1].vy = other * csin(k - 0x400) / 16384;
        corner[1].vz = 0;
        setVector(&scale, 4096, 4096, 4096);
        rotation.vx = Model_GetViewMetricsBuffer()->yaw - 0x400;
        rotation.vy = 0;
        rotation.vz = 0;
        position.vx = from.vx;
        position.vy = from.vy;
        position.vz = from.vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrixYXZ(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotTransPers(&corner[0], (PSXLONG *)&quad.x2, &interpolation, &flag);
        depth += RotTransPers(&corner[1], (PSXLONG *)&quad.x3, &interpolation, &flag);
        position.vx = u.middle.vx;
        position.vy = u.middle.vy;
        position.vz = u.middle.vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrixYXZ(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth += RotTransPers(&corner[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
        depth += RotTransPers(&corner[1], (PSXLONG *)&quad.x1, &interpolation, &flag);
        depth /= 4;
        if (flag >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
        position.vx = to.vx;
        position.vy = to.vy;
        position.vz = to.vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrixYXZ(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth *= 2;
        depth += RotTransPers(&corner[0], (PSXLONG *)&quad.x2, &interpolation, &flag);
        depth += RotTransPers(&corner[1], (PSXLONG *)&quad.x3, &interpolation, &flag);
        depth /= 4;
        if (flag >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
    }
    t = work->elapsed - config->start - config->beam_time;
    if (t >= 0 && t < config->bolt_time + config->flare_time) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 32, 128, 95, 128, 32, 191, 95, 191);
        if (t < config->bolt_time) {
            red = config->flare_color[0] * t / config->bolt_time;
            green = config->flare_color[1] * t / config->bolt_time;
            blue = config->flare_color[2] * t / config->bolt_time;
            value = (s32)((u32)t << 11) / config->bolt_time + 0x800 + work->frame % 2 * 256;
            value = value < 0x800 ? 0x800 : value > 0x1000 ? 0x1000 : value;
            position.vx = u.middle.vx;
            position.vy = u.middle.vy;
            position.vz = u.middle.vz;
        } else {
            red = config->flare_color[0] * (config->flare_time - t + config->bolt_time) / config->flare_time;
            green = config->flare_color[1] * (config->flare_time - t + config->bolt_time) / config->flare_time;
            blue = config->flare_color[2] * (config->flare_time - t + config->bolt_time) / config->flare_time;
            value = work->frame % 2 * 256 + 0x1000;
            position.vx = to.vx;
            position.vy = to.vy;
            position.vz = to.vz;
        }
        setRGB0(&quad, red, green, blue);
        setVector(&scale, value, value, value);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        q = work->flare_quads;
        for (i = 0; i < 4; i++) {
            depth = RotAverage4(q[0], q[1], q[2], q[3], (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            q += 4;
        }
    }
    t = work->elapsed - config->ribbon_start;
    if (t >= 0 && t < config->flash_time) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash_time - t) * 255 / config->flash_time;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    t = work->elapsed - config->ribbon_start - config->ribbon_time;
    if (t >= 0 && t < config->flash_time) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash_time - t) * 255 / config->flash_time;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    quad.tpage = work->texture[5] >> 16;
    quad.clut = work->texture[5];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 192, 128, 255, 128, 192, 255, 255, 255);
    glow.tpage = work->texture[1] >> 16;
    glow.clut = work->texture[1];
    SetSemiTrans(&glow, 1);
    setUV4(&glow, 32, 128, 95, 128, 32, 191, 95, 191);
    t = work->elapsed - config->ribbon_start;
    if (t >= 0 && t < config->ribbon_time) {
        setRGB0(&quad, config->beam_color[0], config->beam_color[1], config->beam_color[2]);
        setRGB0(&glow, config->flare_color[0], config->flare_color[1], config->flare_color[2]);
        setVector(&corner[0], 0, -config->ribbon_width, 0);
        setVector(&corner[1], 0, config->ribbon_width, 0);
        setVector(&corner[2], 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&u.middle);
        /* c is reused as the ribbon's strip state (-1 start, 1/0 alternate). */
        for (j = 1; j < 4; j++) {
            c = -1;
            depth = 0;
            side = to.vz;
            other = t + 16 / j;
            for (i = 0; i < config->ribbon_length; i++) {
                point = &work->spiral[(other - i) % 16];
                if (t - i >= 0 && t - i < config->ribbon_time) {
                    position.vx = to.vx;
                    position.vy = to.vy;
                    position.vz = to.vz;
                    position.vx *= -1;
                    position.vy *= -1;
                    position.vx += u.middle.vx;
                    position.vy += u.middle.vy;
                    position.vz += u.middle.vz;
                    position.vx = position.vx * t / config->ribbon_time - work->spiral[other % 16].vx;
                    position.vy = position.vy * t / config->ribbon_time - work->spiral[other % 16].vy;
                    position.vx += to.vx;
                    position.vz = side;
                    position.vy += to.vy;
                    position.vx += point->vx;
                    position.vy += point->vy;
                    position.vz += point->vz;
                    side += (u.middle.vz - side) / (config->ribbon_length - i) * step;
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    MulMatrix2(&base, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    if (c == -1) {
                        depth = RotTransPers(&corner[2], (PSXLONG *)&quad.x0, &interpolation, &flag);
                        depth += RotTransPers(&corner[2], (PSXLONG *)&quad.x1, &interpolation, &flag);
                        c = 1;
                    } else {
                        if (c == 0) {
                            depth *= 2;
                            depth += RotTransPers(&corner[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
                            depth += RotTransPers(&corner[1], (PSXLONG *)&quad.x1, &interpolation, &flag);
                            depth /= 4;
                            c = 1;
                        } else {
                            depth *= 2;
                            depth += RotTransPers(&corner[0], (PSXLONG *)&quad.x2, &interpolation, &flag);
                            depth += RotTransPers(&corner[1], (PSXLONG *)&quad.x3, &interpolation, &flag);
                            depth /= 4;
                            c = 0;
                        }
                        if (flag >= 0) {
                            GsSortPoly(&quad, ot, depth);
                        }
                    }
                }
            }
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            q = work->flare_quads;
            for (i = 0; i < 4; i++) {
                depth = RotAverage4(q[0], q[1], q[2], q[3], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2,
                    (PSXLONG *)&glow.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&glow, ot, depth);
                }
                q += 4;
            }
        }
    }
    t = work->elapsed - config->ribbon_start;
    if (t >= 0 && t < config->ribbon_time) {
        quad.tpage = work->texture[5] >> 16;
        quad.clut = work->texture[5];
        SetSemiTrans(&quad, 1);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&position);
        position.vy = -350;
        setVector(&scale, 4096, 4096, 4096);
        value = csin((t << 13) / config->ribbon_time) / 8;
        side = t * 150 % 4096;
        rotation.vx = value;
        rotation.vz = 0;
        rotation.vy = side;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->cylinder, u.screen, depths, projection, flags, 34);
        for (i = 0; i < 16; i++) {
            c = i + 17;
            setXY4(&quad, u.screen[i].vx, u.screen[i].vy, u.screen[c].vx, u.screen[c].vy,
                u.screen[i + 1].vx, u.screen[i + 1].vy, u.screen[c + 1].vx, u.screen[c + 1].vy);
            setUV4(&quad, 192, i * 16 % 128 + 128, 255, i * 16 % 128 + 128,
                192, i * 16 % 128 + 143, 255, i * 16 % 128 + 143);
            depth = AverageZ4(depths[i], depths[i + 1], depths[c], depths[c + 1]);
            flag = flags[i] | flags[i + 1] | flags[c] | flags[c + 1];
            if (!(flag & 0x20)) {
                GsSortPoly(&quad, ot, depth);
            }
        }
        value = -(csin((t << 13) / config->ribbon_time) * 512) / 4096;
        rotation.vx = value + 0x800;
        rotation.vz = 0;
        rotation.vy = side;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->cylinder, u.screen, depths, projection, flags, 34);
        for (i = 0; i < 16; i++) {
            c = i + 17;
            setXY4(&quad, u.screen[i].vx, u.screen[i].vy, u.screen[c].vx, u.screen[c].vy,
                u.screen[i + 1].vx, u.screen[i + 1].vy, u.screen[c + 1].vx, u.screen[c + 1].vy);
            setUV4(&quad, 192, i * 16 % 128 + 128, 255, i * 16 % 128 + 128,
                192, i * 16 % 128 + 143, 255, i * 16 % 128 + 143);
            depth = AverageZ4(depths[i], depths[i + 1], depths[c], depths[c + 1]);
            flag = flags[i] | flags[i + 1] | flags[c] | flags[c + 1];
            if (!(flag & 0x20)) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frame++;
    t = work->elapsed - config->start - config->tube_time;
    if (t < 0) {
        return 0;
    }
    t = work->elapsed - config->fade_out;
    if (t >= config->flash_time) {
        return 2;
    }
    if (t >= 0) {
        if (work->completed != 0) {
            return 0;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

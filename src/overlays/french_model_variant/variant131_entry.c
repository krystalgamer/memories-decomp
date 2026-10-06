#include "../../types.h"
#include "variant131_entry.h"

static const VECTOR D_8013C5BC = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model131State *work;
    Model131Config *config;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[28];
    u16 depths[28], projection[28], flags[28];
    PSXLONG flag, interpolation;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j, k, which, strip_length, value, time, distance, step;
    s32 red, green, blue;
    s32 ring_angle;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C5BC;
    work = (Model131State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        work->config = &D_8013C604[command % 100];
        config = work->config;
        point = work->ring;
        for (i = 0; i < 7; i++) {
            ring_angle = (i + 1) * 4096 / 7;
            setVector(point, config->radius * ccos(ring_angle) / 4096,
                config->radius * csin(ring_angle) / 4096, 0);
            point++;
            j = config->radius * ccos(4096 / 14) / ccos(4096 / 7);
            ring_angle = i * 4096 / 7 - 4096 / 14;
            setVector(point, j * ccos(ring_angle) / 4096, j * csin(ring_angle) / 4096, 0);
            point++;
        }
        point = work->particles;
        for (i = 0; i < 32; point++, i++) {
            j = rand() % config->radius;
            k = rand() % 4096;
            point->vx = j * ccos(k) / 4096;
            point->vy = j * csin(k) / 4096;
            point->vz = direction * (rand() % 450);
            point->pad = i % 16;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C5CC[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        work->command_group = command / 100;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < config->delay) {
        PushMatrix();
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&work->origin, matrix.t[0], matrix.t[1], matrix.t[2]);
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        if (time < config->fade) {
            value = time;
        } else if (time >= config->duration - config->fade) {
            value = config->duration - time;
        } else {
            value = config->fade;
        }
        red = config->beam_r * value / config->fade;
        green = config->beam_g * value / config->fade;
        blue = config->beam_b * value / config->fade;
        setRGB0(&quad, red, green, blue);
        if (time >= config->duration - config->fade) {
            value = 2 * config->fade + time - config->duration;
        }
        j = value * 4096 / config->fade;
        setVector(&scale, j, j, j);
        setVector(&rotation, 0, 0, direction * time * config->spin % 4096);
        copyVector(&position, &work->origin);
        position.vy = -350;
        if (time >= config->duration - config->fade) {
            value = config->fade;
            position.vz += (direction * 900 - position.vz)
                * (time - config->duration + value) / value;
        }
        which = 0;
        strip_length = config->radius * csin(4096 / 14) / ccos(4096 / 7)
            + config->radius * (csin(4096 / 14) << 1) / 4096;
        value = position.vz + (direction * 900 - position.vz) * value / config->fade;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 14);
        distance = direction * 900 - position.vz;
        j = (direction * distance * time / config->fade) % strip_length * 127 / strip_length;
        setUV4(&quad, 0, 127 - j, 127, 127 - j, 0, 127, 127, 127);
        position.vz += (distance * time / config->fade) % strip_length;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, &screen[14], &depths[14], &projection[14], &flags[14], 14);
        for (i = 0; i < 7; i++) {
            setXY4(&quad, screen[i * 2].vx, screen[i * 2].vy,
                screen[i * 2 + 1].vx, screen[i * 2 + 1].vy,
                screen[i * 2 + 14].vx, screen[i * 2 + 14].vy,
                screen[i * 2 + 15].vx, screen[i * 2 + 15].vy);
            depth = AverageZ4(depths[i * 2], depths[i * 2 + 1],
                depths[i * 2 + 14], depths[i * 2 + 15]);
            flag = (flags[i * 2] | flags[i * 2 + 1]
                | flags[i * 2 + 14] | flags[i * 2 + 15]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        copyVector(&vertices[0], &position);
        while (direction * position.vz <= direction * value) {
            setUV4(&quad, 0, 0, 127, 0, 0, 127, 127, 127);
            if (direction * (position.vz + direction * strip_length) >= direction * value) {
                vertices[0].vz = value;
            } else {
                vertices[0].vz = position.vz + direction * strip_length;
            }
            GsSetLsMatrix(&base);
            RotTrans(&vertices[0], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            position.vz += direction * strip_length;
            RotTransPersN(work->ring, &screen[which * 14], &depths[which * 14],
                &projection[which * 14], &flags[which * 14], 14);
            for (i = 0; i < 7; i++) {
                if (which) {
                    j = i * 2 + 14;
                    k = i * 2;
                } else {
                    j = i * 2;
                    k = i * 2 + 14;
                }
                setXY4(&quad, screen[j].vx, screen[j].vy,
                    screen[j + 1].vx, screen[j + 1].vy,
                    screen[k].vx, screen[k].vy,
                    screen[k + 1].vx, screen[k + 1].vy);
                depth = AverageZ4(depths[j], depths[j + 1], depths[k], depths[k + 1]);
                flag = (flags[j] | flags[j + 1] | flags[k] | flags[k + 1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                which ^= 1;
            }
        }
    }
    j = config->fade * (direction * 450 - work->origin.vz)
        / (direction * 900 - work->origin.vz);
    time = work->elapsed - config->delay - j;
    if (time >= 0 && time < config->duration - j * 2) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setRGB0(&quad, config->particle_r, config->particle_g, config->particle_b);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&vertices[0], -config->particle_half_size, -config->particle_half_size, 0);
        setVector(&vertices[1], config->particle_half_size, -config->particle_half_size, 0);
        setVector(&vertices[2], -config->particle_half_size, config->particle_half_size, 0);
        setVector(&vertices[3], config->particle_half_size, config->particle_half_size, 0);
        point = work->particles;
        for (i = 0; i < 32; point++, i++) {
            if (point->pad != -1) {
                setVector(&position, work->origin.vx, work->origin.vy, direction * 450);
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                setUV4(&quad, point->pad / 2 * 32, 128, point->pad / 2 * 32 + 31, 128,
                    point->pad / 2 * 32, 159, point->pad / 2 * 32 + 31, 159);
                GsSortPoly(&quad, ot, (u16)depth);
                point->pad += Model_GetFrameStep();
                if (point->pad >= 16) {
                    if (time < config->duration - j * 2 - 16) {
                        j = rand() % config->radius;
                        k = rand() % 4096;
                        point->vx = j * ccos(k) / 4096;
                        point->vy = j * csin(k) / 4096;
                        point->vz = direction * (rand() % 450);
                        point->pad = 0;
                    } else {
                        point->pad = -1;
                    }
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    j = config->fade * (direction * 450 - work->origin.vz)
        / (direction * 900 - work->origin.vz);
    time = work->elapsed - config->delay;
    if (time < j) {
        return 0;
    } else if (time >= config->duration) {
        return 2;
    } else if (time < config->duration - j) {
        return 4;
    } else {
        if (work->completed) {
            return 0;
        } else {
            work->completed = 1;
            return 1;
        }
    }
}

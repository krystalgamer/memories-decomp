#include "../../types.h"
#include "variant177_entry.h"
#include "../../game/screen_projection.h"

/* The entry is a vertical relative of the French header-131 beam. A spinning
 * tube of fourteen-point rings grows up from the other slot, scrolls its
 * texture and then rises away, while animated sparks fade in and out in a
 * cylinder around its base. A negative command draws a frame; any other
 * command sets the work area up. */

static const VECTOR D_8013C500 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model177State *work;
    Model177Config *config;
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
    u8 red, green, blue;
    s32 ring_angle;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C500;
    work = (Model177State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        work->config = &D_8013C548[command % 100];
        config = work->config;
        point = work->ring;
        for (i = 0; i < 7; i++) {
            ring_angle = (i + 1) * 4096 / 7;
            setVector(point, config->radius * ccos(ring_angle) / 4096, 0,
                config->radius * csin(ring_angle) / 4096);
            point++;
            j = config->radius * ccos(4096 / 14) / ccos(4096 / 7);
            ring_angle = i * 4096 / 7 - 4096 / 14;
            setVector(point, j * ccos(ring_angle) / 4096, 0, j * csin(ring_angle) / 4096);
            point++;
        }
        point = work->particles;
        for (i = 0; i < 32; point++, i++) {
            j = rand() % config->radius;
            k = rand() % 4096;
            point->vx = j * ccos(k) / 4096;
            point->vy = -(rand() % 350) + 175;
            point->vz = j * csin(k) / 4096;
            point->pad = i % 16;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C510[i]);
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
        GsSetLsMatrix(&base);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->origin);
        work->origin.vy = 0;
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
        j = 4096;
        setVector(&scale, j, j, j);
        setVector(&rotation, 0, direction * time * config->spin % 4096, 0);
        copyVector(&position, &work->origin);
        if (time >= config->duration - config->fade) {
            value = config->fade;
            position.vy = -((time - config->duration + value) * 1000) / value;
        }
        which = 0;
        strip_length = config->radius * csin(4096 / 14) / ccos(4096 / 7)
            + config->radius * (csin(4096 / 14) << 1) / 4096;
        value = position.vy - value * 1000 / config->fade;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 14);
        distance = 1000 - position.vy;
        j = (distance * time / config->fade) % strip_length * 127 / strip_length;
        setUV4(&quad, 0, 127 - j, 127, 127 - j, 0, 127, 127, 127);
        position.vy -= (distance * time / config->fade) % strip_length;
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
        while (position.vy >= value) {
            setUV4(&quad, 0, 0, 127, 0, 0, 127, 127, 127);
            if (value >= position.vy - strip_length) {
                vertices[0].vy = value;
            } else {
                vertices[0].vy = position.vy - strip_length;
            }
            GsSetLsMatrix(&base);
            RotTrans(&vertices[0], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            position.vy -= strip_length;
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
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration - 16) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        if (time < config->fade) {
            value = time;
        } else if (time >= config->duration - config->fade - 16) {
            value = config->duration - time - 16;
        } else {
            value = config->fade;
        }
        red = config->particle_r * value / config->fade;
        green = config->particle_g * value / config->fade;
        blue = config->particle_b * value / config->fade;
        setRGB0(&quad, red, green, blue);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&vertices[0], -config->particle_half_size, -config->particle_half_size, 0);
        setVector(&vertices[1], config->particle_half_size, -config->particle_half_size, 0);
        setVector(&vertices[2], -config->particle_half_size, config->particle_half_size, 0);
        setVector(&vertices[3], config->particle_half_size, config->particle_half_size, 0);
        point = work->particles;
        for (i = 0; i < 32; point++, i++) {
            if (point->pad != -1) {
                setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
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
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                point->pad += Model_GetFrameStep();
                if (point->pad >= 16) {
                    if (time < config->duration - 16) {
                        j = rand() % config->radius;
                        k = rand() % 4096;
                        point->vx = j * ccos(k) / 4096;
                        point->vy = -(rand() % 350);
                        point->vz = j * csin(k) / 4096;
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
    time = work->elapsed - config->delay;
    if (time < 0) {
        return 0;
    } else if (time >= config->duration) {
        return 2;
    } else if (time < config->duration - 16) {
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

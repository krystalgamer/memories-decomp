#include "../../types.h"
#include "variant163_entry.h"

static const VECTOR D_8013BEA8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model163State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR projected[66];
    u16 depths[66], interpolation[66], flags[66];
    PSXLONG flag, parameter;
    s32 direction;
    GsOT *ot;
    Model163Config *config;
    SVECTOR *point;
    SVECTOR *G32 *vertices;
    s32 i, j, time, value, step;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BEA8;
    work = (Model163State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BEF0[command % 100];
        point = work->points;
        for (i = 0; i < 96; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = 0;
        }
        point = &work->points[54];
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; point++, j++) {
                setVector(point, j * config->radius, i * config->radius, 0);
            }
        }
        work->quads[0] = &work->points[54];
        work->quads[1] = &work->points[55];
        work->quads[2] = &work->points[57];
        work->quads[3] = &work->points[58];
        work->quads[4] = &work->points[56];
        work->quads[5] = &work->points[55];
        work->quads[6] = &work->points[59];
        work->quads[7] = &work->points[58];
        work->quads[8] = &work->points[62];
        work->quads[9] = &work->points[61];
        work->quads[10] = &work->points[59];
        work->quads[11] = &work->points[58];
        work->quads[12] = &work->points[60];
        work->quads[13] = &work->points[61];
        work->quads[14] = &work->points[57];
        work->quads[15] = &work->points[58];
        point = &work->points[63];
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->radius * ccos(i << 7) / 4096;
            point->vy = config->radius * csin(i << 7) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 33; point++, i++) {
            value = config->radius + config->thickness;
            point->vx = value * ccos(i << 7) / 4096;
            point->vy = value * csin(i << 7) / 4096;
            point->vz = -direction * config->thickness;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEB8[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    time = work->elapsed - config->delay;
    if (time < 0) {
        point = &work->points[48];
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->first_part + i), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point++;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->second_part + i), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point++;
            copyVector(point, point - 2);
            addVector(point, point - 1);
            applyVector(point, 2, 2, 2, /=);
            point++;
        }
    }
    point = work->points;
    for (i = 0; i < 48; point++, i++) {
        time = work->elapsed - config->delay - i * config->stagger;
        if (time < 0) {
            copyVector(point, &work->points[50]);
            point->pad = ratan2(work->points[53].vz - work->points[50].vz,
                work->points[53].vy - work->points[50].vy) + direction * 1024;
        } else if (time < config->particle_duration) {
            u8 red, green, blue;
            if (time < config->split_time) {
                value = time * 3584 / config->particle_duration + 512;
            } else {
                value = (time << 12) / config->particle_duration + 1536;
            }
            setVector(&scale, value, value, value);
            setVector(&rotation, point->pad, 0, 0);
            red = config->red;
            green = config->green;
            blue = config->blue;
            setRGB0(&quad, red, green, blue);
            copyVector(&position, point);
            if (time < config->split_time) {
                position.vx += work->points[48].vx - work->points[50].vx;
            }
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            vertices = work->quads;
            for (j = 0; j < 4; vertices += 4, j++) {
                depth = RotAverage4(vertices[0], vertices[1], vertices[2], vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            if (time < config->split_time) {
                position.vx += work->points[49].vx - work->points[48].vx;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                vertices = work->quads;
                for (j = 0; j < 4; vertices += 4, j++) {
                    depth = RotAverage4(vertices[0], vertices[1], vertices[2], vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            }
            point->vz = work->points[50].vz +
                (direction * 900 - work->points[50].vz) * time / config->particle_duration;
        }
    }
    time = work->elapsed - config->delay - config->split_time;
    if (time < 0) {
        copyVector(&work->points[129], &work->points[0]);
    } else if (time < config->ring_duration) {
        s32 red, green, blue;
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setUV4(&quad, 64, 0, 95, 0, 64, 127, 95, 127);
        red = config->ring_red * (config->ring_duration - time) / config->ring_duration;
        green = config->ring_green * (config->ring_duration - time) / config->ring_duration;
        blue = config->ring_blue * (config->ring_duration - time) / config->ring_duration;
        setRGB0(&quad, red, green, blue);
        copyVector(&position, &work->points[129]);
        setVector(&rotation, 0, 0, 0);
        value = (time << 13) / config->ring_duration + 4096;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(&work->points[63], projected, depths, interpolation, flags, 66);
        for (i = 0; i < 32; i++) {
            value = i + 33;
            setXY4(&quad, projected[i].vx, projected[i].vy,
                projected[i + 1].vx, projected[i + 1].vy,
                projected[value].vx, projected[value].vy,
                projected[i + 34].vx, projected[i + 34].vy);
            depth = AverageZ4(depths[i], depths[i + 1], depths[value], depths[i + 34]);
            flag = (flags[i] | flags[i + 1] | flags[value] | flags[i + 34]) & 0x20;
            if (depth >= 0 && !flag) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - config->delay - config->split_time;
    if (time >= 0 && time < config->flash_duration) {
        s32 brightness;
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        brightness = 255 * (config->split_time - time) / config->split_time;
        setRGB0(&flash, brightness, brightness, brightness);
        func_8005B260((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->stagger * 48;
    if (time >= config->particle_duration) {
        if (work->completed) {
            return 2;
        } else {
            work->completed++;
            return 1;
        }
    } else {
        time = work->elapsed - config->delay - config->particle_duration;
        return time >= 0 ? 4 : 0;
    }
}

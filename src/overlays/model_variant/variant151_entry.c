#include "../../types.h"
#include "variant151_entry.h"
#include "../../game/screen_projection.h"

/* The entry launches 30 rings one after another from a slot part towards
 * points on a circle in front of the other slot. Each ring grows and turns
 * while it flies, consecutive rings are joined into a textured tube, and a
 * ring still waiting at the part is joined to the previous one by a fan.
 * Sparks brighten around every third point of each flying ring. The flag
 * that says whether the previous ring was projected also holds the spark
 * divisor and texture frame. A negative command draws a frame; any other
 * command sets the work area up and prints its size. */

static const VECTOR gModelVariant151_D0F70 = {4096, 4096, 4096, 0};

s32 ModelVariant151_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 unused;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG interpolation, flag;
    Model151State *work;
    Model151Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, which, k;
    SVECTOR *point, *target;
    s32 j, current, value, t;
    PSXLONG depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant151_D0F70;
    work = (Model151State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant151_D0FC8[command % 100];
        point = work->points;
        for (i = 0; i < 17; i++) {
            point->vx = config->radius * ccos(i << 8) / 4096;
            point->vy = config->radius * csin(i << 8) / 4096;
            point->vz = 0;
            point++;
        }
        point = &work->points[17];
        for (i = 0; i < 30; i++) {
            point->vx = 0;
            point->vy = -350;
            point->vz = direction * 450;
            point->vx += config->spread * ccos(i << 8) / 4096;
            point->vy += config->spread * csin(i << 8) / 4096;
            point->vz += direction * 450;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant151_D0F90[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model151State), sizeof(Model151State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&unused);
    which = 0;
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    SetSemiTrans(&quad, 1);
    value = 0;
    quad.r0 = config->red;
    quad.g0 = config->green;
    quad.b0 = config->blue;
    GsSetLsMatrix(&base);
    GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    origin.vx = matrix.t[0];
    origin.vy = matrix.t[1];
    origin.vz = matrix.t[2];
    point = work->positions;
    target = &work->points[17];
    for (i = 0; i < 30; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t < 0) {
            point->vx = origin.vx;
            point->vy = origin.vy;
            point->vz = origin.vz;
            if (value) {
                GsSetLsMatrix(&base);
                RotTransPers(point, (PSXLONG *)&quad.x0, &interpolation, &flag);
                k = (which ^ 1) * 17;
                quad.x1 = quad.x0;
                quad.y1 = quad.y0;
                for (j = 0; j < 16; j++) {
                    quad.x2 = screen[k + j].vx;
                    quad.y2 = screen[k + j].vy;
                    quad.x3 = screen[k + j + 1].vx;
                    quad.y3 = screen[k + j + 1].vy;
                    setUV4(&quad, 0, j * 16 % 128, 31, j * 16 % 128, 0, j * 16 % 128 + 15, 31, j * 16 % 128 + 15);
                    depth = (depths[k + j] + depths[k + j + 1]) / 8;
                    if (flag >= 0) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
                value = 0;
            }
        } else if (t < config->duration) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            current = (t << 12) / config->duration;
            setVector(&scale, current, current, current);
            current = (t << 10) / config->duration;
            setVector(&rotation, 0, 0, current);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            current = which * 17;
            k = (which ^ 1) * 17;
            RotTransPersN(work->points, &screen[current], &depths[current], &projection[current], &flags[current], 17);
            which ^= 1;
            if (value) {
                for (j = 0; j < 16; j++) {
                    setXY4(&quad, screen[j + k].vx, screen[j + k].vy,
                        screen[j + current].vx, screen[j + current].vy,
                        screen[j + k + 1].vx, screen[j + k + 1].vy,
                        screen[j + current + 1].vx, screen[j + current + 1].vy);
                    setUV4(&quad, 0, j * 16 % 128, 31, j * 16 % 128, 0, j * 16 % 128 + 15, 31, j * 16 % 128 + 15);
                    depth = AverageZ4(depths[j + k], depths[j + current], depths[j + k + 1], depths[j + current + 1]);
                    if (((flags[j + k] | flags[j + current] | flags[j + k + 1] | flags[j + current + 1]) & 0x20) == 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            } else {
                value = 1;
            }
            point->vx += (target->vx - point->vx) / (config->duration - t) * step;
            point->vy += (target->vy - point->vy) / (config->duration - t) * step;
            point->vz += (target->vz - point->vz) / (config->duration - t) * step;
        }
        point++;
        target++;
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
    setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
    setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
    setVector(&vertices[3], config->spark_size, config->spark_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->positions;
    for (i = 0; i < 30; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t >= 0 && t < config->duration) {
            red = config->spark_r * t / config->duration;
            green = config->spark_g * t / config->duration;
            blue = config->spark_b * t / config->duration;
            setRGB0(&quad, red, green, blue);
            target = &work->points[i % 3];
            for (j = i % 3; j < 16; target += 3, j += 3) {
                value = config->duration;
                position.vx = target->vx;
                position.vy = target->vy;
                position.vz = target->vz;
                position.vx = position.vx * t;
                position.vy = position.vy * t;
                position.vz = position.vz * t;
                position.vx = position.vx / value;
                position.vy = position.vy / value;
                position.vz = position.vz / value;
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
                value = t * 8 / config->duration;
                setUV4(&quad, value * 32, 128, value * 32 + 31, 128, value * 32, 159, value * 32 + 31, 159);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
        point++;
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    t = work->elapsed - config->delay - config->duration * 2 / 3;
    if (t < 0) {
        return 0;
    }
    t = work->elapsed - config->delay - config->spacing * 30;
    if (t < 0) {
        return 4;
    } else if (t >= config->duration) {
        return 2;
    } else if (work->completed) {
        return 0;
    } else {
        work->completed++;
        return 1;
    }
}

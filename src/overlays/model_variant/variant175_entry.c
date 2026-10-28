#include "../../types.h"
#include "variant175_entry.h"
#include "../../game/screen_projection.h"

/* Two waves of rings leave one of this slot's parts, swing sideways and fly
 * to the other slot. While a ring spins in and out it warps the screen behind
 * it, and when it arrives it throws a fading burst of lines. A negative
 * command draws a frame; any other command sets the work area up. */

static const VECTOR D_8013CADC = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_GT4 quad;
    POLY_G4 unused_quad;
    GsGLINE line;
    SVECTOR rotation, position;
    VECTOR scale;
    DISPENV display;
    SVECTOR target;
    DVECTOR projected[51];
    u16 depths[51], interpolation[51], flags[51];
    PSXLONG flag, depth;
    Model175State *work;
    Model175Config *config;
    s32 direction;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j, t, step, value;
    s32 inner, swing;
    s32 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CADC;
    work = (Model175State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CAEC[command];
        point = work->ring;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->radius * ccos(i << 8) / 4096;
            point->vy = config->radius * csin(i << 8) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 17; point++, i++) {
            point->vx = (config->radius - config->thickness) * ccos(i << 8) / 4096;
            point->vy = (config->radius - config->thickness) * csin(i << 8) / 4096;
            point->vz = direction * config->near_z / config->duration;
        }
        for (i = 0; i < 17; point++, i++) {
            point->vx = (config->radius - config->thickness * 2) * ccos(i << 8) / 4096;
            point->vy = (config->radius - config->thickness * 2) * csin(i << 8) / 4096;
            point->vz = 0;
        }
        point = work->positions;
        for (i = 0; i < config->count; i++) {
            setVector(point, 0, 0, 0);
        }
        work->completed = 0;
        work->elapsed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyGT4(&quad);
    SetPolyG4(&unused_quad);
    line.attribute = 0x50000000;
    SetSemiTrans(&quad, 1);
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&target);
    target.vy = -350;
    point = work->positions;
    for (i = 0; i < config->count; point++, i++) {
        t = work->elapsed - config->delay - config->interval * i;
        if (t < 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else if (t < config->duration) {
            if ((t >= 0 && t < (s16)(config->duration / 5)) ||
                (t >= config->duration * 3 / 5 && t < config->duration)) {
                setVector(&position, point->vx, point->vy, point->vz);
                if (t < config->duration * 4 / 5) {
                    /* Storing the swing inside the product keeps retail's
                     * order: the sine is scaled after the direction is read. */
                    position.vx = direction * (swing = csin(t * 2560 / config->duration) * 300) / 4096;
                }
                value = csin(t * 2560 / config->duration);
                if (value < 0) {
                    value = -value;
                }
                value = value < 256 ? 256 : value > 4096 ? 4096 : value;
                setVector(&scale, value, value, value);
                if (t < (s16)(config->duration / 5)) {
                    setVector(&rotation, 0, 2560 - t * 2560 / config->duration, 0);
                } else if (t < config->duration * 4 / 5) {
                    setVector(&rotation, 0, t * 2560 / config->duration, 0);
                } else {
                    setVector(&rotation, 0, 0, 0);
                }
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->ring, projected, depths, interpolation, flags, 51);
                setRGB0(&quad, 255, 255, 255);
                setRGB1(&quad, 255, 255, 255);
                setRGB2(&quad, config->red, config->green, config->blue);
                setRGB3(&quad, config->red, config->green, config->blue);
                GetDispEnv(&display);
                for (j = 0; j < 16; j++) {
                    value = j + 17;
                    inner = j + 34;
                    quad.tpage = GetTPage(2, 1,
                        projected[value].vx + display.disp.x,
                        projected[value].vy + display.disp.y);
                    quad.u0 = projected[value].vx % 64;
                    quad.v0 = projected[value].vy % 256;
                    quad.u1 = projected[j + 18].vx % 256;
                    quad.v1 = projected[j + 18].vy % 256;
                    quad.u2 = projected[inner].vx % 64;
                    quad.v2 = projected[inner].vy % 256;
                    quad.u3 = projected[j + 35].vx % 64;
                    quad.v3 = projected[j + 35].vy % 256;
                    setXY4(&quad, projected[j].vx, projected[j].vy,
                        projected[j + 1].vx, projected[j + 1].vy,
                        projected[value].vx, projected[value].vy,
                        projected[j + 18].vx, projected[j + 18].vy);
                    depth = AverageZ4(depths[j], depths[j + 1], depths[value], depths[j + 18]);
                    flag = (flags[j] | flags[j + 1] | flags[value] | flags[j + 18]) & 0x20;
                    if (depth >= 0 && !flag) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
            }
            if (t < config->duration * 4 / 5) {
                point->vx += (target.vx - point->vx) / (config->duration * 4 / 5 - t) * step;
                point->vy += (target.vy - point->vy) / (config->duration * 4 / 5 - t) * step;
                point->vz += (target.vz - point->vz) / (config->duration * 4 / 5 - t) * step;
            } else {
                point->vz += direction * 900 / (config->duration * 4 / 5) * step;
            }
        }
    }
    for (i = 0; i < config->count; point++, i++) {
        t = work->elapsed - config->second_delay - config->interval * i;
        if (t < 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else if (t < config->duration) {
            if ((t >= 0 && t < (s16)(config->duration / 5)) ||
                (t >= config->duration * 3 / 5 && t < config->duration)) {
                setVector(&position, point->vx, point->vy, point->vz);
                if (t < config->duration * 4 / 5) {
                    position.vx = -direction * (swing = csin(t * 2560 / config->duration) * 300) / 4096;
                }
                value = csin(t * 2560 / config->duration);
                if (value < 0) {
                    value = -value;
                }
                setVector(&scale, value, value, value);
                if (t < (s16)(config->duration / 5)) {
                    setVector(&rotation, 0, t * 2560 / config->duration - 2560, 0);
                } else if (t < config->duration * 4 / 5) {
                    setVector(&rotation, 0, -t * 2560 / config->duration, 0);
                } else {
                    setVector(&rotation, 0, 0, 0);
                }
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(work->ring, projected, depths, interpolation, flags, 51);
                setRGB0(&quad, 255, 255, 255);
                setRGB1(&quad, 255, 255, 255);
                setRGB2(&quad, config->red, config->green, config->blue);
                setRGB3(&quad, config->red, config->green, config->blue);
                GetDispEnv(&display);
                for (j = 0; j < 16; j++) {
                    value = j + 17;
                    inner = j + 34;
                    quad.tpage = GetTPage(2, 1,
                        projected[value].vx + display.disp.x,
                        projected[value].vy + display.disp.y);
                    quad.u0 = projected[value].vx % 64;
                    quad.v0 = projected[value].vy % 256;
                    quad.u1 = projected[j + 18].vx % 256;
                    quad.v1 = projected[j + 18].vy % 256;
                    quad.u2 = projected[inner].vx % 64;
                    quad.v2 = projected[inner].vy % 256;
                    quad.u3 = projected[j + 35].vx % 64;
                    quad.v3 = projected[j + 35].vy % 256;
                    setXY4(&quad, projected[j].vx, projected[j].vy,
                        projected[j + 1].vx, projected[j + 1].vy,
                        projected[value].vx, projected[value].vy,
                        projected[j + 18].vx, projected[j + 18].vy);
                    depth = AverageZ4(depths[j], depths[j + 1], depths[value], depths[j + 18]);
                    flag = (flags[j] | flags[j + 1] | flags[value] | flags[j + 18]) & 0x20;
                    if (depth >= 0 && !flag) {
                        GsSortPoly(&quad, ot, depth);
                    }
                }
            }
            if (t < config->duration * 4 / 5) {
                point->vx += (target.vx - point->vx) / (config->duration * 4 / 5 - t) * step;
                point->vy += (target.vy - point->vy) / (config->duration * 4 / 5 - t) * step;
                point->vz += (target.vz - point->vz) / (config->duration * 4 / 5 - t) * step;
            } else {
                point->vz += direction * 900 / (config->duration * 4 / 5) * step;
            }
        }
    }
    setVector(&position, target.vx, target.vy, target.vz);
    for (i = 0; i < config->count * 2; i++) {
        if (i < config->count) {
            t = work->elapsed - config->delay - config->interval * i - config->duration * 4 / 5;
        } else {
            t = work->elapsed - config->second_delay - config->interval * (i % config->count) -
                config->duration * 4 / 5;
        }
        if (t >= 0 && t < config->lifetime) {
            value = (t << 12) / config->lifetime;
            setVector(&rotation, (i * 3 % 7 - 3) * 192, (i * 4 % 7 - 3) * 192, 0);
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, projected, depths, interpolation, flags, 34);
            for (j = 0; j < 16; j++) {
                value = j + 17;
                line.x0 = projected[j].vx;
                line.y0 = projected[j].vy;
                line.x1 = projected[value].vx;
                line.y1 = projected[value].vy;
                depth = AverageZ4(depths[j], depths[value], depths[j], depths[value]);
                flag = (flags[j] | flags[value]) & 0x20;
                red = config->red * (config->lifetime - t) / config->lifetime;
                green = config->green * (config->lifetime - t) / config->lifetime;
                blue = config->blue * (config->lifetime - t) / config->lifetime;
                setRGB0(&line, 0, 0, 0);
                setRGB1(&line, red, green, blue);
                if (depth >= 0 && !flag) {
                    GsSortGLine(&line, ot, depth & 0xffff);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    t = work->elapsed - config->delay - config->duration * 4 / 5;
    if (t < 0) {
        return 0;
    }
    t = work->elapsed - config->second_delay - config->duration - config->count * config->interval;
    if (t < 0) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 2;
}

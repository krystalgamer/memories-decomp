#include "../../types.h"
#include "variant162_entry.h"

const VECTOR D_8013BDAC = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model162State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    GsLINE line;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[64];
    u16 projected_depth[64], projection[64], flags[64];
    SVECTOR vertices[64];
    SVECTOR delta;
    PSXLONG flag, interpolation;
    GsOT *ot;
    Model162Config *config;
    SVECTOR *point, *spiral;
    s32 i, j, time, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BDAC;
    work = (Model162State *)context;
    Model_GetActiveSlotIndex();
    {
        s32 step = Model_GetFrameStep();
        if (command >= 0) {
            config = work->config = &D_8013BDD8[command];
            point = work->points;
            for (i = 0; i < 256; point++, i++) {
                value = config->radius * i / 256;
                point->vx = value * ccos(i * 64) / 4096;
                point->vy = value * csin(i * 64) / 4096;
                point->vz = 0;
            }
            for (i = 0; i < 1; i++) {
                work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BDBC[i]);
            }
            work->elapsed = 0;
            work->completed = 0;
            return 0;
        }
        config = work->config;
        ot = func_80058F10();
        PushMatrix();
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        SetPolyFT4(&quad);
        SetPolyF4(&flash);
        line.attribute = 0x50000000;
        time = work->elapsed - config->line_delay;
        if (time >= 0 && time < config->line_duration) {
            line.r = config->r * (config->line_duration - time) / config->line_duration;
            line.g = config->g * (config->line_duration - time) / config->line_duration;
            line.b = config->b * (config->line_duration - time) / config->line_duration;
            point = vertices;
            GsSetLsMatrix(&base);
            i = 0;
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part - 1), &matrix);
            copyVector(&delta, &position);
            applyVector(&delta, -1, -1, -1, *=);
            applyVector(&delta, matrix.t[0], matrix.t[1], matrix.t[2], +=);
            applyVector(&delta, 2, 2, 2, *=);
            value = ratan2(-delta.vy, delta.vz) + Model_GetActiveSlotIndex() * 2048;
            setVector(&rotation, value, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            spiral = work->points;
            value = (time << 8) / config->line_duration;
            for (j = 0; i < 64; j += value, point++, i++) {
                point->vx = delta.vx * time / config->line_duration * i / 64;
                point->vy = delta.vy * time / config->line_duration * i / 64;
                point->vz = delta.vz * time / config->line_duration * i / 64;
                addVector(point, &spiral[j / 64]);
            }
            RotTransPersN(vertices, screen, projected_depth, projection, flags, 64);
            for (i = 0; i < 63; i++) {
                setXY2(&line, screen[i].vx, screen[i].vy, screen[i + 1].vx, screen[i + 1].vy);
                depth = projected_depth[i] / 4;
                flag = flags[i] & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortLine(&line, ot, depth);
                }
            }
        }
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 0, 31, 0, 0, 127, 31, 127);
        time = work->elapsed - config->quad_delay;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&work->anchor, matrix.t[0], matrix.t[1], matrix.t[2]);
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part - 1), &matrix);
            copyVector(&delta, &work->anchor);
            applyVector(&delta, -1, -1, -1, *=);
            applyVector(&delta, matrix.t[0], matrix.t[1], matrix.t[2], +=);
            applyVector(&delta, 2, 2, 2, *=);
            value = ratan2(-delta.vy, delta.vz) + Model_GetActiveSlotIndex() * 2048;
            setVector(&work->rotation, value, 0, 0);
        } else if (time < config->quad_duration) {
            s32 red, green, blue, remaining;
            setVector(&vertices[0], -config->half_width / 2, -config->half_width * 2, 0);
            setVector(&vertices[1], config->half_width / 2, -config->half_width * 2, 0);
            setVector(&vertices[2], -config->half_width / 2, config->half_width * 2, 0);
            setVector(&vertices[3], config->half_width / 2, config->half_width * 2, 0);
            remaining = config->quad_duration - time;
            red = config->quad_r * remaining / config->quad_duration;
            green = config->quad_g * remaining / config->quad_duration;
            blue = config->quad_b * remaining / config->quad_duration;
            setRGB0(&quad, red, green, blue);
            value = (time << 8) / config->quad_duration;
            point = work->points;
            for (i = 0; i < value; point += 2, i += 2) {
                setVector(&scale, 4096, 4096, 4096);
                RotTransSV(point, &position, &flag);
                copyVector(&position, point);
                applyVector(&position, 2, 2, 2, *=);
                addVector(&position, &work->anchor);
                copyVector(&rotation, &work->rotation);
                rotation.vz = i * 64;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
        time = work->elapsed - config->quad_delay;
        if (time >= 0 && time < config->quad_duration) {
            s32 brightness;
            setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
            brightness = 255 * (config->quad_duration - time) / config->quad_duration;
            setRGB0(&flash, brightness, brightness, brightness);
            func_8005B260((u32 *)&flash, ot, 1, 1);
        }
        PopMatrix();
        work->elapsed += step;
        time = work->elapsed - config->quad_delay;
        if (time < 0) {
            return 0;
        }
        if (time < config->quad_duration) {
            return 4;
        }
        {
            u8 result;
            if (work->completed != 0) {
                result = 2;
            } else {
                work->completed++;
                result = 1;
            }
            return result;
        }
    }
}

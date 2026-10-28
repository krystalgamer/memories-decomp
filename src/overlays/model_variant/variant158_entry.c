#include "../../types.h"
#include "variant158_entry.h"
#include "../../game/screen_projection.h"

/* Particles leave one of this slot's parts on a rising arc, fly to random
 * targets on a sphere around the other slot and stretch upwards as they fade
 * there; each target then throws a growing, pulsing burst. A negative command
 * draws a frame; any other command sets the work area up and prints its
 * size. */

static const VECTOR D_8013C11C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model158State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flat;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[6];
    PSXLONG flag, interpolation;
    Model158Config *config;
    GsOT *ot;
    SVECTOR *point, *target;
    s32 step, i, j, k, amount, time, value;
    s32 green_half, blue;
    s32 red, green;
    s32 fade_red, fade_green, fade_blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C11C;
    work = (Model158State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->targets;
        work->config = &D_8013C174[command % 100];
        config = work->config;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        vertices[0].vy = -350;
        for (i = 0; i < config->count; i++, point++) {
            amount = rand() % 4096;
            value = rand() % 2048;
            k = rand() % config->spread;
            setVector(point, k * csin(amount) / 4096 * csin(value) / 4096,
                k * ccos(value) / 4096, k * ccos(amount) / 4096 * csin(value) / 4096);
            point->vx += vertices[0].vx;
            point->vy += vertices[0].vy;
            point->vz += vertices[0].vz;
        }
        for (i = 0; i < 2; i++) {
            j = Model_GetActiveSlotIndex();
            work->texture[i] = ModelTexture_PackPageClutCopy(j, 1, &D_8013C13C[i]);
        }
        work->elapsed = 0;
        work->updates = 0;
        work->completed = 0;
        printf("%d (0x%x) Byte\n", sizeof(Model158State), sizeof(Model158State));
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    target = work->targets;
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flat);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; point++, target++, i++) {
        time = work->elapsed - config->delay - i * config->interval;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->pad = 0;
        } else if (time < config->travel_duration) {
            copyVector(&position, point);
            position.vy += config->lift * time / config->travel_duration * csin(point->pad) / 4096;
            setVector(&scale, 4096, 4096, 4096);
            if (time < config->travel_duration / 3 || !((i + work->updates) & 1)) {
                red = 0;
                green = 0;
                blue = 0;
            } else {
                red = config->red * time / config->travel_duration;
                j = config->green * time / config->travel_duration;
                blue = config->blue * time / config->travel_duration;
                green = j;
            }
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, i % 3 * 32, (i % 9) / 3 * 32,
                i % 3 * 32 + 31, (i % 9) / 3 * 32,
                i % 3 * 32, (i % 9) / 3 * 32 + 31,
                i % 3 * 32 + 31, (i % 9) / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (target->vx - point->vx) / (config->travel_duration - time) * step;
            point->vy += (target->vy - point->vy) / (config->travel_duration - time) * step;
            point->vz += (target->vz - point->vz) / (config->travel_duration - time) * step;
            point->pad += (0x2000 - point->pad) / (config->travel_duration - time) * step;
        } else if (time < config->travel_duration + config->fade_duration) {
            k = time - config->travel_duration;
            copyVector(&position, target);
            amount = k * 0x3000 / config->fade_duration + 4096;
            setVector(&scale, 4096, amount, 4096);
            amount = config->fade_duration - k;
            fade_red = config->red * amount / config->fade_duration;
            fade_green = config->green * amount / config->fade_duration;
            fade_blue = config->blue * amount / config->fade_duration;
            setRGB0(&quad, fade_red, fade_green, fade_blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, i % 3 * 32, (i % 9) / 3 * 32,
                i % 3 * 32 + 31, (i % 9) / 3 * 32,
                i % 3 * 32, (i % 9) / 3 * 32 + 31,
                i % 3 * 32 + 31, (i % 9) / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    setVector(&rotation, 0, 0, 0);
    point = work->bursts;
    target = work->targets;
    for (i = 0; i < config->count; point++, target++, i++) {
        time = work->elapsed - config->delay - i * config->interval - config->travel_duration;
        if (time < 0) {
            copyVector(point, target);
        } else if (time < config->burst_duration && !((work->updates + i) & 1)) {
            copyVector(&position, point);
            amount = (time << 13) / config->burst_duration + 4096;
            setVector(&scale, amount, amount, amount);
            if (time < config->burst_duration / 2) {
                red = config->burst_red * time / config->burst_duration;
                green_half = config->burst_green * time / config->burst_duration;
                blue = config->burst_blue * time / config->burst_duration;
            } else {
                amount = config->burst_duration - time;
                red = config->burst_red * amount / config->burst_duration;
                green_half = config->burst_green * amount / config->burst_duration;
                blue = config->burst_blue * amount / config->burst_duration;
            }
            red *= 2;
            green = green_half * 2;
            blue *= 2;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
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
    PopMatrix();
    work->elapsed += step;
    work->updates++;
    time = work->elapsed - config->delay - config->travel_duration;
    if (time < 0) {
        return 0;
    }
    time -= config->count * config->interval + config->fade_duration;
    if (time > 0) {
        if (work->completed) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

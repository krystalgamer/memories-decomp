#include "../../types.h"
#include "variant113_entry.h"
#include "../../game/screen_projection.h"

/* Four pulsing horizontal 3x3 grids scroll vertically through the other
 * slot's position, a pulsing burst quad flashes there on even frames, and
 * sparks spaced over the effect fly from its centre to random points on a
 * sphere, fading as they travel. A negative command draws a frame; any other
 * command sets the work area up. */

static const VECTOR D_8013C374 = {4096, 4096, 4096, 0};

#define DRAW_QUAD(a, b, c, d) \
    depth = RotAverage4(&vertices[a], &vertices[b], &vertices[c], &vertices[d], \
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, \
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag); \
    if (depth >= 0 && flag >= 0) { \
        GsSortPoly(&quad, ot, (u16)depth); \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model113State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 gradient; /* initialised but never drawn */
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    PSXLONG flag, interpolation;
    GsOT *ot;
    s32 step;
    Model113Config *config;
    s32 i, time, elevation, value, reduction;
    SVECTOR *point;
    u8 red, green, blue;
    u8 fade_red, fade_green, fade_blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C374;
    work = (Model113State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        work->config = &D_8013C3BC[command];
        config = work->config;
        point = work->rings;
        for (i = -4; i < 0; point++, i++) {
            setVector(point, 0, config->ring_span / 4 * i, 0);
        }
        /* Each spark target is a random point on a sphere of the configured
         * radius; value holds its azimuth. */
        point = work->targets;
        for (i = 0; i < config->count; point++, i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            setVector(point, config->radius * ccos(value) / 4096 * csin(elevation) / 4096,
                config->radius * ccos(elevation) / 4096,
                config->radius * csin(value) / 4096 * csin(elevation) / 4096);
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->center);
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C384[i]);
        }
        work->elapsed = 0;
        work->updates = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < config->delay) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&gradient);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&vertices[0], -config->ring_half_size, 0, -config->ring_half_size);
    setVector(&vertices[1], 0, 0, -config->ring_half_size);
    setVector(&vertices[2], config->ring_half_size, 0, -config->ring_half_size);
    setVector(&vertices[3], -config->ring_half_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->ring_half_size, 0, 0);
    setVector(&vertices[6], -config->ring_half_size, 0, config->ring_half_size);
    setVector(&vertices[7], 0, 0, config->ring_half_size);
    setVector(&vertices[8], config->ring_half_size, 0, config->ring_half_size);
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    setVector(&rotation, 0, 0, 0);
    time = work->elapsed - config->delay;
    point = work->rings;
    for (i = 0; i < 4; point++, i++) {
        setVector(&position, work->center.vx, work->center.vy, work->center.vz);
        position.vy = point->vy;
        value = csin((time << 12) / config->duration * 4 + -(position.vy * 2048) / config->ring_span) / 4 + 0xC00;
        setVector(&scale, value, value, value);
        if (time < config->fade_duration) {
            red = config->red * time / config->fade_duration;
            green = config->green * time / config->fade_duration;
            blue = config->blue * time / config->fade_duration;
            setRGB0(&quad, red, green, blue);
        } else if (time >= config->duration - config->fade_duration) {
            red = config->red * (config->duration - time) / config->fade_duration;
            green = config->green * (config->duration - time) / config->fade_duration;
            blue = config->blue * (config->duration - time) / config->fade_duration;
            setRGB0(&quad, red, green, blue);
        } else {
            setRGB0(&quad, config->red, config->green, config->blue);
        }
        if (time >= config->interval * (i + 1)) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            DRAW_QUAD(0, 1, 3, 4);
            DRAW_QUAD(2, 1, 5, 4);
            DRAW_QUAD(6, 7, 3, 4);
            DRAW_QUAD(8, 7, 5, 4);
        }
        /* value becomes the frames per quarter span; it shrinks once the
         * fade-in has ended, so the grids scroll faster and faster. */
        if (time <= config->fade_duration) {
            value = config->interval;
        } else {
            reduction = config->interval *
                csin(((time - config->fade_duration) << 11) / (config->duration - config->fade_duration)) / 4096 - 2;
            value = config->interval - reduction;
        }
        point->vy = (point->vy - config->ring_span / 4 / value * step) % config->ring_span;
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration && !(work->updates & 1)) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
        setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
        setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
        setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
        if (time < config->fade_duration) {
            red = config->spark_red * time / config->fade_duration;
            green = config->spark_green * time / config->fade_duration;
            blue = config->spark_blue * time / config->fade_duration;
            setRGB0(&quad, red, green, blue);
        } else if (time >= config->duration - config->fade_duration) {
            fade_red = config->spark_red * (config->duration - time) / config->fade_duration;
            fade_green = config->spark_green * (config->duration - time) / config->fade_duration;
            fade_blue = config->spark_blue * (config->duration - time) / config->fade_duration;
            setRGB0(&quad, fade_red, fade_green, fade_blue);
        } else {
            setRGB0(&quad, config->spark_red, config->spark_green, config->spark_blue);
        }
        setUV4(&quad, 32, 0, 95, 0, 32, 63, 95, 63);
        setVector(&position, work->center.vx, work->center.vy, work->center.vz);
        value = csin((time << 12) / config->duration * 4 + -work->center.vy * 2048 / config->ring_span) / 4 + 0xC00;
        setVector(&scale, value, value, value);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        DRAW_QUAD(0, 1, 2, 3);
    }
    /* value becomes the launch spacing between consecutive sparks. */
    value = (config->duration - config->fade_duration * 2) / config->count;
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[1], config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[2], -config->spark_half_size, config->spark_half_size, 0);
    setVector(&vertices[3], config->spark_half_size, config->spark_half_size, 0);
    setUV4(&quad, 32, 0, 95, 0, 32, 63, 95, 63);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    point = work->targets;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * value - config->fade_duration;
        if (time >= 0 && time < config->spark_duration && !((work->updates + i) & 1)) {
            setVector(&position, point->vx * time / config->spark_duration,
                point->vy * time / config->spark_duration,
                point->vz * time / config->spark_duration);
            position.vx += work->center.vx;
            position.vy += work->center.vy;
            position.vz += work->center.vz;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            red = config->spark_red * (config->spark_duration - time) / config->spark_duration;
            green = config->spark_green * (config->spark_duration - time) / config->spark_duration;
            blue = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
            setRGB0(&quad, red, green, blue);
            DRAW_QUAD(0, 1, 2, 3);
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->updates++;
    time = work->elapsed - config->delay;
    if (time >= config->interval) {
        if (time >= config->duration - 2) {
            if (work->completed != 0) {
                return 2;
            }
            work->completed = 1;
            return 1;
        }
        return 4;
    }
    return 0;
}

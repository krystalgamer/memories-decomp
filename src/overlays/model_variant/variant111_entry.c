#include "../../types.h"
#include "variant111_entry.h"
#include "../../game/screen_projection.h"

/* Header 111's entry: up to 64 textured quads start at a model part and,
 * spacing frames apart, run three legs of travel_duration frames each, with
 * their own colour fade and scale ramp per leg. */

#define DRAW_QUAD(a, b, c, d) \
    depth = RotAverage4(&vertices[a], &vertices[b], &vertices[c], &vertices[d], \
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, \
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag); \
    if (depth >= 0 && flag >= 0) { \
        GsSortPoly(&quad, ot, (u16)depth); \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model111State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 gradient;
    GsGLINE line; /* never drawn: retail only sets its attribute; the type fits the frame */
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    PSXLONG flag, interpolation;
    Model111Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 i, arrive, travel, leave, value;
    s16 red, green, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BCF8;
    work = (Model111State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BD24[command];
        point = work->positions;
        /* Retail zeroes the first position count times. */
        for (i = 0; i < config->count; i++) {
            setVector(point, 0, 0, 0);
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BD08[i]);
        }
        work->elapsed = -config->delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        point = work->positions;
        PushMatrix();
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        for (i = 0; i < config->count; point++, i++) {
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        PopMatrix();
        return 0;
    }
    point = work->positions;
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&gradient);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    line.attribute = 0x50000000;
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], 0, -config->half_size, 0);
    setVector(&vertices[2], config->half_size, -config->half_size, 0);
    setVector(&vertices[3], -config->half_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->half_size, 0, 0);
    setVector(&vertices[6], -config->half_size, config->half_size, 0);
    setVector(&vertices[7], 0, config->half_size, 0);
    setVector(&vertices[8], config->half_size, config->half_size, 0);
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    for (i = 0; i < config->count; point++, i++) {
        arrive = work->elapsed - config->spacing * i;
        travel = arrive - config->travel_duration -
            config->distance * 2 * config->travel_duration / (450 - config->distance);
        leave = travel - config->travel_duration;
        if (config->travel_duration < arrive && travel < 0) {
            point->vz = direction * config->distance;
        }
        setVector(&position, point->vx, point->vy, point->vz);
        value = 4096;
        if (arrive >= 0 && arrive < config->travel_duration) {
            value = (arrive << 11) / config->travel_duration;
            point->vy += (-350 - point->vy) / (config->travel_duration - arrive);
            point->vz += (-direction * config->distance - point->vz) / (config->travel_duration - arrive);
            red = config->red * (config->travel_duration - arrive) / config->travel_duration;
            green = config->green * (config->travel_duration - arrive) / config->travel_duration;
            blue = config->blue * (config->travel_duration - arrive) / config->travel_duration;
            setRGB0(&quad, red, green, blue);
        }
        if (travel >= 0 && travel < config->travel_duration) {
            value = (travel << 11) / config->travel_duration + 2048;
            point->vx += -point->vx / (config->travel_duration - travel);
            point->vy = -350;
            point->vz += (direction * 450 - point->vz) / (config->travel_duration - travel);
            red = config->red * travel / config->travel_duration;
            green = config->green * travel / config->travel_duration;
            blue = config->blue * travel / config->travel_duration;
            setRGB0(&quad, red, green, blue);
        }
        if (leave >= 0 && leave < config->travel_duration) {
            point->vx += -point->vx / (config->travel_duration - leave);
            point->vy = -350;
            point->vz += (direction * (config->spread + 450) - point->vz) / (config->travel_duration - leave);
            red = config->red * (config->travel_duration - leave) / config->travel_duration / 2;
            green = config->green * (config->travel_duration - leave) / config->travel_duration / 2;
            blue = config->blue * (config->travel_duration - leave) / config->travel_duration / 2;
            value = 4096;
            setRGB0(&quad, red, green, blue);
        }
        if ((arrive >= 0 && arrive < config->travel_duration) ||
            (travel >= 0 && travel < config->travel_duration) ||
            (leave >= 0 && leave < config->travel_duration)) {
            setVector(&scale, value, value, value);
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
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    travel = work->elapsed - config->travel_duration -
        config->distance * 2 * config->travel_duration / (450 - config->distance);
    leave = travel - config->travel_duration - config->spacing * config->count;
    if (travel < config->travel_duration) {
        return 0;
    }
    if (leave >= config->travel_duration) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}

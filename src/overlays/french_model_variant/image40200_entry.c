#include "../../types.h"
#include "image40200_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image40200State *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    POLY_F4 unused;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[6];
    PSXLONG flag, p;
    Image40200Config *config;
    GsOT *ot;
    SVECTOR *point, *target;
    s32 step, i, value, angle, secondary, age;
    u16 red, green, blue;
    s16 third;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C108;
    work = (Image40200State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->targets;
        config = work->config = &D_8013C150[command % 100];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corners[0]);
        corners[0].vy = -350;
        for (i = 0; i < config->count; i++, point++) {
            value = rand() % 4096;
            angle = rand() % 2048;
            secondary = rand() % config->spread;
            setVector(point,
                secondary * csin(value) / 4096 * csin(angle) / 4096,
                secondary * ccos(angle) / 4096,
                secondary * ccos(value) / 4096 * csin(angle) / 4096);
            addVector(point, &corners[0]);
        }
        for (i = 0; i < 2; i++) {
            work->textures[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013C118[i]);
        }
        work->elapsed = 0;
        work->updates = 0;
        work->notified = 0;
        return 0;
    }

    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    target = work->targets;
    SetPolyFT4(&quad);
    SetPolyF4(&unused);
    quad.tpage = work->textures[1] >> 16;
    quad.clut = work->textures[1];
    SetSemiTrans(&quad, 1);
    setVector(&corners[0], -config->travel_extent, -config->travel_extent, 0);
    setVector(&corners[1], config->travel_extent, -config->travel_extent, 0);
    setVector(&corners[2], -config->travel_extent, config->travel_extent, 0);
    setVector(&corners[3], config->travel_extent, config->travel_extent, 0);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; point++, target++, i++) {
        age = work->elapsed - config->delay - i * config->spacing;
        if (age < 0) {
            GsSetLsMatrix(&light);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part),
                       &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->pad = 0;
        } else if (age < config->travel_duration) {
            copyVector(&position, point);
            position.vy += (config->wobble_height * age / config->travel_duration) *
                           csin(point->pad) / 4096;
            setVector(&scale, 4096, 4096, 4096);
            third = config->travel_duration / 3;
            if (age < third || (i + work->updates) % 2 == 0) {
                red = 0;
                green = 0;
                blue = 0;
            } else {
                red = config->travel_rgb[0] * age / config->travel_duration;
                green = config->travel_rgb[1] * age / config->travel_duration;
                blue = config->travel_rgb[2] * age / config->travel_duration;
            }
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &p, &flag);
            setUV4(&quad, i % 3 * 32, (i % 9) / 3 * 32,
                i % 3 * 32 + 31, (i % 9) / 3 * 32,
                i % 3 * 32, (i % 9) / 3 * 32 + 31,
                i % 3 * 32 + 31, (i % 9) / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (target->vx - point->vx) / (config->travel_duration - age) * step;
            point->vy += (target->vy - point->vy) / (config->travel_duration - age) * step;
            point->vz += (target->vz - point->vz) / (config->travel_duration - age) * step;
            point->pad += (8192 - point->pad) / (config->travel_duration - age) * step;
        } else if (age < config->travel_duration + config->hold_duration) {
            secondary = age - config->travel_duration;
            copyVector(&position, target);
            value = secondary * 12288 / config->hold_duration + 4096;
            setVector(&scale, 4096, value, 4096);
            value = config->hold_duration - secondary;
            red = config->travel_rgb[0] * value / config->hold_duration;
            green = config->travel_rgb[1] * value / config->hold_duration;
            blue = config->travel_rgb[2] * value / config->hold_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &p, &flag);
            setUV4(&quad, i % 3 * 32, (i % 9) / 3 * 32,
                i % 3 * 32 + 31, (i % 9) / 3 * 32,
                i % 3 * 32, (i % 9) / 3 * 32 + 31,
                i % 3 * 32 + 31, (i % 9) / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    quad.tpage = work->textures[0] >> 16;
    quad.clut = work->textures[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
    setVector(&corners[0], -config->echo_extent, -config->echo_extent, 0);
    setVector(&corners[1], config->echo_extent, -config->echo_extent, 0);
    setVector(&corners[2], -config->echo_extent, config->echo_extent, 0);
    setVector(&corners[3], config->echo_extent, config->echo_extent, 0);
    setVector(&rotation, 0, 0, 0);
    point = work->echoes;
    target = work->targets;
    for (i = 0; i < config->count; point++, target++, i++) {
        age = work->elapsed - config->delay - i * config->spacing - config->travel_duration;
        if (age < 0) {
            copyVector(point, target);
        } else if (age < config->echo_duration && (work->updates + i) % 2 == 0) {
            s32 echo_red, echo_green, echo_blue;

            copyVector(&position, point);
            value = age * 8192 / config->echo_duration + 4096;
            setVector(&scale, value, value, value);
            value = config->echo_duration - age;
            if (age < config->echo_duration / 2) {
                echo_red = config->echo_rgb[0] * age / config->echo_duration;
                echo_green = config->echo_rgb[1] * age / config->echo_duration;
                echo_blue = config->echo_rgb[2] * age / config->echo_duration;
            } else {
                echo_red = config->echo_rgb[0] * value / config->echo_duration;
                echo_green = config->echo_rgb[1] * value / config->echo_duration;
                echo_blue = config->echo_rgb[2] * value / config->echo_duration;
            }
            red = echo_red * 2;
            green = echo_green * 2;
            blue = echo_blue * 2;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &p, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->updates++;
    age = work->elapsed - config->delay - config->travel_duration;
    if (age < 0) {
        return 0;
    }
    age -= config->count * config->spacing + config->hold_duration;
    if (age <= 0) {
        return 4;
    }
    if (!work->notified) {
        work->notified++;
        return 1;
    }
    return 2;
}

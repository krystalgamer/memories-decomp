#include "../../types.h"
#include "image40200_entry.h"

s32 FrenchModelVariant175_HandleRequest(ModelVariantTravelBurstState *context, s32 command)
{
    ModelVariantTravelBurstState *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    POLY_F4 unused;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[6];
    long flag, p;
    ModelVariantTravelBurstConfig *config;
    GsOT *ot;
    SVECTOR *point, *target;
    s32 step, i, value, angle, secondary, age;
    u16 red, green, blue;
    s16 third;
    long depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gFrenchModelVariant175_UnitScale;
    work = context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->targets;
        config = work->config = &gFrenchModelVariant175_aConfigs[command % 100];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, &corners[0]);
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
            work->texture[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant175_aImages[i]);
        }
        work->elapsed = 0;
        work->updates = 0;
        work->completed = 0;
        return 0;
    }

    config = work->config;
    point = work->positions;
    ot = Graphics_GetDefaultOrderingTable();
    light = *Model_GetLightSourceMatrix();
    PushMatrix();
    target = work->targets;
    SetPolyFT4(&quad);
    SetPolyF4(&unused);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    SetSemiTrans(&quad, 1);
    setVector(&corners[0], -config->half_size, -config->half_size, 0);
    setVector(&corners[1], config->half_size, -config->half_size, 0);
    setVector(&corners[2], -config->half_size, config->half_size, 0);
    setVector(&corners[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; point++, target++, i++) {
        age = work->elapsed - config->delay - i * config->interval;
        if (age < 0) {
            GsSetLsMatrix(&light);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part),
                       &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->pad = 0;
        } else if (age < config->travel_duration) {
            copyVector(&position, point);
            position.vy += (config->lift * age / config->travel_duration) *
                           csin(point->pad) / 4096;
            setVector(&scale, 4096, 4096, 4096);
            third = config->travel_duration / 3;
            if (age < third || (i + work->updates) % 2 == 0) {
                red = 0;
                green = 0;
                blue = 0;
            } else {
                red = config->red * age / config->travel_duration;
                green = config->green * age / config->travel_duration;
                blue = config->blue * age / config->travel_duration;
            }
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &p, &flag);
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
        } else if (age < config->travel_duration + config->fade_duration) {
            secondary = age - config->travel_duration;
            copyVector(&position, target);
            value = secondary * 12288 / config->fade_duration + 4096;
            setVector(&scale, 4096, value, 4096);
            value = config->fade_duration - secondary;
            red = config->red * value / config->fade_duration;
            green = config->green * value / config->fade_duration;
            blue = config->blue * value / config->fade_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &p, &flag);
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
    setVector(&corners[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&corners[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&corners[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&corners[3], config->burst_half_size, config->burst_half_size, 0);
    setVector(&rotation, 0, 0, 0);
    point = work->bursts;
    target = work->targets;
    for (i = 0; i < config->count; point++, target++, i++) {
        age = work->elapsed - config->delay - i * config->interval - config->travel_duration;
        if (age < 0) {
            copyVector(point, target);
        } else if (age < config->burst_duration && (work->updates + i) % 2 == 0) {
            s32 echo_red, echo_green, echo_blue;

            copyVector(&position, point);
            value = age * 8192 / config->burst_duration + 4096;
            setVector(&scale, value, value, value);
            value = config->burst_duration - age;
            if (age < config->burst_duration / 2) {
                echo_red = config->burst_red * age / config->burst_duration;
                echo_green = config->burst_green * age / config->burst_duration;
                echo_blue = config->burst_blue * age / config->burst_duration;
            } else {
                echo_red = config->burst_red * value / config->burst_duration;
                echo_green = config->burst_green * value / config->burst_duration;
                echo_blue = config->burst_blue * value / config->burst_duration;
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
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &p, &flag);
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
    age -= config->count * config->interval + config->fade_duration;
    if (age <= 0) {
        return 4;
    }
    if (!work->completed) {
        work->completed++;
        return 1;
    }
    return 2;
}

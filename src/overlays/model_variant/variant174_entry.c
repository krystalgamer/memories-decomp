#include "../../types.h"
#include "variant174_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry is the French header-177 entry with sparks added: a spinning grid
 * glows at one of the slot's parts while ribbons leave it one after another,
 * fly towards the other slot and fade, a screen flash fades out, and each
 * ribbon then throws out a ring of tumbling sparks. A negative command draws
 * a frame; any other command sets the work area up and prints its size. */

static const VECTOR gModelVariant174_D17AC = {4096, 4096, 4096, 0};

s32 ModelVariant174_HandleRequest(u8 *context, s32 command)
{
    Model174State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[2][17];
    u16 depths[2][17], projection[2][17], flags[2][17];
    PSXLONG flag, interpolation;
    Model174Config *config;
    GsOT *ot;
    s32 step;
    s32 i;
    SVECTOR *point, *origin;
    SVECTOR *G32 *face;
    u8 red, green, blue;
    DVECTOR *screen0, *screen1;
    u16 *depth0, *depth1, *flags0, *flags1;
    s32 j, time, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant174_D17AC;
    work = (Model174State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant174_D183C[command % 100];
        point = work->ribbon;
        origin = work->ribbon + 17;
        for (i = 0; i < 17; point++, origin++, i++) {
            point->vx = -direction * (config->half_width * (i << 1) / 16 - config->half_width);
            point->vy = 0;
            point->vz = direction * (config->depth * csin(i * 128) / 4096 - config->depth);
            setVector(origin, point->vx, point->vy, point->vz);
            origin->vy = -config->height * csin(i * 128) / 4096;
        }
        point = work->vertices;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++, point++) {
                setVector(point, j * config->grid_half_size, i * config->grid_half_size, 0);
            }
        }
        face = work->faces;
        point = work->vertices;
        face[0] = point;
        face[1] = point + 1;
        face[2] = point + 3;
        face[3] = point + 4;
        face[4] = point + 2;
        face[5] = point + 5;
        face[6] = point + 1;
        face[7] = point + 4;
        face[8] = point + 8;
        face[9] = point + 7;
        face[10] = point + 5;
        face[11] = point + 4;
        face[12] = point + 6;
        face[13] = point + 3;
        face[14] = point + 7;
        face[15] = point + 4;
        point = work->spark_points;
        for (i = 0; i < config->count * config->sparks; point++, i++) {
            point->vx = rand() % config->spark_spread * 2 - config->spark_spread;
            point->vy = rand() % config->spark_spread * 2 - config->spark_spread;
            point->vz = rand() % config->spark_spread * 2 - config->spark_spread;
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant174_D17CC[i]);
        }
        work->elapsed = 0;
        work->animation = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model174State), sizeof(Model174State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->count * config->spacing + config->fade_duration) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, work->animation % 2 * 64, 64,
               work->animation % 2 * 64 + 63, 64,
               work->animation % 2 * 64, 127,
               work->animation % 2 * 64 + 63, 127);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        value = work->animation % 2 * 256 + 4096;
        setVector(&rotation, 0, 0, time * 32);
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        value = config->count * config->spacing + config->fade_duration - time;
        if (value >= 0 && value < config->fade_duration) {
            red = config->grid_red * value / config->fade_duration;
            green = config->grid_green * value / config->fade_duration;
            blue = config->grid_blue * value / config->fade_duration;
        } else {
            red = config->grid_red;
            green = config->grid_green;
            blue = config->grid_blue;
        }
        setRGB0(&quad, red, green, blue);
        face = work->faces;
        for (i = 0; i < 4; i++, face += 4) {
            depth = RotAverage4(face[0], face[1], face[2], face[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 0, 0, 31, 0, 0, 127, 31, 127);
    point = work->positions;
    origin = &work->origin;
    red = config->red;
    green = config->green;
    blue = config->blue;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point->pad = 0;
            setVector(origin, point->vx, point->vy, point->vz);
        } else if (time < config->travel_duration + config->fade_duration) {
            if (time < config->travel_duration) {
                red = config->red * time / config->travel_duration;
                green = config->green * time / config->travel_duration;
                blue = config->blue * time / config->travel_duration;
            } else if (time < config->travel_duration + config->fade_duration) {
                value = config->travel_duration + config->fade_duration - time;
                red = config->red * value / config->fade_duration;
                green = config->green * value / config->fade_duration;
                blue = config->blue * value / config->fade_duration;
            }
            setRGB0(&quad, red, green, blue);
            value = time * 4096 / config->travel_duration;
            setVector(&scale, value, value, value);
            setVector(&position, point->vx, point->vy, point->vz);
            setVector(&rotation, 0, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ribbon, screen[0], depths[0], projection[0], flags[0], 34);
            screen0 = screen[0];
            screen1 = screen[1];
            depth0 = depths[0];
            depth1 = depths[1];
            flags0 = flags[0];
            flags1 = flags[1];
            for (j = 0; j < 16; screen0++, screen1++, depth0++, depth1++, flags0++, flags1++, j++) {
                setXY4(&quad, screen1[0].vx, screen1[0].vy,
                       screen1[1].vx, screen1[1].vy,
                       screen0[0].vx, screen0[0].vy, screen0[1].vx, screen0[1].vy);
                setUV4(&quad, j * 16, 0, j * 16 + 16, 0,
                       j * 16, 31, j * 16 + 16, 31);
                depth = AverageZ4(depth0[0], depth0[1], depth1[0], depth1[1]);
                flag = (flags0[0] | flags0[1] | flags1[0] | flags1[1]) & 0x20;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            if (time < config->travel_duration) {
                point->vx += -point->vx / (config->travel_duration - time) * step;
                point->vy += -point->vy / (config->travel_duration - time) * step;
                point->vz += (direction * 450 - point->vz) / (config->travel_duration - time) * step;
            } else if (time < config->travel_duration + config->fade_duration) {
                point->vx += -origin->vx / config->travel_duration * step;
                point->vy += -origin->vy / config->travel_duration * step;
                point->vz += (direction * 450 - origin->vz) / config->travel_duration * step;
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = config->flash_red * (config->flash_duration - time) / config->flash_duration;
        green = config->flash_green * (config->flash_duration - time) / config->flash_duration;
        blue = config->flash_blue * (config->flash_duration - time) / config->flash_duration;
        setRGB0(&flash, red, green, blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    quad.tpage = work->texture[3] >> 16;
    quad.clut = work->texture[3];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 0, 128, 31, 128, 0, 159, 31, 159);
    setVector(&vertices[0], -config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[1], config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[2], -config->spark_half_size, config->spark_half_size, 0);
    setVector(&vertices[3], config->spark_half_size, config->spark_half_size, 0);
    point = work->spark_points;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->delay - i * config->spacing - config->travel_duration;
        if (time >= 0 && time < config->spark_duration) {
            red = config->spark_red * (config->spark_duration - time) / config->spark_duration;
            green = config->spark_green * (config->spark_duration - time) / config->spark_duration;
            blue = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
            setRGB0(&quad, red, green, blue);
            value = (time << 11) / config->spark_duration + 4096;
            setVector(&scale, value, value, value);
            for (j = 0; j < config->sparks; point++, j++) {
                setVector(&position, point->vx * time / config->spark_duration,
                    point->vy * time / config->spark_duration - 350,
                    point->vz * time / config->spark_duration + direction * 450);
                setVector(&rotation, ratan2(point->vz, point->vy), 0, time * (8192 / config->spark_duration));
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
        } else {
            point += config->sparks;
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->animation++;
    time = work->elapsed - config->delay - config->travel_duration;
    if (time >= 0) {
        time -= config->count * config->spacing;
        if (time < 0) {
            return 4;
        }
        if (time > config->spark_duration) {
            return 2;
        }
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}

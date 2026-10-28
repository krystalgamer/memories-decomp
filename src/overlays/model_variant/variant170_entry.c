#include "../../types.h"
#include "variant170_entry.h"
#include "../../game/graphics_constants.h"
#include "../../game/screen_projection.h"

/* Three white flashes frame the effect while a glow gathers at one of this
 * slot's parts and leaves a sparkling trail on its way back to this slot; a
 * beam of rings and a band of shaded rings then reach out to the other slot,
 * where sixty sparks burst one after another. A negative command draws a
 * frame; any other command sets the work area up and prints its size. */

static const VECTOR D_8013CFC4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model170State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G4 band;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[2][33];
    u16 depths[2][33], projection[2][33], flags[2][33];
    PSXLONG flag, interpolation;
    Model170Config *config;
    GsOT *ot;
    s32 step;
    s32 i, j, level, column;
    SVECTOR *point, *row;
    s32 time, amount, value;
    u8 red, green, blue;
    DVECTOR *screen0, *screen1;
    u16 *depth0, *depth1, *flags0, *flags1;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CFC4;
    work = (Model170State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013D038[command % 100];
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&work->origin);
        work->origin.vy = -1500;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
        work->target.vy = 0;
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->radius * csin(i << 7) / 4096;
            point->vy = 0;
            point->vz = config->radius * ccos(i << 7) / 4096;
        }
        point = work->bands[0];
        row = work->bands[1];
        for (i = 0; i < 17; point++, row++, i++) {
            point->vx = config->inner_radius * csin(i << 8) / 4096;
            point->vy = config->inner_radius * ccos(i << 8) / 4096;
            point->vz = 0;
            row->vx = config->outer_radius * csin(i << 8) / 4096;
            row->vy = config->outer_radius * ccos(i << 8) / 4096;
            row->vz = 0;
        }
        point = work->trail[0];
        for (i = 0; i < 400; point++, i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
        }
        work->head = 0;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        point = work->sparks;
        vertices[0].vy = -350;
        for (i = 0; i < 60; point++, i++) {
            point->vx = rand() % config->radius * 2 - config->radius;
            point->vy = rand() % config->radius * 2 - config->radius;
            point->vz = rand() % config->radius * 2 - config->radius;
            point->vx += vertices[0].vx;
            point->vy += vertices[0].vy;
            point->vz += vertices[0].vz;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CFE4[i]);
        }
        work->elapsed = 0;
        work->animation = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model170State), sizeof(Model170State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyG4(&band);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash_duration - time) * 255 / config->flash_duration;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->spark_delay - config->spacing * 60 - config->spark_duration;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash_duration - time) * 255 / config->flash_duration;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->delay;
    value = ((s16 *)Model_GetViewMetricsBuffer())[1] - ((Model_GetActiveSlotIndex() ^ 1) << 11);
    depth = work->elapsed - config->spark_delay - config->spacing * 60 - config->spark_duration;
    if (time >= 0 && (value > 512 && value < 1536) && depth < 0) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        /* The third flash brightens with the camera yaw, measured from the
         * other slot's side, between 512 and 1536; the three separate
         * colour assignments keep the retail scheduling. */
        depth = csin((value - 512) * 2);
        if (depth >= 0) {
            if (depth < 4097) {
                level = depth;
            } else {
                level = 4096;
            }
        } else {
            level = 0;
        }
        red = level * 255 / 4096;
        green = level * 255 / 4096;
        blue = level * 255 / 4096;
        setRGB0(&flash, red, green, blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    /* A glow gathers at the slot part and drifts back to this slot,
     * dropping a row of twenty trail points every frame. */
    time = work->elapsed - config->gather_delay;
    if (time >= 0 && time < config->gather_duration + config->trail_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 16, 0, 79, 0, 16, 63, 79, 63);
        SetSemiTrans(&quad, 1);
        setVector(&rotation, 0, 0, 0);
        value = work->animation % 2 * 512 + 4096;
        setVector(&scale, value, value, value);
        if (time < config->gather_duration) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&work->gather, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        setVector(&position, work->gather.vx, work->gather.vy, work->gather.vz);
        if (time >= config->gather_duration && time < config->gather_duration + config->trail_duration) {
            value = time - config->gather_duration;
            position.vx += (work->origin.vx - work->gather.vx) * value / config->trail_duration;
            position.vy += (work->origin.vy - work->gather.vy) * value / config->trail_duration;
            position.vz += (work->origin.vz - work->gather.vz) * value / config->trail_duration;
        }
        point = work->trail[work->head];
        for (i = 0; i < 20; point++, i++) {
            setVector(point, position.vx, position.vy, position.vz);
            point->vx += rand() % config->half_size - config->half_size / 2;
            point->vy += rand() % config->half_size - config->half_size / 2;
            point->vz += rand() % config->half_size - config->half_size / 2;
        }
        work->head = (work->head + 1) % 20;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = config->gather_red;
        green = config->gather_green;
        blue = config->gather_blue;
        if (time < config->gather_duration) {
            red = red * time / config->gather_duration;
            green = green * time / config->gather_duration;
            blue = blue * time / config->gather_duration;
        }
        setRGB0(&quad, red, green, blue);
        setVector(&vertices[0], -config->half_size, -config->half_size, 0);
        setVector(&vertices[1], config->half_size, -config->half_size, 0);
        setVector(&vertices[2], -config->half_size, config->half_size, 0);
        setVector(&vertices[3], config->half_size, config->half_size, 0);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    /* The trail rows sparkle as randomly dropped one-pixel boxes. */
    time = work->elapsed - config->gather_delay - config->gather_duration;
    if (time >= 0 && time < config->trail_duration) {
        GsSetLsMatrix(&base);
        for (i = 0; i < 20; i++) {
            box.r = config->spark_red * i / 20;
            box.g = config->spark_green * i / 20;
            box.b = config->spark_blue * i / 20;
            point = work->trail[(work->head + i) % 20];
            for (j = 0; j < 20; point++, j++) {
                depth = RotTransPers(point, (PSXLONG *)&box.x, &interpolation, &flag);
                if (depth >= 0 && flag >= 0 && rand() % 100 < 40) {
                    GsSortBoxFill(&box, ot, (u16)depth);
                }
            }
        }
    }
    time = work->elapsed - config->delay;
    value = work->elapsed - config->spark_delay - config->spacing * 60 - config->spark_duration;
    if (time >= 0 && value < 0) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        if (time < config->travel_duration) {
            setVector(&vertices[0], work->origin.vx, work->origin.vy, work->origin.vz);
            vertices[0].vx += (work->target.vx - work->origin.vx) * time / config->travel_duration;
            vertices[0].vy += (work->target.vy - work->origin.vy) * time / config->travel_duration;
            vertices[0].vz += (work->target.vz - work->origin.vz) * time / config->travel_duration;
        } else {
            setVector(&vertices[0], work->target.vx, work->target.vy, work->target.vz);
        }
        setVector(&rotation, 0, time * 20 % 4096, 0);
        setVector(&scale, 0, 0, 0);
        setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = config->red;
        green = config->green;
        blue = config->blue;
        setRGB0(&quad, red, green, blue);
        RotTransPersN(work->ring, screen[0], depths[0], projection[0], flags[0], 33);
        /* Eight growing rings joined into a tube reach out to the other
         * slot; the two screen-space rows alternate between iterations. */
        value = 1;
        for (i = 1; i < 9; i++) {
            /* depth doubles as the row offset of the current ring; a
             * variable set only here would be scheduled differently. */
            depth = value * 33;
            amount = (value ^ 1) * 33;
            screen0 = screen[0] + depth;
            screen1 = screen[0] + amount;
            depth0 = depths[0] + depth;
            depth1 = depths[0] + amount;
            flags0 = flags[0] + depth;
            flags1 = flags[0] + amount;
            if (time < config->travel_duration) {
                amount = (work->animation % 2 * 256 + 4096) * i / 8 * time / config->travel_duration;
            } else {
                amount = (work->animation % 2 * 256 + 4096) * i / 8;
            }
            setVector(&scale, amount, amount, amount);
            setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
            position.vx += (vertices[0].vx - work->origin.vx) * i / 8;
            position.vy += (vertices[0].vy - work->origin.vy) * i / 8;
            position.vz += (vertices[0].vz - work->origin.vz) * i / 8;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen0, depth0, projection[0] + depth, flags0, 33);
            setUV4(&quad, 0, 64 - i * 8, 31, 64 - i * 8, 0, 71 - i * 8, 31, 71 - i * 8);
            for (j = 0; j < 32; screen0++, screen1++, depth0++, depth1++, flags0++, flags1++, j++) {
                setXY4(&quad, screen0[0].vx, screen0[0].vy, screen0[1].vx, screen0[1].vy,
                       screen1[0].vx, screen1[0].vy, screen1[1].vx, screen1[1].vy);
                depth = AverageZ4(depth0[0], depth0[1], depth1[0], depth1[1]);
                flag = (flags0[0] | flags0[1] | flags1[0] | flags1[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
            value ^= 1;
        }
    }
    time = work->elapsed - config->delay;
    value = work->elapsed - config->spark_delay - config->spacing * 60 - config->spark_duration;
    if (time >= 0 && value < 0) {
        /* Six shaded double rings follow the beam. */
        SetSemiTrans(&band, 1);
        if (time < config->travel_duration) {
            setVector(&vertices[0], work->origin.vx, work->origin.vy, work->origin.vz);
            vertices[0].vx += (work->target.vx - work->origin.vx) * time / config->travel_duration;
            vertices[0].vy += (work->target.vy - work->origin.vy) * time / config->travel_duration;
            vertices[0].vz += (work->target.vz - work->origin.vz) * time / config->travel_duration;
        } else {
            setVector(&vertices[0], work->target.vx, work->target.vy, work->target.vz);
        }
        setVector(&rotation, 0, 0, 0);
        setUV4(&quad, 0, 0, 31, 0, 0, 63, 31, 63);
        for (i = 0; i < 6; i++) {
            red = config->band_red * (6 - i) / 6;
            green = config->band_green * (6 - i) / 6;
            blue = config->band_blue * (6 - i) / 6;
            setRGB0(&band, 0, 0, 0);
            setRGB1(&band, 0, 0, 0);
            setRGB2(&band, red, green, blue);
            setRGB3(&band, red, green, blue);
            if (time < config->travel_duration) {
                amount = (i + 1) * 4096 / 6 * time / config->travel_duration;
            } else {
                amount = (i + 1) * 4096 / 6;
            }
            setVector(&scale, amount, amount, amount);
            setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
            position.vx += (vertices[0].vx - work->origin.vx) * i / 6;
            position.vy += (vertices[0].vy - work->origin.vy) * i / 6;
            position.vz += (vertices[0].vz - work->origin.vz) * i / 6;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->bands[0], screen[0], depths[0], projection[0], flags[0], 34);
            screen0 = screen[0];
            screen1 = screen[0] + 17;
            depth0 = depths[0];
            depth1 = depths[0] + 17;
            flags0 = flags[0];
            flags1 = flags[0] + 17;
            for (j = 0; j < 16; screen0++, screen1++, depth0++, depth1++, flags0++, flags1++, j++) {
                setXY4(&band, screen0[0].vx, screen0[0].vy, screen0[1].vx, screen0[1].vy,
                       screen1[0].vx, screen1[0].vy, screen1[1].vx, screen1[1].vy);
                depth = AverageZ4(depth0[0], depth0[1], depth1[0], depth1[1]);
                flag = (flags0[0] | flags0[1] | flags1[0] | flags1[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&band, ot, (u16)depth, 1);
                }
            }
        }
    }
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[1], config->spark_half_size, -config->spark_half_size, 0);
    setVector(&vertices[2], -config->spark_half_size, config->spark_half_size, 0);
    setVector(&vertices[3], config->spark_half_size, config->spark_half_size, 0);
    setVector(&rotation, 0, 0, 0);
    /* The sparks burst one after another around the other slot. */
    point = work->sparks;
    for (i = 0; i < 60; point++, i++) {
        time = work->elapsed - config->spark_delay - i * config->spacing;
        if (time >= 0 && time < config->spark_duration) {
            value = (work->animation + i) % 2 * 512 + 4096;
            setVector(&scale, value, value, value);
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            column = time / 2 % 3 * 32;
            setUV4(&quad, column + 160, 0, column + 191, 0, column + 160, 31, column + 191, 31);
            amount = config->spark_duration - time;
            red = config->spark_red * amount / config->spark_duration;
            green = config->spark_green * amount / config->spark_duration;
            blue = config->spark_blue * amount / config->spark_duration;
            setRGB0(&quad, red, green, blue);
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
    work->animation++;
    time = work->elapsed - config->gather_delay - config->gather_duration - config->trail_duration;
    if (time >= 0) {
        time = work->elapsed - config->spark_delay - config->spacing * 60 - config->spark_duration;
        if (time < 0) {
            return 4;
        }
        if (time >= config->flash_duration) {
            return 2;
        }
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}

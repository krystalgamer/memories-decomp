#include "../../types.h"
#include "variant133_entry.h"
#include "../../game/screen_projection.h"

/* Header 133's entry: after the start delay latches a slot part's position, a
 * fan of quads between two rings around the part opens and closes (drawn
 * twice, the second copy flipped), then the screen flashes, eight shells of
 * one-pixel boxes grow from the outer ring points and four sparks per point
 * fly out from them. The return value is 1 once when the shells start, 2 once
 * the sparks end and 0 otherwise. */

static const VECTOR D_8013C854 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[32];
    u16 depths[32], projection[32], flags[32];
    PSXLONG flag, interpolation;
    Model133State *work;
    s32 side;
    GsOT *ot;
    s32 step;
    Model133Config *config;
    /* j doubles as the radius, angle, tilt and scale temporary, as the
     * allocation requires. */
    s32 i, j;
    s32 time, pitch;
    PSXLONG depth;
    SVECTOR *point, *anchor;
    DVECTOR *projected;
    u16 *depth_cursor, *flag_cursor;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C854;
    work = (Model133State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C8B8[command % 100];
        point = work->inner;
        for (i = -4; i < 5; i++) {
            point->vx = config->radius * csin(config->arc * i / 8) / 4096;
            point->vy = -config->height;
            point->vz = side * config->radius * ccos(config->arc * i / 8) / 4096;
            point++;
        }
        j = config->radius + config->ring_radius;
        for (i = -4; i < 5; i++) {
            point->vx = j * csin(config->arc * i / 8) / 4096;
            point->vy = 0;
            point->vz = side * j * ccos(config->arc * i / 8) / 4096;
            point++;
        }
        point = work->dots;
        for (i = 0; i < 32; i++) {
            j = rand() % 4096;
            pitch = rand() % 2048 - 1024;
            point->vx = config->box_radius * ccos(j) / 4096 * csin(pitch) / 4096;
            point->vy = config->box_radius * ccos(pitch + 1024) / 4096;
            point->vz = config->box_radius * csin(j) / 4096 * csin(pitch) / 4096;
            point++;
        }
        point = work->sparks;
        for (i = 0; i < 32; i++) {
            j = rand() % 4096;
            pitch = rand() % 2048 - 1024;
            point->vx = config->spark_radius * ccos(j) / 4096 * csin(pitch) / 4096;
            point->vy = config->spark_radius * ccos(pitch + 1024) / 4096;
            point->vz = side * config->spark_radius * csin(j) / 4096 * csin(pitch) / 4096;
            point++;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C864[i]);
        }
        work->elapsed = 0;
        work->hit = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 31, 0, 0, 127, 31, 127);
    time = work->elapsed - config->delay;
    if (time < 0) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        work->origin.vx = matrix.t[0];
        work->origin.vy = matrix.t[1];
        work->origin.vz = matrix.t[2];
    } else if (time < config->open + config->close) {
        position.vx = work->origin.vx;
        position.vy = work->origin.vy;
        position.vz = work->origin.vz;
        setVector(&scale, 4096, 4096, 4096);
        j = side * config->tilt * (config->open - time) / config->open;
        setVector(&rotation, j, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->inner, screen, depths, projection, flags, 18);
        if (time <= config->open) {
            red = config->fan_red * time / config->open;
            green = config->fan_green * time / config->open;
            blue = config->fan_blue * time / config->open;
        } else {
            red = config->fan_red * (config->open + config->close - time) / config->close;
            green = config->fan_green * (config->open + config->close - time) / config->close;
            blue = config->fan_blue * (config->open + config->close - time) / config->close;
        }
        setRGB0(&quad, red, green, blue);
        for (j = 0; j < 8; j++) {
            quad.x0 = screen[j + 9].vx;
            quad.y0 = screen[j + 9].vy;
            quad.x1 = screen[j + 10].vx;
            quad.y1 = screen[j + 10].vy;
            quad.x2 = screen[j].vx;
            quad.y2 = screen[j].vy;
            quad.x3 = screen[j + 1].vx;
            quad.y3 = screen[j + 1].vy;
            depth = AverageZ4(depths[j], depths[j + 1], depths[j + 9], depths[j + 10]);
            flag = (flags[j] | flags[j + 1] | flags[j + 9] | flags[j + 10]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        j = side * config->tilt * (config->open - time) / config->open;
        setVector(&rotation, j, 0, 2048);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrixZYX(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->inner, screen, depths, projection, flags, 18);
        if (time <= config->open) {
            red = config->fan_red * time / config->open;
            green = config->fan_green * time / config->open;
            blue = config->fan_blue * time / config->open;
        } else {
            red = config->fan_red * (config->open + config->close - time) / config->close;
            green = config->fan_green * (config->open + config->close - time) / config->close;
            blue = config->fan_blue * (config->open + config->close - time) / config->close;
        }
        setRGB0(&quad, red, green, blue);
        for (j = 0; j < 8; j++) {
            quad.x0 = screen[j + 9].vx;
            quad.y0 = screen[j + 9].vy;
            quad.x1 = screen[j + 10].vx;
            quad.y1 = screen[j + 10].vy;
            quad.x2 = screen[j].vx;
            quad.y2 = screen[j].vy;
            quad.x3 = screen[j + 1].vx;
            quad.y3 = screen[j + 1].vy;
            depth = AverageZ4(depths[j], depths[j + 1], depths[j + 9], depths[j + 10]);
            flag = (flags[j] | flags[j + 1] | flags[j + 9] | flags[j + 10]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - config->delay - config->open;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
            GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
        red = config->flash_red * (config->flash_duration - time) / config->flash_duration;
        green = config->flash_green * (config->flash_duration - time) / config->flash_duration;
        blue = config->flash_blue * (config->flash_duration - time) / config->flash_duration;
        setRGB0(&flash, red, green, blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->delay - config->open;
    if (time >= 0 && time < config->close) {
        j = (time << 12) / config->close;
        setVector(&scale, j, j, j);
        box.r = config->box_red * (config->close - time) / config->close;
        box.g = config->box_green * (config->close - time) / config->close;
        box.b = config->box_blue * (config->close - time) / config->close;
        anchor = work->outer;
        for (i = 0; i < 8; i++) {
            setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
            position.vx += anchor->vx;
            position.vy += anchor->vy;
            position.vz += anchor->vz;
            setVector(&rotation, i * 1529, 0, i * 384);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->dots, screen, depths, projection, flags, 32);
            projected = screen;
            depth_cursor = depths;
            flag_cursor = flags;
            for (j = 0; j < 32; j++) {
                box.x = projected->vx;
                box.y = projected->vy;
                depth = *depth_cursor >> 2;
                flag = *flag_cursor & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortBoxFill(&box, ot, depth);
                }
                projected++;
                depth_cursor++;
                flag_cursor++;
            }
            anchor++;
        }
    }
    time = work->elapsed - config->delay - config->open;
    if (time >= 0 && time < config->spark_duration) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setUV4(&quad, 0, 128, 31, 128, 0, 159, 31, 159);
        setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
        setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
        setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
        setVector(&vertices[3], config->spark_size, config->spark_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        red = config->spark_red * (config->spark_duration - time) / config->spark_duration;
        green = config->spark_green * (config->spark_duration - time) / config->spark_duration;
        blue = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
        setRGB0(&quad, red, green, blue);
        point = work->sparks;
        anchor = work->outer;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 4; j++) {
                setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
                position.vx += anchor->vx;
                position.vy += anchor->vy;
                position.vz += anchor->vz;
                position.vx += (point->vx - anchor->vx) * time / config->spark_duration;
                position.vy += (point->vy - anchor->vy) * time / config->spark_duration;
                position.vz -= (point->vz - anchor->vz) * time / config->spark_duration;
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
                point++;
            }
            anchor++;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (work->elapsed >= config->delay + config->open + config->spark_duration) {
        return 2;
    }
    if (work->elapsed >= config->delay + config->open) {
        if (work->hit) {
            return 0;
        }
        work->hit++;
        return 1;
    }
    return 0;
}

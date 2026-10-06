#include "../../types.h"
#include "variant132_entry.h"

static const VECTOR D_8013BEB8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model132State *work;
    MATRIX base, matrix, radial;
    POLY_FT4 quad;
    POLY_F4 flat;
    SVECTOR rotation, position;
    VECTOR scale;
    DISPENV display;
    SVECTOR vertices[4];
    DVECTOR projected[34];
    u16 depths[34], interpolation[34], flags[34];
    PSXLONG flag, parameter;
    GsOT *ot;
    Model132Config *config;
    SVECTOR *point;
    s32 i, j, time, value, step;
    s16 third;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BEB8;
    work = (Model132State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BEE4[command];
        point = work->path;
        for (i = 0; i < 42; point++, i++) {
            point->vx = config->radius;
            point->vy = -config->path_height * i / 128 * 3;
            point->vz = 0;
        }
        for (i = 0; i < 85; point++, i++) {
            value = i * 3072 / 85;
            point->vx = config->radius + config->curve_width / 2
                - config->curve_width / 2 * ccos(value) / 4096;
            point->vy = -config->path_height
                - config->curve_width / 2 * csin(value) / 4096;
            point->vz = 0;
        }
        point = work->ring;
        for (i = 0; i < 17; point++, i++) {
            value = config->radius + config->curve_width + config->half_size;
            point->vx = value * ccos(i * 256) / 4096;
            point->vy = -config->ring_height / 2;
            point->vz = value * csin(i * 256) / 4096;
            setVector(&point[17], point->vx, point->vy, point->vz);
            point[17].vy += config->ring_height;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEC8[i]);
        }
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, work->position);
        work->position[1] = 0;
        work->elapsed = 0;
        work->started = 0;
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
    SetPolyF4(&flat);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    for (j = 0; j < config->radial_count; j++) {
        setVector(&rotation, 0, (j << 12) / config->radial_count, 0);
        setVector(&position, work->position[0], work->position[1], work->position[2]);
        setVector(&scale, 4096, 4096, 4096);
        point = work->path;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)radial.t, &flag);
        RotMatrix(&rotation, &radial);
        MulMatrix2(&base, &radial);
        ScaleMatrix(&radial, &scale);
        for (i = 0; i < config->particle_count; point += 128 / config->particle_count, i++) {
            time = work->elapsed - config->delay - i * 2;
            if (time >= 0 && time < config->particle_duration) {
                u8 green, blue;
                setVector(&position, point->vx, point->vy, point->vz);
                setVector(&rotation, 0, 0, 0);
                third = config->particle_count / 3;
                if (i < third) {
                    setVector(&scale, 4096, 4096, 4096);
                } else {
                    value = 4096 + ((i - third) << 12) / (config->particle_count * 2 / 3);
                    setVector(&scale, value, value, value);
                }
                GsSetLsMatrix(&radial);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
                value = time * 8 / config->particle_duration;
                setUV4(&quad, value % 4 * 32, value / 4 * 32,
                    value % 4 * 32 + 31, value / 4 * 32,
                    value % 4 * 32, value / 4 * 32 + 31,
                    value % 4 * 32 + 31, value / 4 * 32 + 31);
                green = config->green * i / config->particle_count;
                blue = config->blue * i / config->particle_count;
                setRGB0(&quad, config->red, green, blue);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
            }
        }
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->flash_duration) {
        u8 red, green, blue;
        red = config->flash_red * (config->flash_duration - time) / config->flash_duration;
        green = config->flash_green * (config->flash_duration - time) / config->flash_duration;
        blue = config->flash_blue * (config->flash_duration - time) / config->flash_duration;
        setRGB0(&flat, red, green, blue);
        GetDispEnv(&display);
        setXY4(&flat, 0, 0, display.disp.w - 1, 0,
            0, display.disp.h - 1, display.disp.w - 1, display.disp.h - 1);
        func_8005B260((u32 *)&flat, ot, 1, 1);
    }
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->ring_count; i++) {
        third = config->particle_count / 3;
        time = work->elapsed - config->delay - third * (i + 1) / config->ring_count;
        if (time >= 0 && time < config->ring_duration) {
            u8 red, green, blue;
            setVector(&position, work->position[0], work->position[1], work->position[2]);
            position.vy = (config->curve_width / 2 - config->path_height)
                * (i + 1) / config->ring_count;
            value = (time << 12) / config->ring_duration;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, projected, depths, interpolation, flags, 34);
            red = config->ring_red * (config->ring_duration - time) / config->ring_duration;
            green = config->ring_green * (config->ring_duration - time) / config->ring_duration;
            blue = config->ring_blue * (config->ring_duration - time) / config->ring_duration;
            setRGB0(&flat, red, green, blue);
            for (j = 0; j < 16; j++) {
                value = j + 17;
                setXY4(&flat, projected[j].vx, projected[j].vy,
                    projected[j + 1].vx, projected[j + 1].vy,
                    projected[value].vx, projected[value].vy,
                    projected[value + 1].vx, projected[value + 1].vy);
                depth = AverageZ4(depths[j], depths[j + 1], depths[value], depths[value + 1]);
                flag = (flags[j] | flags[j + 1] | flags[value] | flags[value + 1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    func_8005B260((u32 *)&flat, ot, 1, 1);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay;
    if (time >= config->particle_count * 2 + config->particle_duration) {
        return 2;
    } else {
        if (work->started) {
            return 0;
        } else {
            work->started = 1;
            return 1;
        }
    }
}

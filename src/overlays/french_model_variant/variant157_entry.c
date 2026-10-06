#include "../../types.h"
#include "variant157_entry.h"

static const VECTOR D_8013C058 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model157State *work;
    Model157Config *config;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G3 triangle;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR projected[34];
    u16 depths[34], interpolation_values[34], flags[34];
    PSXLONG interpolation, flag, depth;
    GsOT *ot;
    s32 step, i, j, time, phase_time, angle, terminal_duration;
    SVECTOR *point, *history;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C058;
    work = (Model157State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C084[command % 100];
        point = &work->origin;
        setVector(point, 0, 0, 0);
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            time = i * 128;
            setVector(point, config->radius * ccos(time) / 4096, 0,
                config->radius * csin(time) / 4096);
        }
        point = work->starts;
        history = work->trails;
        for (i = 0; i < 32; point++, i++) {
            time = rand() % (config->radius - config->half_size * 2) * 2 -
                (config->radius - config->half_size * 2);
            angle = i * 4096 / 32;
            setVector(point, time * ccos(angle) / 4096, 0,
                time * csin(angle) / 4096);
            point->pad = 0;
            for (j = 0; j < 6; j++, history++) {
                copyVector(history, point);
            }
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C068[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        work->toggle = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG3(&triangle);
    SetPolyF4(&flash);
    time = work->elapsed - config->delay;
    if (time < 0) {
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->origin);
        work->origin.vy = 0;
    } else {
        if (time < config->ring_duration) {
            red = config->red * time / config->ring_duration;
            green = config->green * time / config->ring_duration;
            blue = config->blue * time / config->ring_duration;
        } else {
            s32 ring_time = time - 12;

            time = ring_time - (config->ring_duration + config->spacing * 32);
            if (time < 0) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else {
                time = config->ring_duration - time;
                red = config->red * time / config->ring_duration;
                green = config->green * time / config->ring_duration;
                blue = config->blue * time / config->ring_duration;
            }
        }
        setRGB0(&triangle, red, green, blue);
        setRGB1(&triangle, 0, 0, 0);
        setRGB2(&triangle, 0, 0, 0);
        time = work->toggle * 256;
        setVector(&scale, time + 4096, time + 4096, time + 4096);
        work->toggle ^= 1;
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        depths[0] = RotTransPers(&work->origin, (PSXLONG *)&projected[0], &interpolation, &flag) * 4;
        copyVector(&position, &work->origin);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, &projected[1], &depths[1], &interpolation_values[1], &flags[1], 33);
        for (i = 1; i < 33; i++) {
            setXY3(&triangle, projected[0].vx, projected[0].vy,
                projected[i].vx, projected[i].vy, projected[i + 1].vx, projected[i + 1].vy);
            depth = AverageZ3(depths[0], depths[i], depths[i + 1]);
            if (flag >= 0) {
                flag = (flags[i] | flags[i + 1]) & 0x20;
            }
            if (depth >= 0 && flag == 0) {
                func_8005B260((u32 *)&triangle, ot, (u16)depth, 1);
                if (red == config->red) {
                    func_8005B260((u32 *)&triangle, ot, (u16)depth, 0);
                } else {
                    func_8005B260((u32 *)&triangle, ot, (u16)depth, 2);
                }
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 3072);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->sprite_red, config->sprite_green, config->sprite_blue);
    point = work->starts;
    for (i = 0; i < 32; point++, i++) {
        time = work->elapsed - config->delay - config->ring_duration - config->spacing * i;
        if (time >= 0 && time < config->fall_duration) {
            SVECTOR *row;

            copyVector(&position, &work->origin);
            addVector(&position, point);
            if (position.vy <= -config->half_size) {
                setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
                vertices[0].vx = vertices[2].vx = -config->half_size;
            } else {
                time = position.vy * 64 / config->half_size + 64;
                setUV4(&quad, time, 0, 64, 0, time, 63, 64, 63);
                vertices[0].vx = vertices[2].vx = position.vy;
            }
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
            row = &work->trails[i * 6];
            history = &row[point->pad];
            copyVector(history, point);
            point->vy -= config->fall_distance / config->fall_duration * step;
            point->pad = (point->pad + 1) % 6;
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 3072);
    setVector(&scale, 4096, 4096, 4096);
    point = work->starts;
    for (i = 0; i < 32; point++, i++) {
        for (j = 1; j < 7; j++) {
            time = work->elapsed - config->delay - config->ring_duration - config->spacing * i - j * 2;
            if (time >= 0 && time < config->fall_duration) {
                SVECTOR *row;

                red = config->trail_red * j / 6;
                green = config->trail_green * j / 6;
                blue = config->trail_blue * j / 6;
                row = &work->trails[i * 6];
                history = &row[(point->pad + j - 1) % 6];
                setRGB0(&quad, red, green, blue);
                copyVector(&position, &work->origin);
                addVector(&position, history);
                if (position.vy <= -config->half_size) {
                    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
                    vertices[0].vx = vertices[2].vx = -config->half_size;
                } else {
                    time = position.vy * 64 / config->half_size + 64;
                    setUV4(&quad, time, 0, 64, 0, time, 63, 64, 63);
                    vertices[0].vx = vertices[2].vx = position.vy;
                }
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
            }
        }
    }
    time = work->elapsed - config->delay - config->ring_duration;
    if (time >= 0 && time < config->flash_duration) {
        red = (config->flash_duration - time) * 255 / config->flash_duration;
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        setRGB0(&flash, red, red, red);
        func_8005B260((u32 *)&flash, ot, 1, 1);
    }
    phase_time = time - 12;
    time = phase_time - config->spacing * 32;
    if (time >= 0 && time < config->flash_duration) {
        red = (config->flash_duration - time) * 255 / config->flash_duration;
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        setRGB0(&flash, red, red, red);
        func_8005B260((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->ring_duration;
    if (time < 0) {
        return 0;
    }
    phase_time = config->spacing * 32;
    terminal_duration = config->ring_duration + 12;
    if (time >= phase_time + terminal_duration) {
        return 2;
    }
    if (time >= phase_time) {
        if (work->completed) {
            return 0;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

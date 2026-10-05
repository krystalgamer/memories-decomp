#include "../../types.h"
#include "variant199_entry.h"

const VECTOR D_8013C594 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model199State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    ModelEffectAdjustment adjustment;
    PSXLONG flag, interpolation;
    Model199Config *config;
    GsOT *ot;
    SVECTOR *point;
    SVECTOR *G32 *face;
    s32 step, i, j, value2, radius, value, time, blue;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C594;
    work = (Model199State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C630[command];
        func_80057E20(Model_GetActiveSlotIndex() ^ 1, &adjustment);
        point = work->rays;
        for (i = 128; i > 0; point++, i--) {
            value = rand() % 4096;
            value2 = (config->ray_radius + adjustment.x) * i / 128;
            point->vx = value2 * ccos(value) / 4096;
            point->vy = -rand() % config->ray_height + config->ray_height * i / 128;
            point->vz = value2 * csin(value) / 4096;
        }
        point = work->grid;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; point++, j++) {
                point->vx = j * config->grid_radius;
                point->vy = i * config->grid_radius;
                point->vz = 0;
            }
        }
        point = work->grid;
        face = work->quads;
        work->quads[0] = point;
        face[1] = &work->grid[1];
        face[2] = &work->grid[3];
        face[3] = &work->grid[4];
        face[4] = &work->grid[2];
        face[5] = &work->grid[5];
        face[6] = &work->grid[1];
        face[7] = &work->grid[4];
        face[8] = &work->grid[8];
        face[9] = &work->grid[7];
        face[10] = &work->grid[5];
        face[11] = &work->grid[4];
        face[12] = &work->grid[6];
        face[13] = &work->grid[3];
        face[14] = &work->grid[7];
        face[15] = &work->grid[4];
        point = work->sphere;
        for (i = 0; i < 32; point++, i++) {
            value = rand() % 4096;
            value2 = rand() % 2048;
            radius = rand() % config->sphere_radius;
            point->vx = radius * ccos(value) / 4096 * csin(value2) / 4096;
            point->vy = radius * ccos(value2) / 4096;
            point->vz = radius * csin(value) / 4096 * csin(value2) / 4096;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C5A4[i]);
        }
        work->texture[4] = func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013C5A4[4]);
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&vertices[2], -config->ray_size / 4, 0, 0);
    setVector(&vertices[3], config->ray_size / 4, 0, 0);
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&adjustment);
        setVector(&vertices[2], -config->ray_size / 4, 0, 0);
        setVector(&vertices[3], config->ray_size / 4, 0, 0);
        point = work->rays;
        for (i = 0; i < 128 / config->ray_group_size; i++) {
            time = work->elapsed - config->ray_delay - i * config->ray_spacing;
            if (time >= 0) {
                s32 red, green;
                if (time < config->ray_fade_in) {
                    red = config->ray_r * time / config->ray_fade_in;
                    green = config->ray_g * time / config->ray_fade_in;
                    blue = config->ray_b * time / config->ray_fade_in;
                } else {
                    red = config->ray_r;
                    green = config->ray_g;
                    blue = config->ray_b;
                }
                setRGB0(&quad, red, green, blue);
                for (j = 0; j < config->ray_group_size; point++, j++) {
                    s32 frame;
                    value = rand() % config->ray_size / 2 - config->ray_size / 4;
                    setVector(&vertices[0], -config->ray_size / 4 + value, -config->ray_size, 0);
                    setVector(&vertices[1], config->ray_size / 4 + value, -config->ray_size, 0);
                    copyVector(&position, point);
                    position.vx += adjustment.x;
                    position.vy += adjustment.y;
                    position.vz += adjustment.z;
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                    frame = time % 16 / 2;
                    setUV4(&quad, frame * 32, 0, frame * 32 + 31, 0,
                           frame * 32, 63, frame * 32 + 31, 63);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            } else {
                point += config->ray_group_size;
            }
        }
    } else if (time < config->ray_duration) {
        s32 red, green, blue, remaining;
        remaining = config->ray_duration - time;
        red = config->ray_r * remaining / config->ray_duration;
        green = config->ray_g * remaining / config->ray_duration;
        blue = config->ray_b * remaining / config->ray_duration;
        setRGB0(&quad, red, green, blue);
        point = work->rays;
        for (i = 0; i < 128; point++, i++) {
            s32 frame;
            value = rand() % config->ray_size / 2 - config->ray_size / 4;
            setVector(&vertices[0], -config->ray_size / 4 + value, -config->ray_size, 0);
            setVector(&vertices[1], config->ray_size / 4 + value, -config->ray_size, 0);
            copyVector(&position, point);
            addVector(&position, &work->anchor);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            frame = (time + i) % 16 / 2;
            setUV4(&quad, frame * 32, 0, frame * 32 + 31, 0,
                   frame * 32, 63, frame * 32 + 31, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += point->vx / (time + config->grid_duration) * step;
            point->vy += (point->vy - config->ray_height) / (time + config->grid_duration) * step;
            point->vz += point->vz / (time + config->grid_duration) * step;
        }
    }
    time = work->elapsed - config->burst_delay;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        blue = 255 * (config->flash_duration - time);
        blue /= config->flash_duration;
        setRGB0(&flash, blue, blue, blue);
        func_8005B260((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->anchor);
    } else if (time < config->grid_duration) {
        s32 red, green, blue, remaining;
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 128, 64, 191, 64, 128, 127, 191, 127);
        remaining = config->grid_duration - time;
        red = config->grid_r * remaining / config->grid_duration;
        green = config->grid_g * remaining / config->grid_duration;
        blue = config->grid_b * remaining / config->grid_duration;
        setVector(&rotation, 0, 0, time << 6);
        setRGB0(&quad, red, green, blue);
        value = (time << 12) / config->grid_duration;
        setVector(&scale, value, value, value);
        face = work->quads;
        copyVector(&position, &work->anchor);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 4; face += 4, i++) {
            depth = RotAverage4(face[0], face[1], face[2], face[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    quad.tpage = work->texture[4] >> 16;
    quad.clut = work->texture[4];
    setVector(&vertices[0], -config->sphere_size, -config->sphere_size, 0);
    setVector(&vertices[1], config->sphere_size, -config->sphere_size, 0);
    setVector(&vertices[2], -config->sphere_size, config->sphere_size, 0);
    setVector(&vertices[3], config->sphere_size, config->sphere_size, 0);
    time = work->elapsed - config->burst_delay;
    if (time >= 0 && time < config->sphere_duration) {
        s32 red, green, blue, remaining;
        value = (time << 3) / config->sphere_duration;
        setUV4(&quad, value * 32, 192, value * 32 + 31, 192,
               value * 32, 223, value * 32 + 31, 223);
        remaining = config->sphere_duration - time;
        red = config->sphere_r * remaining / config->sphere_duration;
        green = config->sphere_g * remaining / config->sphere_duration;
        blue = config->sphere_b * remaining / config->sphere_duration;
        setRGB0(&quad, red, green, blue);
        point = work->sphere;
        for (i = 0; i < 32; point++, i++) {
            copyVector(&position, point);
            addVector(&position, &work->anchor);
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
            point->vy -= config->sphere_fall / config->sphere_duration * step;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    value = (s32)config->flash_duration < (s32)config->grid_duration
        ? (s32)config->grid_duration : (s32)config->flash_duration;
    value = value < config->ray_duration ? (s32)config->ray_duration : value;
    value = value < config->sphere_duration ? (s32)config->sphere_duration : value;
    time = work->elapsed - config->ray_delay;
    if (time < 0) {
        return 0;
    }
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        return 4;
    }
    if (time < value) {
        if (work->completed != 0) {
            return 0;
        }
        work->completed++;
        return 1;
    }
    return 2;
}

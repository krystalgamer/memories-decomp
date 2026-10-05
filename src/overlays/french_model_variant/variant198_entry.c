#include "../../types.h"
#include "variant198_entry.h"

const VECTOR D_8013BD74 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model198State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model198Config *config;
    GsOT *ot;
    SVECTOR *point;
    s32 direction, i, j, k, angle, factor, time, color_numerator;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BD74;
    work = (Model198State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BDBC[command];
        Model_CopySlotU16Values(Model_GetActiveSlotIndex(), (u16 *)&work->origin);
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->opponent);
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = config->ring_radius * ccos(angle) / 4096;
            point->vy = -config->ring_width;
            point->vz = config->ring_radius * csin(angle) / 4096;
        }
        factor = config->ring_radius - config->ring_width;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = factor * ccos(angle) / 4096;
            point->vy = 0;
            point->vz = factor * csin(angle) / 4096;
        }
        point = work->rays;
        for (i = 0; i < 16; i++) {
            point->vx = config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
            point->vx = -config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BD84[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        work->triggered = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
    position.vy = config->ring_y;
    for (j = 0; j < config->phase_count; j++) {
        time = work->elapsed - config->start_delay - j * config->phase_spacing;
        if (time >= 0 && time < config->ring_duration && work->triggered == j) {
            func_8005F7B0(50, (s16)(config->phase_spacing / 3));
            work->triggered++;
        }
        if (time >= 0 && time < config->ring_duration && j > 0) {
            s32 red, green, blue, remaining, scale_value;
            scale_value = (time << 12) / config->ring_duration;
            setVector(&scale, scale_value, scale_value, scale_value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 66);
            remaining = config->ring_duration - time;
            color_numerator = config->ring_r * remaining;
            red = color_numerator / config->ring_duration;
            color_numerator = config->ring_g * remaining;
            green = color_numerator / config->ring_duration;
            color_numerator = config->ring_b * remaining;
            blue = color_numerator / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            for (i = 0; i < 32; i++) {
                scale_value = i + 33;
                setXY4(&quad, screen[i].vx, screen[i].vy,
                       screen[i + 1].vx, screen[i + 1].vy,
                       screen[scale_value].vx, screen[scale_value].vy,
                       screen[scale_value + 1].vx, screen[scale_value + 1].vy);
                depth = AverageZ4(depths[i], depths[i + 1], depths[i + 33], depths[i + 34]);
                flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
                setUV4(&quad, i * 8, 0, i * 8 + 7, 0, i * 8, 31, i * 8 + 7, 31);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    for (j = 0; j < config->phase_count; j++) {
        time = work->elapsed - config->start_delay - j * config->phase_spacing;
        if (time >= 0 && time < config->flash_duration) {
            s32 remaining;
            setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
            remaining = config->flash_duration - time;
            color_numerator = remaining * 256;
            color_numerator -= remaining;
            color_numerator /= config->flash_duration;
            setRGB0(&flash, color_numerator, color_numerator, color_numerator);
            func_8005B260((u32 *)&flash, ot, 1, 1);
        }
    }
    setVector(&vertices[0], -config->ray_half_width, -config->ray_height, 0);
    setVector(&vertices[1], config->ray_half_width, -config->ray_height, 0);
    setVector(&vertices[2], -config->ray_half_width, 0, 0);
    setVector(&vertices[3], config->ray_half_width, 0, 0);
    setVector(&rotation, 0, 0, 0);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
    for (k = 1; k < config->phase_count; k++) {
        point = work->rays;
        for (i = 0; i < 16; i++) {
            time = work->elapsed - config->start_delay - config->ray_spacing * i
                - k * config->phase_spacing - config->ray_delay;
            if (time >= 0 && time < config->ray_duration) {
                for (j = 0; j < 2; point++, j++) {
                    s32 red, green, blue, remaining;
                    setVector(&position, point->vx, point->vy, point->vz);
                    position.vx += work->opponent.vx;
                    position.vy += work->opponent.vy;
                    position.vz += work->opponent.vz;
                    position.vy = 0;
                    factor = (time << 12) / config->ray_duration;
                    setVector(&scale, 4096, factor, 4096);
                    remaining = config->ray_duration - time;
                    color_numerator = config->ray_r * remaining;
                    red = color_numerator / config->ray_duration;
                    color_numerator = config->ray_g * remaining;
                    green = color_numerator / config->ray_duration;
                    color_numerator = config->ray_b * remaining;
                    blue = color_numerator / config->ray_duration;
                    setRGB0(&quad, red, green, blue);
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            } else {
                point += 2;
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    factor = config->ray_spacing * 16 + config->ray_duration;
    time = work->elapsed - config->start_delay - config->ray_delay;
    if (time < 0) {
        return 0;
    }
    time = work->elapsed - config->start_delay - (config->phase_count - 1) * config->phase_spacing
        - config->ray_delay - factor;
    if (time >= 0) {
        return 2;
    }
    for (i = 0; i < config->phase_count; i++) {
        time = work->elapsed - config->start_delay - i * config->phase_spacing - config->ray_delay;
        if (time >= 0 && time < factor && work->completed == i) {
            work->completed++;
            if (work->completed == config->phase_count) {
                return 1;
            }
            return 4;
        }
    }
    return 0;
}

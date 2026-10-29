#include "../../types.h"
#include "image122172_entry.h"

#ifndef GRAPHICS_DEFAULT_HEIGHT
#define GRAPHICS_DEFAULT_HEIGHT 256
#endif
#include "../../game/graphics_constants.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Model142State *work;
    MATRIX base, matrix;
    POLY_G4 trail;
    POLY_FT4 quad;
    POLY_F4 flash;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[96];
    DVECTOR screen[96];
    u16 depths[96], interpolation_values[96], flags[96];
    PSXLONG flag, interpolation;
    s32 direction;
    GsOT *ot;
    Model142Config *config;
    SVECTOR *point, *vertex;
    s32 i, j, time, index;
    s32 first0, first1, second0, second1;
    s32 next_sample;
    PSXLONG depth;
    u16 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C52C;
    work = (Model142State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C558[command];
        point = work->history;
        for (i = 0; i < 96; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = -config->history_length;
        }
        point = work->dots;
        for (i = 0; i < 32; point++, i++) {
            point->vx = rand() % (config->radius * 2) - config->radius;
            point->vy = rand() % (config->radius * 2) - config->radius;
            point->vz = rand() % (config->radius * 2) - config->radius;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C53C[i]);
        }
        work->elapsed = 0;
        work->event = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyG4(&trail);
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    box.attribute = 0x50000000;
    box.w = 2;
    box.h = 2;
    SetSemiTrans(&trail, 1);
    time = work->elapsed - config->trail_delay;
    if (time < 0) {
        work->history[0].pad = -config->history_length;
    }
    if (work->history[0].pad < 0) {
        for (i = 0; i < config->trail_count; i++) {
            point = &work->history[(i * config->history_length + config->history_length + work->history[0].pad) * 2];
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point++;
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i] + 1), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        work->history[0].pad++;
    } else {
        for (i = 0; i < config->trail_count; i++) {
            point = &work->history[(i * config->history_length + work->history[0].pad) * 2];
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
            point++;
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i] + 1), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        work->history[0].pad = (work->history[0].pad + 1) % config->history_length;
    }
    time = work->elapsed - config->trail_delay;
    if (time >= 0 && time < config->trail_duration) {
        GsSetLsMatrix(&base);
        if (work->history[0].pad < 0) {
            index = config->history_length + work->history[0].pad;
        } else {
            index = work->history[0].pad;
        }
        vertex = vertices;
        for (i = 0; i < config->trail_count; i++) {
            for (j = 0; j < config->history_length; j++) {
                point = work->history + i * config->history_length * 2 +
                    (index + config->history_length - j - 1) % config->history_length * 2;
                vertex->vx = point->vx + (point[1].vx - point->vx) * j / (config->history_length * 2);
                vertex->vy = point->vy + (point[1].vy - point->vy) * j / (config->history_length * 2);
                vertex->vz = point->vz + (point[1].vz - point->vz) * j / (config->history_length * 2);
                vertex++;
                point++;
                vertex->vx = point->vx + (point[-1].vx - point->vx) * j / (config->history_length * 2);
                vertex->vy = point->vy + (point[-1].vy - point->vy) * j / (config->history_length * 2);
                vertex->vz = point->vz + (point[-1].vz - point->vz) * j / (config->history_length * 2);
                vertex++;
            }
        }
        RotTransPersN(vertices, screen, depths, interpolation_values, flags, 96);
        for (i = 0; i < config->trail_count; i++) {
            for (j = 1; j < config->history_length; j++) {
                first0 = i * config->history_length - 1;
                first0 = (first0 + j) * 2;
                first1 = first0 + 1;
                second0 = (i * config->history_length + j) * 2;
                second1 = second0 + 1;
                setXY4(&trail,
                    screen[first0].vx, screen[first0].vy,
                    screen[first1].vx, screen[first1].vy,
                    screen[second0].vx, screen[second0].vy,
                    screen[second1].vx, screen[second1].vy);
                depth = AverageZ4(depths[first0], depths[first1], depths[second0], depths[second1]);
                flag = (flags[first0] | flags[first1] | flags[second0] | flags[second1]) & 0x20;
                red = config->trail_r * (config->history_length - j) / config->history_length;
                green = config->trail_g * (config->history_length - j) / config->history_length;
                blue = config->trail_b * (config->history_length - j) / config->history_length;
                setRGB0(&trail, red, green, blue);
                setRGB1(&trail, red, green, blue);
                next_sample = j + 1;
                red = config->trail_r * (config->history_length - next_sample) / config->history_length;
                green = config->trail_g * (config->history_length - next_sample) / config->history_length;
                blue = config->trail_b * (config->history_length - next_sample) / config->history_length;
                setRGB2(&trail, red, green, blue);
                setRGB3(&trail, red, green, blue);
                if (depth >= 0 && !flag) {
                    Graphics_SubmitDrawModePacket((u32 *)&trail, ot, (u16)depth, 1);
                }
            }
        }
    }
    setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
        GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
    for (i = 0; i < config->event_count; i++) {
        time = work->elapsed - config->event_times[i];
        if (time >= 0 && time < config->flash_duration) {
            blue = (config->flash_duration - time) * 255 / config->flash_duration;
            setRGB0(&flash, blue, blue, blue);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setRGB0(&quad, config->sprite_r, config->sprite_g, config->sprite_b);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    setVector(&position, 0, -350, direction * 450);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    for (i = 0; i < config->event_count; i++) {
        time = work->elapsed - config->event_times[i];
        if (time >= 0 && time < config->flash_duration) {
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad,
                (time * 8 / config->flash_duration) % 4 * 32,
                (time * 8 / config->flash_duration) / 4 * 32,
                (time * 8 / config->flash_duration) % 4 * 32 + 31,
                (time * 8 / config->flash_duration) / 4 * 32,
                (time * 8 / config->flash_duration) % 4 * 32,
                (time * 8 / config->flash_duration) / 4 * 32 + 31,
                (time * 8 / config->flash_duration) % 4 * 32 + 31,
                (time * 8 / config->flash_duration) / 4 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    setVector(&position, 0, -350, direction * 450);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    for (i = 0; i < config->event_count; i++) {
        time = work->elapsed - config->event_times[i];
        if (time >= 0 && time < config->dot_duration) {
            vertex = work->dots;
            for (j = 0; j < 32; vertex++, j++) {
                box.r = config->dot_r * (config->dot_duration - time) / config->dot_duration;
                box.g = config->dot_g * (config->dot_duration - time) / config->dot_duration;
                box.b = config->dot_b * (config->dot_duration - time) / config->dot_duration;
                copyVector(&vertices[0], vertex);
                vertices[0].vx *= time;
                vertices[0].vy *= time;
                vertices[0].vz *= time;
                depth = RotTransPers(&vertices[0], (PSXLONG *)&box.x, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortBoxFill(&box, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->event_times[0]) {
        return 0;
    }
    if (work->elapsed >= config->event_times[config->event_count - 1] &&
        work->elapsed >= config->trail_duration) {
        return 2;
    }
    for (i = 0; i < config->event_count; i++) {
        if (work->elapsed >= config->event_times[i] && work->event == i) {
            work->event = i + 1;
            if (i == config->event_count - 1) {
                return 1;
            } else {
                return 4;
            }
        }
    }
    return 0;
}

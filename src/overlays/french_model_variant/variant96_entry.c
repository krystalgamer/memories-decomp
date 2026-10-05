#include "../../types.h"
#include "variant96_entry.h"

const VECTOR D_8013BC4C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model96State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_GT4 quad;
    POLY_F4 flash;
    VECTOR scale;
    DVECTOR outer_screen[65], inner_screen[65];
    u16 outer_depth[65], inner_depth[65], projection[65];
    u16 outer_flags[65], inner_flags[65];
    SVECTOR opponent;
    PSXLONG flag;
    Model96Config *config;
    GsOT *ot;
    SVECTOR *inner, *position;
    s32 *factor;
    s32 direction, i, j, angle, time, brightness;
    PSXLONG depth;

    scale = D_8013BC4C;
    work = (Model96State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BC78[command];
        position = work->outer;
        inner = work->inner;
        for (i = 0; i <= config->segments; position++, inner++, i++) {
            angle = i << 12;
            position->vx = config->outer_radius * ccos(angle / config->segments) / 4096;
            position->vy = config->outer_radius * csin(angle / config->segments) / 4096;
            position->vz = direction * config->outer_radius / 2;
            inner->vx = config->inner_radius * ccos(angle / config->segments) / 4096;
            inner->vy = config->inner_radius * csin(angle / config->segments) / 4096;
            inner->vz = 0;
        }
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&work->positions[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->prepared = 0;
        work->elapsed = -config->delay;
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BC5C[i]);
        }
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (!work->prepared) {
        position = work->positions;
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&opponent);
        opponent.vy = -350;
        for (i = 0; i < config->count; position++, i++) {
            position->vx = matrix.t[0] + (opponent.vx - matrix.t[0]) * i / config->count;
            position->vy = matrix.t[1] + (opponent.vy - matrix.t[1]) * i / config->count;
            position->vz = matrix.t[2] + (opponent.vz * 2 - matrix.t[2]) * i / config->count;
            work->scales[i] = 0;
        }
        work->rotation.vx = -ratan2(opponent.vy - matrix.t[1], opponent.vz * 2 - matrix.t[2]);
        work->rotation.vy = ratan2(opponent.vx - matrix.t[0], opponent.vz * 2 - matrix.t[2]);
        work->rotation.vz = 0;
        work->prepared = 1;
    }
    SetPolyGT4(&quad);
    SetPolyF4(&flash);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 31, 0, 0, 128, 31, 128);
    setRGB0(&quad, config->r, config->g, config->b);
    setRGB1(&quad, config->r, config->g, config->b);
    setRGB2(&quad, 0, 0, 0);
    setRGB3(&quad, 0, 0, 0);
    position = work->positions;
    factor = work->scales;
    for (i = 0; i < config->count; position++, factor++, i++) {
        if (*factor > 0 && *factor < 6144) {
            setVector(&scale, *factor, *factor, *factor);
            GsSetLsMatrix(&base);
            RotTrans(position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&work->rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->outer, outer_screen, outer_depth, projection, outer_flags, config->segments + 1);
            RotTransPersN(work->inner, inner_screen, inner_depth, projection, inner_flags, config->segments + 1);
            if (*factor > 4096) {
                quad.r0 = quad.r1 = config->r * (6144 - *factor) / 2048;
                quad.g0 = quad.g1 = config->g * (6144 - *factor) / 2048;
                quad.b0 = quad.b1 = config->b * (6144 - *factor) / 2048;
            }
            for (j = 0; j < config->segments; j++) {
                setXY4(&quad, outer_screen[j].vx, outer_screen[j].vy,
                       outer_screen[j + 1].vx, outer_screen[j + 1].vy,
                       inner_screen[j].vx, inner_screen[j].vy,
                       inner_screen[j + 1].vx, inner_screen[j + 1].vy);
                depth = AverageZ4(outer_depth[j], outer_depth[j + 1], inner_depth[j], inner_depth[j + 1]);
                flag = (outer_flags[j] | outer_flags[j + 1] | inner_flags[j] | inner_flags[j + 1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    func_8005B260((u32 *)&quad, ot, (u16)depth, 1);
                }
            }
        }
        if (i * config->spacing < work->elapsed) {
            *factor = ((work->elapsed - i * config->spacing) << 12) / config->duration;
        }
    }
    time = work->elapsed - config->spacing * config->count * 2 / 3;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        brightness = 255 * (config->flash_duration - time) / config->flash_duration;
        setRGB0(&flash, brightness, brightness, brightness);
        func_8005B260((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->spacing * config->count * 2 / 3) {
        return 0;
    }
    if (work->elapsed < config->spacing * config->count + config->duration * 2) {
        return 4;
    }
    {
        u8 result;
        if (work->completed != 0) {
            result = 2;
        } else {
            work->completed++;
            result = 1;
        }
        return result;
    }
}

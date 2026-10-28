#include "../../types.h"
#include "variant168_entry.h"
#include "../../game/screen_projection.h"

/* The entry is the French header-178 particles with a cone added: particles
 * rise from one of the slot's parts, fly to endpoints around the other slot
 * and fade, four burst quads fly out from each endpoint, and a spinning,
 * growing, fading cone of eight triangles sits on the same part. A negative
 * command draws a frame; any other command sets the work area up and prints
 * its size. */

static const VECTOR D_8013C18C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model168State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G3 cone;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model168Config *config;
    GsOT *ot;
    SVECTOR *point, *endpoint;
    SVECTOR *G32 *tri;
    s32 direction, step, i, j, time, value;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C18C;
    work = (Model168State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C2A8[command % 100];
        point = work->positions;
        for (i = 0; i < 64; point++, i++) {
            setVector(point, 0, 0, 0);
            point->pad = 0;
        }
        point = work->endpoints;
        for (i = 0; i < 64; point++, i++) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vx += rand() % config->spread * 2 - config->spread;
            point->vy += rand() % config->spread;
            point->vz += direction * (rand() % config->spread);
            point->pad = 0;
        }
        point = work->velocities;
        for (i = 0; i < 4; point++, i++) {
            point->vx = rand() % config->velocity_spread * 2 - config->velocity_spread;
            point->vy = -config->rise;
            point->vz = rand() % config->velocity_spread * 2 - config->velocity_spread;
        }
        point = &work->apex;
        setVector(point, 0, 0, 0);
        point = work->rim;
        for (i = 0; i < 4; point++, i++) {
            point->vx = config->cone_radius * ccos(i << 10) / 4096;
            point->vy = config->cone_radius * csin(i << 10) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 4; point++, i++) {
            point->vx = config->cone_radius / 16 * ccos((i << 10) + 512) / 4096;
            point->vy = config->cone_radius / 16 * csin((i << 10) + 512) / 4096;
            point->vz = 0;
        }
        tri = work->triangles[0];
        point = &work->apex;
        tri[0] = point;
        tri[1] = &work->rim[0];
        tri[2] = &work->rim[4];
        tri[3] = point;
        tri[4] = &work->rim[0];
        tri[5] = &work->rim[7];
        tri[6] = point;
        tri[7] = &work->rim[1];
        tri[8] = &work->rim[5];
        tri[9] = point;
        tri[10] = &work->rim[1];
        tri[11] = &work->rim[4];
        tri[12] = point;
        tri[13] = &work->rim[2];
        tri[14] = &work->rim[6];
        tri[15] = point;
        tri[16] = &work->rim[2];
        tri[17] = &work->rim[5];
        tri[18] = point;
        tri[19] = &work->rim[3];
        tri[20] = &work->rim[7];
        tri[21] = point;
        tri[22] = &work->rim[3];
        tri[23] = &work->rim[6];
        for (i = 1; i < 9; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C1AC[i]);
        }
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &D_8013C1AC[0]);
        work->elapsed = 0;
        work->completed = 0;
        printf("%d (0x%x) Byte\n", sizeof(Model168State), sizeof(Model168State));
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    endpoint = work->endpoints;
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyG3(&cone);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    for (i = 0; i < config->count; point++, endpoint++, i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        } else if (time < config->travel + config->fade) {
            value = time * 3584 / config->travel + 512;
            setVector(&scale, value, value, value);
            if (time < config->travel) {
                setRGB0(&quad, config->r, config->g, config->b);
            } else {
                s32 red, green, blue;
                value = config->travel + config->fade - time;
                red = config->r * value / config->fade;
                green = config->g * value / config->fade;
                blue = config->b * value / config->fade;
                setRGB0(&quad, red, green, blue);
            }
            setVector(&position, point->vx, point->vy, point->vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            value = (time / 2 + i) % 8;
            setUV4(&quad, value % 4 * 64, value / 4 * 87, value % 4 * 64 + 63, value / 4 * 87,
                   value % 4 * 64, value / 4 * 87 + 86, value % 4 * 64 + 63, value / 4 * 87 + 86);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            GsSortPoly(&quad, ot, (u16)depth);
            if (time < config->travel) {
                point->vx += (endpoint->vx - point->vx) / (config->travel - time);
                point->vy += (endpoint->vy - point->vy) / (config->travel - time);
                point->vz += (endpoint->vz - point->vz) / (config->travel - time);
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setRGB0(&quad, config->burst_r, config->burst_g, config->burst_b);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    point = work->endpoints;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * config->spacing - config->travel;
        if (time >= 0 && time < config->burst_duration) {
            endpoint = work->velocities;
            for (j = 0; j < 4; endpoint++, j++) {
                setVector(&position, endpoint->vx, endpoint->vy, endpoint->vz);
                position.vx *= time;
                position.vy *= time;
                position.vz *= time;
                position.vx /= config->burst_duration;
                position.vy /= config->burst_duration;
                position.vz /= config->burst_duration;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                value = (time << 3) / config->burst_duration;
                setUV4(&quad, value * 32, 174, value * 32 + 31, 174,
                       value * 32, 205, value * 32 + 31, 205);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - config->cone_delay;
    if (time >= 0 && time < config->cone_duration) {
        SetSemiTrans(&cone, 1);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&rotation, 0, 0, time << 6);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        value = (time << 12) / config->cone_duration;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        {
            s32 red, green, blue, left;
            left = config->cone_duration - time;
            red = config->cone_r * left / config->cone_duration;
            green = config->cone_g * left / config->cone_duration;
            blue = config->cone_b * left / config->cone_duration;
            setRGB1(&cone, 0, 0, 0);
            setRGB2(&cone, 0, 0, 0);
            setRGB0(&cone, red, green, blue);
        }
        tri = work->triangles[0];
        for (i = 0; i < 8; i++) {
            depth = RotAverage3(tri[0], tri[1], tri[2],
                (PSXLONG *)&cone.x0, (PSXLONG *)&cone.x1, (PSXLONG *)&cone.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&cone, ot, (u16)depth, 1);
            }
            tri += 3;
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->travel;
    if (time < 0) {
        return 0;
    }
    if (time >= config->spacing * config->count + config->burst_duration) {
        return 2;
    }
    if (time < config->spacing * config->count) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 0;
}

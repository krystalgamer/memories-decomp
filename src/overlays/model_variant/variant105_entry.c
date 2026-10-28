#include "../../types.h"
#include "variant105_entry.h"

/* Header 105's entry: after the start delay, eight spinning sprites fade in
 * one after another around a slot part, arc over towards scattered points
 * near the other slot trailing two fainter copies, then burst open there.
 * The return value is 0 until the bursts start, 1 while they run and 2
 * once they have all ended. */

static const VECTOR D_8013C414 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR offset, origin;
    SVECTOR next[8];
    PSXLONG flag, interpolation;
    Model105State *work;
    s32 side;
    GsOT *ot;
    Model105Config *config;
    s32 i, j;
    s32 t;
    s32 depth;
    s32 size;
    SVECTOR *point, *target, *vertex;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C414;
    work = (Model105State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C440[command];
        point = work->spins;
        for (i = 0; i < 8; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = i << 9;
            point++;
        }
        point = work->trails[0];
        for (i = 0; i < 24; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point++;
        }
        work->index = 0;
        point = work->targets;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        for (i = 0; i < 8; i++) {
            point->vx = rand() % (config->spread * 2) + vertices[0].vx - config->spread;
            point->vy = rand() % (config->spread * 2) + vertices[0].vy - config->spread;
            point->vz = rand() % (config->spread * 2) + vertices[0].vz - config->spread;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C424[i]);
        }
        work->elapsed = -config->delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    setVector(&offset, 0, 0, side * config->offset);
    setVector(&origin, 0, 0, 0);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    point = work->spins;
    target = &work->trails[0][work->index];
    for (i = 0; i < 8; i++) {
        t = work->elapsed - i * config->spacing;
        if (t >= 0 && work->elapsed < config->launch) {
            if (t >= config->fade) {
                quad.r0 = config->red;
                quad.g0 = config->green;
                quad.b0 = config->blue;
            } else {
                red = config->red * t / config->fade;
                green = config->green * t / config->fade;
                blue = config->blue * t / config->fade;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
            }
            setUV4(&quad, (t / 2 + i) % 3 * 32, 0, (t / 2 + i) % 3 * 32 + 31, 0, (t / 2 + i) % 3 * 32, 31, (t / 2 + i) % 3 * 32 + 31, 31);
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            GsSetLsMatrix(&base);
            RotMatrixZYX(point, &matrix);
            ApplyMatrixSV(&matrix, &offset, target);
            target->vx += position.vx;
            target->vy += position.vy;
            target->vz += position.vz;
            rotation.vx = point->vx;
            rotation.vy = point->vy;
            rotation.vz = point->vz;
            vertex = vertices;
            size = 4096;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrixZYX(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&rotation, 0, 0, t << 7);
            position.vx = offset.vx;
            position.vy = offset.vy;
            position.vz = offset.vz;
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(vertex, &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vy += 4096 / config->launch * 4;
        }
        point++;
        target += 3;
    }
    t = work->elapsed - config->launch;
    if (t >= 0 && t < config->travel) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setRGB0(&quad, config->red, config->green, config->blue);
        point = &work->trails[0][work->index];
        target = work->targets;
        for (i = 0; i < 8; i++) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            position.vy -= config->arc * csin((t << 11) / config->travel) / 4096;
            setVector(&rotation, 0, 0, t << 7);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, (t / 2 + i) % 3 * 32, 0, (t / 2 + i) % 3 * 32 + 31, 0, (t / 2 + i) % 3 * 32, 31, (t / 2 + i) % 3 * 32 + 31, 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            next[i].vx = point->vx + (target->vx - point->vx) / (config->travel - t) * 2;
            next[i].vy = point->vy + (target->vy - point->vy) / (config->travel - t) * 2;
            next[i].vz = point->vz + (target->vz - point->vz) / (config->travel - t) * 2;
            point += 3;
            target++;
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    point = work->trails[0];
    for (i = 0; i < 8; i++) {
        t = work->elapsed - i * config->spacing;
        if (t >= 0 && work->elapsed < config->launch + config->travel) {
            for (j = 1; j < 3; j++) {
                if (t >= config->fade) {
                    red = config->red;
                    green = config->green;
                    blue = config->blue;
                } else {
                    red = config->red * t / config->fade;
                    green = config->green * t / config->fade;
                    blue = config->blue * t / config->fade;
                }
                quad.r0 = red * (3 - j) / 6;
                quad.g0 = green * (3 - j) / 6;
                quad.b0 = blue * (3 - j) / 6;
                position.vx = point[(work->index + j) % 3].vx;
                position.vy = point[(work->index + j) % 3].vy;
                position.vz = point[(work->index + j) % 3].vz;
                if (work->elapsed >= config->launch + j * 2) {
                    position.vy -= config->arc * csin(((work->elapsed - config->launch - j * 2) << 11) / config->travel) / 4096;
                }
                setVector(&rotation, 0, 0, t << 7);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                setUV4(&quad, (t / 2 + i) % 3 * 32, 0, (t / 2 + i) % 3 * 32 + 31, 0, (t / 2 + i) % 3 * 32, 31, (t / 2 + i) % 3 * 32 + 31, 31);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
        point += 3;
    }
    work->index = (work->index + 2) % 3;
    if (work->elapsed >= config->launch) {
        point = &work->trails[0][work->index];
        for (i = 0; i < 8; i++) {
            point->vx = (&next[i])->vx;
            point->vy = (&next[i])->vy;
            point->vz = (&next[i])->vz;
            point += 3;
        }
    }
    setVector(&vertices[0], -config->burst_size, -config->burst_size, 0);
    setVector(&vertices[1], config->burst_size, -config->burst_size, 0);
    setVector(&vertices[2], -config->burst_size, config->burst_size, 0);
    setVector(&vertices[3], config->burst_size, config->burst_size, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    point = work->targets;
    for (i = 0; i < 8; i++) {
        t = work->elapsed - (config->launch + config->travel + i * config->burst_spacing);
        if (t >= 0 && t < config->burst) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            setVector(&rotation, 0, 0, 0);
            size = (t << 12) / config->burst * 3 + 4096;
            setVector(&scale, size, size, size);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, (t / 2 + i) % 3 * 32, 0, (t / 2 + i) % 3 * 32 + 31, 0, (t / 2 + i) % 3 * 32, 31, (t / 2 + i) % 3 * 32 + 31, 31);
            red = config->red * (config->burst - t) / config->burst;
            green = config->green * (config->burst - t) / config->burst;
            blue = config->blue * (config->burst - t) / config->burst;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        point++;
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    t = work->elapsed - (config->launch + config->travel);
    if (t < 0) {
        return 0;
    }
    if (t > config->burst_spacing * 8 + config->burst) {
        return 2;
    }
    return 1;
}

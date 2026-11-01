#include "../../types.h"
#include "variant135_entry.h"
#include "../../game/screen_projection.h"

/* The entry throws 48 textured sparks from one slot part towards the other
 * slot, bursts beam crosses over the targets and then sweeps a ribbon of grid
 * quads between two pairs of slot parts. A negative command draws a frame;
 * any other command sets the work area up and prints its size. The French
 * header-152 and 282 images build this source without the printf. */

static const VECTOR gModelVariant135_D1680 = {4096, 4096, 4096, 0};

s32 ModelVariant135_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    Model135State *work;
    Model135Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, t, s, depth;
    SVECTOR *point;
    SVECTOR *target;
    s32 column, row;
    s32 height;
    u8 red, green, blue;
    SVECTOR *G32 *corner;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant135_D1680;
    work = (Model135State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant135_D17B8[command % 100];
        point = work->sparks;
        for (i = 0; i < 48; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point->pad = 0;
            point++;
        }
        point = work->targets;
        for (i = 0; i < 48; i++) {
            point->vx = rand() % config->spread - config->spread / 2;
            point->vy = -350;
            point->vz = direction * (config->spread * 2 + 450);
            point->pad = 0;
            point++;
        }
        point = work->cross;
        for (i = 0; i < 4; i++) {
            point->vx = rand() % config->cross_spread * 2 - config->cross_spread;
            point->vy = -config->cross_height;
            point->vz = rand() % config->cross_spread * 2 - config->cross_spread;
            point++;
        }
        point = work->waves;
        for (i = 0; i < 96; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point->pad = 0;
            point++;
        }
        point = work->grid;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                point->vx = j * config->grid;
                point->vy = i * config->grid;
                point->vz = 0;
                point++;
            }
        }
        point = work->beams;
        for (i = 0; i < 48; i++) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)point);
            point->vx += rand() % config->target_spread * 2 - config->target_spread;
            point->vy = rand() % config->target_spread - 350;
            point->vz += direction * (rand() % config->target_spread - config->target_spread / 2);
            point++;
        }
        work->quads[0][0] = &work->grid[0];
        work->quads[0][1] = &work->grid[1];
        work->quads[0][2] = &work->grid[3];
        work->quads[0][3] = &work->grid[4];
        work->quads[1][0] = &work->grid[2];
        work->quads[1][1] = &work->grid[1];
        work->quads[1][2] = &work->grid[5];
        work->quads[1][3] = &work->grid[4];
        work->quads[2][0] = &work->grid[8];
        work->quads[2][1] = &work->grid[7];
        work->quads[2][2] = &work->grid[5];
        work->quads[2][3] = &work->grid[4];
        work->quads[3][0] = &work->grid[6];
        work->quads[3][1] = &work->grid[7];
        work->quads[3][2] = &work->grid[3];
        work->quads[3][3] = &work->grid[4];
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant135_D16A0[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant135_D16A0[1]);
        for (i = 2; i < 10; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant135_D16A0[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model135State), sizeof(Model135State));
#endif
        return 0;
    }
    config = work->config;
    point = work->sparks;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    target = work->targets;
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    setVector(&rotation, 0, 0, 0);
    setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
    setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
    setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
    setVector(&vertices[3], config->spark_size, config->spark_size, 0);
    for (i = 0; i < 48; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
        } else if (t < config->rise + config->fade) {
            s = t * 0xE00 / config->rise + 0x200;
            setVector(&scale, s, s, s);
            if (t < config->rise) {
                quad.r0 = config->red;
                quad.g0 = config->green;
                quad.b0 = config->blue;
            } else {
                s = config->rise + config->fade - t;
                red = config->red * s / config->fade;
                green = config->green * s / config->fade;
                blue = config->blue * s / config->fade;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
            }
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            s = (t / 2 + i) % 8;
            column = s % 4 << 6;
            row = s / 4 * 87;
            quad.u0 = column;
            quad.v0 = row;
            quad.u1 = column + 63;
            quad.v1 = row;
            quad.u2 = column;
            quad.v2 = row + 86;
            quad.u3 = column + 63;
            quad.v3 = row + 86;
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (t < config->rise) {
                point->vx += (target->vx - point->vx) / (config->rise - t) * step;
                point->vy += (target->vy - point->vy) / (config->rise - t) * step;
                point->vz += (target->vz - point->vz) / (config->rise - t) * step;
            }
        }
        point++;
        target++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setUV4(&quad, 64, 174, 127, 174, 64, 237, 127, 237);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&vertices[0], -config->beam_size, -config->beam_size, 0);
    setVector(&vertices[1], config->beam_size, -config->beam_size, 0);
    setVector(&vertices[2], -config->beam_size, config->beam_size, 0);
    setVector(&vertices[3], config->beam_size, config->beam_size, 0);
    point = work->beams;
    for (i = 0; i < 48; i++) {
        t = work->elapsed - config->wave_delay - i * config->spacing - config->rise;
        if (t >= 0 && t < config->beam) {
            red = config->brightness * (config->beam - t) / config->beam;
            quad.r0 = red;
            quad.g0 = red;
            quad.b0 = red;
            target = work->cross;
            for (j = 0; j < 4; j++) {
                position.vx = target->vx;
                position.vy = target->vy;
                position.vz = target->vz;
                position.vx = position.vx * t;
                position.vy = position.vy * t;
                position.vz = position.vz * t;
                position.vx = position.vx / config->beam;
                position.vy = position.vy / config->beam;
                position.vz = position.vz / config->beam;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                target++;
            }
        }
        point++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 174, 63, 174, 0, 237, 63, 237);
    t = work->elapsed - config->wave_delay;
    if (t < 0) {
        point = work->anchors;
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->wave_part + i), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point++;
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->wave_end_part + i), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point++;
            point->vx = point[-2].vx;
            point->vy = point[-2].vy;
            point->vz = point[-2].vz;
            point->vx += point[-1].vx;
            point->vy += point[-1].vy;
            point->vz += point[-1].vz;
            point->vx = point->vx / 2;
            point->vy = point->vy / 2;
            point->vz = point->vz / 2;
            point++;
        }
    }
    point = work->waves;
    for (i = 0; i < 48; i++) {
        t = work->elapsed - config->wave_delay - i * config->wave_spacing;
        if (t < 0) {
            point->vx = work->anchors[2].vx;
            point->vy = work->anchors[2].vy;
            point->vz = work->anchors[2].vz;
            point->pad = ratan2(work->anchors[5].vz - work->anchors[2].vz, work->anchors[5].vy - work->anchors[2].vy) + direction * 1024;
        } else if (t < config->wave) {
            if (t < config->wave_stage) {
                s = t * 0xE00 / config->wave + 0x200;
            } else {
                s = t * 0xC00 / config->wave + 0x600;
            }
            setVector(&scale, s, s, s);
            setVector(&rotation, point->pad, 0, 0);
            red = config->wave_red;
            green = config->wave_green;
            blue = config->wave_blue;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            if (t < config->wave_stage) {
                position.vx += work->anchors[0].vx - work->anchors[2].vx;
            }
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            corner = work->quads[0];
            for (j = 0; j < 4; j++) {
                depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                corner += 4;
            }
            if (t < config->wave_stage) {
                position.vx += work->anchors[1].vx - work->anchors[0].vx;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                corner = work->quads[0];
                for (j = 0; j < 4; j++) {
                    depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    corner += 4;
                }
            }
            height = direction * 900;
            point->vz = work->anchors[2].vz + (height - work->anchors[2].vz) * t / config->wave;
            s = (height - work->anchors[2].vz) * config->wave_stage / config->wave
                * (work->anchors[5].vy - work->anchors[2].vy) / (work->anchors[5].vz - work->anchors[2].vz);
            if (t < config->wave_stage) {
                point->vy = work->anchors[2].vy + s * t / config->wave_stage;
            } else {
                point->vy = work->anchors[2].vy
                    + (-350 - work->anchors[2].vy - s) * csin((t - config->wave_stage) * 1024 / (config->wave - config->wave_stage)) / 4096
                    + s;
            }
        }
        point++;
    }
    PopMatrix();
    work->elapsed += step;
    t = work->elapsed - config->delay - config->spacing * 48;
    if (t >= config->rise + config->fade) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    t = work->elapsed - config->wave_delay - config->wave;
    return t >= 0 ? 4 : 0;
}

#include "../../types.h"
#include "variant155_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry sends five cards around an orbit in front of the other slot,
 * each trailing a column of fading sparks, spins a glowing cross beneath
 * them, lifts a ring of dust and flashes the screen at the end. A negative
 * command draws a frame; any other command sets the work area up and prints
 * its size. The French header-172 and 302 images build this source without
 * the printf and with a 256-line flash. */

static const VECTOR gModelVariant155_D1750 = {4096, 4096, 4096, 0};

s32 ModelVariant155_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[6]; /* card corners, the other slot's floor point, the next orbit point */
    PSXLONG flag, interpolation, depth;
    Model155State *work;
    Model155Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, k, t, r;
    SVECTOR *point, *offset, *ahead;
    SVECTOR *G32 (*quads)[4];
    s32 period;
    s32 lag;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant155_D1750;
    work = (Model155State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant155_D17FC[command % 100];
        point = work->path;
        for (i = 0; i < 256; i++) {
            point->vx = direction * config->radius * csin(i << 4) / 4096;
            point->vy = -config->lift + config->height * csin(i << 5) / 4096;
            point->vz = config->radius * ccos(i << 4) / 4096;
            point++;
        }
        for (i = 0; i < 5; i++) {
            work->phase[i] = 0;
        }
        point = work->sparks;
        for (i = 0; i < 80; i++) {
            point->vx = rand() % config->size * 2 - config->size;
            point->vy = rand() % config->size * 2 - config->size;
            point->vz = rand() % config->size * 2 - config->size;
            point++;
        }
        point = work->grid;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                point->vx = j * config->cross;
                point->vy = 0;
                point->vz = i * config->cross;
                point++;
            }
        }
        quads = work->quads;
        point = work->grid;
        quads[0][0] = point;
        quads[0][1] = &work->grid[1];
        quads[0][2] = &work->grid[3];
        quads[0][3] = &work->grid[4];
        quads[1][0] = &work->grid[2];
        quads[1][1] = &work->grid[5];
        quads[1][2] = &work->grid[1];
        quads[1][3] = &work->grid[4];
        quads[2][0] = &work->grid[8];
        quads[2][1] = &work->grid[7];
        quads[2][2] = &work->grid[5];
        quads[2][3] = &work->grid[4];
        quads[3][0] = &work->grid[6];
        quads[3][1] = &work->grid[3];
        quads[3][2] = &work->grid[7];
        quads[3][3] = &work->grid[4];
        point = work->dust;
        for (i = 0; i < 96; i++) {
            r = rand() % 4096;
            period = rand() % config->cross;
            point->vx = period * csin(r) / 4096;
            point->vy = 0;
            point->vz = period * ccos(r) / 4096;
            point++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant155_D1770[i]);
        }
        work->elapsed = 0;
        work->frame = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model155State), sizeof(Model155State));
#endif
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
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[4]);
    vertices[4].vy = 0;
    period = 0x300 / config->period;
    if (work->elapsed < config->end) {
        for (i = 0; i < 5; i++) {
            t = work->elapsed - config->delay - i * config->spacing;
            if (t >= 0) {
                for (j = 0; j < 16; j++) {
                    setVector(&rotation, 0, 0, 0);
                    r = (j << 8) + 0x1000;
                    setVector(&scale, r, r, r);
                    if (t < config->spacing) {
                        red = config->spark_red * t / config->spacing;
                        green = config->spark_green * t / config->spacing;
                        blue = config->spark_blue * t / config->spacing;
                    } else {
                        red = config->spark_red;
                        green = config->spark_green;
                        blue = config->spark_blue;
                    }
                    box.r = red * (16 - j) / 16;
                    box.g = green * (16 - j) / 16;
                    box.b = blue * (16 - j) / 16;
                    lag = j * (period << 1) - 0x100;
                    r = (work->phase[i] - lag) % 256;
                    position.vx = vertices[4].vx;
                    position.vy = vertices[4].vy;
                    position.vz = vertices[4].vz;
                    /* &work->path[r], spelled to keep the original address order */
                    offset = (SVECTOR *)(r * sizeof(SVECTOR) + (u32)work + 4);
                    position.vx += offset->vx;
                    position.vy += offset->vy;
                    position.vz += offset->vz;
                    position.vy += config->size + config->lift * (j + 1) / 16;
                    if (position.vy < 0 && ((j + work->frame) & 1) == 0) {
                        GsSetLsMatrix(&base);
                        RotTrans(&position, (VECTOR *)matrix.t, &flag);
                        RotMatrix(&rotation, &matrix);
                        MulMatrix2(&base, &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                        point = &work->sparks[i * 16];
                        for (k = 0; k < 16; k++) {
                            if (rand() % 100 < 70) {
                                depth = RotTransPers(point, (PSXLONG *)&box.x, &interpolation, &flag);
                                if (depth >= 0 && flag >= 0) {
                                    GsSortBoxFill(&box, ot, (u16)depth);
                                }
                            }
                            point++;
                        }
                    }
                }
            }
        }
    }
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[4]);
    vertices[4].vy = 0;
    if (work->elapsed < config->end) {
        for (i = 0; i < 5; i++) {
            t = work->elapsed - config->delay - i * config->spacing;
            if (t >= 0) {
                if (t < config->spacing) {
                    red = config->red * t / config->spacing;
                    green = config->green * t / config->spacing;
                    blue = config->blue * t / config->spacing;
                    SetSemiTrans(&quad, 1);
                } else {
                    red = config->red;
                    green = config->green;
                    blue = config->blue;
                    SetSemiTrans(&quad, 0);
                }
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
                setVector(&rotation, 0, 0, 0);
                position.vx = vertices[4].vx;
                position.vy = vertices[4].vy;
                position.vz = vertices[4].vz;
                position.vx += (work->path + work->phase[i])->vx;
                position.vy += (work->path + work->phase[i])->vy;
                position.vz += (work->path + work->phase[i])->vz;
                vertices[5].vx = vertices[4].vx;
                vertices[5].vy = vertices[4].vy;
                vertices[5].vz = vertices[4].vz;
                r = (work->phase[i] + 0x300 / config->period) % 256;
                ahead = (SVECTOR *)(r * sizeof(SVECTOR) + (u32)work + 4);
                vertices[5].vx += ahead->vx;
                vertices[5].vy += ahead->vy;
                vertices[5].vz += ahead->vz;
                GsSetLsMatrix(&base);
                r = RotAverageNclip3(&vertices[4], &vertices[5], &position, (PSXLONG *)&flash.x0, (PSXLONG *)&flash.x1,
                    (PSXLONG *)&flash.x2, &interpolation, &depth, &flag);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                if (r < 0) {
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                } else {
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x3, (PSXLONG *)&quad.x2,
                        &interpolation, &flag);
                }
                r = t % 3;
                setUV4(&quad, 0, r * 64, 63, r * 64, 0, r * 64 + 63, 63, r * 64 + 63);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                work->phase[i] = (work->phase[i] + 0x300 / config->period * step) % 256;
            }
        }
    }
    t = work->elapsed - config->delay;
    if (t >= 0 && work->elapsed < config->end) {
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 192, 63, 192, 0, 255, 63, 255);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[4]);
        vertices[4].vy = 0;
        position.vx = vertices[4].vx;
        position.vy = vertices[4].vy;
        position.vz = vertices[4].vz;
        r = (work->frame % 2 << 8) + 0x1000;
        rotation.vx = 0;
        setVector(&scale, r, r, r);
        rotation.vy = direction * config->spin * t % 4096 + 4096;
        rotation.vz = 0;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (work->frame & 1) {
            r = config->spacing * 5;
            if (t < r) {
                red = config->cross_red * t / r;
                green = config->cross_green * t / r;
                blue = config->cross_blue * t / r;
            } else {
                red = config->cross_red;
                green = config->cross_green;
                blue = config->cross_blue;
            }
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
        } else {
            quad.r0 = 0;
            quad.g0 = 0;
            quad.b0 = 0;
        }
        quads = work->quads;
        for (i = 0; i < 4; i++) {
            depth = RotAverage4((*quads)[0], (*quads)[1], (*quads)[2], (*quads)[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            quads++;
        }
    }
    if (work->elapsed < config->end) {
        quad.tpage = work->texture[4] >> 16;
        quad.clut = work->texture[4];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 128, 0, 159, 0, 128, 127, 159, 127);
        setVector(&vertices[0], -config->dust_step / 8, 0, 0);
        setVector(&vertices[1], 0, 0, 0);
        setVector(&vertices[2], config->dust_step / 8, 0, 0);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[4]);
        vertices[4].vy = 0;
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        point = work->dust;
        for (i = 0; i < 96; i++) {
            t = work->elapsed - config->delay - config->spacing * 5 - i * 3;
            if (t >= 0 && t < config->dust) {
                red = config->dust_red * (config->dust - t) / config->dust;
                green = config->dust_green * (config->dust - t) / config->dust;
                blue = config->dust_blue * (config->dust - t) / config->dust;
                position.vx = vertices[4].vx;
                position.vy = vertices[4].vy;
                position.vz = vertices[4].vz;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&quad.x3,
                    (PSXLONG *)&flash.x0, (PSXLONG *)&quad.x2, &interpolation, &flag);
                if (flag >= 0) {
                    position.vy += config->dust_step;
                    if (position.vy > 0) {
                        position.vy = 0;
                    }
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = (depth + RotAverage3(&vertices[0], &vertices[1], &vertices[2], (PSXLONG *)&quad.x1,
                        (PSXLONG *)&flash.x1, (PSXLONG *)&quad.x0, &interpolation, &flag)) / 2;
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
                point->vy -= config->dust_rise / config->dust * step;
            }
            point++;
        }
    }
    t = work->elapsed - config->end;
    if (t >= 0 && t < config->flash) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash - t) * 255 / config->flash;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frame++;
    t = work->elapsed - config->delay - config->spacing * 5;
    if (t < 0) {
        return 0;
    }
    t = work->elapsed - config->end;
    if (t < 0) {
        return 4;
    }
    if (t >= config->flash) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}

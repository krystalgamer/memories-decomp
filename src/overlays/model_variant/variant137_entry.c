#include "../../types.h"
#include "variant137_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry gathers 64 sparks onto one slot part, swells a ring there, lifts
 * an orb that spins and flies to the other slot, rains 16 drops after it and
 * flashes the screen. A negative command draws a frame; any other command
 * sets the work area up and prints its size. */

static const VECTOR D_8013CC14 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G4 shade;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR anchor;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model137State *work;
    Model137Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, t, s, r, depth;
    s32 fade, level;
    u8 red, green, blue;
    SVECTOR *point;
    SVECTOR *G32 *corner;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CC14;
    work = (Model137State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CC88[command % 100];
        point = work->sparks;
        for (i = 0; i < 64; i++) {
            s = rand() % 4096;
            r = rand() % config->spread;
            point->vx = r * ccos(s) / 4096;
            point->vy = rand() % config->height * 2 - config->height;
            point->vz = r * csin(s) / 4096;
            point->pad = 0;
            point++;
        }
        point = work->ring;
        for (i = 0; i < 33; i++) {
            point->vx = config->ring_radius * ccos(i << 7) / 4096;
            point->vy = 0;
            point->vz = config->ring_radius * csin(i << 7) / 4096;
            point->pad = 0;
            point++;
        }
        for (i = 0; i < 33; i++) {
            s = config->ring_radius + config->ring_width;
            point->vx = s * ccos(i << 7) / 4096;
            point->vy = 0;
            point->vz = s * csin(i << 7) / 4096;
            point->pad = 0;
            point++;
        }
        point = work->grid;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                point->vx = j * config->grid;
                point->vy = i * config->grid / 8;
                point->vz = i * config->grid;
                point++;
            }
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
        work->orb.vx = 0;
        work->orb.vy = 0;
        work->orb.vz = 0;
        work->angle = 0;
        point = work->drops;
        for (i = 0; i < 16; i++) {
            point->vx = direction * (config->drop_spread * (i << 1) / 16 - config->drop_spread);
            point->vy = -350;
            point->vz = direction * 450;
            point++;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CC34[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model137State), sizeof(Model137State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyG4(&shade);
    t = work->elapsed - config->spark_start;
    if (t >= 0 && t < config->spark) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
        setVector(&rotation, 0, 0, 0);
        setVector(&vertices[0], -config->height, -config->height, 0);
        setVector(&vertices[1], config->height, -config->height, 0);
        setVector(&vertices[2], -config->height, config->height, 0);
        setVector(&vertices[3], config->height, config->height, 0);
        GsSetLsMatrix(&base);
        point = work->sparks;
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        anchor.vx = matrix.t[0];
        anchor.vy = matrix.t[1];
        anchor.vz = matrix.t[2];
        s = (t << 12) / config->spark;
        setVector(&scale, s, s, s);
        s = config->spark - t;
        red = config->red * s / config->spark;
        green = config->green * s / config->spark;
        blue = config->blue * s / config->spark;
        quad.r0 = red;
        quad.g0 = green;
        quad.b0 = blue;
        for (i = 0; i < 64; i++) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            position.vx = position.vx * t;
            position.vy = position.vy * t;
            position.vz = position.vz * t;
            position.vx = position.vx / config->spark;
            position.vy = position.vy / config->spark;
            position.vz = position.vz / config->spark;
            position.vx += anchor.vx;
            position.vy += anchor.vy;
            position.vz += anchor.vz;
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
            point++;
        }
    }
    t = work->elapsed - config->spark_start;
    if (t >= 0 && t < config->spark) {
        setVector(&rotation, 0, 0, 0);
        s = t * 0xE00 / config->spark + 0x200;
        setVector(&scale, s, s, s);
        s = config->spark - t;
        red = config->red * s / config->spark;
        green = config->green * s / config->spark;
        blue = config->blue * s / config->spark;
        shade.r0 = red;
        shade.g0 = green;
        shade.b0 = blue;
        shade.r1 = red;
        shade.g1 = green;
        shade.b1 = blue;
        shade.r2 = 0;
        shade.g2 = 0;
        shade.b2 = 0;
        shade.r3 = 0;
        shade.g3 = 0;
        shade.b3 = 0;
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 66);
        for (i = 0; i < 32; i++) {
            shade.x0 = screen[i].vx;
            shade.y0 = screen[i].vy;
            shade.x1 = screen[i + 1].vx;
            shade.y1 = screen[i + 1].vy;
            s = i + 33;
            shade.x2 = screen[s].vx;
            shade.y2 = screen[s].vy;
            shade.x3 = screen[i + 34].vx;
            shade.y3 = screen[i + 34].vy;
            depth = AverageZ4(depths[i], depths[i + 1], depths[s], depths[i + 34]);
            flag = (flags[i] | flags[i + 1] | flags[s] | flags[i + 34]) & 0x20;
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&shade, ot, (u16)depth, 1);
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 32, 63, 32, 0, 95, 63, 95);
    t = work->elapsed - config->orb_start;
    if (t < 0) {
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), 0), &matrix);
        work->orb.vx = matrix.t[0];
        work->orb.vy = matrix.t[1] - config->lift;
        work->orb.vz = matrix.t[2];
    } else {
        s = t / 2 % 2 * 256 + 4096;
        setVector(&scale, s, s, s);
        position.vx = work->orb.vx;
        position.vy = work->orb.vy;
        position.vz = work->orb.vz;
        if (t < config->grow) {
            setVector(&rotation, 0, 0, 0);
            red = config->orb_red * t / config->grow;
            green = config->orb_green * t / config->grow;
            blue = config->orb_blue * t / config->grow;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
        } else {
            setVector(&rotation, 0, work->angle, 0);
            work->angle = (work->angle + config->spin * t / config->grow) % 4096;
            t -= config->grow + config->hold;
            if (t >= 0 && t < config->flight) {
                position.vx -= direction * config->sway * csin((t << 11) / config->flight) / 4096;
                position.vy += (-350 - work->orb.vy) * csin((t << 10) / config->flight) / 4096;
                position.vz += (direction * 450 - work->orb.vz) * t / config->flight;
                quad.r0 = config->red;
                quad.g0 = config->green;
                quad.b0 = config->blue;
            }
            t -= config->flight;
            s = config->flight / 2;
            if (t >= 0 && t < s) {
                position.vx += direction * config->sway * csin((t << 11) / config->flight) / 4096;
                position.vy = -350;
                position.vz = direction * 450;
                red = config->orb_red * (s - t) / s;
                green = config->orb_green * (s - t) / s;
                blue = config->orb_blue * (s - t) / s;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
            }
        }
        s = work->elapsed - config->orb_start - config->grow - config->hold - config->flight - config->flight / 2;
        if (s < 0) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            corner = work->quads[0];
            for (i = 0; i < 4; i++) {
                depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                corner += 4;
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setUV4(&quad, 0, 96, 31, 96, 0, 223, 31, 223);
    setVector(&vertices[0], -config->drop_size / 2, -config->drop_size * 2, 0);
    setVector(&vertices[1], config->drop_size / 2, -config->drop_size * 2, 0);
    setVector(&vertices[2], -config->drop_size / 2, config->drop_size * 2, 0);
    setVector(&vertices[3], config->drop_size / 2, config->drop_size * 2, 0);
    point = work->drops;
    for (i = 0; i < 16; i++) {
        t = work->elapsed - config->drop_start - i * config->drop_spacing;
        if (t >= 0 && t < config->drop) {
            s = 4096;
            setVector(&scale, s, s, s);
            fade = config->drop - t;
            red = config->drop_red * fade / config->drop;
            green = config->drop_green * fade / config->drop;
            blue = config->drop_blue * fade / config->drop;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            position.vy -= config->drop_size * (t << 1) / config->drop;
            setVector(&rotation, 0, 0, 0);
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
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            position.vy += config->drop_size * ((t * 3) << 1) / config->drop;
            setVector(&rotation, 0, 0, 2048);
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
        }
        point++;
    }
    t = work->elapsed - config->spark_start;
    if (t >= 0 && t < config->flash) {
        red = (config->flash - t) * 255 / config->flash;
        shade.r0 = red / 2;
        shade.g0 = red / 2;
        shade.b0 = red / 2;
        shade.r1 = red;
        shade.g1 = red;
        shade.b1 = red;
        shade.r2 = red;
        shade.g2 = red;
        shade.b2 = red;
        shade.r3 = red;
        shade.g3 = red;
        shade.b3 = red;
        GsSetLsMatrix(&base);
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        vertices[0].vx = matrix.t[0];
        vertices[0].vy = matrix.t[1];
        vertices[0].vz = matrix.t[2];
        RotTransPers(&vertices[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
        setXY4(&shade, 0, 0, quad.x0, 0, 0, quad.y0, quad.x0, quad.y0);
        Graphics_SubmitDrawModePacket((u32 *)&shade, ot, 1, 1);
        setXY4(&shade, 319, 0, quad.x0, 0, 319, quad.y0, quad.x0, quad.y0);
        Graphics_SubmitDrawModePacket((u32 *)&shade, ot, 1, 1);
        setXY4(&shade, 319, GRAPHICS_DEFAULT_HEIGHT - 1, quad.x0, GRAPHICS_DEFAULT_HEIGHT - 1, 319, quad.y0, quad.x0, quad.y0);
        Graphics_SubmitDrawModePacket((u32 *)&shade, ot, 1, 1);
        setXY4(&shade, 0, GRAPHICS_DEFAULT_HEIGHT - 1, quad.x0, GRAPHICS_DEFAULT_HEIGHT - 1, 0, quad.y0, quad.x0, quad.y0);
        Graphics_SubmitDrawModePacket((u32 *)&shade, ot, 1, 1);
    }
    t = work->elapsed - config->drop_start;
    if (t >= 0 && t < config->flash) {
        level = (config->flash - t) * 255 / config->flash;
        setXY4(&shade, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        setRGB0(&shade, level, level, level);
        setRGB1(&shade, level, level, level);
        setRGB2(&shade, level, level, level);
        setRGB3(&shade, level, level, level);
        Graphics_SubmitDrawModePacket((u32 *)&shade, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    t = work->elapsed - config->drop_start;
    if (t < 0) {
        return 0;
    }
    if (t >= config->drop_spacing * 16 + config->drop) {
        return 2;
    }
    if (t < config->drop_spacing * 16) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 0;
}

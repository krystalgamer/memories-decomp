#include "../../types.h"
#include "variant154_entry.h"
#include "../../game/graphics_constants.h"

/* The entry sends a trail of grid copies from one of the slot's parts
 * towards a point in front of the other slot, pulses a ring on the part and
 * on that point, joins them with a beam of ring segments, bursts rings and
 * grids out of the point and ends with a screen flash. A negative command
 * draws a frame; any other command sets the work area up and prints its
 * size. The French header-171 and 301 images build this source without the
 * printf and with the PAL screen height. */

static const VECTOR gModelVariant154_D2394 = {4096, 4096, 4096, 0};

s32 ModelVariant154_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model154State *work;
    Model154Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, k, t, value, depth, zoom, other;
    u8 red, green, blue;
    SVECTOR *point, *previous;
    SVECTOR *G32 *p;
    SVECTOR *G32 (*q)[4];
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant154_D2394;
    work = (Model154State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->grid;
        config = work->config = &gModelVariant154_D2408[command % 100];
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++) {
                value = config->size * 48 / 80;
                point->vx = value * (j << 1) / 4 - value;
                point->vy = config->size * (i << 1) / 4 - config->size;
                point->vz = direction * config->size / 4 * csin(j << 9) / 4096 * csin(i << 9) / 4096;
                point++;
            }
        }
        q = work->quads;
        point = work->grid;
        previous = &work->grid[5];
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                (*q)[0] = point;
                (*q)[1] = point + 1;
                (*q)[2] = previous;
                (*q)[3] = previous + 1;
                q++;
                point++;
                previous++;
            }
            point++;
            previous++;
        }
        point = work->ring;
        value = config->radius * 2 / 3;
        for (i = 0; i < 17; i++) {
            point->vx = value * ccos(i << 8) / 4096;
            point->vy = value * csin(i << 8) / 4096;
            point->vz = 0;
            point++;
        }
        point = work->rings;
        for (i = 0; i < 33; i++) {
            point->vx = config->radius * ccos(i << 7) / 4096;
            point->vy = config->radius * csin(i << 7) / 4096;
            point->vz = 0;
            point++;
        }
        value = config->radius + config->spread;
        for (i = 0; i < 33; i++) {
            point->vx = value * ccos(i << 7) / 4096;
            point->vy = value * csin(i << 7) / 4096;
            point->vz = -direction * config->depth;
            point++;
        }
        work->head = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant154_D23B4[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model154State), sizeof(Model154State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&burst);
    t = work->elapsed - config->delay;
    GsSetLsMatrix(&base);
    GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    vertices[0].vx = matrix.t[0];
    vertices[0].vy = matrix.t[1];
    vertices[0].vz = matrix.t[2];
    if (t < 0) {
        point = work->trail;
        for (i = 0; i < 8; i++) {
            point->vx = vertices[0].vx;
            point->vy = vertices[0].vy;
            point->vz = vertices[0].vz;
            point->pad = 0;
            point++;
        }
    } else if (t < (config->period + 16) * config->count) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        setUV4(&quad, 0, 0, 47, 0, 0, 79, 47, 79);
        SetSemiTrans(&quad, 1);
        value = ratan2(vertices[0].vy + 350, direction * 450 - vertices[0].vz) - ((Model_GetActiveSlotIndex() ^ 1) << 11);
        setVector(&rotation, value, 0, 0);
        point = &work->trail[work->head];
        previous = &work->trail[(work->head + 7) % 8];
        if (t % (config->period + 16) < config->period) {
            point->vx = previous->vx;
            point->vy = previous->vy;
            point->vz = previous->vz;
            point->vx += (0 - previous->vx) / (config->period - t % (config->period + 16)) * step;
            point->vy += (-350 - previous->vy) / (config->period - t % (config->period + 16)) * step;
            point->vz += (direction * 450 - previous->vz) / (config->period - t % (config->period + 16)) * step;
            point->pad = 1;
        } else {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point->pad = 0;
        }
        work->head = (work->head + 1) % 8;
        for (k = 0; k < 8; k++) {
            point = &work->trail[(work->head + k) % 8];
            if (point->pad != 0) {
                zoom = 4096;
                setVector(&scale, zoom, zoom, zoom);
                red = config->red * k / 8;
                green = config->green * k / 8;
                blue = config->blue * k / 8;
                setRGB0(&quad, red, green, blue);
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                q = work->quads;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                value = 12;
                zoom = 20;
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 4; j++) {
                        depth = RotAverage4((*q)[0], (*q)[1], (*q)[2], (*q)[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
                        setUV4(&quad, j * value, i * zoom, j * value + (value - 1), i * zoom,
                            j * value, i * zoom + (zoom - 1), j * value + (value - 1), i * zoom + (zoom - 1));
                        if (depth >= 0 && flag >= 0) {
                            GsSortPoly(&quad, ot, (u16)depth);
                        }
                        q++;
                    }
                }
            }
        }
    }
    t = work->elapsed - config->delay;
    if (t >= 0 && t < (config->period + 16) * config->count) {
        setVector(&vertices[0], -config->radius, -config->radius, 0);
        setVector(&vertices[1], config->radius, -config->radius, 0);
        setVector(&vertices[2], -config->radius, config->radius, 0);
        setVector(&vertices[3], config->radius, config->radius, 0);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        SetSemiTrans(&quad, 1);
        setVector(&rotation, 0, 0, t << 7);
        value = (work->frames % 2 << 8) + 4096;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        if (t < (config->period + 16) * (config->count - 1)) {
            red = config->ring_red;
            green = config->ring_green;
            blue = config->ring_blue;
        } else {
            value = (config->period + 16) * config->count - t;
            red = config->ring_red * value / (config->period + 16);
            green = config->ring_green * value / (config->period + 16);
            blue = config->ring_blue * value / (config->period + 16);
        }
        setRGB0(&quad, red, green, blue);
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
    t = work->elapsed - config->delay;
    if (t >= 0 && t < (config->period + 16) * config->count) {
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&vertices[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        setVector(&vertices[1], 0, -350, direction * 450);
        RotTransPers(&vertices[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
        RotTransPers(&vertices[1], (PSXLONG *)&quad.x1, &interpolation, &flag);
        k = ((quad.x1 - quad.x0) * (quad.x1 - quad.x0) + (quad.y1 - quad.y0) * (quad.y1 - quad.y0)) / 4096;
        /* The shifted test is always false; it keeps the squared distance stored. */
        if ((u32)t >> 31 >> 1) {
            t += k;
        }
        value = ratan2(vertices[0].vy - vertices[1].vy, vertices[1].vz - vertices[0].vz) + (Model_GetActiveSlotIndex() << 11);
        setVector(&rotation, value, 0, 0);
        value = (work->frames % 2 << 8) + 4096;
        setVector(&position, vertices[0].vx, vertices[0].vy, vertices[0].vz);
        setVector(&scale, value, value, value);
        k = 32;
        if (t < (config->period + 16) * (config->count - 1)) {
            red = config->beam_red;
            green = config->beam_green;
            blue = config->beam_blue;
        } else {
            value = (config->period + 16) * config->count - t;
            red = config->beam_red * value / (config->period + 16);
            green = config->beam_green * value / (config->period + 16);
            blue = config->beam_blue * value / (config->period + 16);
        }
        setRGB0(&flash, red, green, blue);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->ring, screen, depths, projection, flags, 17);
        value = 1;
        for (i = 0; i < k; i++) {
            if (i < k - 1) {
                position.vx += (vertices[1].vx - vertices[0].vx) / k;
                position.vy += (vertices[1].vy - vertices[0].vy) / k;
                position.vz += (vertices[1].vz - vertices[0].vz) / k;
            } else {
                position.vx = vertices[1].vx;
                position.vy = vertices[1].vy;
                position.vz = vertices[1].vz;
            }
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            zoom = value * 17;
            other = (value ^ 1) * 17;
            RotTransPersN(work->ring, &screen[zoom], &depths[zoom], &projection[zoom], &flags[zoom], 17);
            sa = &screen[other];
            sb = &screen[zoom];
            da = &depths[other];
            db = &depths[zoom];
            fa = &flags[other];
            fb = &flags[zoom];
            for (j = 0; j < 16; j++) {
                setXY4(&flash, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                depth = AverageZ4(da[0], da[1], db[0], db[1]);
                flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)depth, 1);
                }
                sa++;
                sb++;
                da++;
                db++;
                fa++;
                fb++;
            }
            value ^= 1;
        }
    }
    t = work->elapsed - config->delay;
    if (t >= 0 && t < (config->period + 16) * config->count) {
        setVector(&vertices[0], -config->radius, -config->radius, 0);
        setVector(&vertices[1], config->radius, -config->radius, 0);
        setVector(&vertices[2], -config->radius, config->radius, 0);
        setVector(&vertices[3], config->radius, config->radius, 0);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        SetSemiTrans(&quad, 1);
        setVector(&rotation, 0, 0, t << 7);
        value = (work->frames % 2 << 10) + 4096;
        setVector(&position, 0, -350, direction * 450);
        setVector(&scale, value, value, value);
        if (t < (config->period + 16) * (config->count - 1)) {
            red = config->ring_red;
            green = config->ring_green;
            blue = config->ring_blue;
        } else {
            value = (config->period + 16) * config->count - t;
            red = config->ring_red * value / (config->period + 16);
            green = config->ring_green * value / (config->period + 16);
            blue = config->ring_blue * value / (config->period + 16);
        }
        setRGB0(&quad, red, green, blue);
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
    if (work->elapsed - config->delay - config->period < 0) {
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&vertices[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        setVector(&vertices[1], 0, -350, direction * 450);
        work->heading = ratan2(vertices[0].vy - vertices[1].vy, vertices[1].vz - vertices[0].vz) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
    }
    burst.tpage = work->texture[0] >> 16;
    burst.clut = work->texture[0];
    setUV4(&burst, 0, 80, 31, 80, 0, 207, 31, 207);
    SetSemiTrans(&burst, 1);
    setVector(&rotation, work->heading, 0, 0);
    setVector(&position, 0, -350, direction * 450);
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay - (config->period + 16) * i - config->period;
        if (t >= 0 && t < config->burst) {
            value = (t << 12) / config->burst + 4096;
            setVector(&scale, value, value, value);
            red = ((config->burst - t) << 7) / config->burst;
            setRGB0(&burst, red, red, red);
            setRGB1(&burst, red, red, red);
            red = config->burst_red * (config->burst - t) / config->burst;
            green = config->burst_green * (config->burst - t) / config->burst;
            blue = config->burst_blue * (config->burst - t) / config->burst;
            setRGB2(&burst, red, green, blue);
            setRGB3(&burst, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->rings, screen, depths, projection, flags, 66);
            sa = &screen[0];
            sb = &screen[33];
            da = &depths[0];
            db = &depths[33];
            fa = &flags[0];
            fb = &flags[33];
            for (j = 0; j < 32; j++) {
                setXY4(&burst, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                depth = AverageZ4(da[0], da[1], db[0], db[1]);
                flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&burst, ot, (u16)depth);
                }
                sa++;
                sb++;
                da++;
                db++;
                fa++;
                fb++;
            }
        }
    }
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    setUV4(&quad, 0, 0, 47, 0, 0, 79, 47, 79);
    SetSemiTrans(&quad, 1);
    GsSetLsMatrix(&base);
    GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&vertices[0], matrix.t[0], matrix.t[1], matrix.t[2]);
    value = ratan2(vertices[0].vy + 350, direction * 450 - vertices[0].vz) - ((Model_GetActiveSlotIndex() ^ 1) << 11);
    setVector(&rotation, value, 0, 0);
    setVector(&position, 0, -350, direction * 450);
    for (k = 0; k < config->count; k++) {
        t = work->elapsed - config->delay - (config->period + 16) * k - config->period;
        if (t >= 0 && t < config->burst) {
            zoom = (t << 14) / config->burst + 4096;
            setVector(&scale, zoom, zoom, zoom);
            red = config->red * (config->burst - t) / config->burst;
            green = config->green * (config->burst - t) / config->burst;
            blue = config->blue * (config->burst - t) / config->burst;
            setRGB0(&quad, red, green, blue);
            value = 12;
            zoom = 20;
            q = work->quads;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    depth = RotAverage4((*q)[0], (*q)[1], (*q)[2], (*q)[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    setUV4(&quad, j * value, i * zoom, j * value + (value - 1), i * zoom,
                        j * value, i * zoom + (zoom - 1), j * value + (value - 1), i * zoom + (zoom - 1));
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    q++;
                }
            }
        }
    }
    t = work->elapsed - config->flash_delay;
    if (t >= 0 && t < config->flash) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash - t) * 255 / config->flash;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    t = work->elapsed - config->delay - config->period;
    if (t < 0) {
        return 0;
    }
    t -= config->count * (config->period + 16);
    if (t < 0) {
        return 4;
    }
    if (t >= config->burst) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}

#include "../../types.h"
#include "variant169_entry.h"
#include "../../game/graphics_constants.h"

/* The header-154 effect without the closing grid burst: a trail of
 * growing grid copies travels from one of the slot's parts towards a point
 * in front of the other slot, rings pulse on the part and on that point, a
 * widening beam of ring segments joins them, rings burst out of the point and
 * a screen flash ends it. A negative command draws a frame; any other command
 * sets the work area up and prints its size. */

static const VECTOR D_8013D0C4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
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
    Model169State *work;
    Model169Config *config;
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
    scale = D_8013D0C4;
    work = (Model169State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->grid;
        config = work->config = &D_8013D138[command % 100];
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++) {
                value = config->grid_size;
                point->vx = value * (j << 1) / 4 - value;
                point->vy = config->grid_size * (i << 1) / 4 - config->grid_size;
                point->vz = direction * config->grid_size / 4 * csin(j << 9) / 4096 * csin(i << 9) / 4096;
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
        point = work->beam_ring;
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
        value = config->radius + config->ring_width;
        for (i = 0; i < 33; i++) {
            point->vx = value * ccos(i << 7) / 4096;
            point->vy = value * csin(i << 7) / 4096;
            point->vz = -direction * config->depth;
            point++;
        }
        work->cursor = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D0E4[i]);
        }
        work->elapsed = 0;
        work->frame = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model169State), sizeof(Model169State));
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
    if (t < 0) {
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        vertices[0].vx = matrix.t[0];
        vertices[0].vy = matrix.t[1];
        vertices[0].vz = matrix.t[2];
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
        setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
        SetSemiTrans(&quad, 1);
        /*
         * vertices is a local that only the waiting branch above fills, so while the
         * trail is moving retail reads stale stack data here (whatever a previous call
         * left in that slot); the C mirrors the uninitialized read.
         */
        value = ratan2(-350 - vertices[0].vy, direction * 450 - vertices[0].vz) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
        setVector(&rotation, value, 0, 0);
        point = &work->trail[work->cursor];
        previous = &work->trail[(work->cursor + 7) % 8];
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
        work->cursor = (work->cursor + 1) % 8;
        for (k = 0; k < 8; k++) {
            point = &work->trail[(work->cursor + k) % 8];
            if (point->pad != 0) {
                value = t % (config->period + 16) - (8 - k) * 2;
                zoom = value * 2048 / config->period + 2048;
                zoom = zoom < 0 ? 0 : zoom > 0x3000 ? 0x3000 : zoom;
                setVector(&scale, zoom, zoom, zoom);
                red = config->trail_red * k / 8;
                green = config->trail_green * k / 8;
                blue = config->trail_blue * k / 8;
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
                value = 16;
                zoom = 16;
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
        value = (work->frame % 2 << 8) + 2048;
        setVector(&scale, value, value, value);
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        if (t < (config->period + 16) * (config->count - 1)) {
            red = config->sprite_red;
            green = config->sprite_green;
            blue = config->sprite_blue;
        } else {
            value = (config->period + 16) * config->count - t;
            red = config->sprite_red * value / (config->period + 16);
            green = config->sprite_green * value / (config->period + 16);
            blue = config->sprite_blue * value / (config->period + 16);
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
        setVector(&vertices[1], 0, -350, direction * 450);
        setVector(&vertices[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        RotTransPers(&vertices[0], (PSXLONG *)&quad.x0, &interpolation, &flag);
        RotTransPers(&vertices[1], (PSXLONG *)&quad.x1, &interpolation, &flag);
        k = ((quad.x1 - quad.x0) * (quad.x1 - quad.x0) + (quad.y1 - quad.y0) * (quad.y1 - quad.y0)) / 4096;
        /* The shifted test is always false; it keeps the squared distance stored
         * and the reload registers in step with the retail code. */
        if ((u32)t >> 31 >> 1) {
            t += k + direction + step + config->count;
        }
        value = ratan2(vertices[0].vy - vertices[1].vy, vertices[1].vz - vertices[0].vz) + (Model_GetActiveSlotIndex() << 11);
        setVector(&rotation, value, 0, 0);
        value = 2048;
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
        RotTransPersN(work->beam_ring, screen, depths, projection, flags, 17);
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
            zoom = ((work->frame % 2 << 8) + 2048) * (i + 1) / k + 2048;
            setVector(&scale, zoom, zoom, zoom);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            zoom = value * 17;
            other = (value ^ 1) * 17;
            RotTransPersN(work->beam_ring, &screen[zoom], &depths[zoom], &projection[zoom], &flags[zoom], 17);
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
        value = (work->frame % 2 << 10) + 4096;
        setVector(&position, 0, -350, direction * 450);
        setVector(&scale, value, value, value);
        if (t < (config->period + 16) * (config->count - 1)) {
            red = config->sprite_red;
            green = config->sprite_green;
            blue = config->sprite_blue;
        } else {
            value = (config->period + 16) * config->count - t;
            red = config->sprite_red * value / (config->period + 16);
            green = config->sprite_green * value / (config->period + 16);
            blue = config->sprite_blue * value / (config->period + 16);
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
        setVector(&vertices[1], 0, -350, direction * 450);
        setVector(&vertices[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->angle = ratan2(vertices[0].vy - vertices[1].vy, vertices[1].vz - vertices[0].vz) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
    }
    burst.tpage = work->texture[0] >> 16;
    burst.clut = work->texture[0];
    setUV4(&burst, 0, 80, 31, 80, 0, 207, 31, 207);
    SetSemiTrans(&burst, 1);
    setVector(&rotation, work->angle, 0, 0);
    setVector(&position, 0, -350, direction * 450);
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay - (config->period + 16) * i - config->period;
        if (t >= 0 && t < config->ring_duration) {
            value = (t << 12) / config->ring_duration + 4096;
            setVector(&scale, value, value, value);
            red = ((config->ring_duration - t) << 7) / config->ring_duration;
            setRGB0(&burst, red, red, red);
            setRGB1(&burst, red, red, red);
            red = config->ring_red * (config->ring_duration - t) / config->ring_duration;
            green = config->ring_green * (config->ring_duration - t) / config->ring_duration;
            blue = config->ring_blue * (config->ring_duration - t) / config->ring_duration;
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
    t = work->elapsed - config->flash_delay;
    if (t >= 0 && t < config->flash_duration) {
        setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
            GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash_duration - t) * 255 / config->flash_duration;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frame++;
    t = work->elapsed - config->delay - config->period;
    if (t < 0) {
        return 0;
    }
    t -= config->count * (config->period + 16);
    if (t < 0) {
        return 4;
    }
    if (t >= config->ring_duration) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}

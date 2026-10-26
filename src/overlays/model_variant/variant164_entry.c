#include "../../types.h"
#include "variant164_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry draws particles that rise from one of the slot's parts, pulse
 * around their targets, gather onto a point next to the other slot and burst
 * out again behind a glow, a ring and a fading screen flash. A negative
 * command draws a frame; any other command sets the work area up and prints
 * its size. */

static const VECTOR D_8013CE60 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    PSXLONG flag, interpolation;
    Model164State *work;
    Model164Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 low, i, value, elevation, r, t, s, depth;
    u8 red, green, blue;
    SVECTOR *point, *target;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CE60;
    work = (Model164State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->targets;
        config = work->config = &D_8013CED4[command % 100];
        for (i = 0; i < config->count; i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            r = rand() % config->radius;
            point->vx = r * csin(value) / 4096 * csin(elevation) / 4096;
            point->vy = r * ccos(elevation) / 4096;
            point->vz = r * ccos(value) / 4096 * csin(elevation) / 4096;
            point++;
        }
        point = work->ring;
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->centre);
        work->centre.vy = -350;
        for (i = 0; i < 33; i++) {
            point->vx = config->ring_radius * ccos(i << 7) / 4096;
            point->vy = config->ring_radius * csin(i << 7) / 4096;
            point->vz = 0;
            point++;
        }
        for (i = 0; i < 33; i++) {
            point->vx = config->glow_size * ccos(i << 7) / 4096;
            point->vy = config->glow_size * csin(i << 7) / 4096;
            point->vz = 0;
            point++;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CE80[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
        work->mode = command / 100;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model164State), sizeof(Model164State));
#endif
        return 0;
    }
    config = work->config;
    point = work->positions;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    target = work->targets;
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point->pad = 0;
        } else if (t < config->rise + config->fade) {
            setVector(&position, point->vx, point->vy, point->vz);
            value = (t << 12) / config->rise;
            value = value < 0 ? 0 : value > 4096 ? 4096 : value;
            setVector(&scale, value, value, value);
            if (t < config->rise) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frames & 1) {
                value = config->rise + config->fade - t;
                red = config->red * value / config->fade;
                green = config->green * value / config->fade;
                blue = config->blue * value / config->fade;
            } else {
                red = 0;
                green = 0;
                blue = 0;
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
            setUV4(&quad, i % 3 * 32, i % 9 / 3 * 32, i % 3 * 32 + 31, i % 9 / 3 * 32,
                i % 3 * 32, i % 9 / 3 * 32 + 31, i % 3 * 32 + 31, i % 9 / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (t < config->rise) {
                point->vx += (target->vx / 2 - point->vx) / (config->rise - t) * step;
                point->vy += (target->vy / 2 - 350 - point->vy) / (config->rise - t) * step;
                point->vz += (target->vz / 2 - direction * 225 - point->vz) / (config->rise - t) * step;
            }
        }
        point++;
        target++;
    }
    quad.tpage = work->texture[2] >> 16;
    quad.clut = work->texture[2];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    point = work->positions;
    target = work->targets;
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay2 - i * config->spacing;
        value = work->elapsed - config->delay - i * config->spacing - config->rise - config->fade;
        if (value >= 0 && t < 0) {
            point->vx = target->vx;
            point->vy = target->vy;
            point->vz = target->vz;
            point->vx += work->centre.vx;
            point->vy += work->centre.vy;
            point->vz += work->centre.vz;
            point->pad = 0;
        } else if (t >= 0 && work->elapsed < config->start) {
            setVector(&position, point->vx, point->vy, point->vz);
            switch (work->mode) {
            case 1:
                value = ((work->frames % 2) << 10) + 4096;
                setVector(&scale, value, value, value);
                break;
            case 2:
                value = csin(((t % config->pulse) << 12) / config->pulse) / 8 + 4096;
                setVector(&scale, value, value, value);
                break;
            case 3:
                value = ((work->frames % 2) << 10) + 4096;
                setVector(&scale, value, value, value);
                Model_SetSlotPlaybackDirection(Model_GetActiveSlotIndex() ^ 1, 0);
                break;
            default:
                setVector(&scale, 4096, 4096, 4096);
                break;
            }
            if (t >= config->pulse) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frames & 1) {
                red = config->red * t / config->pulse;
                green = config->green * t / config->pulse;
                blue = config->blue * t / config->pulse;
            } else {
                red = 0;
                green = 0;
                blue = 0;
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
            setUV4(&quad, i % 3 * 32, i % 9 / 3 * 32, i % 3 * 32 + 31, i % 9 / 3 * 32,
                i % 3 * 32, i % 9 / 3 * 32 + 31, i % 3 * 32 + 31, i % 9 / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        point++;
        target++;
    }
    t = work->elapsed - config->start;
    if (t >= 0 && t < config->gather) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->size, -config->size, 0);
        setVector(&vertices[1], config->size, -config->size, 0);
        setVector(&vertices[2], -config->size, config->size, 0);
        setVector(&vertices[3], config->size, config->size, 0);
        setVector(&rotation, 0, 0, t * config->spin % 4096);
        point = work->positions;
        for (i = 0; i < config->count; i++) {
            setVector(&position, point->vx, point->vy, point->vz);
            value = ((config->gather - t) << 12) / config->gather;
            setVector(&scale, value, value, value);
            red = config->red;
            green = config->green;
            blue = config->blue;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, i % 3 * 32, i % 9 / 3 * 32, i % 3 * 32 + 31, i % 9 / 3 * 32,
                i % 3 * 32, i % 9 / 3 * 32 + 31, i % 3 * 32 + 31, i % 9 / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (work->centre.vx - point->vx) / (config->gather - t) * step;
            point->vy += (work->centre.vy - point->vy) / (config->gather - t) * step;
            point->vz += (work->centre.vz - point->vz) / (config->gather - t) * step;
            point++;
        }
    }
    t = work->elapsed - config->start - config->gather;
    if (t >= 0 && t < config->burst && work->mode == 3) {
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->size, -config->size, 0);
        setVector(&vertices[1], config->size, -config->size, 0);
        setVector(&vertices[2], -config->size, config->size, 0);
        setVector(&vertices[3], config->size, config->size, 0);
        point = work->positions;
        target = work->targets;
        setVector(&rotation, 0, 0, 0);
        for (i = 0; i < config->count; i++) {
            setVector(&position, point->vx, point->vy, point->vz);
            value = (t << 12) / config->burst;
            setVector(&scale, value, value, value);
            red = config->red * (config->burst - t) / config->burst;
            green = config->green * (config->burst - t) / config->burst;
            blue = config->blue * (config->burst - t) / config->burst;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, i % 3 * 32, i % 9 / 3 * 32, i % 3 * 32 + 31, i % 9 / 3 * 32,
                i % 3 * 32, i % 9 / 3 * 32 + 31, i % 3 * 32 + 31, i % 9 / 3 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (target->vx * 3 + work->centre.vx - point->vx) / (config->burst - t) * step;
            point->vy += (target->vy * 3 + work->centre.vy - point->vy) / (config->burst - t) * step;
            point->vz += (target->vz * 3 + work->centre.vz - point->vz) / (config->burst - t) * step;
            point++;
            target++;
        }
    }
    t = work->elapsed - config->start - config->gather;
    if (t >= 0 && t < config->burst) {
        Model_SetSlotPlaybackDirection(Model_GetActiveSlotIndex() ^ 1, 1);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->glow_size, -config->glow_size, 0);
        setVector(&vertices[1], config->glow_size, -config->glow_size, 0);
        setVector(&vertices[2], -config->glow_size, config->glow_size, 0);
        setVector(&vertices[3], config->glow_size, config->glow_size, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&position, work->centre.vx, work->centre.vy, work->centre.vz);
        value = (t << 12) / config->burst + 4096;
        setVector(&scale, value, value, value);
        red = config->glow_red * (config->burst - t) / config->burst;
        green = config->glow_green * (config->burst - t) / config->burst;
        blue = config->glow_blue * (config->burst - t) / config->burst;
        setRGB0(&quad, red, green, blue);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
            &interpolation, &flag);
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
        RotTransPersN(work->ring, screen, depths, projection, flags, 66);
        sa = screen;
        sb = &screen[33];
        da = depths;
        db = &depths[33];
        fa = flags;
        for (i = 0; i < 32; i++) {
            setXY4(&quad, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
            depth = AverageZ4(da[0], da[1], db[0], db[1]);
            low = fa[0];
            flag = (fa[1] | low) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            sa++;
            sb++;
            da++;
            db++;
            fa++;
        }
    }
    t = work->elapsed - config->start - config->gather;
    if (t >= 0 && t < config->burst) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        s = (config->burst - t) * 255 / config->burst;
        setRGB0(&flash, s, s, s);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    t = work->elapsed - config->start - config->gather;
    if (t < 0) {
        return 0;
    }
    if (config->burst < t) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}

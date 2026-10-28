#include "../../types.h"
#include "variant148_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry launches shells from one of the slot's parts that spin towards
 * points on a ring in front of the other slot, then burst into sparks behind
 * an optional screen flash. A negative command draws a frame; any other
 * command sets the work area up and prints its size. */

static const VECTOR D_8013C708 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_FT3 tri;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[30];
    u16 depths[30], projection[30], flags[30];
    PSXLONG flag, interpolation;
    Model148State *work;
    Model148Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, t;
    SVECTOR *point, *target;
    s32 k, r, value, elevation, depth;
    u8 red, green, blue;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C708;
    work = (Model148State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->points;
        point->vx = 0;
        config = work->config = &D_8013C744[command % 100];
        point->vy = 0;
        point->vz = -config->size / 2;
        point = &work->points[1];
        for (i = 1; i < 5; i++) {
            for (j = 0; j < 7; j++) {
                r = rand() % config->radius / 2;
                value = rand() % 4096;
                point->vx = config->radius * csin((i << 11) / 5) / 4096 * ccos((j << 12) / 6) / 4096 + r * csin(value) / 4096;
                point->vy = config->radius * csin((i << 11) / 5) / 4096 * csin((j << 12) / 6) / 4096;
                point->vz = -config->size / 2 + config->size * i / 4 + r * ccos(value) / 4096;
                point++;
            }
        }
        point->vx = 0;
        point->vy = 0;
        point->vz = config->size / 2 + config->size / 4;
        point = work->targets;
        for (i = 0; i < 24; i++) {
            r = rand() % config->spread / 2 + config->spread / 2;
            value = rand() % 4096;
            point->vx = r * ccos(value) / 4096;
            point->vy = r * csin(value) / 4096;
            point->vz = direction * (rand() % config->spread + 450);
            point++;
        }
        point = work->sparks;
        for (i = 0; i < 32; i++) {
            r = config->burst / 2 + rand() % config->burst / 2;
            value = rand() % 4096;
            elevation = rand() % 2048;
            point->vx = r * ccos(value) / 4096 * csin(elevation) / 4096;
            point->vy = r * ccos(elevation) / 4096;
            point->vz = r * csin(value) / 4096 * csin(elevation) / 4096;
            point++;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C728[i]);
        }
        work->elapsed = 0;
        if (command >= 100) {
            work->flash = 1;
        } else {
            work->flash = 0;
        }
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model148State), sizeof(Model148State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyFT3(&tri);
    SetSemiTrans(&quad, 1);
    SetSemiTrans(&tri, 1);
    point = work->positions;
    target = work->targets;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    tri.tpage = work->texture[0] >> 16;
    tri.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    setUV3(&tri, 32, 0, 0, 63, 63, 63);
    for (i = 0; i < 24; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = 0;
            point->vy = 0;
            point->vz = matrix.t[2];
            point->pad = rand() % 4096;
        } else if (t < config->rise) {
            red = config->red * t / config->rise;
            green = config->green * t / config->rise;
            blue = config->blue * t / config->rise;
            tri.r0 = quad.r0 = red;
            tri.g0 = quad.g0 = green;
            tri.b0 = quad.b0 = blue;
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            r = -direction * (config->rise - t) * config->roll % 4096;
            value = ratan2(matrix.t[1] + 350, direction * 450 - matrix.t[2]) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
            setVector(&rotation, value, 0, r);
            matrix.t[0] = 0;
            matrix.t[2] = 0;
            RotMatrix(&rotation, &matrix);
            GsSetLsMatrix(&matrix);
            RotTransSV(point, &position, &flag);
            setVector(&rotation, value, 0, point->pad);
            r = t * 0xC00 / config->rise + 0x400;
            setVector(&scale, r, r, r);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->points, screen, depths, projection, flags, 30);
            sa = &screen[1];
            sb = &screen[2];
            da = &depths[1];
            db = &depths[2];
            fa = &flags[1];
            fb = &flags[2];
            for (j = 0; j < 6; j++) {
                setXY3(&tri, screen[0].vx, screen[0].vy, sa->vx, sa->vy, sb->vx, sb->vy);
                depth = AverageZ3(depths[0], *da, *db);
                flag = (flags[0] | *fa | *fb) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&tri, ot, (u16)depth);
                }
                sa++;
                sb++;
                da++;
                db++;
                fa++;
                fb++;
            }
            sa = &screen[1];
            sb = &screen[8];
            da = &depths[1];
            db = &depths[8];
            fa = &flags[1];
            fb = &flags[8];
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 6; k++) {
                    setXY4(&quad, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                    depth = AverageZ4(da[0], da[1], db[0], db[1]);
                    flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    sa++;
                    sb++;
                    da++;
                    db++;
                    fa++;
                    fb++;
                }
                sa++;
                sb++;
                da++;
                db++;
                fa++;
                fb++;
            }
            sb = &screen[29];
            db = &depths[29];
            fb = &flags[29];
            for (j = 0; j < 6; j++) {
                setXY3(&tri, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb->vx, sb->vy);
                depth = AverageZ3(da[0], da[1], *db);
                flag = (fa[0] | fa[1] | *fb) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&tri, ot, (u16)depth);
                }
                sa++;
                da++;
                fa++;
            }
            point->vx += (target->vx - point->vx) / (config->rise - t) * step;
            point->vy += (target->vy - point->vy) / (config->rise - t) * step;
            point->vz += (target->vz - point->vz) / (config->rise - t) * step;
            point->pad = (point->pad + config->spin) % 4096;
        }
        point++;
        target++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
    setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
    setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
    setVector(&vertices[3], config->spark_size, config->spark_size, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->targets;
    for (i = 0; i < 24; i++) {
        t = work->elapsed - config->delay - i * config->spacing - config->rise;
        if (t >= 0 && t < config->fade) {
            target = work->sparks;
            for (j = 0; j < 32; j++) {
                red = config->red * (config->fade - t) / config->fade;
                green = config->green * (config->fade - t) / config->fade;
                blue = config->blue * (config->fade - t) / config->fade;
                setRGB0(&quad, red, green, blue);
                position.vx = target->vx;
                position.vy = target->vy;
                position.vz = target->vz;
                position.vx *= t;
                position.vy *= t;
                position.vz *= t;
                position.vx /= config->fade;
                position.vy /= config->fade;
                position.vz /= config->fade;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                position.vy -= 350;
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = t * config->roll + j * config->spin;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (j & 1) {
                    setUV4(&quad, 64, 0, 127, 0, 64, 63, 127, 63);
                } else {
                    setUV4(&quad, 64, 32, 95, 32, 64, 63, 95, 63);
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                target++;
            }
        }
        point++;
    }
    t = work->elapsed - config->delay;
    if (t >= 0 && t < config->flash && work->flash == 1) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash - t) * 255 / config->flash;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    t = work->elapsed - config->delay - config->rise;
    if (t < 0) {
        return 0;
    }
    t -= config->spacing * 24;
    if (t >= config->fade) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    return 4;
}

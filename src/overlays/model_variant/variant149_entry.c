#include "../../types.h"
#include "variant149_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* The entry gathers five spinning orbs on one slot part, flies each to a
 * point in front of the other slot, bursts it into a ring of light with a
 * spark and flashes the screen. A negative command draws a frame; any other
 * command sets the work area up and prints its size. The French header-166
 * and 296 images build this source without the printf and with a 256-line
 * flash. */

static const VECTOR gModelVariant149_D14B4 = {4096, 4096, 4096, 0};

s32 ModelVariant149_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 glow;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[51];
    u16 depths[51], projection[51], flags[51];
    PSXLONG flag, interpolation;
    Model149State *work;
    Model149Config *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, t;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;
    SVECTOR *point, *target, *angle;
    s32 r, value, depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant149_D14B4;
    work = (Model149State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &gModelVariant149_D15EC[command % 100];
        point = work->targets;
        for (i = 0; i < 5; i++) {
            r = rand() % config->spread / 2 + config->spread / 2;
            value = rand() % 4096;
            point->vx = r * ccos(value) / 4096;
            point->vy = r * csin(value) / 4096 - 350;
            point->vz = direction * (rand() % config->spread + 450);
            point++;
        }
        point = work->ring;
        for (i = 0; i < 17; i++) {
            point->vx = config->inner * ccos(i << 8) / 4096;
            point->vy = config->inner * csin(i << 8) / 4096;
            point->vz = 0;
            point++;
        }
        for (i = 0; i < 17; i++) {
            point->vx = config->outer * ccos(i << 8) / 4096;
            point->vy = config->outer * csin(i << 8) / 4096;
            point->vz = -direction * config->depth;
            point++;
        }
        r = config->inner + (config->outer - config->inner) / 2;
        for (i = 0; i < 17; i++) {
            point->vx = r * ccos(i << 8) / 4096;
            point->vy = r * csin(i << 8) / 4096;
            point->vz = -direction * config->depth * r / config->outer;
            point++;
        }
        for (i = 0; i < 10; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant149_D14D4[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model149State), sizeof(Model149State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&glow);
    SetSemiTrans(&quad, 0);
    quad.tpage = work->texture[9] >> 16;
    quad.clut = work->texture[9];
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    point = work->orbs;
    target = work->targets;
    angle = work->rotations;
    for (i = 0; i < 5; i++) {
        t = work->elapsed - config->delay[i];
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point->pad = rand() % 4096;
            work->frames[i] = rand() % 4;
            angle->vx = ratan2(matrix.t[1] - target->vy, target->vz - matrix.t[2]) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
            angle->vy = ratan2(target->vx - matrix.t[0], target->vz - matrix.t[2]) + ((Model_GetActiveSlotIndex() ^ 1) << 11);
            angle->vz = 0;
        } else if (t < config->flight) {
            red = config->red;
            green = config->green;
            blue = config->blue;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            setVector(&rotation, 0, 0, point->pad);
            r = t * 0xC00 / config->flight + 0x400;
            setVector(&scale, r, r, r);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            r = work->frames[i];
            setUV4(&quad, r % 2 * 64, r / 2 * 64, r % 2 * 64 + 63, r / 2 * 64,
                r % 2 * 64, r / 2 * 64 + 63, r % 2 * 64 + 63, r / 2 * 64 + 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            point->vx += (target->vx - point->vx) / (config->flight - t) * step;
            point->vy += (target->vy - point->vy) / (config->flight - t) * step;
            point->vz += (target->vz - point->vz) / (config->flight - t) * step;
            point->pad = (point->pad + config->spin * step) % 4096;
            work->frames[i] = (work->frames[i] + 1) % 4;
        }
        point++;
        target++;
        angle++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&vertices[0], -config->burst_size, -config->burst_size, 0);
    setVector(&vertices[1], config->burst_size, -config->burst_size, 0);
    setVector(&vertices[2], -config->burst_size, config->burst_size, 0);
    setVector(&vertices[3], config->burst_size, config->burst_size, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    for (i = 0; i < 5; i++) {
        t = work->elapsed - config->delay[i];
        if (t >= 0 && t < config->burst) {
            GsSetLsMatrix(&base);
            GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            r = (t << 3) / config->burst;
            setUV4(&quad, r % 4 * 64, r / 4 * 64 + 128, r % 4 * 64 + 63, r / 4 * 64 + 128,
                r % 4 * 64, r / 4 * 64 + 191, r % 4 * 64 + 63, r / 4 * 64 + 191);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 128, 0, 159, 0, 128, 127, 159, 127);
    SetSemiTrans(&glow, 1);
    glow.tpage = work->texture[0] >> 16;
    glow.clut = work->texture[0];
    setUV4(&glow, 128, 0, 159, 0, 128, 127, 159, 127);
    point = work->targets;
    target = work->rotations;
    for (i = 0; i < 5; i++) {
        t = work->elapsed - config->delay[i] - config->flight;
        if (t >= 0 && t < config->ring) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            rotation.vx = target->vx;
            rotation.vy = target->vy;
            rotation.vz = target->vz;
            r = (t << 12) / config->ring;
            setVector(&scale, r, r, r);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 51);
            red = config->ring_red * (config->ring - t) / config->ring;
            green = config->ring_green * (config->ring - t) / config->ring;
            blue = config->ring_blue * (config->ring - t) / config->ring;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            sa = screen;
            sb = &screen[17];
            da = depths;
            db = &depths[17];
            fa = flags;
            fb = &flags[17];
            for (j = 0; j < 16; j++) {
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
            red = (config->ring - t) * 255 / config->ring;
            setRGB0(&glow, red, red, red);
            setRGB1(&glow, red, red, red);
            setRGB2(&glow, 0, 0, 0);
            setRGB3(&glow, 0, 0, 0);
            sa = screen;
            sb = &screen[34];
            da = depths;
            db = &depths[34];
            fa = flags;
            fb = &flags[34];
            for (j = 0; j < 16; j++) {
                setXY4(&glow, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                depth = AverageZ4(da[0], da[1], db[0], db[1]);
                flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&glow, ot, (u16)depth);
                }
                sa++;
                sb++;
                da++;
                db++;
                fa++;
                fb++;
            }
        }
        point++;
        target++;
    }
    for (i = 0; i < 5; i++) {
        t = work->elapsed - config->delay[i] - config->flight;
        if (t >= 0 && t < config->ring) {
            setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
            red = (config->ring - t) * 255 / config->ring;
            setRGB0(&flash, red, red, red);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        }
    }
    PopMatrix();
    work->elapsed += step;
    t = work->elapsed - config->delay[0] - config->flight;
    if (t < 0) {
        return 0;
    }
    for (i = 0; i < 5; i++) {
        t = work->elapsed - config->delay[i] - config->flight;
        if (t >= config->ring && i == 4) {
            return 2;
        }
        if (t >= 0 && work->completed == i) {
            work->completed = i + 1;
            if (work->completed < 5) {
                return 4;
            }
            return 1;
        }
    }
    return 0;
}

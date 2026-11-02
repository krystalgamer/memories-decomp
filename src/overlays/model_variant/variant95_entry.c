#include "../../types.h"
#include "variant95_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 95's entry: a textured sphere grows on one of the slot's parts and
 * flies to a point in front of it, then a widening beam of quads streams
 * towards the other slot behind an optional screen flash. A non-negative
 * command sets the work area up; a negative command draws a frame and
 * returns 0 until the beam starts, 4 while it lasts, 1 on the first frame
 * after that and 2 later. */

static const VECTOR gModelVariant95_D13E4 = {4096, 4096, 4096, 0};

s32 ModelVariant95_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_FT3 tri;
    POLY_GT4 beam;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[498];
    u16 depths[498], projection[498], flags[498];
    PSXLONG flag;
    Model95State *work;
    s32 direction;
    GsOT *ot;
    Model95Config *config;
    /* k is shared between the scale, the pole index and the quad index as in
     * the original; splitting it changes the register allocation. */
    s32 i, j, t, k;
    SVECTOR *point;
    u8 red, green, blue;
    s32 depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant95_D13E4;
    work = (Model95State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &gModelVariant95_D142C[command];
        point = work->points;
        work->points[0].vx = 0;
        point->vy = 0;
        point->vz = direction * config->radius;
        point = &work->points[1];
        for (i = 1; i < 16; i++) {
            for (j = 0; j < 33; j++) {
                point->vx = config->radius * csin(i << 7) / 4096 * ccos(j << 7) / 4096;
                point->vy = config->radius * csin(i << 7) / 4096 * csin(j << 7) / 4096;
                point->vz = direction * config->radius * ccos(i << 7) / 4096;
                point++;
            }
        }
        point->vx = 0;
        point->vy = 0;
        point->vz = -direction * config->radius;
        point = work->ring;
        for (i = 0; i < 33; i++) {
            point->vx = config->radius * ccos(i << 7) / 4096;
            point->vy = config->radius * csin(i << 7) / 4096;
            point->vz = 0;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant95_D13F4[i]);
        }
        work->elapsed = -config->start;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        PushMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        work->position.vx = matrix.t[0];
        work->position.vy = matrix.t[1];
        work->position.vz = matrix.t[2];
        PopMatrix();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyFT3(&tri);
    SetPolyGT4(&beam);
    SetPolyF4(&flash);
    t = work->elapsed - config->delay;
    if (t >= 0 && t < config->grow + config->life) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        tri.tpage = work->texture[1] >> 16;
        tri.clut = work->texture[1];
        if (t < config->grow) {
            SetSemiTrans(&quad, 0);
            SetSemiTrans(&tri, 0);
            quad.r0 = config->red;
            quad.g0 = config->green;
            quad.b0 = config->blue;
            tri.r0 = config->red;
            tri.g0 = config->green;
            tri.b0 = config->blue;
        } else {
            SetSemiTrans(&quad, 1);
            SetSemiTrans(&tri, 1);
            red = config->red * (config->life - t + config->grow) / config->life;
            green = config->green * (config->life - t + config->grow) / config->life;
            blue = config->blue * (config->life - t + config->grow) / config->life;
            tri.r0 = quad.r0 = red;
            tri.g0 = quad.g0 = green;
            tri.b0 = quad.b0 = blue;
        }
        GsSetLsMatrix(&base);
        position.vx = work->position.vx;
        position.vy = work->position.vy;
        position.vz = work->position.vz;
        if (t < config->grow) {
            rotation.vx = ratan2(work->position.vy + 350, direction * 450 - work->position.vz);
            rotation.vy = ratan2(work->position.vx, direction * 450 - work->position.vz);
        } else {
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
        }
        if (t < config->grow) {
            k = t * 0xE00 / config->grow + 0x200;
            scale.vx = k;
            scale.vy = k;
            scale.vz = k;
        } else {
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = (config->life - t + config->grow) * 4096 / config->life;
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->points, screen, depths, projection, flags, 497);
        k = 496;
        for (i = 0; i < 32; i++) {
            setXY3(&tri, screen[0].vx, screen[0].vy, screen[i + 1].vx, screen[i + 1].vy, screen[i + 2].vx, screen[i + 2].vy);
            setUV3(&tri, 32, 63 - t * 4 % 64, 0, 126 - t * 4 % 64, 63, 126 - t * 4 % 64);
            depth = AverageZ3(depths[0], depths[i + 1], depths[i + 2]);
            flag = (flags[0] | flags[i + 1] | flags[i + 2]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&tri, ot, (u16)depth);
            }
            setXY3(&tri, screen[i + 463].vx, screen[i + 463].vy, screen[i + 464].vx, screen[i + 464].vy, screen[k].vx, screen[k].vy);
            setUV3(&tri, 0, 63 - t * 4 % 64, 63, 63 - t * 4 % 64, 32, 126 - t * 4 % 64);
            depth = AverageZ3(depths[i + 463], depths[i + 464], depths[k]);
            flag = (flags[i + 463] | flags[i + 464] | flags[k]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&tri, ot, (u16)depth);
            }
        }
        for (j = 0; j < 14; j++) {
            for (i = 0; i < 32; i++) {
                k = j * 33 + i + 1;
                setXY4(&quad, screen[k].vx, screen[k].vy, screen[k + 1].vx, screen[k + 1].vy,
                    screen[(j + 1) * 33 + i + 1].vx, screen[(j + 1) * 33 + i + 1].vy, screen[(j + 1) * 33 + i + 2].vx, screen[(j + 1) * 33 + i + 2].vy);
                setUV4(&quad, 0, 63 - t * 4 % 64, 63, 63 - t * 4 % 64, 0, 126 - t * 4 % 64, 63, 126 - t * 4 % 64);
                depth = AverageZ4(depths[k], depths[k + 1], depths[(j + 1) * 33 + i + 1], depths[(j + 1) * 33 + i + 2]);
                flag = (flags[(j + 1) * 33 + i + 1] | flags[(j + 1) * 33 + i + 2] | flags[k]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
        if (t < config->grow) {
            work->position.vx += -work->position.vx / (config->grow - t) * Model_GetFrameStep();
            work->position.vy += -(work->position.vy + 350) / (config->grow - t) * Model_GetFrameStep();
            work->position.vz += (direction * 450 - work->position.vz) / (config->grow - t) * Model_GetFrameStep();
        }
    }
    t = work->elapsed - config->delay - config->grow;
    if (t >= 0 && t < config->life) {
        SetSemiTrans(&beam, 1);
        beam.tpage = work->texture[0] >> 16;
        beam.clut = work->texture[0];
        setUV4(&beam, 64, 0, 95, 0, 64, 63, 95, 63);
        if (t < config->fade) {
            beam.r2 = config->beam_red;
            beam.g2 = config->beam_green;
            beam.b2 = config->beam_blue;
            beam.r3 = config->beam_red;
            beam.g3 = config->beam_green;
            beam.b3 = config->beam_blue;
        } else {
            red = config->beam_red * (config->life - t) / (config->life - config->fade);
            green = config->beam_green * (config->life - t) / (config->life - config->fade);
            blue = config->beam_blue * (config->life - t) / (config->life - config->fade);
            beam.r3 = beam.r2 = red;
            beam.g3 = beam.g2 = green;
            beam.b3 = beam.b2 = blue;
        }
        setRGB0(&beam, 0, 0, 0);
        setRGB1(&beam, 0, 0, 0);
        for (i = 0; i < 32; i++) {
            GsSetLsMatrix(&base);
            position.vx = 0;
            position.vy = -350;
            position.vz = direction * (config->travel * t / config->life + 450);
            k = (t << 14) / config->life + 4096;
            scale.vz = 4096;
            scale.vx = k;
            scale.vy = k;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 33);
            setXY2(&beam, screen[i].vx, screen[i].vy, screen[i + 1].vx, screen[i + 1].vy);
            GsSetLsMatrix(&base);
            setVector(&position, 0, -350, direction * 450);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 33);
            beam.x2 = screen[i].vx;
            beam.y2 = screen[i].vy;
            beam.x3 = screen[i + 1].vx;
            beam.y3 = screen[i + 1].vy;
            depth = (depths[i] + depths[i + 1]) / 8;
            flag = (flags[i] | flags[i + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&beam, ot, depth, 1);
            }
        }
    }
    t = work->elapsed - config->delay - config->grow;
    if (t >= 0 && t < config->flash) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->flash - t) * 255 / config->flash;
        SetSemiTrans(&flash, 1);
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    t = work->elapsed - config->delay - config->grow;
    if (t < 0) {
        return 0;
    }
    if (t < config->life) {
        return 4;
    }
    if (work->completed == 0) {
        work->completed++;
        return 1;
    }
    return 2;
}

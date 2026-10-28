#include "../../types.h"
#include "variant148_entry.h"

/* Header 148's entry: 24 textured cones fly from a slot part toward random
 * targets on the other side, each then bursts into 32 spinning sparks, and a
 * long command (100 and up) adds a fading screen flash. The return value is 0
 * before the first cone lands, 4 while bursts remain, then 1 once and 2. */

static const VECTOR D_8013C708 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_FT3 fan;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[30];
    u16 depths[30];
    u16 projection[30];
    u16 flags[30];
    PSXLONG flag;
    PSXLONG interpolation;
    Model148State *work;
    Model148Config *config;
    s32 side;
    GsOT *ot;
    s32 step;
    s32 i, j, k, red, green, blue, time, radius, angle, elevation, depth, value;
    SVECTOR *p;
    SVECTOR *q;
    DVECTOR *top;
    DVECTOR *bottom;
    u16 *top_z;
    u16 *bottom_z;
    u16 *top_clip;
    u16 *bottom_clip;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C708;
    work = (Model148State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        p = work->cone;
        p->vx = 0;
        config = work->config = &D_8013C744[command % 100];
        p->vy = 0;
        p->vz = -config->length / 2;
        p++;
        for (i = 1; i < 5; i++) {
            for (j = 0; j < 7; j++) {
                radius = rand() % config->radius / 2;
                angle = rand() % 4096;
                p->vx = config->radius * csin(i * 2048 / 5) / 4096 * ccos(j * 4096 / 6) / 4096
                    + radius * csin(angle) / 4096;
                p->vy = config->radius * csin(i * 2048 / 5) / 4096 * csin(j * 4096 / 6) / 4096;
                p->vz = -config->length / 2 + config->length * i / 4 + radius * ccos(angle) / 4096;
                p++;
            }
        }
        p->vx = 0;
        p->vy = 0;
        p->vz = config->length / 2 + config->length / 4;
        p = work->targets;
        for (i = 0; i < 24; i++) {
            radius = rand() % config->spread / 2 + config->spread / 2;
            angle = rand() % 4096;
            p->vx = radius * ccos(angle) / 4096;
            p->vy = radius * csin(angle) / 4096;
            p->vz = side * (rand() % config->spread + 450);
            p++;
        }
        p = work->sparks;
        for (i = 0; i < 32; i++) {
            radius = config->shell / 2 + rand() % config->shell / 2;
            angle = rand() % 4096;
            elevation = rand() % 2048;
            p->vx = radius * ccos(angle) / 4096 * csin(elevation) / 4096;
            p->vy = radius * ccos(elevation) / 4096;
            p->vz = radius * csin(angle) / 4096 * csin(elevation) / 4096;
            p++;
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
        work->hit = 0;
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
    SetPolyFT3(&fan);
    SetSemiTrans(&quad, 1);
    SetSemiTrans(&fan, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    fan.tpage = work->texture[0] >> 16;
    fan.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    setUV3(&fan, 32, 0, 0, 63, 63, 63);
    p = work->heads;
    q = work->targets;
    for (i = 0; i < 24; i++) {
        time = work->elapsed - config->delay - i * config->stagger;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            p->vx = 0;
            p->vy = 0;
            p->vz = matrix.t[2];
            p->pad = rand() % 4096;
        } else if (time < config->fly_duration) {
            red = config->red * time / config->fly_duration;
            green = config->green * time / config->fly_duration;
            blue = config->blue * time / config->fly_duration;
            quad.r0 = red;
            fan.r0 = red;
            quad.g0 = green;
            fan.g0 = green;
            quad.b0 = blue;
            fan.b0 = blue;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            radius = -side * (config->fly_duration - time) * config->spin % 4096;
            angle = ratan2(matrix.t[1] + 350, side * 450 - matrix.t[2]) + (Model_GetActiveSlotIndex() ^ 1) * 2048;
            rotation.vx = angle;
            rotation.vy = 0;
            rotation.vz = radius;
            matrix.t[0] = 0;
            matrix.t[2] = 0;
            RotMatrix(&rotation, &matrix);
            GsSetLsMatrix(&matrix);
            RotTransSV(p, &position, &flag);
            rotation.vx = angle;
            rotation.vy = 0;
            rotation.vz = p->pad;
            radius = time * 3 * 1024 / config->fly_duration + 1024;
            scale.vx = radius;
            scale.vy = radius;
            scale.vz = radius;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->cone, screen, depths, projection, flags, 30);
            top = &screen[1];
            bottom = &screen[2];
            top_z = &depths[1];
            bottom_z = &depths[2];
            top_clip = &flags[1];
            bottom_clip = &flags[2];
            for (j = 0; j < 6; j++) {
                fan.x0 = screen[0].vx;
                fan.y0 = screen[0].vy;
                fan.x1 = top->vx;
                fan.y1 = top->vy;
                fan.x2 = bottom->vx;
                fan.y2 = bottom->vy;
                depth = AverageZ3(depths[0], *top_z, *bottom_z);
                flag = (flags[0] | *top_clip | *bottom_clip) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&fan, ot, (u16)depth);
                }
                top++;
                bottom++;
                top_z++;
                bottom_z++;
                top_clip++;
                bottom_clip++;
            }
            top = &screen[1];
            bottom = &screen[8];
            top_z = &depths[1];
            bottom_z = &depths[8];
            top_clip = &flags[1];
            bottom_clip = &flags[8];
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 6; k++) {
                    quad.x0 = top[0].vx;
                    quad.y0 = top[0].vy;
                    quad.x1 = top[1].vx;
                    quad.y1 = top[1].vy;
                    quad.x2 = bottom[0].vx;
                    quad.y2 = bottom[0].vy;
                    quad.x3 = bottom[1].vx;
                    quad.y3 = bottom[1].vy;
                    depth = AverageZ4(top_z[0], top_z[1], bottom_z[0], bottom_z[1]);
                    flag = (top_clip[0] | top_clip[1] | bottom_clip[0] | bottom_clip[1]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    top++;
                    bottom++;
                    top_z++;
                    bottom_z++;
                    top_clip++;
                    bottom_clip++;
                }
                top++;
                bottom++;
                top_z++;
                bottom_z++;
                top_clip++;
                bottom_clip++;
            }
            bottom = &screen[29];
            bottom_z = &depths[29];
            bottom_clip = &flags[29];
            for (j = 0; j < 6; j++) {
                fan.x0 = top[0].vx;
                fan.y0 = top[0].vy;
                fan.x1 = top[1].vx;
                fan.y1 = top[1].vy;
                fan.x2 = bottom->vx;
                fan.y2 = bottom->vy;
                depth = AverageZ3(top_z[0], top_z[1], *bottom_z);
                flag = (top_clip[0] | top_clip[1] | *bottom_clip) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&fan, ot, (u16)depth);
                }
                top++;
                top_z++;
                top_clip++;
            }
            p->vx += (q->vx - p->vx) / (config->fly_duration - time) * step;
            p->vy += (q->vy - p->vy) / (config->fly_duration - time) * step;
            p->vz += (q->vz - p->vz) / (config->fly_duration - time) * step;
            p->pad = (p->pad + config->twist) % 4096;
        }
        p++;
        q++;
    }
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    p = work->targets;
    for (i = 0; i < 24; i++) {
        time = work->elapsed - config->delay - i * config->stagger - config->fly_duration;
        if (time >= 0 && time < config->burst_duration) {
            q = work->sparks;
            for (j = 0; j < 32; j++) {
                red = config->red * (config->burst_duration - time) / config->burst_duration;
                green = config->green * (config->burst_duration - time) / config->burst_duration;
                blue = config->blue * (config->burst_duration - time) / config->burst_duration;
                quad.r0 = red;
                quad.g0 = green;
                quad.b0 = blue;
                position.vx = q->vx;
                position.vy = q->vy;
                position.vz = q->vz;
                position.vx *= time;
                position.vy *= time;
                position.vz *= time;
                position.vx /= config->burst_duration;
                position.vy /= config->burst_duration;
                position.vz /= config->burst_duration;
                position.vx += p->vx;
                position.vy += p->vy;
                position.vz += p->vz;
                position.vy -= 350;
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = time * config->spin + j * config->twist;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (j & 1) {
                    setUV4(&quad, 64, 0, 127, 0, 64, 63, 127, 63);
                } else {
                    setUV4(&quad, 64, 32, 95, 32, 64, 63, 95, 63);
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                q++;
            }
        }
        p++;
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->flash_duration && work->flash == 1) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        value = (config->flash_duration - time) * 255 / config->flash_duration;
        setRGB0(&flash, value, value, value);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->delay - config->fly_duration;
    if (time < 0) {
        return 0;
    }
    time -= config->stagger * 24;
    if (time >= config->burst_duration) {
        if (work->hit) {
            return 2;
        }
        work->hit++;
        return 1;
    }
    return 4;
}

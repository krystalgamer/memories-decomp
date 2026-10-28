#include "../../types.h"
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "variant182_entry.h"

/* French header 182's entry: a staggered swarm of textured sprites flies out from a
 * slot part to scattered points, pulses there, converges on the other slot
 * and bursts into an expanding ring with a fading screen flash. The return
 * value is 0 until the burst starts, then 1 once, 0 and finally 2. */

static const VECTOR D_8013CCC8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66];
    u16 projection[66];
    u16 flags[66];
    PSXLONG interpolation;
    PSXLONG flag;
    Model182State *work;
    Model182Config *config;
    s32 side;
    GsOT *ot;
    s32 step;
    u8 red, green, blue;
    s32 i, j, time, radius, elevation, depth, value;
    SVECTOR *p;
    SVECTOR *q;
    DVECTOR *top;
    DVECTOR *bottom;
    u16 *top_z;
    u16 *bottom_z;
    u16 *top_clip;
    u16 clip;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CCC8;
    work = (Model182State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        p = work->offsets;
        config = work->config = &D_8013CD64[command % 100];
        for (i = 0; i < config->count; i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = rand() % config->spread;
            p->vx = radius * csin(value) / 4096 * csin(elevation) / 4096;
            p->vy = radius * ccos(elevation) / 4096;
            p->vz = radius * ccos(value) / 4096 * csin(elevation) / 4096;
            p++;
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
        work->target.vy = -350;
        p = work->ring;
        for (i = 0; i < 33; i++) {
            p->vx = config->outer_radius * ccos(i * 128) / 4096;
            p->vy = config->outer_radius * csin(i * 128) / 4096;
            p->vz = 0;
            p++;
        }
        for (i = 0; i < 33; i++) {
            p->vx = config->inner_radius * ccos(i * 128) / 4096;
            p->vy = config->inner_radius * csin(i * 128) / 4096;
            p->vz = 0;
            p++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CCD8[i]);
        }
        work->elapsed = 0;
        work->frame = 0;
        work->hit = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    p = work->sprites;
    q = work->offsets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->delay - i * config->stagger;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            p->vx = matrix.t[0];
            p->vy = matrix.t[1];
            p->vz = matrix.t[2];
            p->pad = 0;
        } else if (time < config->fly_duration + config->hold_duration) {
            if (time < config->fly_duration) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frame & 1) {
                value = config->fly_duration + config->hold_duration - time;
                red = config->red * value / config->hold_duration;
                green = config->green * value / config->hold_duration;
                blue = config->blue * value / config->hold_duration;
            } else {
                blue = green = red = 0;
            }
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            GsSetLsMatrix(&base);
            RotTransPers(p, (PSXLONG *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->target, (PSXLONG *)&quad.x1, &interpolation, &flag);
            if (quad.x0 < quad.x1) {
                rotation.vx = 0;
                rotation.vy = 0;
            } else {
                rotation.vx = 0;
                rotation.vy = 2048;
            }
            rotation.vz = 0;
            position.vx = p->vx;
            position.vy = p->vy;
            position.vz = p->vz;
            value = time * 4096 / config->fly_duration;
            j = 0;
            if (value >= 0) {
                j = 4096;
                if (value <= 4096) {
                    j = value;
                }
            }
            value = j;
            scale.vx = value;
            scale.vy = value;
            scale.vz = value;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256,
                D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (time < config->fly_duration) {
                p->vx += (q->vx / 2 - p->vx) / (config->fly_duration - time) * step;
                p->vy += (q->vy / 2 - 350 - p->vy) / (config->fly_duration - time) * step;
                p->vz += (q->vz / 2 - side * 225 - p->vz) / (config->fly_duration - time) * step;
            }
        }
        p++;
        q++;
    }
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    p = work->sprites;
    q = work->offsets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->pulse_delay - i * config->stagger;
        value = work->elapsed - config->delay - i * config->stagger - config->fly_duration - config->hold_duration;
        if (value >= 0 && time < 0) {
            p->vx = q->vx;
            p->vy = q->vy;
            p->vz = q->vz;
            p->vx += work->target.vx;
            p->vy += work->target.vy;
            p->vz += work->target.vz;
            p->pad = 0;
        } else if (time >= 0 && work->elapsed < config->converge_start) {
            GsSetLsMatrix(&base);
            RotTransPers(p, (PSXLONG *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->target, (PSXLONG *)&quad.x1, &interpolation, &flag);
            if (quad.x0 < quad.x1) {
                rotation.vx = 0;
                rotation.vy = 0;
            } else {
                rotation.vx = 0;
                rotation.vy = 2048;
            }
            rotation.vz = 0;
            position.vx = p->vx;
            position.vy = p->vy;
            position.vz = p->vz;
            value = csin(time % config->pulse_period * 4096 / config->pulse_period) / 8 + 4096;
            scale.vx = value;
            scale.vy = value;
            scale.vz = value;
            if (time >= config->pulse_period) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frame & 1) {
                red = config->red * time / config->pulse_period;
                green = config->green * time / config->pulse_period;
                blue = config->blue * time / config->pulse_period;
            } else {
                blue = green = red = 0;
            }
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256,
                D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        p++;
        q++;
    }
    time = work->elapsed - config->converge_start;
    if (time >= 0 && time < config->converge_duration) {
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
        work->target.vy = -350;
        setVector(&vertices[0], -config->half_size, -config->half_size, 0);
        setVector(&vertices[1], config->half_size, -config->half_size, 0);
        setVector(&vertices[2], -config->half_size, config->half_size, 0);
        setVector(&vertices[3], config->half_size, config->half_size, 0);
        p = work->sprites;
        for (i = 0; i < config->count; i++) {
            GsSetLsMatrix(&base);
            RotTransPers(p, (PSXLONG *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->target, (PSXLONG *)&quad.x1, &interpolation, &flag);
            if (quad.x0 < quad.x1) {
                rotation.vx = 0;
                rotation.vy = 0;
            } else {
                rotation.vx = 0;
                rotation.vy = 2048;
            }
            rotation.vz = 0;
            position.vx = p->vx;
            position.vy = p->vy;
            position.vz = p->vz;
            value = (config->converge_duration - time) * 4096 / config->converge_duration;
            scale.vx = value;
            scale.vy = value;
            scale.vz = value;
            red = config->red;
            green = config->green;
            blue = config->blue;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256,
                D_8013CCD8[i % 3].px % 64 * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph,
                (D_8013CCD8[i % 3].px % 64 + D_8013CCD8[i % 3].pw) * 4, D_8013CCD8[i % 3].py % 256 + D_8013CCD8[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            p->vx += (work->target.vx - p->vx) / (config->converge_duration - time) * step;
            p->vy += (work->target.vy - p->vy) / (config->converge_duration - time) * step;
            p->vz += (work->target.vz - p->vz) / (config->converge_duration - time) * step;
            p++;
        }
    }
    time = work->elapsed - config->converge_start - config->converge_duration;
    if (time >= 0 && time < config->burst_duration) {
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->inner_radius, -config->inner_radius, 0);
        setVector(&vertices[1], config->inner_radius, -config->inner_radius, 0);
        setVector(&vertices[2], -config->inner_radius, config->inner_radius, 0);
        setVector(&vertices[3], config->inner_radius, config->inner_radius, 0);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        position.vx = work->target.vx;
        position.vy = work->target.vy;
        position.vz = work->target.vz;
        value = time * 4096 / config->burst_duration + 4096;
        scale.vx = value;
        scale.vy = value;
        scale.vz = value;
        red = config->red * (config->burst_duration - time) / config->burst_duration;
        green = config->green * (config->burst_duration - time) / config->burst_duration;
        blue = config->blue * (config->burst_duration - time) / config->burst_duration;
        quad.r0 = red;
        quad.g0 = green;
        quad.b0 = blue;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
        quad.tpage = work->texture[4] >> 16;
        quad.clut = work->texture[4];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
        RotTransPersN(work->ring, screen, depths, projection, flags, 66);
        top = screen;
        bottom = &screen[33];
        top_z = depths;
        bottom_z = &depths[33];
        top_clip = flags;
        for (i = 0; i < 32; i++) {
            quad.x0 = top[0].vx;
            quad.y0 = top[0].vy;
            quad.x1 = top[1].vx;
            quad.y1 = top[1].vy;
            quad.x2 = bottom[0].vx;
            quad.y2 = bottom[0].vy;
            quad.x3 = bottom[1].vx;
            quad.y3 = bottom[1].vy;
            depth = AverageZ4(top_z[0], top_z[1], bottom_z[0], bottom_z[1]);
            clip = top_clip[0];
            flag = (top_clip[1] | clip) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            top++;
            bottom++;
            top_z++;
            bottom_z++;
            top_clip++;
        }
    }
    time = work->elapsed - config->converge_start - config->converge_duration;
    if (time >= 0 && time < config->burst_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->burst_duration - time) * 255 / config->burst_duration;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frame++;
    time = work->elapsed - config->converge_start - config->converge_duration;
    if (time < 0) {
        return 0;
    }
    if (time > config->burst_duration) {
        return 2;
    }
    if (work->hit) {
        return 0;
    }
    work->hit++;
    return 1;
}

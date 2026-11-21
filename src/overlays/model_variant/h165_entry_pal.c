#include "../../types.h"
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "h165_entry_pal.h"

/* French header 182's entry: a staggered swarm of textured sprites flies out from a
 * slot part to scattered points, pulses there, converges on the other slot
 * and bursts into an expanding ring with a fading screen flash. The return
 * value is 0 until the burst starts, then 1 once, 0 and finally 2. */

static const VECTOR gFrenchModelVariant182_UnitScale = {4096, 4096, 4096, 0};

s32 FrenchModelVariant182_HandleRequest(ModelVariantGatherState *context, s32 command)
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
    long interpolation;
    long flag;
    ModelVariantGatherState *work;
    ModelVariantGatherConfig *config;
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
    scale = gFrenchModelVariant182_UnitScale;
    work = context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        p = work->targets;
        config = work->config = &gFrenchModelVariant182_aConfigs[command % 100];
        for (i = 0; i < config->count; i++) {
            value = rand() % 4096;
            elevation = rand() % 2048;
            radius = rand() % config->radius;
            p->vx = radius * csin(value) / 4096 * csin(elevation) / 4096;
            p->vy = radius * ccos(elevation) / 4096;
            p->vz = radius * ccos(value) / 4096 * csin(elevation) / 4096;
            p++;
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, &work->centre);
        work->centre.vy = -350;
        p = work->ring;
        for (i = 0; i < 33; i++) {
            p->vx = config->ring_radius * ccos(i * 128) / 4096;
            p->vy = config->ring_radius * csin(i * 128) / 4096;
            p->vz = 0;
            p++;
        }
        for (i = 0; i < 33; i++) {
            p->vx = config->glow_size * ccos(i * 128) / 4096;
            p->vy = config->glow_size * csin(i * 128) / 4096;
            p->vz = 0;
            p++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant182_aImages[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = Graphics_GetDefaultOrderingTable();
    base = *Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    p = work->positions;
    q = work->targets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            p->vx = matrix.t[0];
            p->vy = matrix.t[1];
            p->vz = matrix.t[2];
            p->pad = 0;
        } else if (time < config->rise + config->fade) {
            if (time < config->rise) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frames & 1) {
                value = config->rise + config->fade - time;
                red = config->red * value / config->fade;
                green = config->green * value / config->fade;
                blue = config->blue * value / config->fade;
            } else {
                blue = green = red = 0;
            }
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
            GsSetLsMatrix(&base);
            RotTransPers(p, (long *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->centre, (long *)&quad.x1, &interpolation, &flag);
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
            value = time * 4096 / config->rise;
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
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            if (time < config->rise) {
                p->vx += (q->vx / 2 - p->vx) / (config->rise - time) * step;
                p->vy += (q->vy / 2 - 350 - p->vy) / (config->rise - time) * step;
                p->vz += (q->vz / 2 - side * 225 - p->vz) / (config->rise - time) * step;
            }
        }
        p++;
        q++;
    }
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    p = work->positions;
    q = work->targets;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->delay2 - i * config->spacing;
        value = work->elapsed - config->delay - i * config->spacing - config->rise - config->fade;
        if (value >= 0 && time < 0) {
            p->vx = q->vx;
            p->vy = q->vy;
            p->vz = q->vz;
            p->vx += work->centre.vx;
            p->vy += work->centre.vy;
            p->vz += work->centre.vz;
            p->pad = 0;
        } else if (time >= 0 && work->elapsed < config->start) {
            GsSetLsMatrix(&base);
            RotTransPers(p, (long *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->centre, (long *)&quad.x1, &interpolation, &flag);
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
            value = csin(time % config->pulse * 4096 / config->pulse) / 8 + 4096;
            scale.vx = value;
            scale.vy = value;
            scale.vz = value;
            if (time >= config->pulse) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else if (work->frames & 1) {
                red = config->red * time / config->pulse;
                green = config->green * time / config->pulse;
                blue = config->blue * time / config->pulse;
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
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        p++;
        q++;
    }
    time = work->elapsed - config->start;
    if (time >= 0 && time < config->gather) {
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, &work->centre);
        work->centre.vy = -350;
        setVector(&vertices[0], -config->size, -config->size, 0);
        setVector(&vertices[1], config->size, -config->size, 0);
        setVector(&vertices[2], -config->size, config->size, 0);
        setVector(&vertices[3], config->size, config->size, 0);
        p = work->positions;
        for (i = 0; i < config->count; i++) {
            GsSetLsMatrix(&base);
            RotTransPers(p, (long *)&quad.x0, &interpolation, &flag);
            RotTransPers(&work->centre, (long *)&quad.x1, &interpolation, &flag);
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
            value = (config->gather - time) * 4096 / config->gather;
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
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &interpolation, &flag);
            quad.tpage = work->texture[i % 3] >> 16;
            quad.clut = work->texture[i % 3];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256,
                gFrenchModelVariant182_aImages[i % 3].px % 64 * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph,
                (gFrenchModelVariant182_aImages[i % 3].px % 64 + gFrenchModelVariant182_aImages[i % 3].pw) * 4, gFrenchModelVariant182_aImages[i % 3].py % 256 + gFrenchModelVariant182_aImages[i % 3].ph);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            p->vx += (work->centre.vx - p->vx) / (config->gather - time) * step;
            p->vy += (work->centre.vy - p->vy) / (config->gather - time) * step;
            p->vz += (work->centre.vz - p->vz) / (config->gather - time) * step;
            p++;
        }
    }
    time = work->elapsed - config->start - config->gather;
    if (time >= 0 && time < config->burst) {
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        SetSemiTrans(&quad, 1);
        setVector(&vertices[0], -config->glow_size, -config->glow_size, 0);
        setVector(&vertices[1], config->glow_size, -config->glow_size, 0);
        setVector(&vertices[2], -config->glow_size, config->glow_size, 0);
        setVector(&vertices[3], config->glow_size, config->glow_size, 0);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        position.vx = work->centre.vx;
        position.vy = work->centre.vy;
        position.vz = work->centre.vz;
        value = time * 4096 / config->burst + 4096;
        scale.vx = value;
        scale.vy = value;
        scale.vz = value;
        red = config->red * (config->burst - time) / config->burst;
        green = config->green * (config->burst - time) / config->burst;
        blue = config->blue * (config->burst - time) / config->burst;
        quad.r0 = red;
        quad.g0 = green;
        quad.b0 = blue;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
            (long *)&quad.x3, &interpolation, &flag);
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
    time = work->elapsed - config->start - config->gather;
    if (time >= 0 && time < config->burst) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->burst - time) * 255 / config->burst;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->start - config->gather;
    if (time < 0) {
        return 0;
    }
    if (time > config->burst) {
        return 2;
    }
    if (work->completed) {
        return 0;
    }
    work->completed++;
    return 1;
}

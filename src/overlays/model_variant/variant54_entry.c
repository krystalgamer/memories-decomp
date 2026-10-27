#include "../../types.h"
#include "variant54_entry.h"

/* Header 54's entry: three textured trails spiral in towards the opposing
 * slot and leave flaring glints at their heads, then a stack of textured
 * rings rises around that slot while rising sparks fade out over their life
 * and an optional white screen flash fades in the first frames. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model54State *work;
    Model54Config *config;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 strip;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR from, to, mid;
    PSXLONG flag, interpolation;
    GsOT *ot;
    SVECTOR *p, *q, *w;
    s32 b;
    s32 c;
    s32 i, j, k, n, a, r, s, t, z, d, e;
    u8 red, green, blue;
    u8 tail_red, tail_green, tail_blue;

    work = (Model54State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        config = work->config = &D_8013C6D4[command];
        p = work->ring[0];
        for (i = 0; i < 17; i++) {
            p->vx = config->radius * ccos(i << 8) / 4096;
            p->vy = 0;
            p->vz = config->radius * csin(i << 8) / 4096;
            p++;
            p->vx = config->top_radius * ccos(i << 8) / 4096;
            p->vy = -config->height;
            p->vz = config->top_radius * csin(i << 8) / 4096;
            p++;
        }
        p = work->spiral[0];
        for (i = 0; i < 3; i++) {
            e = (i << 12) / 3;
            for (j = 0; j < 20; j++) {
                p->vx = ccos(e) * 1000 / 4096;
                p->vy = -1000;
                p->vz = csin(e) * 1000 / 4096;
                p++;
            }
        }
        q = work->sparks;
        for (i = 0; i < config->sparks; i++) {
            a = rand() % 4096;
            q->vx = config->radius * ccos(a) / 4096;
            q->vy = -rand() % config->height;
            q->vz = config->radius * csin(a) / 4096;
            q++;
        }
        for (i = 0; i < 4; i++) {
            work->textures[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C648[i]);
        }
        work->texture = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, D_8013C6B8);
        work->head = 0;
        work->stage = 0;
        work->elapsed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < config->wait) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&strip);
    w = work->sparks;
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    quad.tpage = work->texture >> 16;
    quad.clut = work->texture;
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&from);
    for (i = 0; i < config->sparks; i++) {
        t = work->elapsed - config->start - i * config->interval;
        if (t >= 0 && t < config->life) {
            position.vx = from.vx;
            position.vy = from.vy;
            position.vz = from.vz;
            position.vy = 0;
            position.vx += w->vx;
            position.vy += w->vy;
            position.vz += w->vz;
            position.vy -= config->rise * t / config->life;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            z = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
            a = t * 8 / config->life;
            quad.v0 = 0xC0;
            quad.v1 = 0xC0;
            quad.v2 = 0xDF;
            quad.v3 = 0xDF;
            quad.u0 = a << 5;
            quad.u1 = (a << 5) + 31;
            quad.u2 = a << 5;
            quad.u3 = (a << 5) + 31;
            r = config->life - t;
            red = config->spark_red * r / config->life;
            green = config->spark_green * r / config->life;
            blue = config->spark_blue * r / config->life;
            setRGB0(&quad, red, green, blue);
            if (z >= 0 && flag >= 0) {
                GsSortPoly(&quad, func_80058F10(), z);
            }
        }
        w++;
    }
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&position);
    position.vy = 0;
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    if (work->elapsed < config->start + 20) {
        for (i = 0; i < 3; i++) {
            b = (config->start - work->elapsed << 12) / (config->start - config->wait);
            e = (i << 12) / 3;
            d = b + e;
            (work->spiral[0] + (i * 20 + work->head))->vx = b * 1000 / 4096 * ccos(d) / 4096;
            (work->spiral[0] + (i * 20 + work->head))->vy = -(b * 1000) / 4096;
            (work->spiral[0] + (i * 20 + work->head))->vz = b * 1000 / 4096 * csin(d) / 4096;
        }
        work->head = (work->head + 1) % 20;
        strip.tpage = work->textures[0] >> 16;
        strip.clut = work->textures[0];
        setRGB0(&strip, config->red, config->green, config->blue);
        for (i = 0; i < 3; i++) {
            GsSetLsMatrix(&matrix);
            for (j = 0; j < 19; j++) {
                strip.v0 = 0x40;
                strip.v1 = 0x40;
                strip.v2 = 0x4F;
                strip.v3 = 0x4F;
                strip.u0 = (j << 7) / 20;
                strip.u1 = (j << 7) / 20 + 5;
                strip.u2 = (j << 7) / 20;
                strip.u3 = (j << 7) / 20 + 5;
                p = &work->spiral[i][(work->head + j) % 20];
                q = &work->spiral[i][(work->head + j + 1) % 20];
                from.vx = p->vx;
                from.vy = p->vy;
                from.vz = p->vz;
                to.vx = q->vx;
                to.vy = q->vy;
                to.vz = q->vz;
                if (p->vy < 0) {
                    strip.r0 = red = config->red / (20 - j);
                    strip.g0 = green = config->green / (20 - j);
                    strip.b0 = blue = config->blue / (20 - j);
                    strip.r1 = tail_red = config->red / (19 - j);
                    strip.g1 = tail_green = config->green / (19 - j);
                    strip.b1 = tail_blue = config->blue / (19 - j);
                    setRGB3(&strip, tail_red, tail_green, tail_blue);
                    setRGB2(&strip, red, green, blue);
                    SetSemiTrans(&strip, 1);
                    from.vy += config->width / 2;
                    to.vy += config->width / 2;
                    z = RotAverage4(p, q, &from, &to,
                                    (PSXLONG *)&strip.x2, (PSXLONG *)&strip.x3, (PSXLONG *)&strip.x0, (PSXLONG *)&strip.x1,
                                    &interpolation, &flag);
                    if (z >= 0 && flag >= 0) {
                        GsSortPoly(&strip, func_80058F10(), z);
                    }
                    from.vy -= config->width;
                    to.vy -= config->width;
                    z = RotAverage4(p, q, &from, &to,
                                    (PSXLONG *)&strip.x2, (PSXLONG *)&strip.x3, (PSXLONG *)&strip.x0, (PSXLONG *)&strip.x1,
                                    &interpolation, &flag);
                    if (z >= 0 && flag >= 0) {
                        GsSortPoly(&strip, func_80058F10(), z);
                    }
                }
            }
        }
        quad.tpage = work->textures[3] >> 16;
        quad.clut = work->textures[3];
        setRGB0(&quad, config->red, config->green, config->blue);
        setUV4(&quad, 0x80, 0x40, 0xBF, 0x40, 0x80, 0x7F, 0xBF, 0x7F);
        setVector(&from, -config->width, -config->width, 0);
        setVector(&to, config->width, config->width, 0);
        setVector(&mid, 0, 0, 0);
        setVector(&rotation, 0, 0, 0);
        s = rand() % 1024 + 4096;
        setVector(&scale, s, s, s);
        for (i = 0; i < 3; i++) {
            p = &work->spiral[i][(work->head + 19) % 20];
            if (p->vy < 0) {
                GsSetLsMatrix(&base);
                Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&position);
                position.vy = 0;
                position.vx += p->vx;
                position.vy += p->vy;
                position.vz += p->vz;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                z = RotTransPers3(&from, &to, &mid, (PSXLONG *)&strip.x0, (PSXLONG *)&strip.x1, (PSXLONG *)&strip.x2,
                                  &interpolation, &flag);
                SetSemiTrans(&quad, 1);
                if (z >= 0 && flag >= 0) {
                    quad.x0 = strip.x0;
                    quad.y0 = strip.y0;
                    quad.x1 = strip.x2;
                    quad.y1 = strip.y0;
                    quad.x2 = strip.x0;
                    quad.y2 = strip.y2;
                    quad.x3 = strip.x2;
                    quad.y3 = strip.y2;
                    GsSortPoly(&quad, func_80058F10(), z);
                    quad.x0 = strip.x1;
                    quad.x2 = strip.x1;
                    GsSortPoly(&quad, func_80058F10(), z);
                    quad.y0 = strip.y1;
                    quad.y1 = strip.y1;
                    GsSortPoly(&quad, func_80058F10(), z);
                    quad.x0 = strip.x0;
                    quad.x2 = strip.x0;
                    GsSortPoly(&quad, func_80058F10(), z);
                }
            }
        }
    }
    if (work->elapsed >= config->start) {
        s = work->elapsed - config->delay;
        s = s >= 0 ? (s > config->rings ? config->rings : s) : 0;
        quad.tpage = work->textures[2] >> 16;
        quad.clut = work->textures[2];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&from);
        for (j = s; j < config->rings; j++) {
            base = *(MATRIX *)Model_GetLightSourceMatrix();
            GsSetLsMatrix(&base);
            n = config->height * (work->elapsed - config->start) / 20;
            n = n >= 0 ? (n > config->height ? config->height : n) : 0;
            position.vx = config->radius / 4 * ccos(j + work->elapsed << 8) / 4096;
            position.vy = -j * n / 2;
            position.vz = config->radius / 4 * csin(j + work->elapsed << 8) / 4096;
            position.vz += from.vz;
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            rotation.vx = 0;
            rotation.vy = (j + work->elapsed / 2) * 0x556 / 16;
            rotation.vz = 0;
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            k = (config->rings - j << 12) / config->rings;
            scale.vz = k;
            scale.vx = k;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setRGB0(&quad, config->ring_red, config->ring_green, config->ring_blue);
            p = work->ring[0];
            q = work->ring[1];
            for (i = 0; i < 16; i++) {
                z = RotTransPers4(p, p + 1, q, q + 1,
                                  (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x3, (PSXLONG *)&quad.x2,
                                  &interpolation, &flag);
                quad.u0 = 0xC0;
                quad.v0 = i * 8 + 0x40;
                quad.u1 = 0xFF;
                quad.v1 = i * 8 + 0x40;
                quad.u2 = 0xC0;
                quad.v2 = i * 8 + 0x47;
                quad.u3 = 0xFF;
                quad.v3 = i * 8 + 0x47;
                SetSemiTrans(&quad, 1);
                if (z >= 0 && flag >= 0) {
                    GsSortPoly(&quad, func_80058F10(), z);
                }
                p += 2;
                q = work->ring[(i + 2) % 16];
            }
        }
    }
    t = work->elapsed - config->start;
    if (t >= 0 && t < config->flash) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        c = (config->flash - t) * 255 / config->flash;
        setRGB0(&flash, c, c, c);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed >= config->start) {
        t = work->elapsed - config->delay - config->rings;
        a = work->elapsed - config->start - config->sparks * config->interval - config->life;
        if (t >= 0) {
            if (work->stage != 0) {
                return (a > 0) * 2;
            }
            work->stage++;
            return 1;
        }
        return 4;
    }
    return 0;
}

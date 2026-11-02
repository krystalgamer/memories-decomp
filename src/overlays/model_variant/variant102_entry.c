#include "../../types.h"
#include "variant102_entry.h"

/* The entry grows a dome of shaded quads ring by ring over the active slot,
 * then launches its 96 petals one ring at a time towards scattered targets
 * around the other slot while the remaining dome segments stay drawn, plays
 * a short glow on every arrived petal, a centre flash and a burst of 48
 * sparks. A negative command draws a frame; any other command builds the
 * mesh, the petal targets and the spark offsets and loads the textures. */

static const VECTOR gModelVariant102_D24BC = {4096, 4096, 4096, 0};

s32 ModelVariant102_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_GT4 shell;
    POLY_GT3 cap;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[110];
    u16 depths[110], projection[110], flags[110];
    PSXLONG flag, interpolation;
    Model102State *work;
    Model102Config *config;
    s32 direction;
    GsOT *ot;
    s32 row, i, t, depth, a, b;
    SVECTOR *p, *q, *target;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant102_D24BC;
    work = (Model102State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    /* The spark radius and the dome ring count share this block-scoped
     * variable; declaring it here rather than with the other locals keeps
     * its stack slot after the scale address that the struct copy leaves
     * behind. */
    {
        s32 value;

        if (command >= 0) {
            config = work->config = &gModelVariant102_D2558[command];
            p = work->mesh;
            for (row = 0; row < 12; row++) {
                for (i = 0; i < 8; i++) {
                    t = i << 9;
                    p->vx = config->radius * (12 - row) / 12 * ccos(t) / 4096
                        + rand() % (config->radius / 2) * (12 - row) / 12;
                    p->vy = -config->height * row / 12 + rand() % (config->radius / 2) * (12 - row) / 12;
                    p->vz = config->radius * (12 - row) / 12 * csin(t) / 4096
                        + rand() % (config->radius / 2) * (12 - row) / 12;
                    p++;
                }
                p->vx = p[-8].vx;
                p->vy = p[-8].vy;
                p->vz = p[-8].vz;
                p++;
            }
            p->vx = 0;
            p->vy = -config->height;
            p->vz = 0;
            q = work->petals;
            p = &work->mesh[107];
            for (i = 0; i < 8; i++) {
                q->vx = work->mesh[108].vx;
                q->vy = work->mesh[108].vy;
                q->vz = work->mesh[108].vz;
                q->vx += p[0].vx;
                q->vy += p[0].vy;
                q->vz += p[0].vz;
                q->vx += p[-1].vx;
                q->vy += p[-1].vy;
                q->vz += p[-1].vz;
                q->vx = q->vx / 3;
                q->vy = q->vy / 3;
                q->vz = q->vz / 3 - direction * 450;
                p--;
                q++;
            }
            p = &work->mesh[107];
            for (row = 0; row < 11; row++) {
                for (i = 0; i < 8; i++) {
                    q->vx = p[0].vx;
                    q->vy = p[0].vy;
                    q->vz = p[0].vz;
                    q->vx += p[-1].vx;
                    q->vy += p[-1].vy;
                    q->vz += p[-1].vz;
                    q->vx += p[-9].vx;
                    q->vy += p[-9].vy;
                    q->vz += p[-9].vz;
                    q->vx += p[-10].vx;
                    q->vy += p[-10].vy;
                    q->vz += p[-10].vz;
                    q->vx = q->vx / 4;
                    q->vy = q->vy / 4;
                    q->vz = q->vz / 4 - direction * 450;
                    p--;
                    q++;
                }
                p--;
            }
            p = work->targets;
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
            Model_GetSlotDimensionsWithMinimums(Model_GetActiveSlotIndex() ^ 1, (ModelEffectAdjustment *)&vertices[1]);
            for (row = 0; row < 96; row++) {
                p->vx = vertices[0].vx + (rand() % (config->spread * 2) - config->spread);
                p->vy = vertices[0].vy + (rand() % (config->spread * 2) - config->spread);
                p->vz = vertices[0].vz + (rand() % (config->spread * 2) - config->spread);
                p->pad = rand() % 4096;
                p++;
            }
            p = work->sparks;
            for (row = 0; row < 48; row++) {
                a = rand() % 4096;
                b = rand() % 4096;
                value = rand() % config->spark_radius + config->spark_radius;
                p->vx = value * ccos(a) / 4096 * ccos(b) / 4096;
                p->vy = value * csin(a) / 4096 * ccos(b) / 4096;
                p->vz = value * csin(b) / 4096;
                p++;
            }
            for (row = 0; row < 5; row++) {
                work->texture[row] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant102_D24CC[row]);
            }
            work->elapsed = 0;
            work->completed = 0;
            return 0;
        }
        config = work->config;
        ot = func_80058F10();
        if (work->elapsed < config->delay) {
            work->elapsed += Model_GetFrameStep();
            return 0;
        }
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        GsSetLsMatrix(&base);
        PushMatrix();
        SetPolyGT4(&shell);
        SetPolyGT3(&cap);
        SetPolyFT4(&quad);
        t = work->elapsed - config->delay;
        if (t >= 0 && t < config->rise + config->hold) {
            shell.tpage = work->texture[4] >> 16;
            shell.clut = work->texture[4];
            setRGB0(&shell, config->red * 2, config->green * 2, config->blue * 2);
            setRGB1(&shell, config->red, config->green, config->blue);
            setRGB2(&shell, config->red, config->green, config->blue);
            setRGB3(&shell, config->red >> 1, config->green >> 1, config->blue >> 1);
            SetSemiTrans(&shell, 1);
            setUV4(&shell, 0, 160, 63, 160, 0, 223, 63, 223);
            cap.tpage = work->texture[4] >> 16;
            cap.clut = work->texture[4];
            setRGB0(&cap, config->red >> 1, config->green >> 1, config->blue >> 1);
            setRGB1(&cap, config->red, config->green, config->blue);
            setRGB2(&cap, config->red * 2, config->green * 2, config->blue * 2);
            SetSemiTrans(&cap, 1);
            setUV3(&cap, 0, 160, 63, 160, 0, 223);
            setVector(&position, 0, 0, -direction * 450);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->mesh, screen, depths, projection, flags, 109);
            value = t * 11 / config->rise;
            value = value < 0 ? 0 : value > 11 ? 11 : value;
            for (row = 0; row < value; row++) {
                for (i = 0; i < 8; i++) {
                    a = row * 9 + i;
                    b = (row + 1) * 9 + i;
                    setXY4(&shell, screen[a].vx, screen[a].vy, screen[a + 1].vx, screen[a + 1].vy,
                        screen[b].vx, screen[b].vy, screen[b + 1].vx, screen[b + 1].vy);
                    depth = AverageZ4(depths[a], depths[a + 1], depths[b], depths[b + 1]);
                    flag = (flags[a] | flags[a + 1] | flags[b + 1]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&shell, ot, (u16)depth, 1);
                    }
                }
            }
            if (t >= config->rise) {
                for (row = 0; row < 8; row++) {
                    setXY3(&cap, screen[108].vx, screen[108].vy, screen[99 + row].vx, screen[99 + row].vy,
                        screen[100 + row].vx, screen[100 + row].vy);
                    /*
                     * RotTransPersN above projects a literal 109 points (0..108), so nothing in
                     * this call writes depths[109] or flags[109]; retail reads those stale stack
                     * slots anyway, and the C mirrors that.
                     */
                    depth = AverageZ3(depths[109], depths[99 + row], depths[100 + row]);
                    flag = (flags[109] | flags[99 + row] | flags[100 + row]) & 0x20;
                    if (depth >= 0 && flag == 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&cap, ot, (u16)depth, 1);
                    }
                }
            }
        }
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&vertices[0], -config->size, -config->size, 0);
        setVector(&vertices[1], config->size, -config->size, 0);
        setVector(&vertices[2], -config->size, config->size, 0);
        setVector(&vertices[3], config->size, config->size, 0);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        for (row = 0; row < 11; row++) {
            red = config->petal_red * (12 - row / 2) / 12;
            green = config->petal_green * (12 - row / 2) / 12;
            blue = config->petal_blue * (12 - row / 2) / 12;
            setRGB0(&quad, red, green, blue);
            t = work->elapsed - config->delay - config->rise * row / 11;
            if (t >= 0 && t < config->grow) {
                p = &work->mesh[row * 9];
                q = &work->mesh[(row + 1) * 9];
                setUV4(&quad, t / 4 % 8 * 32, 0, t / 4 % 8 * 32 + 31, 0, t / 4 % 8 * 32, 31, t / 4 % 8 * 32 + 31, 31);
                for (i = 0; i < 8; i++) {
                    setVector(&position, p->vx, p->vy, p->vz);
                    position.vx += (q->vx - p->vx) * t / config->grow;
                    position.vy += (q->vy - p->vy) * t / config->grow;
                    position.vz += (q->vz - p->vz) * t / config->grow - direction * 450;
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
                    p++;
                    q++;
                }
            }
        }
        shell.tpage = work->texture[4] >> 16;
        shell.clut = work->texture[4];
        setRGB0(&shell, config->red >> 1, config->green >> 1, config->blue >> 1);
        setRGB1(&shell, config->red, config->green, config->blue);
        setRGB2(&shell, config->red, config->green, config->blue);
        setRGB3(&shell, config->red * 2, config->green * 2, config->blue * 2);
        SetSemiTrans(&shell, 1);
        cap.tpage = work->texture[4] >> 16;
        cap.clut = work->texture[4];
        setRGB0(&cap, config->red >> 1, config->green >> 1, config->blue >> 1);
        setRGB1(&cap, config->red, config->green, config->blue);
        setRGB2(&cap, config->red * 2, config->green * 2, config->blue * 2);
        SetSemiTrans(&cap, 1);
        setVector(&scale, 4096, 4096, 4096);
        for (row = 0; row < 12; row++) {
            t = work->elapsed - config->delay - config->rise - config->hold - row * config->spacing;
            if (t >= 0 && t < config->flight) {
                setUV4(&shell, 64, 160, 127, 160, 64, 223, 127, 223);
                setUV3(&cap, 64, 160, 127, 160, 127, 223);
                a = config->height * (row + 1) / 96;
                setVector(&vertices[0], -a, -a, 0);
                setVector(&vertices[1], a, -a, 0);
                setVector(&vertices[2], -a, a, 0);
                setVector(&vertices[3], a, a, 0);
                q = &work->petals[row * 8];
                target = &work->targets[row * 8];
                for (i = 0; i < 8; i++) {
                    setVector(&rotation, ((t << 6) + target->pad) % 4096, 0, (t * 76 + target->pad) % 4096);
                    setVector(&position, q->vx, q->vy, q->vz);
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&shell.x0, (PSXLONG *)&shell.x1, (PSXLONG *)&shell.x2, (PSXLONG *)&shell.x3,
                        &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&shell, ot, (u16)depth, 1);
                    }
                    q->vx += (target->vx - q->vx) / (config->flight - t) * 2;
                    q->vy += (target->vy - q->vy) / (config->flight - t) * 2;
                    q->vz += (target->vz - q->vz) / (config->flight - t) * 2;
                    q++;
                    target++;
                }
            }
            if (work->elapsed - config->delay - config->rise >= config->hold && t < 0) {
                setUV4(&shell, 63, 223, 0, 223, 63, 160, 0, 160);
                setUV3(&cap, 0, 160, 63, 160, 0, 223);
                setVector(&position, 0, 0, -direction * 450);
                setVector(&rotation, 0, 0, 0);
                setVector(&scale, 4096, 4096, 4096);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                if (row != 0) {
                    p = &work->mesh[107] - (row - 1) * 9;
                    for (i = 0; i < 8; i++) {
                        depth = RotAverage4(p, p - 1, p - 9, p - 10,
                            (PSXLONG *)&shell.x0, (PSXLONG *)&shell.x1, (PSXLONG *)&shell.x2, (PSXLONG *)&shell.x3,
                            &interpolation, &flag);
                        if (depth >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&shell, ot, (u16)depth, 1);
                        }
                        p--;
                    }
                } else {
                    p = &work->mesh[107];
                    for (i = 0; i < 8; i++) {
                        depth = RotAverage3(&work->mesh[108], p, p - 1,
                            (PSXLONG *)&cap.x0, (PSXLONG *)&cap.x1, (PSXLONG *)&cap.x2, &interpolation, &flag);
                        if (depth >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&cap, ot, (u16)depth, 1);
                        }
                        p--;
                    }
                }
            }
        }
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        setVector(&rotation, 0, 0, 0);
        a = config->radius;
        setVector(&vertices[0], -a, -a, 0);
        setVector(&vertices[1], a, -a, 0);
        setVector(&vertices[2], -a, a, 0);
        setVector(&vertices[3], a, a, 0);
        for (row = 0; row < 12; row++) {
            t = work->elapsed - config->delay - config->rise - config->hold - row * config->spacing - config->flight;
            if (t >= 0 && t < 12) {
                a = (row + 1) * 4096 / 12;
                setVector(&scale, a, a, a);
                p = &work->targets[row * 8];
                for (i = 0; i < 8; i++) {
                    setVector(&position, p->vx, p->vy, p->vz);
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    setUV4(&quad, t / 4 << 6, 32, (t / 4 << 6) + 63, 32, t / 4 << 6, 95, (t / 4 << 6) + 63, 95);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    p++;
                }
            }
        }
        t = work->elapsed - config->delay - config->rise - config->hold;
        if (t >= 0 && t < 32) {
            quad.tpage = work->texture[2] >> 16;
            quad.clut = work->texture[2];
            SetSemiTrans(&quad, 1);
            setRGB0(&quad, config->petal_red, config->petal_green, config->petal_blue);
            setVector(&vertices[0], -config->glow_size, -config->glow_size, 0);
            setVector(&vertices[1], config->glow_size, -config->glow_size, 0);
            setVector(&vertices[2], -config->glow_size, config->glow_size, 0);
            setVector(&vertices[3], config->glow_size, config->glow_size, 0);
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, 0, 0, 0);
            setVector(&position, 0, -350, -direction * 450);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            setUV4(&quad, t / 4 % 4 * 32, t / 4 / 4 * 32 + 96, t / 4 % 4 * 32 + 31, t / 4 / 4 * 32 + 96,
                t / 4 % 4 * 32, t / 4 / 4 * 32 + 127, t / 4 % 4 * 32 + 31, t / 4 / 4 * 32 + 127);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        t = work->elapsed - config->delay - config->rise - config->hold;
        if (t >= 0 && t < config->burst) {
            quad.tpage = work->texture[3] >> 16;
            quad.clut = work->texture[3];
            SetSemiTrans(&quad, 1);
            red = config->spark_red * (config->burst - t) / config->burst;
            green = config->spark_green * (config->burst - t) / config->burst;
            blue = config->spark_blue * (config->burst - t) / config->burst;
            setRGB0(&quad, red, green, blue);
            setVector(&vertices[0], -config->spark_size, -config->spark_size, 0);
            setVector(&vertices[1], config->spark_size, -config->spark_size, 0);
            setVector(&vertices[2], -config->spark_size, config->spark_size, 0);
            setVector(&vertices[3], config->spark_size, config->spark_size, 0);
            setUV4(&quad, 192, 64, 223, 64, 192, 95, 223, 95);
            setVector(&rotation, 0, 0, 0);
            a = (t << 12) / config->burst;
            setVector(&scale, a, a, a);
            p = work->sparks;
            for (row = 0; row < 48; row++) {
                position.vx = p->vx * t / config->burst;
                position.vy = p->vy * t / config->burst - 350;
                position.vz = p->vz * t / config->burst - direction * 450;
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
                p++;
            }
        }
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        t = work->elapsed - config->delay - config->rise - config->hold;
        if (t >= config->flight) {
            if (t < config->flight + config->spacing * 12 + 12) {
                return 4;
            }
            if (work->completed) {
                return 2;
            }
            work->completed = 1;
            return 1;
        }
        return 0;
    }
}

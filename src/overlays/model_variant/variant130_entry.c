#include "../../types.h"
#include "variant130_entry.h"

static const VECTOR gModelVariant130_D0B5C = {4096, 4096, 4096, 0};

s32 ModelVariant130_HandleRequest(u8 *context, s32 command)
{
    Model130State *work;
    Model130Config *config;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR unused; /* unreferenced frame slot kept by the original */
    PSXLONG flag, interpolation;
    s32 direction;
    GsOT *ot;
    SVECTOR *p, *q;
    PSXLONG depth;
    s32 i, j, n, t, s, z;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant130_D0B5C;
    work = (Model130State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &gModelVariant130_D0B88[command % 100];
        p = work->positions;
        for (i = 0; i < config->count; i++) {
            setVector(p, 0, 0, 0);
            p++;
        }
        p = work->targets;
        for (i = 0; i < config->count; i++, p++) {
            p->vx = rand() % config->spread * 2 - config->spread;
            p->vy = rand() % config->spread * 2 - config->spread - 350;
            p->vz = direction * 900;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant130_D0B6C[i]);
        }
        work->elapsed = 0;
        work->done = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    setVector(&vertices[0], -config->size, -config->size, 0);
    setVector(&vertices[1], config->size, -config->size, 0);
    setVector(&vertices[2], -config->size, config->size, 0);
    setVector(&vertices[3], config->size, config->size, 0);
    p = work->positions;
    q = work->targets;
    for (i = 0; i < config->count; p++, q++, i++) {
        t = work->elapsed - config->delay - i * config->interval;
        if (t < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            p->vx = matrix.t[0];
            p->vy = matrix.t[1];
            p->vz = matrix.t[2];
        } else if (t < config->duration) {
            z = direction * p->vz;
            if (z < 451) {
                red = config->red;
                green = config->green;
                blue = config->blue;
            } else {
                red = config->red * (900 - z) / 450;
                green = config->green * (900 - z) / 450;
                blue = config->blue * (900 - z) / 450;
            }
            n = 1;
            if (direction * p->vz >= config->trail_depth) {
                n = config->trail;
            }
            for (j = 0; j < n; j++) {
                quad.r0 = red * (config->trail - j) / config->trail;
                quad.g0 = green * (config->trail - j) / config->trail;
                quad.b0 = blue * (config->trail - j) / config->trail;
                position.vx = p->vx - (q->vx - p->vx) / (config->duration - t) * (j << 1);
                position.vy = p->vy - (q->vy - p->vy) / (config->duration - t) * (j << 1);
                position.vz = p->vz - (q->vz - p->vz) / (config->duration - t) * (j << 1);
                if (j != 0 && direction * position.vz <= config->trail_depth) {
                    continue;
                }
                s = config->spin * t - config->spin * j;
                rotation.vz = s;
                rotation.vx = 0;
                rotation.vy = 0;
                if (t < config->grow) {
                    s = (t << 12) / config->grow;
                } else {
                    z = direction * position.vz;
                    if (z > 450) {
                        s = 0x4000 - (900 - z) * 3 * 4096 / 450;
                    } else {
                        s = 4096;
                    }
                }
                setVector(&scale, s, s, s);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
            }
            p->vx += (q->vx - p->vx) / (config->duration - t) * 2;
            p->vy += (q->vy - p->vy) / (config->duration - t) * 2;
            p->vz += (q->vz - p->vz) / (config->duration - t) * 2;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    t = work->elapsed - config->delay - config->count * config->interval - config->duration;
    if (t > 0) {
        return 2;
    }
    p = work->positions;
    for (i = 0; i < config->count; p++, i++) {
        if (direction * p->vz >= 450 && work->done == i) {
            work->done = i + 1;
            if (work->done < config->count) {
                return 4;
            }
            return 1;
        }
    }
    switch (Model_GetSlotAnimationIndex(Model_GetActiveSlotIndex() ^ 1)) {
    case 5:
    case 8:
        return 4;
    }
    return 0;
}

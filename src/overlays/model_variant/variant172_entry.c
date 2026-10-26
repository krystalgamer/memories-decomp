#include "../../types.h"
#include "variant172_entry.h"
#include "../../game/screen_projection.h"

/* Header 172's entry: groups of dots appear around one of the slot's parts
 * after their delays, gather as one-pixel boxes onto points spread through a
 * sphere in front of the other slot, and then play eight frames of the texture
 * as quads at those points. */

static const VECTOR D_8013BCF8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model172State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    PSXLONG interpolation, flag;
    GsOT *ot;
    Model172Config *config;
    SVECTOR *point, *target;
    s32 i, j, a, b, radius, time, frame, u, v, step;
    s32 n0, n1, n2, n3;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BCF8;
    work = (Model172State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BD34[command % 100];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        vertices[0].vy = -350;
        point = work->targets;
        /* Retail fills 513 targets: the last one overwrites texture[0] and
         * completed, which are both set below. */
        for (i = 0; i <= 512; i++) {
            j = rand() % 4096;
            b = rand() % 2048;
            radius = rand() % config->radius;
            point->vx = radius * csin(j) / 4096 * csin(b) / 4096;
            point->vy = radius * ccos(b) / 4096;
            point->vz = radius * ccos(j) / 4096 * csin(b) / 4096;
            point->vx += vertices[0].vx;
            point->vy += vertices[0].vy;
            point->vz += vertices[0].vz;
            point++;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BD18[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model172State), sizeof(Model172State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    j = 4096;
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    point = work->positions;
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    GsSetLsMatrix(&base);
    target = work->targets;
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&origin, matrix.t[0], matrix.t[1], matrix.t[2]);
    setVector(&vertices[0], -config->jitter, -config->jitter, 0);
    setVector(&vertices[1], config->jitter, -config->jitter, 0);
    setVector(&vertices[2], -config->jitter, config->jitter, 0);
    setVector(&vertices[3], config->jitter, config->jitter, 0);
    box.r = config->red;
    box.g = config->green;
    box.b = config->blue;
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, j, j, j);
    GsSetLsMatrix(&base);
    for (i = 0; i < config->total / config->group; i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&origin, matrix.t[0], matrix.t[1], matrix.t[2]);
            for (j = 0; j < config->group; j++) {
                setVector(point, origin.vx, origin.vy, origin.vz);
                point->vx += rand() % config->jitter * 2 - config->jitter;
                point->vy += rand() % config->jitter * 2 - config->jitter;
                point->vz += rand() % config->jitter * 2 - config->jitter;
                target++;
                point++;
            }
        } else if (time < config->gather_duration) {
            n0 = time * 8;
            j = n0 / config->gather_duration;
            setUV4(&quad, j % 4 * 64, j / 4 * 64, j % 4 * 64 + 63, j / 4 * 64,
                j % 4 * 64, j / 4 * 64 + 63, j % 4 * 64 + 63, j / 4 * 64 + 63);
            for (j = 0; j < config->group; j++) {
                depth = RotTransPers(point, (PSXLONG *)&box.x, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortBoxFill(&box, ot, (u16)depth);
                }
                v = (target->vx - point->vx) / (config->gather_duration - time);
                point->vx += v * step;
                point->vy += (target->vy - point->vy) / (config->gather_duration - time) * step;
                point->vz += (target->vz - point->vz) / (config->gather_duration - time) * step;
                point++;
                target++;
            }
        } else {
            point += config->group;
            target += config->group;
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    frame = config->quad_red;
    u = config->quad_green;
    n2 = config->quad_blue;
    setRGB0(&quad, frame, u, n2);
    point = work->targets;
    for (i = 0; i < config->total / config->group; i++) {
        time = work->elapsed - config->delay - i * config->spacing - config->gather_duration;
        if (time >= 0 && time < config->fly_duration) {
            n0 = time * 8;
            j = n0 / config->fly_duration;
            setUV4(&quad, j % 4 * 32, j / 4 * 32, j % 4 * 32 + 31, j / 4 * 32, j % 4 * 32, j / 4 * 32 + 31, j % 4 * 32 + 31, j / 4 * 32 + 31);
            for (n1 = 0; n1 < config->group; n1++) {
                setVector(&position, point->vx, point->vy, point->vz);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                point++;
            }
        } else {
            point += config->group;
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->gather_duration;
    if (time < 0) {
        return 0;
    }
    time -= config->spacing * config->total / config->group + config->fly_duration;
    if (time < 0) {
        return 4;
    }
    if (work->completed != 0) {
        return 2;
    }
    work->completed++;
    return 1;
}

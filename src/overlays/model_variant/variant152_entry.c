#include "../../types.h"
#include "variant152_entry.h"

/* The entry trails two textured ribbons out of the two slot data entries
 * towards spiral targets on the other side, then scatters spinning sparks
 * along each ribbon. A negative command draws a frame; any other command sets
 * the work area up and prints its size. */

static const VECTOR D_8013C138 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 fill;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corner[4];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG interpolation, flag;
    Model152State *work;
    Model152Config *config;
    GsOT *ot;
    s32 step;
    /* v is a shared scratch value (swirl angle, previous-segment flag, spark
     * lifetime and spark frame) and otz holds the projected depth, the growth
     * scale and the current buffer row; sharing them reproduces the original
     * register allocation. */
    s32 i, j, k, b, cur, t, v, otz, z, direction;
    SVECTOR *point, *target, *origin;
    DVECTOR *edge, *row;
    u16 *depth, *clip, *row_depth, *row_clip;
    u8 *color;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C138;
    work = (Model152State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C190[command % 100];
        point = work->ring;
        for (j = 0; j < 17; j++) {
            point->vx = config->ring_radius * ccos(j << 8) / 4096;
            point->vy = config->ring_radius * csin(j << 8) / 4096;
            point->vz = 0;
            point++;
        }
        point = work->target[0];
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 30; j++) {
                point->vx = 0;
                point->vy = -350;
                point->vz = direction * 450;
                v = ((i % 2) * 2 - 1) * (j << 12) / 16;
                point->vx += config->swirl_radius * csin(v) / 4096;
                point->vy += config->swirl_radius * ccos(v) / 4096;
                point->vz += direction * 450;
                point++;
            }
        }
        for (j = 0; j < 2; j++) {
            work->texture[j] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C158[j]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model152State), sizeof(Model152State));
#endif
    } else {
        config = work->config;
        ot = func_80058F10();
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        PushMatrix();
        SetPolyFT4(&quad);
        SetPolyF4(&fill);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        if (work->elapsed < config->delay) {
            point = work->origin;
            for (j = 0; j < 2; j++) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->entry[j]), &matrix);
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
                point++;
            }
        }
        point = work->ribbon[0];
        target = work->target[0];
        origin = work->origin;
        color = config->color[0];
        for (k = 0; k < 2; k++) {
            quad.r0 = color[0];
            quad.g0 = color[1];
            quad.b0 = color[2];
            cur = v = 0;
            for (j = 0; j < 30; j++) {
                t = work->elapsed - config->delay - j * config->spacing;
                if (t < 0) {
                    point->vx = origin->vx;
                    point->vy = origin->vy;
                    point->vz = origin->vz;
                    if (v) {
                        GsSetLsMatrix(&base);
                        otz = RotTransPers(point, (PSXLONG *)&quad.x0, &interpolation, &flag);
                        b = (cur ^ 1) * 17;
                        quad.x1 = quad.x0;
                        quad.y1 = quad.y0;
                        edge = &screen[b];
                        row = edge + 1;
                        depth = &depths[b];
                        row_depth = depth + 1;
                        for (i = 0; i < 16; i++) {
                            quad.x2 = edge->vx;
                            quad.y2 = edge->vy;
                            quad.x3 = row->vx;
                            quad.y3 = row->vy;
                            setUV4(&quad, 0, (i << 4) % 128, 31, (i << 4) % 128, 0, (i << 4) % 128 + 15, 31,
                                   (i << 4) % 128 + 15);
                            z = ((*depth + *row_depth) / 8 + otz) / 2;
                            if (z >= 0 && flag >= 0) {
                                GsSortPoly(&quad, ot, z);
                            }
                            edge++;
                            row++;
                            depth++;
                            row_depth++;
                        }
                        v = 0;
                    }
                } else if (t < config->duration) {
                    position.vx = point->vx;
                    position.vy = point->vy;
                    position.vz = point->vz;
                    otz = (t << 12) / config->duration;
                    scale.vx = otz;
                    scale.vy = otz;
                    scale.vz = otz;
                    otz = (t << 10) / config->duration;
                    rotation.vx = 0;
                    rotation.vy = 0;
                    rotation.vz = otz;
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    MulMatrix2(&base, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    otz = cur * 17;
                    b = (cur ^ 1) * 17;
                    RotTransPersN(work->ring, &screen[otz], &depths[otz], &projection[otz], &flags[otz], 17);
                    cur ^= 1;
                    if (v) {
                        edge = &screen[b];
                        row = &screen[otz];
                        depth = &depths[b];
                        row_depth = &depths[otz];
                        clip = &flags[b];
                        row_clip = &flags[otz];
                        for (i = 0; i < 16; i++) {
                            quad.x0 = edge[0].vx;
                            quad.y0 = edge[0].vy;
                            quad.x1 = row[0].vx;
                            quad.y1 = row[0].vy;
                            quad.x2 = edge[1].vx;
                            quad.y2 = edge[1].vy;
                            quad.x3 = row[1].vx;
                            quad.y3 = row[1].vy;
                            setUV4(&quad, 0, (i << 4) % 128, 31, (i << 4) % 128, 0, (i << 4) % 128 + 15, 31,
                                   (i << 4) % 128 + 15);
                            z = AverageZ4(depth[0], row_depth[0], depth[1], row_depth[1]);
                            flag = (clip[0] | row_clip[0] | clip[1] | row_clip[1]) & 0x20;
                            if (z >= 0 && flag == 0) {
                                GsSortPoly(&quad, ot, z);
                            }
                            edge++;
                            row++;
                            depth++;
                            row_depth++;
                            clip++;
                            row_clip++;
                        }
                    } else {
                        v = 1;
                    }
                    point->vx += (target->vx + origin->vx - point->vx) / (config->duration - t) * step;
                    point->vy += (target->vy - point->vy) / (config->duration - t) * step;
                    point->vz += (target->vz - point->vz) / (config->duration - t) * step;
                }
                point++;
                target++;
            }
            origin++;
            color += 4;
        }
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        corner[0].vx = -config->spark_size;
        corner[0].vy = -config->spark_size;
        corner[0].vz = 0;
        corner[1].vx = config->spark_size;
        corner[1].vy = -config->spark_size;
        corner[1].vz = 0;
        corner[2].vx = -config->spark_size;
        corner[2].vy = config->spark_size;
        corner[2].vz = 0;
        corner[3].vx = config->spark_size;
        corner[3].vy = config->spark_size;
        corner[3].vz = 0;
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        point = work->ribbon[0];
        color = config->color[2];
        for (k = 0; k < 2; k++) {
            for (j = 0; j < 30; j++) {
                t = work->elapsed - config->delay - j * config->spacing;
                if (t >= 0 && t < config->duration) {
                    red = color[0] * t / config->duration;
                    green = color[1] * t / config->duration;
                    blue = color[2] * t / config->duration;
                    setRGB0(&quad, red, green, blue);
                    target = &work->ring[j % 3];
                    for (i = j % 3; i < 16; i += 3) {
                        v = config->duration;
                        position.vx = target->vx;
                        position.vy = target->vy;
                        position.vz = target->vz;
                        position.vx *= t;
                        position.vy *= t;
                        position.vz *= t;
                        position.vx /= v;
                        position.vy /= v;
                        position.vz /= v;
                        position.vx += point->vx;
                        position.vy += point->vy;
                        position.vz += point->vz;
                        GsSetLsMatrix(&base);
                        RotTrans(&position, (VECTOR *)matrix.t, &flag);
                        RotMatrix(&rotation, &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                        z = RotAverage4(&corner[0], &corner[1], &corner[2], &corner[3], (PSXLONG *)&quad.x0,
                                        (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                        &interpolation, &flag);
                        v = (t << 3) / config->duration;
                        quad.v0 = 128;
                        quad.v1 = 128;
                        quad.v2 = 159;
                        quad.v3 = 159;
                        quad.u0 = v << 5;
                        quad.u1 = (v << 5) + 31;
                        quad.u2 = v << 5;
                        quad.u3 = (v << 5) + 31;
                        if (z >= 0 && flag >= 0) {
                            GsSortPoly(&quad, ot, z);
                        }
                        target += 3;
                    }
                }
                point++;
            }
            color += 4;
        }
        PopMatrix();
        work->elapsed += step;
        work->frames++;
        t = work->elapsed - config->delay - config->duration * 2 / 3;
        if (t < 0) {
            return 0;
        }
        t = work->elapsed - config->delay - config->spacing * 30;
        if (t < 0) {
            return 4;
        }
        if (t >= config->duration) {
            return 2;
        }
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}

#include "../../types.h"
#include "variant666_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 666's entry: three blooms hover in a circle above the model,
 * each a pulsing square that launches a spark on every pulse. A spark
 * draws a fading line from its bloom towards a target point and, a
 * quarter of the way through its life, opens a ring of eight textured
 * triangles round a centre square. The setup also prints the work-area
 * size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL666_FADE(e, offset, value, divisor)                                      \
    (e).color[0] = gModelVariant666_D1AB8[work->command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = gModelVariant666_D1AB8[work->command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = gModelVariant666_D1AB8[work->command].color[(offset) + 2] * (value) / (divisor)

s32 ModelVariant666_HandleRequest(u8 *context, s32 command)
{
    Model666State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model666ViewSnapshot view;
    SVECTOR tip, mid;
    PSXLONG interpolation, flag, sxy;
    s32 result, j, step, inactive;
    s32 i, k;
    s32 value, sign, ended;
    u32 tex;
    PSXLONG depth;

    work = (Model666State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model666State));
#ifndef VERSION_FRENCH
        printf(gModelVariant666_D1A4C, sizeof(Model666State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant666_D1A64[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant666_D1A64[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant666_D1A64[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 3;
            work->sparks_per_bloom = 16;
            break;
        }
        work->radius = gModelVariant666_D1AB8[command].radius;
        work->height = gModelVariant666_D1AB8[command].height;
        for (i = 0; i < work->count; i++) {
            work->blooms[i].duration = 480;
            work->blooms[i].active = 0;
            work->blooms[i].elapsed = 0;
            work->blooms[i].mode = 1;
            work->blooms[i].coord = Model_GetCameraCoordinateUnit();
            work->blooms[i].frame = 0;
            work->blooms[i].color[0] = gModelVariant666_D1AB8[command].color[0];
            work->blooms[i].color[1] = gModelVariant666_D1AB8[command].color[1];
            work->blooms[i].color[2] = gModelVariant666_D1AB8[command].color[2];
            work->blooms[i].u = 0;
            work->blooms[i].v = 32;
            setVector(&work->blooms[i].scale, 4096, 4096, 4096);
            setVector(&work->blooms[i].target, 0, 0, 0);
            setVector(&work->blooms[i].rotation, 0, work->slot * 2048, 0);
            value = work->radius;
            k = (i << 12) / 3 + 0x800;
            setVector(&work->blooms[i].position,
                work->origin.vx + value * rsin(work->slot * 2048 + k) / 4096,
                work->origin.vy - work->height,
                work->origin.vz + value * rcos(work->slot * 2048 + k) / 4096);
            work->blooms[i].cursor = 0;
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->sparks_per_bloom; i++) {
                work->sparks[i][j].duration = 32;
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].mode = 1;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = gModelVariant666_D1AB8[command].color[0];
                work->sparks[i][j].color[1] = gModelVariant666_D1AB8[command].color[1];
                work->sparks[i][j].color[2] = gModelVariant666_D1AB8[command].color[2];
                work->sparks[i][j].spawned = 0;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                sign = i % 2 * 2 - 1;
                value = work->height;
                setVector(&work->sparks[i][j].target,
                    sign * value * rsin(i << 8) / 4096,
                    work->height * 3,
                    sign * value * rcos(i << 8) / 4096);
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                work->sparks[i][j].position = work->blooms[j].position;
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->sparks_per_bloom; i++) {
                work->rings[i][j].duration = 32;
                work->rings[i][j].active = 0;
                work->rings[i][j].elapsed = 0;
                work->rings[i][j].mode = 1;
                work->rings[i][j].coord = Model_GetCameraCoordinateUnit();
                work->rings[i][j].color[0] = gModelVariant666_D1AB8[command].color[3];
                work->rings[i][j].color[1] = gModelVariant666_D1AB8[command].color[4];
                work->rings[i][j].color[2] = gModelVariant666_D1AB8[command].color[5];
                work->rings[i][j].u = 0;
                work->rings[i][j].v = 0;
                setVector(&work->rings[i][j].scale, 4096, 4096, 4096);
                setVector(&work->rings[i][j].target, 0, 0, 0);
                setVector(&work->rings[i][j].rotation, 0, work->slot * 2048, 0);
                sign = i % 2 * 2 - 1;
                value = work->height / 2;
                setVector(&work->rings[i][j].position,
                    work->blooms[j].position.vx + sign * value * rsin(i << 8) / 4096,
                    work->origin.vy + work->radius * rsin((i + j) % 8 << 9) / 4096,
                    work->blooms[j].position.vz + sign * value * rcos(i << 8) / 4096);
            }
        }
        work->delay = gModelVariant666_D1AB8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->flare[0], -value, -value, 0);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        setVector(&work->root, 0, 0, 0);
        /* Reassigning the radius here keeps the original store order. */
        value = work->radius;
        for (i = 0; i < 9; i++) {
            setVector(&work->rim[i], value * rsin(i << 9) / 4096, value * rcos(i << 9) / 4096, 0);
        }
        value = work->radius / 2;
        setVector(&work->ring[0], -value, -value, 0);
        setVector(&work->ring[1], value, -value, 0);
        setVector(&work->ring[2], -value, value, 0);
        setVector(&work->ring[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed == 0) {
            for (i = 0; i < work->count; i++) {
                work->blooms[i].active = 1;
            }
        }
        if (work->flags[4] == 0 && work->elapsed > 432) {
            result = 1;
            work->flags[4] = result;
        }
        break;
    }
    PushMatrix();
    view = *(Model666ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->blooms[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ended = 0;
            if (work->blooms[i].elapsed < (u16)(work->blooms[i].duration / 10)) {
                value = work->blooms[i].elapsed * 5 * 8192 / work->blooms[i].duration;
            } else {
                value = 4096;
                ended = 1;
            }
            if (!(work->blooms[i].frame & 1)) {
                value /= 2;
            }
            setVector(&work->blooms[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->blooms[i].scale);
            RotMatrixZ(work->blooms[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->blooms[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            RotMatrixY(work->blooms[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->blooms[i].position.vx;
            unit.matrix.t[1] = work->blooms[i].position.vy;
            unit.matrix.t[2] = work->blooms[i].position.vz;
            unit.super = work->blooms[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->blooms[i].duration * 9 / 10 < work->blooms[i].elapsed) {
                value = (work->blooms[i].duration - work->blooms[i].elapsed) * 10;
                sign = work->blooms[i].duration;
                MODEL666_FADE(work->blooms[i], 0, value, sign);
                ended = 0;
            }
            setPolyFT4(&work->quad);
            work->quad.tpage = work->tpage[0];
            work->quad.clut = work->clut[0];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->blooms[i].u, work->blooms[i].v,
                work->blooms[i].u + 63, work->blooms[i].v,
                work->blooms[i].u, work->blooms[i].v + 63,
                work->blooms[i].u + 63, work->blooms[i].v + 63);
            setRGB0(&work->quad, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
            depth = RotAverage4(&work->flare[0], &work->flare[1], &work->flare[2], &work->flare[3],
                (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
            }
            if (ended == 1) {
                work->sparks[work->blooms[i].cursor][i].active = ended;
                work->sparks[work->blooms[i].cursor][i].elapsed = 0;
                work->sparks[work->blooms[i].cursor][i].color[0] = gModelVariant666_D1AB8[work->command].color[0];
                work->sparks[work->blooms[i].cursor][i].color[1] = gModelVariant666_D1AB8[work->command].color[1];
                work->sparks[work->blooms[i].cursor][i].color[2] = gModelVariant666_D1AB8[work->command].color[2];
                work->sparks[work->blooms[i].cursor][i].spawned = 0;
                if (++work->blooms[i].cursor >= work->sparks_per_bloom) {
                    work->blooms[i].cursor = 0;
                }
            }
            work->blooms[i].elapsed += step;
            work->blooms[i].frame++;
            if (work->blooms[i].duration < work->blooms[i].elapsed) {
                work->blooms[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->count; j++) {
        for (i = 0; i < work->sparks_per_bloom; i++) {
            if (work->sparks[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i][j].rotation.vx, &unit.matrix);
                RotMatrixY(work->sparks[i][j].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->sparks[i][j].position.vx;
                unit.matrix.t[1] = work->sparks[i][j].position.vy;
                unit.matrix.t[2] = work->sparks[i][j].position.vz;
                unit.super = work->sparks[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i][j].elapsed > work->sparks[i][j].duration * 3 / 4) {
                    value = (work->sparks[i][j].duration - work->sparks[i][j].elapsed) * 4;
                    sign = work->sparks[i][j].duration;
                    MODEL666_FADE(work->sparks[i][j], 0, value, sign);
                }
                work->line.attribute = 0x50000000;
                work->line.r0 = 0;
                work->line.g0 = 0;
                work->line.b0 = 0;
                work->line.r1 = work->sparks[i][j].color[0];
                work->line.g1 = work->sparks[i][j].color[1];
                work->line.b1 = work->sparks[i][j].color[2];
                if (work->sparks[i][j].elapsed < work->sparks[i][j].duration * 3 / 4) {
                    value = work->sparks[i][j].elapsed * 4;
                    sign = work->sparks[i][j].duration * 3;
                    tip.vx = work->sparks[i][j].target.vx * value / sign;
                    tip.vy = work->sparks[i][j].target.vy * value / sign;
                    tip.vz = work->sparks[i][j].target.vz * value / sign;
                } else {
                    tip.vx = work->sparks[i][j].target.vx;
                    tip.vy = work->sparks[i][j].target.vy;
                    tip.vz = work->sparks[i][j].target.vz;
                }
                mid.vx = (work->root.vx + tip.vx) / 2;
                mid.vy = (work->root.vy + tip.vy) / 2;
                mid.vz = (work->root.vz + tip.vz) / 2;
                RotTransPers(&work->root, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&tip, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                }
                if (work->sparks[i][j].duration / 4 < work->sparks[i][j].elapsed && work->sparks[i][j].spawned == 0) {
                    work->rings[i][j].active = 1;
                    work->rings[i][j].elapsed = 0;
                    work->rings[i][j].color[0] = gModelVariant666_D1AB8[work->command].color[3];
                    work->rings[i][j].color[1] = gModelVariant666_D1AB8[work->command].color[4];
                    work->rings[i][j].color[2] = gModelVariant666_D1AB8[work->command].color[5];
                    work->rings[i][j].u = 0;
                    work->sparks[i][j].spawned = 1;
                }
                work->sparks[i][j].elapsed += step;
                if (work->sparks[i][j].duration < work->sparks[i][j].elapsed) {
                    work->sparks[i][j].active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->count; j++) {
        for (i = 0; i < work->sparks_per_bloom; i++) {
            if (work->rings[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->rings[i][j].elapsed << 13) / work->rings[i][j].duration + 0x800;
                setVector(&work->rings[i][j].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                RotMatrixZ(work->rings[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->rings[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->rings[i][j].position.vx;
                unit.matrix.t[1] = work->rings[i][j].position.vy;
                unit.matrix.t[2] = work->rings[i][j].position.vz;
                unit.super = work->rings[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i][j].duration / 2 < work->rings[i][j].elapsed) {
                    value = (work->rings[i][j].duration - work->rings[i][j].elapsed) * 2;
                    sign = work->rings[i][j].duration;
                    MODEL666_FADE(work->rings[i][j], 3, value, sign);
                }
                for (k = 0; k < 8; k++) {
                    setPolyFT3(&work->fan);
                    work->fan.tpage = work->tpage[1];
                    work->fan.clut = work->clut[1];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, 32, work->rings[i][j].v + 96,
                        0, work->rings[i][j].v + 159,
                        63, work->rings[i][j].v + 159);
                    setRGB0(&work->fan, work->rings[i][j].color[0], work->rings[i][j].color[1], work->rings[i][j].color[2]);
                    depth = RotAverage3(&work->root, &work->rim[k], &work->rim[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                }
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[2];
                work->quad.clut = work->clut[2];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->rings[i][j].u, work->rings[i][j].v,
                    work->rings[i][j].u + 31, work->rings[i][j].v,
                    work->rings[i][j].u, work->rings[i][j].v + 31,
                    work->rings[i][j].u + 31, work->rings[i][j].v + 31);
                setRGB0(&work->quad, work->rings[i][j].color[0], work->rings[i][j].color[1], work->rings[i][j].color[2]);
                depth = RotAverage4(&work->ring[0], &work->ring[1], &work->ring[2], &work->ring[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                if (work->rings[i][j].u < 224) {
                    work->rings[i][j].u += 32;
                }
                work->rings[i][j].elapsed += step;
                if (work->rings[i][j].duration < work->rings[i][j].elapsed) {
                    work->rings[i][j].active = 0;
                }
                if (work->flags[4] == 0) {
                    result = 4;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->count * (work->sparks_per_bloom * 2 + 1)) {
        result = 2;
    }
    return result;
}

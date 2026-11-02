#include "../../types.h"
#include "variant679us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 679's entry (US): shells grow out of model joints as banded
 * lattice cones with a skirt; each finished shell releases five sparks
 * that drift outwards and an animated burst that throws five trails.
 * The setup also prints the work-area size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL679US_FADE(e, offset, value, divisor)                                    \
    (e).color[0] = gModelVariant679Us_D226C[work->command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = gModelVariant679Us_D226C[work->command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = gModelVariant679Us_D226C[work->command].color[(offset) + 2] * (value) / (divisor)

s32 ModelVariant679Us_HandleRequest(u8 *context, s32 command)
{
    Model679UsState *work;
    MATRIX ls;
    MATRIX rotation;
    GsCOORDUNIT unit;
    Model679UsViewSnapshot view;
    u8 red[2], green[2], blue[2], row[2];
    PSXLONG interpolation, flag;
    VECTOR offset;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, sign, angle;
    u32 tex;
    s32 count;
    PSXLONG depth;

    work = (Model679UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model679UsState));
#ifndef VERSION_FRENCH
        printf(gModelVariant679Us_D2120, sizeof(Model679UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        switch (command) {
        case 0:
            work->joints[0] = 11;
            work->joints[1] = 3;
            work->joints[2] = 7;
            work->joints[3] = 10;
            work->joints[4] = 1;
            work->joints[5] = 0;
            work->joints[6] = 2;
            work->joints[7] = 4;
            work->joints[8] = 9;
            work->joints[9] = 8;
            work->joints[10] = 6;
            work->joints[11] = 5;
            break;
        }
        for (i = 0; i < 8; i++) {
            work->frame_u[i] = (i % 4) << 6;
            work->frame_v[i] = (i / 4 << 6) - 128;
        }
        /* Scheduling barrier: the empty loop after the early count
         * assignment keeps the gModelVariant679Us_D2138 high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * the switch below before register allocation. */
        count = 64;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[3]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[6]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[8]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[9]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant679Us_D2138[10]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        switch (command) {
        case 0:
            work->count = count;
            break;
        }
        work->radius = gModelVariant679Us_D226C[command].radius;
        work->height = gModelVariant679Us_D226C[command].height;
        for (i = 0; i < work->count; i++) {
            work->shells[i].duration = 16;
            work->shells[i].active = 0;
            work->shells[i].elapsed = 0;
            work->shells[i].mode = 1;
            work->shells[i].coord = Model_GetCameraCoordinateUnit();
            work->shells[i].color[0] = gModelVariant679Us_D226C[command].color[0];
            work->shells[i].color[1] = gModelVariant679Us_D226C[command].color[1];
            work->shells[i].color[2] = gModelVariant679Us_D226C[command].color[2];
            work->shells[i].u = 0;
            work->shells[i].v = 0;
            setVector(&work->shells[i].scale, 4096, 4096, 4096);
            setVector(&work->shells[i].target, 0, 0, 0);
            setVector(&work->shells[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->shells[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->sparks[i].duration = 32;
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].color[0] = gModelVariant679Us_D226C[command].color[3];
            work->sparks[i].color[1] = gModelVariant679Us_D226C[command].color[4];
            work->sparks[i].color[2] = gModelVariant679Us_D226C[command].color[5];
            work->sparks[i].u = 0;
            work->sparks[i].v = 64;
            setVector(&work->sparks[i].scale, 4096, 4096, 4096);
            setVector(&work->sparks[i].target, 0, 0, 0);
            setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sparks[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 32;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = gModelVariant679Us_D226C[command].color[3];
            work->bursts[i].color[1] = gModelVariant679Us_D226C[command].color[4];
            work->bursts[i].color[2] = gModelVariant679Us_D226C[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 32;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            value = work->radius * (s32)((u32)(rsin(i * 500) * 5) << 2) / 4096;
            sign = i % 2 * 2 - 1;
            setVector(&work->bursts[i].position,
                sign * value * rsin((i % 16) << 8) / 4096,
                sign * value * rcos((i % 16) << 8) / 4096,
                value * rsin((i % 16) << 8) / 4096);
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        setVector(&offset, work->radius * 5, 0, 0);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            rotation = D_800FE168;
            RotMatrixZ((i << 12) / 5, &rotation);
            ApplyMatrixLV(&rotation, &offset, &work->spark_offsets[i]);
            PopMatrix();
        }
        setVector(&offset, 0, work->radius * 20, 0);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            rotation = D_800FE168;
            RotMatrixZ((i << 11) / 5, &rotation);
            RotMatrixY((i % 3 << 12) / 3, &rotation);
            ApplyMatrixLV(&rotation, &offset, &work->trail_offsets[i]);
            PopMatrix();
        }
        sign = work->height * work->direction;
        for (i = 0; i < 5; i++) {
            value = work->radius * rcos(i * 819 / 4) / 4096;
            for (j = 0; j < 5; j++) {
                setVector(&work->grid[j][i], value * rsin(j << 10) / 4096, value * rcos(j << 10) / 4096,
                    i * sign / 4);
            }
        }
        value = work->radius * 2;
        setVector(&work->square[0], -value, -value, 0);
        setVector(&work->square[1], value, -value, 0);
        setVector(&work->square[2], -value, value, 0);
        setVector(&work->square[3], value, value, 0);
        value = work->radius * 2;
        sign = work->radius * 4;
        for (i = 0; i < 6; i++) {
            angle = (i << 12) / 5;
            setVector(&work->inner[i], value * rsin(angle) / 4096, value * rcos(angle) / 4096, 0);
            setVector(&work->outer[i], sign * rsin(angle) / 4096, sign * rcos(angle) / 4096,
                work->height * 2 / 3 * work->direction);
        }
        value = work->radius * 7;
        setVector(&work->burst[0], -value, -value, 0);
        setVector(&work->burst[1], value, -value, 0);
        setVector(&work->burst[2], -value, value, 0);
        setVector(&work->burst[3], value, value, 0);
        value = work->radius * 4;
        setVector(&work->trail[0], -value, -value, 0);
        setVector(&work->trail[1], value, -value, 0);
        setVector(&work->trail[2], -value, value, 0);
        setVector(&work->trail[3], value, value, 0);
        work->delay = gModelVariant679Us_D226C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed < 162 && work->elapsed >= work->flags[0] * 3 && work->flags[0] < work->count) {
            work->shells[work->flags[0]].active = 1;
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, work->joints[(u8)(work->flags[0] % 12)] + 6);
            GsGetLwUnit(&unit, &ls);
            (work->shells + work->flags[0])->position.vx = ls.t[0];
            (work->shells + work->flags[0])->position.vy = ls.t[1];
            (work->shells + work->flags[0])->position.vz = ls.t[2];
            work->sparks[work->flags[0]].active = 1;
            work->sparks[work->flags[0]].position = work->shells[work->flags[0]].position;
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(Model679UsViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->shells[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = (work->shells[i].elapsed << 12) / work->shells[i].duration;
            setVector(&work->shells[i].scale, 4096, 4096, value);
            ScaleMatrix(&unit.matrix, &work->shells[i].scale);
            unit.matrix.t[0] = work->shells[i].position.vx;
            unit.matrix.t[1] = work->shells[i].position.vy;
            unit.matrix.t[2] = work->shells[i].position.vz;
            unit.super = work->shells[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->shells[i].elapsed > (u32)work->shells[i].duration / 2) {
                value = (work->shells[i].duration - work->shells[i].elapsed) * 2;
                sign = work->shells[i].duration;
                MODEL679US_FADE(work->shells[i], 0, value, sign);
            }
            for (j = 0; j < 4; j++) {
                if (j == 0) {
                    red[0] = 0;
                    green[0] = 0;
                    blue[0] = 0;
                } else {
                    red[0] = work->shells[i].color[0] * (4 - j) / 4;
                    green[0] = work->shells[i].color[1] * (4 - j) / 4;
                    blue[0] = work->shells[i].color[2] * (4 - j) / 4;
                }
                red[1] = work->shells[i].color[0] * (3 - j) / 4;
                green[1] = work->shells[i].color[1] * (3 - j) / 4;
                blue[1] = work->shells[i].color[2] * (3 - j) / 4;
                row[0] = work->shells[i].v + (4 - j) * 12;
                row[1] = work->shells[i].v + (3 - j) * 12;
                for (k = 0; k < 4; k++) {
                    setPolyGT4(&work->band);
                    work->band.tpage = work->tpage[0];
                    work->band.clut = work->clut[0];
                    setSemiTrans(&work->band, 1);
                    setUV4(&work->band, work->shells[i].u, row[0],
                        work->shells[i].u + 31, row[0],
                        work->shells[i].u, row[1],
                        work->shells[i].u + 31, row[1]);
                    setRGB0(&work->band, red[0], green[0], blue[0]);
                    setRGB1(&work->band, red[0], green[0], blue[0]);
                    setRGB2(&work->band, red[1], green[1], blue[1]);
                    setRGB3(&work->band, red[1], green[1], blue[1]);
                    depth = RotAverage4(&work->grid[k][j], &work->grid[k + 1][j],
                        &work->grid[k][j + 1], &work->grid[k + 1][j + 1],
                        (PSXLONG *)&work->band.x0, (PSXLONG *)&work->band.x1,
                        (PSXLONG *)&work->band.x2, (PSXLONG *)&work->band.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->band, func_80058F10(), (u16)depth);
                    }
                }
            }
            for (j = 0; j < 5; j++) {
                setPolyGT4(&work->band);
                work->band.tpage = work->tpage[2];
                work->band.clut = work->clut[2];
                setSemiTrans(&work->band, 1);
                setUV4(&work->band, 32, 0, 95, 0, 32, 16, 95, 16);
                setRGB0(&work->band, work->shells[i].color[0], work->shells[i].color[1], work->shells[i].color[2]);
                setRGB1(&work->band, work->shells[i].color[0], work->shells[i].color[1], work->shells[i].color[2]);
                setRGB2(&work->band, 0, 0, 0);
                setRGB3(&work->band, 0, 0, 0);
                depth = RotAverage4(&work->inner[j], &work->inner[j + 1], &work->outer[j], &work->outer[j + 1],
                    (PSXLONG *)&work->band.x0, (PSXLONG *)&work->band.x1,
                    (PSXLONG *)&work->band.x2, (PSXLONG *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->band, func_80058F10(), (u16)depth);
                }
            }
            work->shells[i].elapsed += step;
            if (work->shells[i].duration < work->shells[i].elapsed) {
                work->shells[i].active = 0;
                work->bursts[i].active = 1;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->sparks[i].active == 1) {
            value = (work->sparks[i].elapsed << 12) / work->sparks[i].duration + 4096;
            setVector(&work->sparks[i].scale, value, value, value);
            if (work->sparks[i].elapsed > (u32)work->sparks[i].duration / 2) {
                value = (work->sparks[i].duration - work->sparks[i].elapsed) * 2;
                sign = work->sparks[i].duration;
                MODEL679US_FADE(work->sparks[i], 3, value, sign);
            }
            for (k = 0; k < 5; k++) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->sparks[i].rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->sparks[i].position.vx
                    + work->spark_offsets[k].vx * work->sparks[i].elapsed / work->sparks[i].duration;
                unit.matrix.t[1] = work->sparks[i].position.vy
                    + work->spark_offsets[k].vy * work->sparks[i].elapsed / work->sparks[i].duration;
                unit.matrix.t[2] = work->sparks[i].position.vz
                    + work->spark_offsets[k].vz * work->sparks[i].elapsed / work->sparks[i].duration;
                unit.super = work->sparks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->sparks[i].u, work->sparks[i].v,
                    work->sparks[i].u + 31, work->sparks[i].v,
                    work->sparks[i].u, work->sparks[i].v + 31,
                    work->sparks[i].u + 31, work->sparks[i].v + 31);
                setRGB0(&work->quad, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
            }
            if (work->sparks[i].u < 224) {
                work->sparks[i].u += 32;
            }
            work->sparks[i].elapsed += step;
            if (work->sparks[i].duration < work->sparks[i].elapsed) {
                work->sparks[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->bursts[i].active == 1) {
            value = (work->bursts[i].elapsed << 13) / work->bursts[i].duration + 4096;
            setVector(&work->bursts[i].scale, value, value, value);
            if (work->bursts[i].elapsed > (u32)work->bursts[i].duration / 2) {
                value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                sign = work->bursts[i].duration;
                MODEL679US_FADE(work->bursts[i], 3, value, sign);
            }
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
            RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            angle = 0xC06;
            RotMatrixY(work->bursts[i].rotation.vy + angle - view.yaw, &unit.matrix);
            unit.matrix.t[0] = work->bursts[i].position.vx;
            unit.matrix.t[1] = work->bursts[i].position.vy;
            unit.matrix.t[2] = work->bursts[i].position.vz;
            unit.super = work->bursts[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            setPolyFT4(&work->quad);
            work->quad.tpage = work->tpage[3];
            work->quad.clut = work->clut[3];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->frame_u[work->bursts[i].u], work->frame_v[work->bursts[i].u],
                work->frame_u[work->bursts[i].u] + 63, work->frame_v[work->bursts[i].u],
                work->frame_u[work->bursts[i].u], work->frame_v[work->bursts[i].u] + 63,
                work->frame_u[work->bursts[i].u] + 63, work->frame_v[work->bursts[i].u] + 63);
            setRGB0(&work->quad, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
            depth = RotAverage4(&work->burst[0], &work->burst[1], &work->burst[2], &work->burst[3],
                (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
            }
            for (j = 0; j < 5; j++) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
            RotMatrixY(work->bursts[i].rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx
                    + work->trail_offsets[j].vx * work->bursts[i].elapsed / work->bursts[i].duration;
                unit.matrix.t[1] = work->bursts[i].position.vy
                    + work->trail_offsets[j].vy * work->bursts[i].elapsed / work->bursts[i].duration;
                unit.matrix.t[2] = work->bursts[i].position.vz
                    + work->trail_offsets[j].vz * work->bursts[i].elapsed / work->bursts[i].duration;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->bursts[i].v, 64,
                    work->bursts[i].v + 31, 64,
                    work->bursts[i].v, 95,
                    work->bursts[i].v + 31, 95);
                setRGB0(&work->quad, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                depth = RotAverage4(&work->trail[0], &work->trail[1], &work->trail[2], &work->trail[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
            }
            if (work->bursts[i].u < 7) {
                work->bursts[i].u++;
            }
            if (work->bursts[i].v < 224) {
                work->bursts[i].v += 32;
            }
            work->bursts[i].elapsed += step;
            if (work->bursts[i].duration < work->bursts[i].elapsed) {
                work->bursts[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->count * 3) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

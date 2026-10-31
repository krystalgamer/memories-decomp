#include "../../types.h"
#include "image60348_entry.h"
#ifdef MODEL_IMAGE60348_WORK_SIZE_PRINTF
#include "../../psyq/stdio.h"
#endif

/* Rings drop fading echoes and release camera-facing sprite/shard/cone bursts. */

#define START_RING(index)                                                       \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);            \
    GsGetLwUnit(&unit, &ls);                                                     \
    work->rings[work->flags[1]].active = 1;                                      \
    setVector(&work->rings[work->flags[1]].position, ls.t[0], ls.t[1], ls.t[2]);  \
    work->flags[1]++;                                                           \
    if (work->flags[1] == work->ring_count) {                                    \
        work->flags[2] = 1;                                                     \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Image60348State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Image60348ViewSnapshot view;
    SVECTOR point[4];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value, amount;
    u32 tex;
    u8 r, g, b;
    PSXLONG depth;

    work = (Image60348State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Image60348State));
#ifdef MODEL_IMAGE60348_WORK_SIZE_PRINTF
        printf(MODEL_IMAGE60348_WORK_SIZE_FORMAT, sizeof(Image60348State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->tint_start.r = 128;
        work->tint_start.g = 128;
        work->tint_start.b = 128;
        work->tint_end.r = 0;
        work->tint_end.g = 0;
        work->tint_end.b = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D6C8[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D6C8[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D6C8[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 2:
            work->ring_count = 8;
            work->echo_count = 16;
            break;
        case 1:
            work->ring_count = 1;
            work->echo_count = 16;
            break;
        }
        work->radius = D_8013D71C[command].radius;
        work->height = D_8013D71C[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].duration = 32;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013D71C[command].color[0];
            work->rings[i].color[1] = D_8013D71C[command].color[1];
            work->rings[i].color[2] = D_8013D71C[command].color[2];
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, work->direction * 1350);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
            work->rings[i].echo = 0;
            work->rings[i].released = 0;
        }
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->echo_count; i++) {
                work->echoes[i][j].active = 0;
                work->echoes[i][j].elapsed = 0;
                work->echoes[i][j].duration = 16;
                work->echoes[i][j].mode = 1;
                work->echoes[i][j].coord = Model_GetCameraCoordinateUnit();
                work->echoes[i][j].color[0] = D_8013D71C[command].color[0];
                work->echoes[i][j].color[1] = D_8013D71C[command].color[1];
                work->echoes[i][j].color[2] = D_8013D71C[command].color[2];
                work->echoes[i][j].u = 0;
                work->echoes[i][j].v = 0;
                setVector(&work->echoes[i][j].scale, 4096, 4096, 4096);
                setVector(&work->echoes[i][j].target, 0, 0, 0);
                setVector(&work->echoes[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->echoes[i][j].position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->ring_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 32;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013D71C[command].color[3];
            work->bursts[i].color[1] = D_8013D71C[command].color[4];
            work->bursts[i].color[2] = D_8013D71C[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i].position, 0, 0, 0);
        }
        work->delay = D_8013D71C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 33; i++) {
            setVector(&work->inner[i], 0, work->radius * rsin(i * 64 + 1024) / 4096,
                work->radius + work->radius * rcos(i * 64 + 1024) / 4096);
            setVector(&work->outer[i], 0, work->radius * rsin(i * 64 + 1024) / 4096,
                work->radius + (work->radius * rcos(i * 64 + 1024) / 4096
                    - work->height * rsin(i << 6) / 4096));
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        value = work->radius / 2;
        amount = work->radius;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], value * rsin(i << 8) / 4096,
                value * rcos(i << 8) / 4096, amount);
            setVector(&work->rim[i], amount * rsin(i << 8) / 4096,
                amount * rcos(i << 8) / 4096, amount * 5 / 4);
        }
        value = (u16)(work->radius / 3);
        amount = work->height * 2;
        setVector(&point[0], -amount, 0, 0);
        setVector(&point[1], 0, value, 0);
        setVector(&point[2], 0, -value, 0);
        setVector(&point[3], amount, 0, 0);
        m = D_800FE168;
        RotMatrixZ(512, &m);
        for (i = 0; i < 4; i++) {
            PushMatrix();
            ApplyMatrixSV(&m, &point[i], &work->spikes[i]);
            PopMatrix();
        }
        value = work->radius * 3 / 2;
        setVector(&work->flash_corners[0], -value, -value, 0);
        setVector(&work->flash_corners[1], value, -value, 0);
        setVector(&work->flash_corners[2], -value, value, 0);
        setVector(&work->flash_corners[3], value, value, 0);
        value = work->radius * 3 / 2;
        amount = work->radius * 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->cone_inner[i], value * rsin(i << 8) / 4096, 0,
                value * rcos(i << 8) / 4096);
            setVector(&work->cone_outer[i], amount * rsin(i << 8) / 4096, 0,
                amount * rcos(i << 8) / 4096);
            setVector(&work->cone_middle[i], (work->cone_inner[i].vx + work->cone_outer[i].vx) / 2,
                (work->cone_inner[i].vy + work->cone_outer[i].vy) / 2,
                (work->cone_inner[i].vz + work->cone_outer[i].vz) / 2);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->flags[1] * 35 + 116 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(21);
            }
            if (work->elapsed < 100) {
                if (work->flags[0] >= 3) {
                    Model_QueueTintRequestForParts(work->slot, 2, work->tint_start, work->tint_end, 10, 21, -1);
                    work->flags[0] = 0;
                } else {
                    work->flags[0] += step;
                }
            }
            Model_SetFrameStepOverride(1);
            break;
        case 1:
            if (work->flags[1] + 148 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(10);
            }
            if (work->elapsed < 120) {
                if (work->flags[0] >= 3) {
                    Model_QueueTintRequestForParts(work->slot, 2, work->tint_start, work->tint_end, 10, 12, -1);
                    work->flags[0] = 0;
                } else {
                    work->flags[0] += step;
                }
            }
            Model_SetFrameStepOverride(1);
            break;
        case 2:
            if (work->flags[1] * 40 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(7);
            }
            break;
        }
        PushMatrix();
        view = *(Image60348ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->rings[i].elapsed << 14) / work->rings[i].duration + 4096;
                setVector(&work->rings[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz
                    + (work->rings[i].target.vz - work->rings[i].position.vz) * work->rings[i].elapsed
                        / work->rings[i].duration;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                    value = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                    amount = work->rings[i].duration;
                    work->rings[i].color[0] = D_8013D71C[work->command].color[0] * value / amount;
                    work->rings[i].color[1] = D_8013D71C[work->command].color[1] * value / amount;
                    work->rings[i].color[2] = D_8013D71C[work->command].color[2] * value / amount;
                }
                for (j = 0; j < 32; j++) {
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->rings[i].u + 64, work->rings[i].v,
                        work->rings[i].u + 64, work->rings[i].v + 63,
                        work->rings[i].u + 127, work->rings[i].v,
                        work->rings[i].u + 127, work->rings[i].v + 63);
                    setRGB0(&work->sprite, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1], &work->outer[j], &work->outer[j + 1],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                }
                work->echoes[work->rings[i].echo][i].active = 1;
                work->echoes[work->rings[i].echo][i].elapsed = 0;
                work->echoes[work->rings[i].echo][i].scale = work->rings[i].scale;
                work->echoes[work->rings[i].echo][i].rotation = work->rings[i].rotation;
                work->echoes[work->rings[i].echo][i].color[0] = work->rings[i].color[0];
                work->echoes[work->rings[i].echo][i].color[1] = work->rings[i].color[1];
                work->echoes[work->rings[i].echo][i].color[2] = work->rings[i].color[2];
                setVector(&work->echoes[work->rings[i].echo][i].position,
                    unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                work->rings[i].echo++;
                if (work->rings[i].echo >= work->echo_count) {
                    work->rings[i].echo = 0;
                }
                if (work->rings[i].elapsed > D_8013D71C[work->command].release && work->rings[i].released == 0) {
                    work->rings[i].released = 1;
                    work->bursts[i].active = 1;
                    setVector(&work->bursts[i].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->rings[i].elapsed << 14) / work->rings[i].duration + 4096;
                setVector(&work->rings[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz + work->radius * work->direction;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->rings[i].u, work->rings[i].v + 64,
                        work->rings[i].u, work->rings[i].v + 191,
                        work->rings[i].u + 31, work->rings[i].v + 64,
                        work->rings[i].u + 31, work->rings[i].v + 191);
                    setRGB1(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->ring[j], &work->rim[j], &work->ring[j + 1], &work->rim[j + 1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->echo_count; i++) {
                if (work->echoes[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->echoes[i][j].scale);
                    RotMatrixY(work->echoes[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->echoes[i][j].position.vx;
                    unit.matrix.t[1] = work->echoes[i][j].position.vy;
                    unit.matrix.t[2] = work->echoes[i][j].position.vz;
                    unit.super = work->echoes[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    value = work->echoes[i][j].duration - work->echoes[i][j].elapsed;
                    amount = work->echoes[i][j].duration;
                    r = work->echoes[i][j].color[0] * value / amount;
                    g = work->echoes[i][j].color[1] * value / amount;
                    b = work->echoes[i][j].color[2] * value / amount;
                    if (work->echoes[i][j].elapsed != 0) {
                        for (k = 0; k < 32; k++) {
                            setPolyFT4(&work->sprite);
                            work->sprite.tpage = work->tpage[0];
                            work->sprite.clut = work->clut[0];
                            setSemiTrans(&work->sprite, 1);
                            setUV4(&work->sprite, work->echoes[i][j].u + 64, work->echoes[i][j].v,
                                work->echoes[i][j].u + 64, work->echoes[i][j].v + 63,
                                work->echoes[i][j].u + 127, work->echoes[i][j].v,
                                work->echoes[i][j].u + 127, work->echoes[i][j].v + 63);
                            setRGB0(&work->sprite, r, g, b);
                            depth = RotAverage4(&work->inner[k], &work->inner[k + 1], &work->outer[k],
                                &work->outer[k + 1], (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                                (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->echoes[i][j].elapsed += step;
                    if (work->echoes[i][j].duration < work->echoes[i][j].elapsed) {
                        work->echoes[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->ring_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->bursts[i].elapsed << 13) / work->bursts[i].duration + 4096;
                setVector(&work->bursts[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    amount = work->bursts[i].duration;
                    work->bursts[i].color[0] = D_8013D71C[work->command].color[3] * value / amount;
                    work->bursts[i].color[1] = D_8013D71C[work->command].color[4] * value / amount;
                    work->bursts[i].color[2] = D_8013D71C[work->command].color[5] * value / amount;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v,
                    work->bursts[i].u + 63, work->bursts[i].v,
                    work->bursts[i].u, work->bursts[i].v + 63,
                    work->bursts[i].u + 63, work->bursts[i].v + 63);
                setRGB0(&work->sprite, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                for (j = 0; j < 2; j++) {
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[0];
                    work->shard.clut = work->clut[0];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->bursts[i].u + 96, work->bursts[i].v,
                        work->bursts[i].u + 64, work->bursts[i].v + 63,
                        work->bursts[i].u + 127, work->bursts[i].v + 63);
                    setRGB0(&work->shard, 0, 0, 0);
                    setRGB1(&work->shard, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    setRGB2(&work->shard, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    depth = RotAverage3(&work->spikes[j * 3], &work->spikes[1], &work->spikes[2],
                        (PSXLONG *)&work->shard.x0, (PSXLONG *)&work->shard.x1, (PSXLONG *)&work->shard.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shard, func_80058F10(), (u16)depth);
                    }
                }
                for (j = 0; j < 4; j++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                    value = j % 2 * 2 - 1;
                    amount = (j << 10) + 0x155;
                    RotMatrixZ(value * work->bursts[i].elapsed * 150, &unit.matrix);
                    RotMatrixX(amount, &unit.matrix);
                    RotMatrixY(amount, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i].position.vx;
                    unit.matrix.t[1] = work->bursts[i].position.vy;
                    unit.matrix.t[2] = work->bursts[i].position.vz;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v + 192,
                        work->bursts[i].u + 63, work->bursts[i].v + 192,
                        work->bursts[i].u, work->bursts[i].v + 255,
                        work->bursts[i].u + 63, work->bursts[i].v + 255);
                    setRGB0(&work->sprite, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    depth = RotAverage4(&work->flash_corners[0], &work->flash_corners[1], &work->flash_corners[2],
                        &work->flash_corners[3], (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                    value = (j << 10) + 0x155;
                    RotMatrixZ(value, &unit.matrix);
                    RotMatrixX(value, &unit.matrix);
                    RotMatrixY(value, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i].position.vx;
                    unit.matrix.t[1] = work->bursts[i].position.vy;
                    unit.matrix.t[2] = work->bursts[i].position.vz;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->bursts[i].u, work->bursts[i].v + 64,
                            work->bursts[i].u, work->bursts[i].v + 128,
                            work->bursts[i].u + 31, work->bursts[i].v + 64,
                            work->bursts[i].u + 31, work->bursts[i].v + 128);
                        setRGB1(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->cone_inner[k], &work->cone_middle[k], &work->cone_inner[k + 1],
                            &work->cone_middle[k + 1], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->bursts[i].u, work->bursts[i].v + 128,
                            work->bursts[i].u, work->bursts[i].v + 191,
                            work->bursts[i].u + 31, work->bursts[i].v + 128,
                            work->bursts[i].u + 31, work->bursts[i].v + 191);
                        setRGB0(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                        setRGB3(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->cone_middle[k], &work->cone_outer[k], &work->cone_middle[k + 1],
                            &work->cone_outer[k + 1], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
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
        if (inactive == work->ring_count * (work->echo_count + 2) && work->flags[2] == 1) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

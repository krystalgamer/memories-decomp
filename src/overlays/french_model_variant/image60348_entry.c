#include "../../types.h"
#include "image60348_entry.h"
#ifdef MODEL_IMAGE60348_WORK_SIZE_PRINTF
#include "../../psyq/stdio.h"
/* image60348_entry_usa.c defines gModelVariant664_szWorkSizeFormat
   before including this source. */
#endif

/* Rings drop fading echoes and release camera-facing sprite/shard/cone bursts. */

#define START_RING(index)                                                       \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, index);            \
    GsGetLwUnit(&unit, &ls);                                                     \
    work->rings[work->flags[1]].head.active = 1;                                      \
    setVector(&work->rings[work->flags[1]].xform.position, ls.t[0], ls.t[1], ls.t[2]);  \
    work->flags[1]++;                                                           \
    if (work->flags[1] == work->ring_count) {                                    \
        work->flags[2] = 1;                                                     \
    }

s32 FrenchModelVariant681_HandleRequest(u8 *context, s32 command)
{
    Image60348State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR point[4];
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value, amount;
    u32 tex;
    u8 r, g, b;
    long depth;

    work = (Image60348State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Image60348State));
#ifdef MODEL_IMAGE60348_WORK_SIZE_PRINTF
        printf(gModelVariant664_szWorkSizeFormat, sizeof(Image60348State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        work->tint_start.r = 128;
        work->tint_start.g = 128;
        work->tint_start.b = 128;
        work->tint_end.r = 0;
        work->tint_end.g = 0;
        work->tint_end.b = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gFrenchModelVariant681_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gFrenchModelVariant681_aImages[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gFrenchModelVariant681_aImages[0]);
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
        work->radius = gFrenchModelVariant681_aConfigs[command].radius;
        work->height = gFrenchModelVariant681_aConfigs[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            work->rings[i].head.duration = 32;
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gFrenchModelVariant681_aConfigs[command].color[0];
            work->rings[i].head.color[1] = gFrenchModelVariant681_aConfigs[command].color[1];
            work->rings[i].head.color[2] = gFrenchModelVariant681_aConfigs[command].color[2];
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 0;
            setVector(&work->rings[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, work->direction * 1350);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
            work->rings[i].echo = 0;
            work->rings[i].released = 0;
        }
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->echo_count; i++) {
                work->echoes[i][j].head.active = 0;
                work->echoes[i][j].head.elapsed = 0;
                work->echoes[i][j].head.duration = 16;
                work->echoes[i][j].head.mode = 1;
                work->echoes[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->echoes[i][j].head.color[0] = gFrenchModelVariant681_aConfigs[command].color[0];
                work->echoes[i][j].head.color[1] = gFrenchModelVariant681_aConfigs[command].color[1];
                work->echoes[i][j].head.color[2] = gFrenchModelVariant681_aConfigs[command].color[2];
                work->echoes[i][j].head.u = 0;
                work->echoes[i][j].head.v = 0;
                setVector(&work->echoes[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->echoes[i][j].target, 0, 0, 0);
                setVector(&work->echoes[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->echoes[i][j].xform.position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->ring_count; i++) {
            work->bursts[i].head.active = 0;
            work->bursts[i].head.elapsed = 0;
            work->bursts[i].head.duration = 32;
            work->bursts[i].head.mode = 1;
            work->bursts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].head.color[0] = gFrenchModelVariant681_aConfigs[command].color[3];
            work->bursts[i].head.color[1] = gFrenchModelVariant681_aConfigs[command].color[4];
            work->bursts[i].head.color[2] = gFrenchModelVariant681_aConfigs[command].color[5];
            work->bursts[i].head.u = 0;
            work->bursts[i].head.v = 0;
            setVector(&work->bursts[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->bursts[i].xform.position, 0, 0, 0);
        }
        work->delay = gFrenchModelVariant681_aConfigs[command].delay;
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
        m = GsIDMATRIX;
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
        switch (work->head.command) {
        case 0:
            if (work->flags[1] * 35 + 116 < work->head.elapsed && work->flags[1] < work->ring_count) {
                START_RING(21);
            }
            if (work->head.elapsed < 100) {
                if (work->flags[0] >= 3) {
                    Model_QueueTintRequestForParts(work->head.slot, 2, work->tint_start, work->tint_end, 10, 21, -1);
                    work->flags[0] = 0;
                } else {
                    work->flags[0] += step;
                }
            }
            Model_SetFrameStepOverride(1);
            break;
        case 1:
            if (work->flags[1] + 148 < work->head.elapsed && work->flags[1] < work->ring_count) {
                START_RING(10);
            }
            if (work->head.elapsed < 120) {
                if (work->flags[0] >= 3) {
                    Model_QueueTintRequestForParts(work->head.slot, 2, work->tint_start, work->tint_end, 10, 12, -1);
                    work->flags[0] = 0;
                } else {
                    work->flags[0] += step;
                }
            }
            Model_SetFrameStepOverride(1);
            break;
        case 2:
            if (work->flags[1] * 40 < work->head.elapsed && work->flags[1] < work->ring_count) {
                START_RING(7);
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->rings[i].head.elapsed << 14) / work->rings[i].head.duration + 4096;
                setVector(&work->rings[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixY(work->rings[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].xform.position.vx;
                unit.matrix.t[1] = work->rings[i].xform.position.vy;
                unit.matrix.t[2] = work->rings[i].xform.position.vz
                    + (work->rings[i].target.vz - work->rings[i].xform.position.vz) * work->rings[i].head.elapsed
                        / work->rings[i].head.duration;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].head.duration / 2 < work->rings[i].head.elapsed) {
                    value = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 2;
                    amount = work->rings[i].head.duration;
                    work->rings[i].head.color[0] = gFrenchModelVariant681_aConfigs[work->head.command].color[0] * value / amount;
                    work->rings[i].head.color[1] = gFrenchModelVariant681_aConfigs[work->head.command].color[1] * value / amount;
                    work->rings[i].head.color[2] = gFrenchModelVariant681_aConfigs[work->head.command].color[2] * value / amount;
                }
                for (j = 0; j < 32; j++) {
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->rings[i].head.u + 64, work->rings[i].head.v,
                        work->rings[i].head.u + 64, work->rings[i].head.v + 63,
                        work->rings[i].head.u + 127, work->rings[i].head.v,
                        work->rings[i].head.u + 127, work->rings[i].head.v + 63);
                    setRGB0(&work->sprite, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1], &work->outer[j], &work->outer[j + 1],
                        (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                        (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->echoes[work->rings[i].echo][i].head.active = 1;
                work->echoes[work->rings[i].echo][i].head.elapsed = 0;
                work->echoes[work->rings[i].echo][i].xform.scale = work->rings[i].xform.scale;
                work->echoes[work->rings[i].echo][i].xform.rotation = work->rings[i].xform.rotation;
                work->echoes[work->rings[i].echo][i].head.color[0] = work->rings[i].head.color[0];
                work->echoes[work->rings[i].echo][i].head.color[1] = work->rings[i].head.color[1];
                work->echoes[work->rings[i].echo][i].head.color[2] = work->rings[i].head.color[2];
                setVector(&work->echoes[work->rings[i].echo][i].xform.position,
                    unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                work->rings[i].echo++;
                if (work->rings[i].echo >= work->echo_count) {
                    work->rings[i].echo = 0;
                }
                if (work->rings[i].head.elapsed > gFrenchModelVariant681_aConfigs[work->head.command].release && work->rings[i].released == 0) {
                    work->rings[i].released = 1;
                    work->bursts[i].head.active = 1;
                    setVector(&work->bursts[i].xform.position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->rings[i].head.elapsed << 14) / work->rings[i].head.duration + 4096;
                setVector(&work->rings[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixY(work->rings[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].xform.position.vx;
                unit.matrix.t[1] = work->rings[i].xform.position.vy;
                unit.matrix.t[2] = work->rings[i].xform.position.vz + work->radius * work->direction;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->rings[i].head.u, work->rings[i].head.v + 64,
                        work->rings[i].head.u, work->rings[i].head.v + 191,
                        work->rings[i].head.u + 31, work->rings[i].head.v + 64,
                        work->rings[i].head.u + 31, work->rings[i].head.v + 191);
                    setRGB1(&work->strip, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->ring[j], &work->rim[j], &work->ring[j + 1], &work->rim[j + 1],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->rings[i].head.elapsed += step;
                if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                    work->rings[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->echo_count; i++) {
                if (work->echoes[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->echoes[i][j].xform.scale);
                    RotMatrixY(work->echoes[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->echoes[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->echoes[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->echoes[i][j].xform.position.vz;
                    unit.super = work->echoes[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    value = work->echoes[i][j].head.duration - work->echoes[i][j].head.elapsed;
                    amount = work->echoes[i][j].head.duration;
                    r = work->echoes[i][j].head.color[0] * value / amount;
                    g = work->echoes[i][j].head.color[1] * value / amount;
                    b = work->echoes[i][j].head.color[2] * value / amount;
                    if (work->echoes[i][j].head.elapsed != 0) {
                        for (k = 0; k < 32; k++) {
                            setPolyFT4(&work->sprite);
                            work->sprite.tpage = work->tpage[0];
                            work->sprite.clut = work->clut[0];
                            setSemiTrans(&work->sprite, 1);
                            setUV4(&work->sprite, work->echoes[i][j].head.u + 64, work->echoes[i][j].head.v,
                                work->echoes[i][j].head.u + 64, work->echoes[i][j].head.v + 63,
                                work->echoes[i][j].head.u + 127, work->echoes[i][j].head.v,
                                work->echoes[i][j].head.u + 127, work->echoes[i][j].head.v + 63);
                            setRGB0(&work->sprite, r, g, b);
                            depth = RotAverage4(&work->inner[k], &work->inner[k + 1], &work->outer[k],
                                &work->outer[k + 1], (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                                (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    work->echoes[i][j].head.elapsed += step;
                    if (work->echoes[i][j].head.duration < work->echoes[i][j].head.elapsed) {
                        work->echoes[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->ring_count; i++) {
            if (work->bursts[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->bursts[i].head.elapsed << 13) / work->bursts[i].head.duration + 4096;
                setVector(&work->bursts[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
                RotMatrixZ(work->bursts[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                unit.super = work->bursts[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i].head.duration / 2 < work->bursts[i].head.elapsed) {
                    value = (work->bursts[i].head.duration - work->bursts[i].head.elapsed) * 2;
                    amount = work->bursts[i].head.duration;
                    work->bursts[i].head.color[0] = gFrenchModelVariant681_aConfigs[work->head.command].color[3] * value / amount;
                    work->bursts[i].head.color[1] = gFrenchModelVariant681_aConfigs[work->head.command].color[4] * value / amount;
                    work->bursts[i].head.color[2] = gFrenchModelVariant681_aConfigs[work->head.command].color[5] * value / amount;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->bursts[i].head.u, work->bursts[i].head.v,
                    work->bursts[i].head.u + 63, work->bursts[i].head.v,
                    work->bursts[i].head.u, work->bursts[i].head.v + 63,
                    work->bursts[i].head.u + 63, work->bursts[i].head.v + 63);
                setRGB0(&work->sprite, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                for (j = 0; j < 2; j++) {
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[0];
                    work->shard.clut = work->clut[0];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->bursts[i].head.u + 96, work->bursts[i].head.v,
                        work->bursts[i].head.u + 64, work->bursts[i].head.v + 63,
                        work->bursts[i].head.u + 127, work->bursts[i].head.v + 63);
                    setRGB0(&work->shard, 0, 0, 0);
                    setRGB1(&work->shard, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                    setRGB2(&work->shard, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                    depth = RotAverage3(&work->spikes[j * 3], &work->spikes[1], &work->spikes[2],
                        (long *)&work->shard.x0, (long *)&work->shard.x1, (long *)&work->shard.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shard, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                for (j = 0; j < 4; j++) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
                    value = j % 2 * 2 - 1;
                    amount = (j << 10) + 0x155;
                    RotMatrixZ(value * work->bursts[i].head.elapsed * 150, &unit.matrix);
                    RotMatrixX(amount, &unit.matrix);
                    RotMatrixY(amount, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                    unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                    unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                    unit.super = work->bursts[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i].head.u, work->bursts[i].head.v + 192,
                        work->bursts[i].head.u + 63, work->bursts[i].head.v + 192,
                        work->bursts[i].head.u, work->bursts[i].head.v + 255,
                        work->bursts[i].head.u + 63, work->bursts[i].head.v + 255);
                    setRGB0(&work->sprite, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                    depth = RotAverage4(&work->flash_corners[0], &work->flash_corners[1], &work->flash_corners[2],
                        &work->flash_corners[3], (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                        (long *)&work->sprite.x2, (long *)&work->sprite.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
                    value = (j << 10) + 0x155;
                    RotMatrixZ(value, &unit.matrix);
                    RotMatrixX(value, &unit.matrix);
                    RotMatrixY(value, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                    unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                    unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                    unit.super = work->bursts[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->bursts[i].head.u, work->bursts[i].head.v + 64,
                            work->bursts[i].head.u, work->bursts[i].head.v + 128,
                            work->bursts[i].head.u + 31, work->bursts[i].head.v + 64,
                            work->bursts[i].head.u + 31, work->bursts[i].head.v + 128);
                        setRGB1(&work->strip, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->cone_inner[k], &work->cone_middle[k], &work->cone_inner[k + 1],
                            &work->cone_middle[k + 1], (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->bursts[i].head.u, work->bursts[i].head.v + 128,
                            work->bursts[i].head.u, work->bursts[i].head.v + 191,
                            work->bursts[i].head.u + 31, work->bursts[i].head.v + 128,
                            work->bursts[i].head.u + 31, work->bursts[i].head.v + 191);
                        setRGB0(&work->strip, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB3(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->cone_middle[k], &work->cone_outer[k], &work->cone_middle[k + 1],
                            &work->cone_outer[k + 1], (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                work->bursts[i].head.elapsed += step;
                if (work->bursts[i].head.duration < work->bursts[i].head.elapsed) {
                    work->bursts[i].head.active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
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

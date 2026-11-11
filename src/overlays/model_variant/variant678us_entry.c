#include "../../types.h"
#include "variant678us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 678's entry: a bubble rises from the model's joint along an arc
 * while a ring of spray sprites drifts out from the same point. When the
 * bubble ends it bursts into an animated sprite, sixteen shards and eight
 * expanding rings. The setup also prints the work-area size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL678_FADE(e, offset, value, divisor)                                      \
    (e).head.color[0] = gModelVariant678Us_aConfigs[work->head.command].color[(offset) + 0] * (value) / (divisor); \
    (e).head.color[1] = gModelVariant678Us_aConfigs[work->head.command].color[(offset) + 1] * (value) / (divisor); \
    (e).head.color[2] = gModelVariant678Us_aConfigs[work->head.command].color[(offset) + 2] * (value) / (divisor)

/* Builds the coordinate unit of element E facing the camera. */
#define MODEL678_BILLBOARD(e)                                                         \
    unit.flg = 0;                                                                     \
    unit.matrix = GsIDMATRIX;                                                         \
    ScaleMatrix(&unit.matrix, &(e).xform.scale);                                           \
    RotMatrixZ((e).xform.rotation.vz, &unit.matrix);                                       \
    RotMatrixX((e).xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix); \
    angle = 0xC06;                                                                    \
    RotMatrixY((e).xform.rotation.vy + angle - view.yaw, &unit.matrix)

s32 ModelVariant678Us_HandleRequest(u8 *context, s32 command)
{
    Model678UsState *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    VECTOR axis;
    long interpolation, flag;
    s32 result;
    s32 j, i, k, n;
    s32 step, inactive;
    s32 span, value, angle;
    u32 tex;
    s16 corner_z;
    long depth;

    work = (Model678UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model678UsState));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model678UsState));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        for (i = 0; i < 8; i++) {
            work->cell_u[i] = (i % 4) * 64;
            work->cell_v[i] = (i / 4) * 64 - 128;
        }
        /* Scheduling barrier: the empty loop after the early corner_z
         * assignment keeps the gModelVariant678Us_aImages high half ahead of the first
         * call's constant argument.  The assignment itself is folded into
         * its single use below before register allocation. */
        corner_z = 0;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[3]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[4]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[5]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[6]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[7]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[8]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[9]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[10]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant678Us_aImages[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        if (command == 0) {
            work->rows = 1;
            work->columns = 1;
        }
        work->place.radius = gModelVariant678Us_aConfigs[command].radius;
        work->place.height = gModelVariant678Us_aConfigs[command].height;
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = GsIDMATRIX;
            if (command == 0) {
                setVector(&axis, work->place.radius * 2, 0, -(work->place.radius * work->direction));
                RotMatrixZ((i << 12) / 5, &m);
                RotMatrixX((s32)((u32)work->direction << 9), &m);
            }
            ApplyMatrixLV(&m, &axis, &work->spray_axes[i]);
            PopMatrix();
        }
        for (i = 0; i < 16; i++) {
            PushMatrix();
            m = GsIDMATRIX;
            if (command == 0) {
                setVector(&axis, 0, work->place.radius * 2, 0);
                RotMatrixZ(i << 7, &m);
                RotMatrixY((i % 3 << 12) / 3, &m);
            }
            ApplyMatrixLV(&m, &axis, &work->shard_axes[i]);
            PopMatrix();
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->columns; i++) {
                work->bubbles[i][j].head.duration = 32;
                work->bubbles[i][j].head.active = 0;
                work->bubbles[i][j].head.elapsed = 0;
                work->bubbles[i][j].head.mode = 1;
                work->bubbles[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->bubbles[i][j].head.color[0] = gModelVariant678Us_aConfigs[command].color[0];
                work->bubbles[i][j].head.color[1] = gModelVariant678Us_aConfigs[command].color[1];
                work->bubbles[i][j].head.color[2] = gModelVariant678Us_aConfigs[command].color[2];
                work->bubbles[i][j].head.u = 0;
                work->bubbles[i][j].head.v = 0;
                setVector(&work->bubbles[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->bubbles[i][j].target, 0, -work->place.height, work->direction * 450);
                setVector(&work->bubbles[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bubbles[i][j].xform.position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->columns; i++) {
                work->bursts[i][j].head.duration = 48;
                work->bursts[i][j].head.active = 0;
                work->bursts[i][j].head.elapsed = 0;
                work->bursts[i][j].head.mode = 1;
                work->bursts[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->bursts[i][j].head.color[0] = gModelVariant678Us_aConfigs[command].color[3];
                work->bursts[i][j].head.color[1] = gModelVariant678Us_aConfigs[command].color[4];
                work->bursts[i][j].head.color[2] = gModelVariant678Us_aConfigs[command].color[5];
                work->bursts[i][j].head.u = 0;
                work->bursts[i][j].head.v = 0;
                setVector(&work->bursts[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->bursts[i][j].target, 0, 0, 0);
                setVector(&work->bursts[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bursts[i][j].xform.position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->columns; i++) {
                work->sprays[i][j].head.duration = 64;
                work->sprays[i][j].head.active = 0;
                work->sprays[i][j].head.elapsed = 0;
                work->sprays[i][j].head.mode = 1;
                work->sprays[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->sprays[i][j].head.color[0] = gModelVariant678Us_aConfigs[command].color[3];
                work->sprays[i][j].head.color[1] = gModelVariant678Us_aConfigs[command].color[4];
                work->sprays[i][j].head.color[2] = gModelVariant678Us_aConfigs[command].color[5];
                work->sprays[i][j].head.u = 0;
                work->sprays[i][j].head.v = 32;
                setVector(&work->sprays[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->sprays[i][j].target, 0, 0, 0);
                setVector(&work->sprays[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->sprays[i][j].xform.position, 0, 0, 0);
            }
        }
        work->place.delay = gModelVariant678Us_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->place.radius;
        setVector(&work->bubble_corners[0], -value, -value, corner_z);
        setVector(&work->bubble_corners[1], value, -value, 0);
        setVector(&work->bubble_corners[2], -value, value, 0);
        setVector(&work->bubble_corners[3], value, value, 0);
        value = work->place.radius * 2;
        setVector(&work->burst_corners[0], -value, -value, 0);
        setVector(&work->burst_corners[1], value, -value, 0);
        setVector(&work->burst_corners[2], -value, value, 0);
        setVector(&work->burst_corners[3], value, value, 0);
        value = work->place.radius >> 1;
        setVector(&work->spray_corners[0], -value, -value, 0);
        setVector(&work->spray_corners[1], value, -value, 0);
        setVector(&work->spray_corners[2], -value, value, 0);
        setVector(&work->spray_corners[3], value, value, 0);
        value = work->place.radius * 2;
        span = work->place.radius * 4;
        for (i = 0; i < 17; i++) {
            setVector(&work->inner_ring[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->outer_ring[i], span * rsin(i << 8) / 4096, span * rcos(i << 8) / 4096, 0);
            setVector(&work->middle_ring[i], (work->inner_ring[i].vx + work->outer_ring[i].vx) / 2,
                (work->inner_ring[i].vy + work->outer_ring[i].vy) / 2,
                (work->inner_ring[i].vz + work->outer_ring[i].vz) / 2);
        }
        value = work->place.radius;
        setVector(&work->shard_corners[0], -value, -value, 0);
        setVector(&work->shard_corners[1], value, -value, 0);
        setVector(&work->shard_corners[2], -value, value, 0);
        setVector(&work->shard_corners[3], value, value, 0);
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
        return result;
    }
    if (work->head.command == 0 && work->flags[0] < work->columns && work->head.elapsed == 0) {
        for (i = 0; i < work->rows; i++) {
            work->bubbles[work->flags[0]][i].head.active = 1;
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 10);
            GsGetLwUnit(&unit, &ls);
            (work->bubbles[work->flags[0]] + i)->xform.position.vx = ls.t[0];
            (work->bubbles[work->flags[0]] + i)->xform.position.vy = ls.t[1];
            (work->bubbles[work->flags[0]] + i)->xform.position.vz = ls.t[2];
            work->sprays[work->flags[0]][i].head.active = 1;
            work->sprays[work->flags[0]][i].xform.position = work->bubbles[work->flags[0]][i].xform.position;
        }
        work->flags[0]++;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (j = 0; j < work->rows; j++) {
        for (i = 0; i < work->columns; i++) {
            if (work->bubbles[i][j].head.active == 1) {
                MODEL678_BILLBOARD(work->bubbles[i][j]);
                unit.matrix.t[0] = work->bubbles[i][j].xform.position.vx;
                unit.matrix.t[1] = work->bubbles[i][j].xform.position.vy
                    + work->bubbles[i][j].target.vy * rsin((work->bubbles[i][j].head.elapsed << 11) / work->bubbles[i][j].head.duration) / 4096;
                unit.matrix.t[2] = work->bubbles[i][j].xform.position.vz
                    + (work->bubbles[i][j].target.vz - work->bubbles[i][j].xform.position.vz) * work->bubbles[i][j].head.elapsed / work->bubbles[i][j].head.duration;
                unit.super = work->bubbles[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->bubbles[i][j].head.u, work->bubbles[i][j].head.v, work->bubbles[i][j].head.u + 31, work->bubbles[i][j].head.v,
                    work->bubbles[i][j].head.u, work->bubbles[i][j].head.v + 31, work->bubbles[i][j].head.u + 31, work->bubbles[i][j].head.v + 31);
                setRGB0(&work->sprite, work->bubbles[i][j].head.color[0], work->bubbles[i][j].head.color[1], work->bubbles[i][j].head.color[2]);
                depth = RotAverage4(&work->bubble_corners[0], &work->bubble_corners[1],
                    &work->bubble_corners[2], &work->bubble_corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->bubbles[i][j].head.u += 32;
                if (work->bubbles[i][j].head.u >= 96) {
                    work->bubbles[i][j].head.u = 0;
                }
                work->bubbles[i][j].head.elapsed += step;
                if (work->bubbles[i][j].head.duration < work->bubbles[i][j].head.elapsed) {
                    work->bubbles[i][j].head.active = 0;
                    work->bursts[i][j].head.active = 1;
                    setVector(&work->bursts[i][j].xform.position,
                        unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->rows; j++) {
        for (i = 0; i < work->columns; i++) {
            if (work->bursts[i][j].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->bursts[i][j].head.elapsed << 12) / work->bursts[i][j].head.duration + 0x1000;
                setVector(&work->bursts[i][j].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->bursts[i][j].xform.scale);
                RotMatrixZ(work->bursts[i][j].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i][j].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->bursts[i][j].xform.rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i][j].xform.position.vx;
                unit.matrix.t[1] = work->bursts[i][j].xform.position.vy;
                unit.matrix.t[2] = work->bursts[i][j].xform.position.vz;
                unit.super = work->bursts[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i][j].head.elapsed > (u32)work->bursts[i][j].head.duration / 2) {
                    value = (work->bursts[i][j].head.duration - work->bursts[i][j].head.elapsed) * 2;
                    span = work->bursts[i][j].head.duration;
                    MODEL678_FADE(work->bursts[i][j], 3, value, span);
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[1];
                work->sprite.clut = work->clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->cell_u[work->bursts[i][j].head.u], work->cell_v[work->bursts[i][j].head.u],
                    work->cell_u[work->bursts[i][j].head.u] + 63, work->cell_v[work->bursts[i][j].head.u],
                    work->cell_u[work->bursts[i][j].head.u], work->cell_v[work->bursts[i][j].head.u] + 63,
                    work->cell_u[work->bursts[i][j].head.u] + 63, work->cell_v[work->bursts[i][j].head.u] + 63);
                setRGB0(&work->sprite, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                depth = RotAverage4(&work->burst_corners[0], &work->burst_corners[1],
                    &work->burst_corners[2], &work->burst_corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                value = j % 2 * 2 - 1;
                for (k = 0; k < 16; k++) {
                    MODEL678_BILLBOARD(work->bursts[i][j]);
                    unit.matrix.t[0] = work->bursts[i][j].xform.position.vx + value * work->shard_axes[k].vx * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                    unit.matrix.t[1] = work->bursts[i][j].xform.position.vy + value * work->shard_axes[k].vy * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                    unit.matrix.t[2] = work->bursts[i][j].xform.position.vz + value * work->shard_axes[k].vz * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                    unit.super = work->bursts[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i][j].head.v, 32, work->bursts[i][j].head.v + 31, 32, work->bursts[i][j].head.v, 63, work->bursts[i][j].head.v + 31, 63);
                    setRGB0(&work->sprite, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                    depth = RotAverage4(&work->shard_corners[0], &work->shard_corners[1],
                        &work->shard_corners[2], &work->shard_corners[3],
                        (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                        (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                for (n = 0; n < 8; n++) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    value = (n * 1000 + 9192) * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                    setVector(&work->bursts[i][j].xform.scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->bursts[i][j].xform.scale);
                    RotMatrixX(n * 400, &unit.matrix);
                    RotMatrixY(n * 500, &unit.matrix);
                    RotMatrixZ(n * 600, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->bursts[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->bursts[i][j].xform.position.vz;
                    unit.super = work->bursts[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[3];
                        work->strip.clut = work->clut[3];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, 0, 64, 31, 64, 0, 127, 31, 127);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                        setRGB3(&work->strip, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                        depth = RotAverage4(&work->inner_ring[k], &work->inner_ring[k + 1],
                            &work->middle_ring[k], &work->middle_ring[k + 1],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[3];
                        work->strip.clut = work->clut[3];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, 0, 64, 31, 64, 0, 127, 31, 127);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                        setRGB3(&work->strip, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1], work->bursts[i][j].head.color[2]);
                        depth = RotAverage4(&work->outer_ring[k], &work->outer_ring[k + 1],
                            &work->middle_ring[k], &work->middle_ring[k + 1],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                if (work->bursts[i][j].head.u < 7) {
                    work->bursts[i][j].head.u++;
                }
                if (work->bursts[i][j].head.v < 224) {
                    work->bursts[i][j].head.v += 32;
                }
                work->bursts[i][j].head.elapsed += step;
                if (work->bursts[i][j].head.duration < work->bursts[i][j].head.elapsed) {
                    work->bursts[i][j].head.active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->rows; j++) {
        for (i = 0; i < work->columns; i++) {
            if (work->sprays[i][j].head.active == 1) {
                value = (work->sprays[i][j].head.elapsed << 12) / work->sprays[i][j].head.duration + 0x1000;
                setVector(&work->sprays[i][j].xform.scale, value, value, value);
                if (work->sprays[i][j].head.elapsed > (u32)work->sprays[i][j].head.duration / 2) {
                    value = (work->sprays[i][j].head.duration - work->sprays[i][j].head.elapsed) * 2;
                    span = work->sprays[i][j].head.duration;
                    MODEL678_FADE(work->sprays[i][j], 3, value, span);
                }
                for (k = 0; k < 5; k++) {
                    MODEL678_BILLBOARD(work->sprays[i][j]);
                    unit.matrix.t[0] = work->sprays[i][j].xform.position.vx + work->spray_axes[k].vx * work->sprays[i][j].head.elapsed / work->sprays[i][j].head.duration;
                    unit.matrix.t[1] = work->sprays[i][j].xform.position.vy + work->spray_axes[k].vy * work->sprays[i][j].head.elapsed / work->sprays[i][j].head.duration;
                    unit.matrix.t[2] = work->sprays[i][j].xform.position.vz + work->spray_axes[k].vz * work->sprays[i][j].head.elapsed / work->sprays[i][j].head.duration;
                    unit.super = work->sprays[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->sprays[i][j].head.u, work->sprays[i][j].head.v, work->sprays[i][j].head.u + 31, work->sprays[i][j].head.v,
                        work->sprays[i][j].head.u, work->sprays[i][j].head.v + 31, work->sprays[i][j].head.u + 31, work->sprays[i][j].head.v + 31);
                    setRGB0(&work->sprite, work->sprays[i][j].head.color[0], work->sprays[i][j].head.color[1], work->sprays[i][j].head.color[2]);
                    depth = RotAverage4(&work->spray_corners[0], &work->spray_corners[1],
                        &work->spray_corners[2], &work->spray_corners[3],
                        (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                        (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                if (work->sprays[i][j].head.u < 224) {
                    work->sprays[i][j].head.u += 32;
                }
                work->sprays[i][j].head.elapsed += step;
                if (work->sprays[i][j].head.duration < work->sprays[i][j].head.elapsed) {
                    work->sprays[i][j].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->rows * (work->columns * 3)) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

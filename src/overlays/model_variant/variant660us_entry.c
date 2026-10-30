#include "../../types.h"
#include "variant660us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 660's entry (North American build): two copies of a glowing
 * sprite and a ring of spinning shards, then eight orbs thrown in turn from
 * two model joints. Each orb carries five sparks circling it, and each
 * landing orb spawns a spinning double band ring and a sphere of Gouraud
 * rays. The setup also prints the work-area size. */

/* The corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic. */
static inline s32 Model660Us_Negate(s32 value)
{
    return -value;
}

s32 func_8013B004(u8 *context, s32 command)
{
    Model660UsState *work;
    MATRIX ls, lw[2], spin;
    GsCOORDUNIT unit;
    Model660UsViewSnapshot view;
    SVECTOR mid, origin;
    VECTOR offset;
    PSXLONG interpolation, flag, sxy;
    s32 result, i, j, n, step, inactive;
    s32 k;
    s32 value, duration;
    u32 tex;
    PSXLONG depth;

    work = (Model660UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model660UsState));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model660UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E674[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E674[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E674[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->copies = 2;
            work->shard_count = 16;
            work->orb_count = 8;
            work->spark_count = 5;
            break;
        }
        work->radius = D_8013E6C8[command].radius;
        work->height = D_8013E6C8[command].height;
        for (i = 0; i < work->copies; i++) {
            work->glows[i].active = 0;
            work->glows[i].elapsed = 0;
            switch (command) {
            case 0:
                work->glows[i].duration = 80;
                break;
            default:
                work->glows[i].duration = 64;
                break;
            }
            work->glows[i].mode = 1;
            work->glows[i].coord = Model_GetCameraCoordinateUnit();
            work->glows[i].color[0] = D_8013E6C8[command].color[0];
            work->glows[i].color[1] = D_8013E6C8[command].color[1];
            work->glows[i].color[2] = D_8013E6C8[command].color[2];
            work->glows[i].u = 0;
            work->glows[i].v = 0;
            setVector(&work->glows[i].scale, 0, 0, 0);
            setVector(&work->glows[i].offset, 0, 0, 0);
            setVector(&work->glows[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->glows[i].position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->shard_count; i++) {
            work->shards[i][0].active = 0;
            work->shards[i][0].elapsed = 0;
            switch (command) {
            case 0:
                work->shards[i][0].duration = 80;
                break;
            default:
                work->shards[i][0].duration = 64;
                break;
            }
            work->shards[i][0].mode = 1;
            work->shards[i][0].coord = Model_GetCameraCoordinateUnit();
            work->shards[i][0].color[0] = D_8013E6C8[command].color[0];
            work->shards[i][0].color[1] = D_8013E6C8[command].color[1];
            work->shards[i][0].color[2] = D_8013E6C8[command].color[2];
            work->shards[i][0].u = 0;
            work->shards[i][0].v = 32;
            setVector(&work->shards[i][0].scale, 0, 0, 0);
            setVector(&work->shards[i][0].offset, 0, 0, 0);
            value = (i % 2) * 2 - 1;
            /* i * 398 * sign folds into i * (sign * 398); the sign shares `value`,
             * which keeps it in retail's register. */
            setVector(&work->shards[i][0].rotation, i * 398 * value, i * 739 * value + work->slot * 2048,
                i * 1080 * value);
            switch (command) {
            case 0:
                setVector(&work->shards[i][0].position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->shards[i][j] = work->shards[i][0];
                work->shards[i][j].rotation.vy += (j % 2) << 11;
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->orbs[i][0].active = 0;
            work->orbs[i][0].elapsed = 0;
            switch (command) {
            case 0:
                work->orbs[i][0].duration = 64;
                /* Retail falls through into the identical default store. */
            default:
                work->orbs[i][0].duration = 64;
                break;
            }
            work->orbs[i][0].mode = 1;
            work->orbs[i][0].coord = Model_GetCameraCoordinateUnit();
            work->orbs[i][0].color[0] = D_8013E6C8[command].color[0];
            work->orbs[i][0].color[1] = D_8013E6C8[command].color[1];
            work->orbs[i][0].color[2] = D_8013E6C8[command].color[2];
            work->orbs[i][0].u = 0;
            work->orbs[i][0].v = 0;
            switch (command) {
            case 0:
                work->arc_x[i][0] = 0;
                work->arc_x[i][1] = 1;
                work->arc_y[i][0] = i % 5 + 1;
                work->arc_y[i][1] = i % 5 + 2;
                break;
            default:
                work->arc_x[i][0] = i % 3 + 1;
                work->arc_x[i][1] = i % 3 + i % 5 + 2;
                work->arc_y[i][0] = i % 5 + 1;
                work->arc_y[i][1] = i % 5 + i % 7 + 2;
                break;
            }
            setVector(&work->orbs[i][0].scale, 2048, 2048, 2048);
            setVector(&work->orbs[i][0].offset, 0, 0, 0);
            setVector(&work->orbs[i][0].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->orbs[i][0].position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->orbs[i][j] = work->orbs[i][0];
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->rings[i][0].duration = 64;
            work->rings[i][0].active = 0;
            work->rings[i][0].elapsed = 0;
            work->rings[i][0].mode = 1;
            work->rings[i][0].coord = Model_GetCameraCoordinateUnit();
            work->rings[i][0].color[0] = D_8013E6C8[command].color[3];
            work->rings[i][0].color[1] = D_8013E6C8[command].color[4];
            work->rings[i][0].color[2] = D_8013E6C8[command].color[5];
            work->rings[i][0].u = 0;
            work->rings[i][0].v = 32;
            setVector(&work->rings[i][0].scale, 0, 0, 0);
            setVector(&work->rings[i][0].offset, 0, 0, 0);
            /* Grouping the slot angle with the offset keeps i * 284 + 1704 as the
             * induction value, as in retail. */
            setVector(&work->rings[i][0].rotation, i * 170 + 1190, i * 284 + (work->slot * 2048 + 1704),
                i * 398 + 1990);
            switch (command) {
            case 0:
                setVector(&work->rings[i][0].position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->rings[i][j] = work->rings[i][0];
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->rays[i][0].duration = 64;
            work->rays[i][0].active = 0;
            work->rays[i][0].elapsed = 0;
            work->rays[i][0].mode = 1;
            work->rays[i][0].coord = Model_GetCameraCoordinateUnit();
            work->rays[i][0].color[0] = D_8013E6C8[command].color[3];
            work->rays[i][0].color[1] = D_8013E6C8[command].color[4];
            work->rays[i][0].color[2] = D_8013E6C8[command].color[5];
            work->rays[i][0].u = 0;
            work->rays[i][0].v = 0;
            setVector(&work->rays[i][0].scale, 0, 0, 0);
            setVector(&work->rays[i][0].offset, 0, 0, 0);
            setVector(&work->rays[i][0].rotation, i * 170 + 850, i * 284 + (work->slot * 2048 + 1704),
                i * 398 + 2786);
            switch (command) {
            case 0:
                setVector(&work->rays[i][0].position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->rays[i][j] = work->rays[i][0];
            }
        }
        /* Each orb carries spark_count sparks spread evenly on a circle of half
         * the radius, tilted by the slot direction. */
        for (n = 0; n < work->copies; n++) {
            for (j = 0; j < work->orb_count; j++) {
                for (i = 0; i < work->spark_count; i++) {
                    work->sparks[i][j][n].duration = 64;
                    work->sparks[i][j][n].active = 0;
                    work->sparks[i][j][n].elapsed = 0;
                    work->sparks[i][j][n].mode = 1;
                    work->sparks[i][j][n].coord = Model_GetCameraCoordinateUnit();
                    work->sparks[i][j][n].color[0] = D_8013E6C8[command].color[3];
                    work->sparks[i][j][n].color[1] = D_8013E6C8[command].color[4];
                    work->sparks[i][j][n].color[2] = D_8013E6C8[command].color[5];
                    work->sparks[i][j][n].u = 0;
                    work->sparks[i][j][n].v = 160;
                    setVector(&work->sparks[i][j][n].scale, 0, 0, 0);
                    value = work->radius / 2;
                    setVector(&offset, value * rsin(((i + j + n) << 12) / work->spark_count) / 4096,
                        value * rcos(((i + j + n) << 12) / work->spark_count) / 4096, 0);
                    PushMatrix();
                    spin = D_800FE168;
                    RotMatrixX(work->direction * 512, &spin);
                    ApplyMatrixLV(&spin, &offset, &work->sparks[i][j][n].offset);
                    PopMatrix();
                    setVector(&work->sparks[i][j][n].rotation, 0, work->slot * 2048, 0);
                    switch (command) {
                    case 0:
                        setVector(&work->sparks[i][j][n].position, 0, 0, 0);
                        break;
                    }
                }
            }
        }
        work->delay = D_8013E6C8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->corners[0], Model660Us_Negate(work->radius), Model660Us_Negate(work->radius), 0);
        setVector(&work->corners[1], work->radius, Model660Us_Negate(work->radius), 0);
        setVector(&work->corners[2], Model660Us_Negate(work->radius), work->radius, 0);
        setVector(&work->corners[3], work->radius, work->radius, 0);
        setVector(&work->triangle[0], 0, 0, 0);
        setVector(&work->triangle[1], work->radius / 4, work->height, 0);
        setVector(&work->triangle[2], -work->radius / 4, work->height, 0);
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_points[i][0][0], work->radius * rsin(i << 8) / 4096,
                work->radius * rcos(i << 8) / 4096, 0);
            setVector(&work->ring_points[i][1][0], work->radius * (s32)((u32)rsin(i << 8) << 1) / 4096,
                work->radius * (s32)((u32)rcos(i << 8) << 1) / 4096, 0);
            setVector(&work->ring_points[i][0][1], work->radius * rsin(i << 8) / 4096, 0,
                work->radius * rcos(i << 8) / 4096);
            setVector(&work->ring_points[i][1][1], work->radius * (s32)((u32)rsin(i << 8) << 1) / 4096, 0,
                work->radius * (s32)((u32)rcos(i << 8) << 1) / 4096);
        }
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&work->sphere[j][i],
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * 3 * rcos(i << 8) / 4096 * (work->radius * 3 * rcos(i << 8) / 4096))
                        * rsin(j << 9) / 4096,
                    work->radius * -(rcos(i << 8) * 3) / 4096,
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * 3 * rcos(i << 8) / 4096 * (work->radius * 3 * rcos(i << 8) / 4096))
                        * rcos(j << 9) / 4096);
            }
        }
        value = work->radius / 4;
        setVector(&work->spark_corners[0], -value, -value, 0);
        setVector(&work->spark_corners[1], value, -value, 0);
        setVector(&work->spark_corners[2], -value, value, 0);
        setVector(&work->spark_corners[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                for (i = 0; i < work->copies; i++) {
                    work->glows[i].active = 1;
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].active = 1;
                    }
                }
            }
            if (work->flags[0] < work->orb_count && work->flags[0] * 40 + 72 < work->elapsed) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 23);
                GsGetLwUnit(&unit, &ls);
                work->orbs[work->flags[0]][0].active = 1;
                applyVector(&work->orbs[work->flags[0]][0].position, ls.t[0], ls.t[1], ls.t[2], +=);
                for (i = 0; i < work->spark_count; i++) {
                    work->sparks[i][work->flags[0]][0].active = 1;
                    work->sparks[i][work->flags[0]][0].position = work->orbs[work->flags[0]][0].position;
                }
                work->flags[0]++;
            }
            if (work->flags[1] < work->orb_count && work->flags[1] * 40 + 92 < work->elapsed) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 25);
                GsGetLwUnit(&unit, &ls);
                work->orbs[work->flags[1]][1].active = 1;
                applyVector(&work->orbs[work->flags[1]][1].position, ls.t[0], ls.t[1], ls.t[2], +=);
                for (i = 0; i < work->spark_count; i++) {
                    work->sparks[i][work->flags[1]][1].active = 1;
                    work->sparks[i][work->flags[1]][1].position = work->orbs[work->flags[1]][1].position;
                }
                work->flags[1]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model660UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        switch (work->command) {
        case 0:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 23);
            GsGetLwUnit(&unit, &lw[1]);
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 25);
            break;
        default:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
            break;
        }
        GsGetLwUnit(&unit, &lw[0]);
        for (i = 0; i < work->copies; i++) {
            if (work->glows[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->glows[i].elapsed << 12) / work->glows[i].duration;
                setVector(&work->glows[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->glows[i].scale);
                RotMatrixZ(work->glows[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->glows[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->glows[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                PushMatrix();
                ApplyMatrixLV(&lw[i], &work->glows[i].position, &offset);
                PopMatrix();
                unit.matrix.t[0] = offset.vx + lw[i].t[0];
                unit.matrix.t[1] = offset.vy + lw[i].t[1];
                unit.matrix.t[2] = offset.vz + lw[i].t[2];
                unit.super = work->glows[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->glows[i].duration / 2 < work->glows[i].elapsed) {
                    value = (work->glows[i].duration - work->glows[i].elapsed) * 2;
                    /* Copying the duration first gives retail's divisor register. */
                    duration = work->glows[i].duration;
                    work->glows[i].color[0] = D_8013E6C8[work->command].color[0] * value / duration;
                    work->glows[i].color[1] = D_8013E6C8[work->command].color[1] * value / duration;
                    work->glows[i].color[2] = D_8013E6C8[work->command].color[2] * value / duration;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->glows[i].u, work->glows[i].v,
                    work->glows[i].u + 31, work->glows[i].v,
                    work->glows[i].u, work->glows[i].v + 31,
                    work->glows[i].u + 31, work->glows[i].v + 31);
                setRGB0(&work->sprite, work->glows[i].color[0], work->glows[i].color[1], work->glows[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->glows[i].u += 32;
                if (work->glows[i].u == 0x60) {
                    work->glows[i].u = 0;
                }
                work->glows[i].elapsed += step;
                if (work->glows[i].duration < work->glows[i].elapsed) {
                    work->glows[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->shard_count; i++) {
                if (work->shards[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->shards[i][j].elapsed << 12) / work->shards[i][j].duration;
                    setVector(&work->shards[i][j].scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->shards[i][j].scale);
                    RotMatrixZ(work->shards[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->shards[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->shards[i][j].rotation.vy, &unit.matrix);
                    PushMatrix();
                    ApplyMatrixLV(&lw[j], &work->shards[i][j].position, &offset);
                    PopMatrix();
                    unit.matrix.t[0] = offset.vx + lw[j].t[0];
                    unit.matrix.t[1] = offset.vy + lw[j].t[1];
                    unit.matrix.t[2] = offset.vz + lw[j].t[2];
                    unit.super = work->shards[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    /* (i + 8) * 6 * step folds into (i + 8) * (step * 6), retail's order;
                     * step * 6 written directly would be folded onto the shard index. */
                    applyVector(&work->shards[i][j].rotation, (i + 8) * 6 * step, (i + 9) * 6 * step,
                        (i + 10) * 7 * step, +=);
                    if (work->shards[i][j].duration / 2 < work->shards[i][j].elapsed) {
                        value = (work->shards[i][j].duration - work->shards[i][j].elapsed) * 2;
                        duration = work->shards[i][j].duration;
                        work->shards[i][j].color[0] = D_8013E6C8[work->command].color[0] * value / duration;
                        work->shards[i][j].color[1] = D_8013E6C8[work->command].color[1] * value / duration;
                        work->shards[i][j].color[2] = D_8013E6C8[work->command].color[2] * value / duration;
                    }
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[1];
                    work->shard.clut = work->clut[1];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->shards[i][j].u + 16, work->shards[i][j].v,
                        work->shards[i][j].u, work->shards[i][j].v + 127,
                        work->shards[i][j].u + 31, work->shards[i][j].v + 127);
                    setRGB0(&work->shard, work->shards[i][j].color[0], work->shards[i][j].color[1], work->shards[i][j].color[2]);
                    setRGB1(&work->shard, 0, 0, 0);
                    setRGB2(&work->shard, 0, 0, 0);
                    depth = RotAverage3(&work->triangle[0], &work->triangle[1], &work->triangle[2],
                        (PSXLONG *)&work->shard.x0, (PSXLONG *)&work->shard.x1, (PSXLONG *)&work->shard.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shard, func_80058F10(), (u16)depth);
                    }
                    work->shards[i][j].elapsed += step;
                    if (work->shards[i][j].duration < work->shards[i][j].elapsed) {
                        work->shards[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->orbs[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->orbs[i][j].scale);
                    RotMatrixZ(work->orbs[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->orbs[i][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->orbs[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    /* The arc's sign shares `value`, which keeps it in retail's register. */
                    value = (j % 2) * 2 - 1;
                    unit.matrix.t[0] = work->orbs[i][j].position.vx + work->direction
                        * (work->arc_x[i][0] * work->orbs[i][j].elapsed
                            * (work->orbs[i][j].elapsed - work->orbs[i][j].duration) / work->arc_x[i][1])
                        * value;
                    unit.matrix.t[1] = work->orbs[i][j].position.vy
                        + work->arc_y[i][0] * work->orbs[i][j].elapsed
                            * (work->orbs[i][j].elapsed - work->orbs[i][j].duration) / work->arc_y[i][1];
                    unit.matrix.t[2] = work->orbs[i][j].position.vz
                        + (work->direction * 450 - work->orbs[i][j].position.vz) * work->orbs[i][j].elapsed
                            / work->orbs[i][j].duration;
                    unit.super = work->orbs[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->orbs[i][j].u, work->orbs[i][j].v,
                        work->orbs[i][j].u + 31, work->orbs[i][j].v,
                        work->orbs[i][j].u, work->orbs[i][j].v + 31,
                        work->orbs[i][j].u + 31, work->orbs[i][j].v + 31);
                    setRGB0(&work->sprite, work->orbs[i][j].color[0], work->orbs[i][j].color[1], work->orbs[i][j].color[2]);
                    depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    work->orbs[i][j].u += 32;
                    if (work->orbs[i][j].u == 0x60) {
                        work->orbs[i][j].u = 0;
                    }
                    work->orbs[i][j].elapsed += step;
                    if (work->orbs[i][j].duration < work->orbs[i][j].elapsed) {
                        work->orbs[i][j].active = 0;
                        work->rings[i][j].active = 1;
                        setVector(&work->rings[i][j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                        work->rays[i][j].active = 1;
                        setVector(&work->rays[i][j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->rings[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->rings[i][j].elapsed << 14) / work->rings[i][j].duration + 2048;
                    setVector(&work->rings[i][j].scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                    RotMatrixZ(work->rings[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->rings[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->rings[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].position.vx;
                    unit.matrix.t[1] = work->rings[i][j].position.vy;
                    unit.matrix.t[2] = work->rings[i][j].position.vz;
                    unit.super = work->rings[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rings[i][j].duration / 2 < work->rings[i][j].elapsed) {
                        value = (work->rings[i][j].duration - work->rings[i][j].elapsed) * 2;
                        duration = work->rings[i][j].duration;
                        work->rings[i][j].color[0] = D_8013E6C8[work->command].color[3] * value / duration;
                        work->rings[i][j].color[1] = D_8013E6C8[work->command].color[4] * value / duration;
                        work->rings[i][j].color[2] = D_8013E6C8[work->command].color[5] * value / duration;
                    }
                    value = step * ((i % 2) * 2 - 1) * ((j % 2) * 2 - 1);
                    /* Written as k * 2 * spin, the front end folds the constant onto the
                     * spin, which retail doubles and triples. */
                    applyVector(&work->rings[i][j].rotation, (i % 3 + 6) * value, (i % 3 + 6) * 2 * value,
                        (i % 3 + 6) * 3 * value, +=);
                    for (k = 0; k < 2; k++) {
                        for (n = 0; n < 16; n++) {
                            setPolyGT4(&work->strip);
                            work->strip.tpage = work->tpage[1];
                            work->strip.clut = work->clut[1];
                            setSemiTrans(&work->strip, 1);
                            setUV4(&work->strip, work->rings[i][j].u, work->rings[i][j].v,
                                work->rings[i][j].u + 31, work->rings[i][j].v,
                                work->rings[i][j].u, work->rings[i][j].v + 127,
                                work->rings[i][j].u + 31, work->rings[i][j].v + 127);
                            setRGB0(&work->strip, 0, 0, 0);
                            setRGB1(&work->strip, 0, 0, 0);
                            setRGB2(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1], work->rings[i][j].color[2]);
                            setRGB3(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1], work->rings[i][j].color[2]);
                            depth = RotAverage4(&work->ring_points[n][0][k], &work->ring_points[n + 1][0][k],
                                &work->ring_points[n][1][k], &work->ring_points[n + 1][1][k],
                                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->rings[i][j].elapsed += step;
                    if (work->rings[i][j].duration < work->rings[i][j].elapsed) {
                        work->rings[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->rays[i][j].active == 1) {
                    unit.flg = 0;
                    value = work->rays[i][j].elapsed * 0x6000 / work->rays[i][j].duration;
                    setVector(&work->rays[i][j].scale, value, value, value);
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->rays[i][j].scale);
                    RotMatrixZ(work->rays[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->rays[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->rays[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rays[i][j].position.vx;
                    unit.matrix.t[1] = work->rays[i][j].position.vy;
                    unit.matrix.t[2] = work->rays[i][j].position.vz;
                    unit.super = work->rays[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rays[i][j].duration / 2 < work->rays[i][j].elapsed) {
                        value = (work->rays[i][j].duration - work->rays[i][j].elapsed) * 2;
                        duration = work->rays[i][j].duration;
                        work->rays[i][j].color[0] = D_8013E6C8[work->command].color[3] * value / duration;
                        work->rays[i][j].color[1] = D_8013E6C8[work->command].color[4] * value / duration;
                        work->rays[i][j].color[2] = D_8013E6C8[work->command].color[5] * value / duration;
                    }
                    setVector(&origin, 0, 0, 0);
                    for (k = 1; k < 8; k++) {
                        for (n = 0; n < 8; n++) {
                            work->line.attribute = 0x50000000;
                            work->line.r0 = 0;
                            work->line.g0 = 0;
                            work->line.b0 = 0;
                            work->line.r1 = work->rays[i][j].color[0];
                            work->line.g1 = work->rays[i][j].color[1];
                            work->line.b1 = work->rays[i][j].color[2];
                            RotTransPers(&origin, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                            RotTransPers(&work->sphere[n][k], (PSXLONG *)&work->line.x1, &interpolation, &flag);
                            mid.vx = (origin.vx + work->sphere[n][k].vx) / 2;
                            mid.vy = (origin.vy + work->sphere[n][k].vy) / 2;
                            mid.vz = (origin.vz + work->sphere[n][k].vz) / 2;
                            depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->rays[i][j].elapsed += step;
                    if (work->rays[i][j].duration < work->rays[i][j].elapsed) {
                        work->rays[i][j].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        for (n = 0; n < work->copies; n++) {
            for (j = 0; j < work->orb_count; j++) {
                for (i = 0; i < work->spark_count; i++) {
                    if (work->sparks[i][j][n].active == 1) {
                        unit.flg = 0;
                        unit.matrix = D_800FE168;
                        setVector(&work->sparks[i][j][n].scale, 4096, 4096, 4096);
                        ScaleMatrix(&unit.matrix, &work->sparks[i][j][n].scale);
                        RotMatrixZ(work->sparks[i][j][n].rotation.vz, &unit.matrix);
                        RotMatrixX(work->sparks[i][j][n].rotation.vx - (view.pitch - 4096) * work->direction,
                            &unit.matrix);
                        RotMatrixY(work->sparks[i][j][n].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                        /* The spark slides out from the orb to its circle offset. */
                        unit.matrix.t[0] = work->sparks[i][j][n].position.vx
                            + work->sparks[i][j][n].offset.vx * work->sparks[i][j][n].elapsed
                                / work->sparks[i][j][n].duration;
                        unit.matrix.t[1] = work->sparks[i][j][n].position.vy
                            + work->sparks[i][j][n].offset.vy * work->sparks[i][j][n].elapsed
                                / work->sparks[i][j][n].duration;
                        unit.matrix.t[2] = work->sparks[i][j][n].position.vz
                            + work->sparks[i][j][n].offset.vz * work->sparks[i][j][n].elapsed
                                / work->sparks[i][j][n].duration;
                        unit.super = work->sparks[i][j][n].coord;
                        GsGetLsUnit(&unit, &ls);
                        GsSetLsMatrix(&ls);
                        if (work->sparks[i][j][n].duration / 2 < work->sparks[i][j][n].elapsed) {
                            value = (work->sparks[i][j][n].duration - work->sparks[i][j][n].elapsed) * 2;
                            duration = work->sparks[i][j][n].duration;
                            work->sparks[i][j][n].color[0] = D_8013E6C8[work->command].color[3] * value / duration;
                            work->sparks[i][j][n].color[1] = D_8013E6C8[work->command].color[4] * value / duration;
                            work->sparks[i][j][n].color[2] = D_8013E6C8[work->command].color[5] * value / duration;
                        }
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[2];
                        work->sprite.clut = work->clut[2];
                        setSemiTrans(&work->sprite, 1);
                        setUV4(&work->sprite, work->sparks[i][j][n].u, work->sparks[i][j][n].v,
                            work->sparks[i][j][n].u + 31, work->sparks[i][j][n].v,
                            work->sparks[i][j][n].u, work->sparks[i][j][n].v + 31,
                            work->sparks[i][j][n].u + 31, work->sparks[i][j][n].v + 31);
                        setRGB0(&work->sprite, work->sparks[i][j][n].color[0], work->sparks[i][j][n].color[1],
                            work->sparks[i][j][n].color[2]);
                        depth = RotAverage4(&work->spark_corners[0], &work->spark_corners[1],
                            &work->spark_corners[2], &work->spark_corners[3],
                            (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                        }
                        /* Step through the eight animation frames, holding the last. */
                        if (work->sparks[i][j][n].u != 0xE0) {
                            work->sparks[i][j][n].u += 32;
                        }
                        work->sparks[i][j][n].elapsed += step;
                        if (work->sparks[i][j][n].duration < work->sparks[i][j][n].elapsed) {
                            work->sparks[i][j][n].active = 0;
                        }
                    } else {
                        inactive++;
                    }
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        /* The +1 sits before the orb term, as retail adds it first. */
        if (work->elapsed > 64 && inactive == work->copies * (work->shard_count + 1 + work->orb_count * 3 + work->orb_count * work->spark_count)) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

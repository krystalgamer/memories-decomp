#include "../../types.h"
#include "variant646_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 646's entry: one or two copies of a glowing sprite, a ring of
 * spinning shards and up to eight orbs thrown along arcs from a model
 * joint; each landing orb spawns a spinning double band ring and a sphere of
 * Gouraud rays. The setup also prints the work-area size. */

/* The corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic: that is retail's three loads
 * of the radius and the zero-extension before the third negation. */
static inline s32 Model646_Negate(s32 value)
{
    return -value;
}

s32 ModelVariant646_HandleRequest(u8 *context, s32 command)
{
    Model646State *work;
    MATRIX ls, lw;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR mid, origin;
    VECTOR offset;
    long interpolation, flag, sxy;
    s32 result, i, step, inactive;
    s32 j, k, n;
    s32 value, duration;
    u32 tex;
    long depth;

    work = (Model646State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model646State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model646State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant646_aImages[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant646_aImages[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->copies = 2;
            work->shard_count = 16;
            work->orb_count = 8;
            break;
        case 1:
            work->copies = 1;
            work->shard_count = 0;
            work->orb_count = 7;
            break;
        case 2:
        case 3:
        case 4:
            work->copies = 1;
            work->shard_count = 16;
            work->orb_count = 4;
            break;
        case 5:
            work->copies = 1;
            work->shard_count = 0;
            work->orb_count = 7;
            break;
        case 6:
            work->copies = 2;
            work->shard_count = 0;
            work->orb_count = 5;
            break;
        case 7:
            work->copies = 1;
            work->shard_count = 0;
            work->orb_count = 2;
            break;
        }
        work->place.radius = gModelVariant646_aConfigs[command].radius;
        work->place.height = gModelVariant646_aConfigs[command].height;
        for (i = 0; i < work->copies; i++) {
            work->glows[i].head.active = 0;
            work->glows[i].head.elapsed = 0;
            if (command == 2) {
                work->glows[i].head.duration = 48;
            } else {
                work->glows[i].head.duration = 64;
            }
            work->glows[i].head.mode = 1;
            work->glows[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->glows[i].head.color[0] = gModelVariant646_aConfigs[command].color[0];
            work->glows[i].head.color[1] = gModelVariant646_aConfigs[command].color[1];
            work->glows[i].head.color[2] = gModelVariant646_aConfigs[command].color[2];
            work->glows[i].head.u = 0;
            work->glows[i].head.v = 0;
            setVector(&work->glows[i].xform.scale, 0, 0, 0);
            setVector(&work->glows[i].target, 0, 0, 0);
            setVector(&work->glows[i].xform.rotation, 0, work->head.slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->glows[i].xform.position, i * 160 - 80, -40, -50);
                break;
            case 2:
                setVector(&work->glows[i].xform.position, 0, -100, -50);
                break;
            }
        }
        for (i = 0; i < work->shard_count; i++) {
            work->shards[i][0].head.active = 0;
            work->shards[i][0].head.elapsed = 0;
            switch (command) {
            case 2:
                work->shards[i][0].head.duration = 48;
                break;
            case 3:
                work->shards[i][0].head.duration = 100;
                break;
            case 4:
                work->shards[i][0].head.duration = 60;
                break;
            default:
                work->shards[i][0].head.duration = 64;
                break;
            }
            work->shards[i][0].head.mode = 1;
            work->shards[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->shards[i][0].head.color[0] = gModelVariant646_aConfigs[command].color[0];
            work->shards[i][0].head.color[1] = gModelVariant646_aConfigs[command].color[1];
            work->shards[i][0].head.color[2] = gModelVariant646_aConfigs[command].color[2];
            work->shards[i][0].head.u = 0;
            work->shards[i][0].head.v = 32;
            setVector(&work->shards[i][0].xform.scale, 0, 0, 0);
            setVector(&work->shards[i][0].target, 0, 0, 0);
            value = (i % 2) * 2 - 1;
            /* i * 398 * sign folds into i * (sign * 398); the sign shares `value`,
             * which keeps it in retail's register. */
            setVector(&work->shards[i][0].xform.rotation, i * 398 * value, i * 739 * value + work->head.slot * 2048,
                i * 1080 * value);
            switch (command) {
            case 0:
                setVector(&work->shards[i][0].xform.position, -80, -40, -50);
                break;
            case 2:
                setVector(&work->shards[i][0].xform.position, 0, -100, -50);
                break;
            case 3:
            case 4:
                setVector(&work->shards[i][0].xform.position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->shards[i][j] = work->shards[i][0];
                work->shards[i][j].xform.rotation.vy += (j % 2) << 11;
                if (command == 0) {
                    setVector(&work->shards[i][j].xform.position, j * 160 - 80, -40, -50);
                }
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->orbs[i][0].head.active = 0;
            work->orbs[i][0].head.elapsed = 0;
            switch (command) {
            case 5:
                work->orbs[i][0].head.duration = 32;
                break;
            case 6:
            case 7:
                work->orbs[i][0].head.duration = 16;
                break;
            default:
                work->orbs[i][0].head.duration = 64;
                break;
            }
            work->orbs[i][0].head.mode = 1;
            work->orbs[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->orbs[i][0].head.color[0] = gModelVariant646_aConfigs[command].color[0];
            work->orbs[i][0].head.color[1] = gModelVariant646_aConfigs[command].color[1];
            work->orbs[i][0].head.color[2] = gModelVariant646_aConfigs[command].color[2];
            work->orbs[i][0].head.u = 0;
            work->orbs[i][0].head.v = 0;
            switch (command) {
            case 5:
            case 6:
            case 7:
                work->arc_x[i][0] = 0;
                work->arc_x[i][1] = 1;
                work->arc_y[i][0] = 0;
                work->arc_y[i][1] = 1;
                break;
            default:
                work->arc_x[i][0] = i % 3 + 1;
                work->arc_x[i][1] = i % 3 + i % 5 + 2;
                work->arc_y[i][0] = i % 5 + 1;
                work->arc_y[i][1] = i % 5 + i % 7 + 2;
                break;
            }
            setVector(&work->orbs[i][0].xform.scale, 2048, 2048, 2048);
            setVector(&work->orbs[i][0].target, 0, 0, 0);
            setVector(&work->orbs[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->orbs[i][0].xform.position, work->direction * 80, -40, work->direction * 50);
                break;
            case 5:
                setVector(&work->orbs[i][0].xform.position, 0, -30, -(work->direction * 10));
                break;
            case 6:
            case 7:
                setVector(&work->orbs[i][0].xform.position, 0, 0, 0);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->orbs[i][j] = work->orbs[i][0];
                if (command == 0) {
                    setVector(&work->orbs[i][j].xform.position, work->direction * (j * -160 + 80), -40,
                        work->direction * 50);
                }
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->rings[i][0].head.duration = 64;
            work->rings[i][0].head.active = 0;
            work->rings[i][0].head.elapsed = 0;
            work->rings[i][0].head.mode = 1;
            work->rings[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i][0].head.color[0] = gModelVariant646_aConfigs[command].color[3];
            work->rings[i][0].head.color[1] = gModelVariant646_aConfigs[command].color[4];
            work->rings[i][0].head.color[2] = gModelVariant646_aConfigs[command].color[5];
            work->rings[i][0].head.u = 0;
            work->rings[i][0].head.v = 32;
            setVector(&work->rings[i][0].xform.scale, 0, 0, 0);
            setVector(&work->rings[i][0].target, 0, 0, 0);
            /* Grouping the slot angle with the offset keeps i * 284 + 1704 as the
             * induction value, as in retail. */
            setVector(&work->rings[i][0].xform.rotation, i * 170 + 1190, i * 284 + (work->head.slot * 2048 + 1704),
                i * 398 + 1990);
            switch (command) {
            case 0:
            case 5:
            case 6:
            case 7:
                setVector(&work->rings[i][0].xform.position, 0, 0, 0);
                break;
            case 1:
            case 2:
            case 3:
                setVector(&work->rings[i][0].xform.position, 0, -350, work->direction * 450);
                break;
            case 4:
                setVector(&work->rings[i][0].xform.position, 0, -300, work->direction * 450);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->rings[i][j] = work->rings[i][0];
            }
        }
        for (i = 0; i < work->orb_count; i++) {
            work->rays[i][0].head.duration = 64;
            work->rays[i][0].head.active = 0;
            work->rays[i][0].head.elapsed = 0;
            work->rays[i][0].head.mode = 1;
            work->rays[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->rays[i][0].head.color[0] = gModelVariant646_aConfigs[command].color[3];
            work->rays[i][0].head.color[1] = gModelVariant646_aConfigs[command].color[4];
            work->rays[i][0].head.color[2] = gModelVariant646_aConfigs[command].color[5];
            work->rays[i][0].head.u = 0;
            work->rays[i][0].head.v = 0;
            setVector(&work->rays[i][0].xform.scale, 0, 0, 0);
            setVector(&work->rays[i][0].target, 0, 0, 0);
            /* Grouping the slot angle with the offset keeps i * 284 + 1704 as the
             * induction value, as in retail. */
            setVector(&work->rays[i][0].xform.rotation, i * 170 + 850, i * 284 + (work->head.slot * 2048 + 1704),
                i * 398 + 2786);
            switch (command) {
            case 0:
            case 5:
            case 6:
            case 7:
                setVector(&work->rays[i][0].xform.position, 0, 0, 0);
                break;
            case 1:
            case 2:
            case 3:
                setVector(&work->rays[i][0].xform.position, 0, -350, work->direction * 450);
                break;
            case 4:
                setVector(&work->rays[i][0].xform.position, 0, -300, work->direction * 450);
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->rays[i][j] = work->rays[i][0];
            }
        }
        work->place.delay = gModelVariant646_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->corners[0], Model646_Negate(work->place.radius), Model646_Negate(work->place.radius), 0);
        setVector(&work->corners[1], work->place.radius, Model646_Negate(work->place.radius), 0);
        setVector(&work->corners[2], Model646_Negate(work->place.radius), work->place.radius, 0);
        setVector(&work->corners[3], work->place.radius, work->place.radius, 0);
        if (command == 4) {
            setVector(&work->triangle[0], 0, 0, 0);
            setVector(&work->triangle[1], work->place.radius * 3 / 2, work->place.height, 0);
            setVector(&work->triangle[2], -(work->place.radius * 3) / 2, work->place.height, 0);
        } else {
            setVector(&work->triangle[0], 0, 0, 0);
            setVector(&work->triangle[1], work->place.radius / 4, work->place.height, 0);
            setVector(&work->triangle[2], -work->place.radius / 4, work->place.height, 0);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_points[i][0][0], work->place.radius * rsin(i << 8) / 4096,
                work->place.radius * rcos(i << 8) / 4096, 0);
            setVector(&work->ring_points[i][1][0], work->place.radius * (rsin(i << 8) << 1) / 4096,
                work->place.radius * (rcos(i << 8) << 1) / 4096, 0);
            setVector(&work->ring_points[i][0][1], work->place.radius * rsin(i << 8) / 4096, 0,
                work->place.radius * rcos(i << 8) / 4096);
            setVector(&work->ring_points[i][1][1], work->place.radius * (rsin(i << 8) << 1) / 4096, 0,
                work->place.radius * (rcos(i << 8) << 1) / 4096);
        }
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&work->sphere[j][i],
                    SquareRoot0(work->place.radius * 3 * (work->place.radius * 3)
                        - work->place.radius * 3 * rcos(i << 8) / 4096 * (work->place.radius * 3 * rcos(i << 8) / 4096))
                        * rsin(j << 9) / 4096,
                    work->place.radius * -(rcos(i << 8) * 3) / 4096,
                    SquareRoot0(work->place.radius * 3 * (work->place.radius * 3)
                        - work->place.radius * 3 * rcos(i << 8) / 4096 * (work->place.radius * 3 * rcos(i << 8) / 4096))
                        * rcos(j << 9) / 4096);
            }
        }
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->copies; i++) {
                    work->glows[i].head.active = 1;
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].head.active = 1;
                    }
                }
            }
            if (work->flags[0] < work->orb_count && work->flags[0] * 20 + 60 < work->head.elapsed) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 1);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->copies; i++) {
                    work->orbs[work->flags[0]][i].head.active = 1;
                    applyVector(&work->orbs[work->flags[0]][i].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->head.elapsed >= work->flags[0] * 5 && work->flags[0] < work->orb_count) {
                work->rings[work->flags[0]][0].head.active = 1;
                work->rays[work->flags[0]][0].head.active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->copies; i++) {
                    work->glows[i].head.active = 1;
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].head.active = 1;
                    }
                }
            }
            if (work->head.elapsed >= 63 && work->flags[0] == 0) {
                for (i = 0; i < work->orb_count; i++) {
                    work->rings[i][0].head.active = 1;
                    work->rays[i][0].head.active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 3:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->copies; i++) {
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].head.active = 1;
                    }
                }
            }
            if (work->head.elapsed >= 81 && work->flags[0] == 0) {
                for (i = 0; i < work->orb_count; i++) {
                    work->rings[i][0].head.active = 1;
                    work->rays[i][0].head.active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 4:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->copies; i++) {
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].head.active = 1;
                    }
                }
            }
            if (work->head.elapsed >= 47 && work->flags[0] == 0) {
                for (i = 0; i < work->orb_count; i++) {
                    work->rings[i][0].head.active = 1;
                    work->rays[i][0].head.active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 5:
            if (work->flags[0] < work->orb_count && work->head.elapsed >= work->flags[0] * 20) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 13);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->copies; i++) {
                    work->orbs[work->flags[0]][i].head.active = 1;
                    applyVector(&work->orbs[work->flags[0]][i].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        case 6:
            if (work->flags[0] < work->orb_count && work->flags[0] * 16 < work->head.elapsed) {
                for (i = 0; i < work->copies; i++) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, i * 3 + 16);
                    GsGetLwUnit(&unit, &ls);
                    work->orbs[work->flags[0]][i].head.active = 1;
                    applyVector(&work->orbs[work->flags[0]][i].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        case 7:
            if (work->flags[0] < work->orb_count && work->head.elapsed >= work->flags[0] * 8) {
                for (i = 0; i < work->copies; i++) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 6);
                    GsGetLwUnit(&unit, &ls);
                    work->orbs[work->flags[0]][i].head.active = 1;
                    applyVector(&work->orbs[work->flags[0]][i].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        switch (work->head.command) {
        case 3:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 19);
            break;
        case 4:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 25);
            break;
        default:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 1);
            break;
        }
        GsGetLwUnit(&unit, &lw);
        for (i = 0; i < work->copies; i++) {
            if (work->glows[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->glows[i].head.elapsed << 12) / work->glows[i].head.duration;
                setVector(&work->glows[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->glows[i].xform.scale);
                RotMatrixZ(work->glows[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->glows[i].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->glows[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                PushMatrix();
                ApplyMatrixLV(&lw, &work->glows[i].xform.position, &offset);
                PopMatrix();
                unit.matrix.t[0] = offset.vx + lw.t[0];
                unit.matrix.t[1] = offset.vy + lw.t[1];
                unit.matrix.t[2] = offset.vz + lw.t[2];
                unit.super = work->glows[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->glows[i].head.duration / 2 < work->glows[i].head.elapsed) {
                    value = (work->glows[i].head.duration - work->glows[i].head.elapsed) * 2;
                    /* Copying the duration first gives retail's divisor register. */
                    duration = work->glows[i].head.duration;
                    work->glows[i].head.color[0] = gModelVariant646_aConfigs[work->head.command].color[0] * value / duration;
                    work->glows[i].head.color[1] = gModelVariant646_aConfigs[work->head.command].color[1] * value / duration;
                    work->glows[i].head.color[2] = gModelVariant646_aConfigs[work->head.command].color[2] * value / duration;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->glows[i].head.u, work->glows[i].head.v,
                    work->glows[i].head.u + 31, work->glows[i].head.v,
                    work->glows[i].head.u, work->glows[i].head.v + 31,
                    work->glows[i].head.u + 31, work->glows[i].head.v + 31);
                setRGB0(&work->sprite, work->glows[i].head.color[0], work->glows[i].head.color[1], work->glows[i].head.color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->glows[i].head.u += 32;
                if (work->glows[i].head.u == 0x60) {
                    work->glows[i].head.u = 0;
                }
                work->glows[i].head.elapsed += step;
                if (work->glows[i].head.duration < work->glows[i].head.elapsed) {
                    work->glows[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->shard_count; i++) {
                if (work->shards[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    value = (work->shards[i][j].head.elapsed << 12) / work->shards[i][j].head.duration;
                    setVector(&work->shards[i][j].xform.scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->shards[i][j].xform.scale);
                    RotMatrixZ(work->shards[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->shards[i][j].xform.rotation.vx, &unit.matrix);
                    RotMatrixY(work->shards[i][j].xform.rotation.vy, &unit.matrix);
                    PushMatrix();
                    ApplyMatrixLV(&lw, &work->shards[i][j].xform.position, &offset);
                    PopMatrix();
                    unit.matrix.t[0] = offset.vx + lw.t[0];
                    unit.matrix.t[1] = offset.vy + lw.t[1];
                    unit.matrix.t[2] = offset.vz + lw.t[2];
                    unit.super = work->shards[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    /* (i + 8) * 6 * step folds into (i + 8) * (step * 6), retail's order;
                     * step * 6 written directly would be folded onto the shard index. */
                    applyVector(&work->shards[i][j].xform.rotation, (i + 8) * 6 * step, (i + 9) * 6 * step,
                        (i + 10) * 7 * step, +=);
                    if (work->shards[i][j].head.duration / 2 < work->shards[i][j].head.elapsed) {
                        value = (work->shards[i][j].head.duration - work->shards[i][j].head.elapsed) * 2;
                        duration = work->shards[i][j].head.duration;
                        work->shards[i][j].head.color[0] = gModelVariant646_aConfigs[work->head.command].color[0] * value / duration;
                        work->shards[i][j].head.color[1] = gModelVariant646_aConfigs[work->head.command].color[1] * value / duration;
                        work->shards[i][j].head.color[2] = gModelVariant646_aConfigs[work->head.command].color[2] * value / duration;
                    }
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[1];
                    work->shard.clut = work->clut[1];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->shards[i][j].head.u + 16, work->shards[i][j].head.v,
                        work->shards[i][j].head.u, work->shards[i][j].head.v + 127,
                        work->shards[i][j].head.u + 31, work->shards[i][j].head.v + 127);
                    setRGB0(&work->shard, work->shards[i][j].head.color[0], work->shards[i][j].head.color[1], work->shards[i][j].head.color[2]);
                    setRGB1(&work->shard, 0, 0, 0);
                    setRGB2(&work->shard, 0, 0, 0);
                    depth = RotAverage3(&work->triangle[0], &work->triangle[1], &work->triangle[2],
                        (long *)&work->shard.x0, (long *)&work->shard.x1, (long *)&work->shard.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shard, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->shards[i][j].head.elapsed += step;
                    if (work->shards[i][j].head.duration < work->shards[i][j].head.elapsed) {
                        work->shards[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->orbs[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->orbs[i][j].xform.scale);
                    RotMatrixZ(work->orbs[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->orbs[i][j].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->orbs[i][j].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    /* The arc's sign shares `value`, which keeps it in retail's register. */
                    value = (j % 2) * 2 - 1;
                    unit.matrix.t[0] = work->orbs[i][j].xform.position.vx + work->direction
                        * (work->arc_x[i][0] * work->orbs[i][j].head.elapsed
                            * (work->orbs[i][j].head.elapsed - work->orbs[i][j].head.duration) / work->arc_x[i][1])
                        * value;
                    unit.matrix.t[1] = work->orbs[i][j].xform.position.vy
                        + work->arc_y[i][0] * work->orbs[i][j].head.elapsed
                            * (work->orbs[i][j].head.elapsed - work->orbs[i][j].head.duration) / work->arc_y[i][1];
                    unit.matrix.t[2] = work->orbs[i][j].xform.position.vz
                        + (work->direction * 450 - work->orbs[i][j].xform.position.vz) * work->orbs[i][j].head.elapsed
                            / work->orbs[i][j].head.duration;
                    unit.super = work->orbs[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->orbs[i][j].head.u, work->orbs[i][j].head.v,
                        work->orbs[i][j].head.u + 31, work->orbs[i][j].head.v,
                        work->orbs[i][j].head.u, work->orbs[i][j].head.v + 31,
                        work->orbs[i][j].head.u + 31, work->orbs[i][j].head.v + 31);
                    setRGB0(&work->sprite, work->orbs[i][j].head.color[0], work->orbs[i][j].head.color[1], work->orbs[i][j].head.color[2]);
                    depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                        (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                        (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->orbs[i][j].head.u += 32;
                    if (work->orbs[i][j].head.u == 0x60) {
                        work->orbs[i][j].head.u = 0;
                    }
                    work->orbs[i][j].head.elapsed += step;
                    if (work->orbs[i][j].head.duration < work->orbs[i][j].head.elapsed) {
                        work->orbs[i][j].head.active = 0;
                        work->rings[i][j].head.active = 1;
                        setVector(&work->rings[i][j].xform.position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                        work->rays[i][j].head.active = 1;
                        setVector(&work->rays[i][j].xform.position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->rings[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    value = (work->rings[i][j].head.elapsed << 14) / work->rings[i][j].head.duration + 2048;
                    setVector(&work->rings[i][j].xform.scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].xform.scale);
                    RotMatrixZ(work->rings[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->rings[i][j].xform.rotation.vx, &unit.matrix);
                    RotMatrixY(work->rings[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->rings[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->rings[i][j].xform.position.vz;
                    unit.super = work->rings[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rings[i][j].head.duration / 2 < work->rings[i][j].head.elapsed) {
                        value = (work->rings[i][j].head.duration - work->rings[i][j].head.elapsed) * 2;
                        duration = work->rings[i][j].head.duration;
                        work->rings[i][j].head.color[0] = gModelVariant646_aConfigs[work->head.command].color[3] * value / duration;
                        work->rings[i][j].head.color[1] = gModelVariant646_aConfigs[work->head.command].color[4] * value / duration;
                        work->rings[i][j].head.color[2] = gModelVariant646_aConfigs[work->head.command].color[5] * value / duration;
                    }
                    value = step * ((i % 2) * 2 - 1) * ((j % 2) * 2 - 1);
                    /* Written as k * 2 * spin, the front end folds the constant onto the
                     * spin, which retail doubles and triples. */
                    applyVector(&work->rings[i][j].xform.rotation, (i % 3 + 6) * value, (i % 3 + 6) * 2 * value,
                        (i % 3 + 6) * 3 * value, +=);
                    for (n = 0; n < 2; n++) {
                        for (k = 0; k < 16; k++) {
                            setPolyGT4(&work->strip);
                            work->strip.tpage = work->tpage[1];
                            work->strip.clut = work->clut[1];
                            setSemiTrans(&work->strip, 1);
                            setUV4(&work->strip, work->rings[i][j].head.u, work->rings[i][j].head.v,
                                work->rings[i][j].head.u + 31, work->rings[i][j].head.v,
                                work->rings[i][j].head.u, work->rings[i][j].head.v + 127,
                                work->rings[i][j].head.u + 31, work->rings[i][j].head.v + 127);
                            setRGB0(&work->strip, 0, 0, 0);
                            setRGB1(&work->strip, 0, 0, 0);
                            setRGB2(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1], work->rings[i][j].head.color[2]);
                            setRGB3(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1], work->rings[i][j].head.color[2]);
                            depth = RotAverage4(&work->ring_points[k][0][n], &work->ring_points[k + 1][0][n],
                                &work->ring_points[k][1][n], &work->ring_points[k + 1][1][n],
                                (long *)&work->strip.x0, (long *)&work->strip.x1,
                                (long *)&work->strip.x2, (long *)&work->strip.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    work->rings[i][j].head.elapsed += step;
                    if (work->rings[i][j].head.duration < work->rings[i][j].head.elapsed) {
                        work->rings[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->orb_count; i++) {
                if (work->rays[i][j].head.active == 1) {
                    unit.flg = 0;
                    value = work->rays[i][j].head.elapsed * 0x6000 / work->rays[i][j].head.duration;
                    setVector(&work->rays[i][j].xform.scale, value, value, value);
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->rays[i][j].xform.scale);
                    RotMatrixZ(work->rays[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->rays[i][j].xform.rotation.vx, &unit.matrix);
                    RotMatrixY(work->rays[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rays[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->rays[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->rays[i][j].xform.position.vz;
                    unit.super = work->rays[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rays[i][j].head.duration / 2 < work->rays[i][j].head.elapsed) {
                        value = (work->rays[i][j].head.duration - work->rays[i][j].head.elapsed) * 2;
                        duration = work->rays[i][j].head.duration;
                        work->rays[i][j].head.color[0] = gModelVariant646_aConfigs[work->head.command].color[3] * value / duration;
                        work->rays[i][j].head.color[1] = gModelVariant646_aConfigs[work->head.command].color[4] * value / duration;
                        work->rays[i][j].head.color[2] = gModelVariant646_aConfigs[work->head.command].color[5] * value / duration;
                    }
                    setVector(&origin, 0, 0, 0);
                    for (n = 1; n < 8; n++) {
                        for (k = 0; k < 8; k++) {
                            work->line.attribute = 0x50000000;
                            work->line.r0 = 0;
                            work->line.g0 = 0;
                            work->line.b0 = 0;
                            work->line.r1 = work->rays[i][j].head.color[0];
                            work->line.g1 = work->rays[i][j].head.color[1];
                            work->line.b1 = work->rays[i][j].head.color[2];
                            RotTransPers(&origin, (long *)&work->line.x0, &interpolation, &flag);
                            RotTransPers(&work->sphere[k][n], (long *)&work->line.x1, &interpolation, &flag);
                            mid.vx = (origin.vx + work->sphere[k][n].vx) / 2;
                            mid.vy = (origin.vy + work->sphere[k][n].vy) / 2;
                            mid.vz = (origin.vz + work->sphere[k][n].vz) / 2;
                            depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortGLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    work->rays[i][j].head.elapsed += step;
                    if (work->rays[i][j].head.duration < work->rays[i][j].head.elapsed) {
                        work->rays[i][j].head.active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        /* The +1 sits before the orb term, as retail adds it first. */
        if (work->head.elapsed > 64 && inactive == work->copies * (work->shard_count + 1 + work->orb_count * 3)) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

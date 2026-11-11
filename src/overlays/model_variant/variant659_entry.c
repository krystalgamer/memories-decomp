#include "../../types.h"
#include "variant659_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 659's entry: animated flare sprites spawned every third frame on a
 * per-command timetable; each finished flare releases a growing ring of
 * Gouraud strips and a camera-facing spark. The setup also prints the
 * work-area size. */

#define SPAWN_FLARE(threshold)                                                  \
    if (work->head.elapsed < threshold) {                                            \
        if (work->flags[1] == 0) {                                              \
            work->flares[work->flags[0]].head.active = 1;                            \
            work->flares[work->flags[0]].head.elapsed = 0;                           \
            work->flares[work->flags[0]].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[0]; \
            work->flares[work->flags[0]].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[1]; \
            work->flares[work->flags[0]].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[2]; \
            work->flares[work->flags[0]].head.u = 0;                                 \
            work->flares[work->flags[0]].head.v = 0;                                 \
            work->flags[0]++;                                                   \
            if (work->count - 1 < work->flags[0]) {                             \
                work->flags[0] = 0;                                             \
            }                                                                   \
        }                                                                       \
        work->flags[1] += step;                                                 \
        if (work->flags[1] >= 3) {                                              \
            work->flags[1] = 0;                                                 \
        }                                                                       \
    }

s32 ModelVariant659_HandleRequest(u8 *context, s32 command)
{
    Model659State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    /* VALUE also holds each flare's +1/-1 side and the 0x1000 ring scale;
     * a separate sign variable or literal scale changes the allocation. */
    s32 value, duration;
    u32 tex;
    long depth;

    work = (Model659State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model659State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model659State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[2]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[3]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[4]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[5]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[6]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant659_aImages[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            work->count = 16;
            break;
        case 2:
            work->count = 32;
            break;
        }
        work->place.radius = gModelVariant659_aConfigs[command].radius;
        work->place.height = gModelVariant659_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->flares[i].head.active = 0;
            work->flares[i].head.elapsed = 0;
            work->flares[i].head.duration = 32;
            work->flares[i].head.mode = 1;
            work->flares[i].xform.coord = Model_GetCameraCoordinateUnit();
            value = (i % 2) * 2 - 1;
            work->flares[i].head.color[0] = gModelVariant659_aConfigs[command].color[0];
            work->flares[i].head.color[1] = gModelVariant659_aConfigs[command].color[1];
            work->flares[i].head.color[2] = gModelVariant659_aConfigs[command].color[2];
            work->flares[i].head.u = 0;
            work->flares[i].head.v = 0;
            setVector(&work->flares[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->flares[i].target, 0, 0, 0);
            setVector(&work->flares[i].xform.rotation, 0, work->head.slot * 2048, value * ((i % 8) << 12) / 8);
            setVector(&work->flares[i].xform.position,
                value * work->place.radius / 2 * rsin((i << 12) / work->count) / 4096,
                value * work->place.radius / 2 * rcos((i << 12) / work->count) / 4096,
                work->place.radius * rsin((i % 4) << 10) / 4096);
            if (work->head.command == 5) {
                work->flares[i].xform.position.vy -= 250;
            } else {
                work->flares[i].xform.position.vy -= 350;
            }
            work->flares[i].xform.position.vz += (work->place.radius + 450) * work->direction;
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            work->rings[i].head.duration = work->flares[i].head.duration;
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant659_aConfigs[command].color[3];
            work->rings[i].head.color[1] = gModelVariant659_aConfigs[command].color[4];
            work->rings[i].head.color[2] = gModelVariant659_aConfigs[command].color[5];
            work->rings[i].head.u = 64;
            work->rings[i].head.v = 88;
            value = 4096;
            setVector(&work->rings[i].xform.scale, value, value, value);
            setVector(&work->rings[i].target, 0, 0, 0);
            work->rings[i].xform.rotation = work->flares[i].xform.rotation;
            work->rings[i].xform.position = work->flares[i].xform.position;
            work->rings[i].xform.position.vz += work->place.radius * -work->direction;
        }
        for (i = 0; i < work->count; i++) {
            work->sparks[i].head.active = 0;
            work->sparks[i].head.elapsed = 0;
            work->sparks[i].head.duration = work->flares[i].head.duration;
            work->sparks[i].head.mode = 1;
            work->sparks[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].head.color[0] = gModelVariant659_aConfigs[command].color[3];
            work->sparks[i].head.color[1] = gModelVariant659_aConfigs[command].color[4];
            work->sparks[i].head.color[2] = gModelVariant659_aConfigs[command].color[5];
            work->sparks[i].head.u = 0;
            work->sparks[i].head.v = 216;
            value = 4096;
            setVector(&work->sparks[i].xform.scale, value, value, value);
            setVector(&work->sparks[i].target, 0, 0, 0);
            setVector(&work->sparks[i].xform.rotation, 0, work->head.slot * 2048, 0);
            work->sparks[i].xform.position = work->rings[i].xform.position;
        }
        work->place.delay = gModelVariant659_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        /* "* -1" keeps the negated radius loads apart from the positive ones. */
        setVector(&work->corners[0], 0, work->place.radius * -1, 0);
        setVector(&work->corners[1], 0, work->place.radius, 0);
        setVector(&work->corners[2], 0, work->place.radius * -1, work->place.radius * 4);
        setVector(&work->corners[3], 0, work->place.radius, work->place.radius * 4);
        for (i = 0; i < 9; i++) {
            setVector(&work->inner[i][0], work->place.radius / 8 * rsin(i << 9) / 4096,
                work->place.radius / 8 * rcos(i << 9) / 4096, 0);
            setVector(&work->outer[i][0], work->place.radius / 2 * rsin(i << 9) / 4096,
                work->place.radius / 2 * rcos(i << 9) / 4096, -work->place.radius / 2);
            setVector(&work->inner[i][1], work->place.radius / 8 * rsin(i << 9) / 4096,
                work->place.radius / 8 * rcos(i << 9) / 4096, 0);
            setVector(&work->outer[i][1], work->place.radius / 2 * rsin(i << 9) / 4096,
                work->place.radius / 2 * rcos(i << 9) / 4096, -work->place.radius);
        }
        value = work->place.radius / 2;
        setVector(&work->quad[0], -value, -value, 0);
        setVector(&work->quad[1], value, -value, 0);
        setVector(&work->quad[2], -value, value, 0);
        setVector(&work->quad[3], value, value, 0);
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
        return result;
    }
    switch (work->head.command) {
    case 0:
        SPAWN_FLARE(46);
        break;
    case 1:
        SPAWN_FLARE(34);
        break;
    case 2:
        SPAWN_FLARE(70);
        break;
    case 3:
        SPAWN_FLARE(34);
        break;
    case 4:
        SPAWN_FLARE(46);
        break;
    case 5:
        SPAWN_FLARE(46);
        Model_SetFrameStepOverride(1);
        break;
    case 6:
        SPAWN_FLARE(44);
        break;
    case 7:
        SPAWN_FLARE(38);
        break;
    case 8:
        SPAWN_FLARE(42);
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->flares[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            setVector(&work->flares[i].xform.scale, 4096, 4096, 4096);
            ScaleMatrix(&unit.matrix, &work->flares[i].xform.scale);
            RotMatrixZ(work->flares[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->flares[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->flares[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->flares[i].xform.position.vx;
            unit.matrix.t[1] = work->flares[i].xform.position.vy;
            unit.matrix.t[2] = work->flares[i].xform.position.vz;
            unit.super = work->flares[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->flares[i].head.duration / 2 < work->flares[i].head.elapsed) {
                /* The divisor is reloaded into DURATION, as in 658. */
                value = (work->flares[i].head.duration - work->flares[i].head.elapsed) * 2;
                duration = work->flares[i].head.duration;
                work->flares[i].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[0] * value / duration;
                work->flares[i].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[1] * value / duration;
                work->flares[i].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[2] * value / duration;
            }
            setPolyFT4(&work->sprite);
            work->sprite.tpage = work->tpage[0];
            work->sprite.clut = work->clut[0];
            setSemiTrans(&work->sprite, 1);
            setUV4(&work->sprite, work->flares[i].head.u, work->flares[i].head.v + 87,
                work->flares[i].head.u + 63, work->flares[i].head.v + 87,
                work->flares[i].head.u, work->flares[i].head.v,
                work->flares[i].head.u + 63, work->flares[i].head.v);
            setRGB0(&work->sprite, work->flares[i].head.color[0], work->flares[i].head.color[1], work->flares[i].head.color[2]);
            depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            switch (work->flares[i].head.u) {
            case 0:
                if (work->flares[i].head.v == 0) {
                    work->flares[i].head.u = 64;
                }
                break;
            case 64:
            case 128:
                work->flares[i].head.u += 64;
                break;
            case 192:
                work->flares[i].head.u = 0;
                work->flares[i].head.v = 88;
                work->rings[i].head.active = 1;
                work->rings[i].head.elapsed = 0;
                work->rings[i].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[3];
                work->rings[i].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[4];
                work->rings[i].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[5];
                value = 4096;
                setVector(&work->rings[i].xform.scale, value, value, value);
                work->sparks[i].head.active = 1;
                work->sparks[i].head.elapsed = 0;
                work->sparks[i].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[3];
                work->sparks[i].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[4];
                work->sparks[i].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[5];
                work->sparks[i].head.u = 0;
                break;
            }
            work->flares[i].head.elapsed += step;
            if (work->flares[i].head.duration < work->flares[i].head.elapsed) {
                work->flares[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            value = (work->rings[i].head.elapsed << 13) / work->rings[i].head.duration + 4096;
            setVector(&work->rings[i].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
            RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->rings[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->rings[i].xform.position.vx;
            unit.matrix.t[1] = work->rings[i].xform.position.vy;
            unit.matrix.t[2] = work->rings[i].xform.position.vz;
            unit.super = work->rings[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->rings[i].head.duration / 2 < work->rings[i].head.elapsed) {
                value = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 2;
                duration = work->rings[i].head.duration;
                work->rings[i].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[3] * value / duration;
                work->rings[i].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[4] * value / duration;
                work->rings[i].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[5] * value / duration;
            }
            for (k = 0; k < 2; k++) {
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->rings[i].head.u, work->rings[i].head.v,
                        work->rings[i].head.u + 31, work->rings[i].head.v,
                        work->rings[i].head.u, work->rings[i].head.v + 127,
                        work->rings[i].head.u + 31, work->rings[i].head.v + 127);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    setRGB3(&work->strip, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    depth = RotAverage4(&work->inner[j][k], &work->inner[j + 1][k],
                        &work->outer[j][k], &work->outer[j + 1][k],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
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
    for (i = 0; i < work->count; i++) {
        if (work->sparks[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->sparks[i].xform.scale);
            RotMatrixZ(work->sparks[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->sparks[i].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
            RotMatrixY(work->sparks[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->sparks[i].xform.position.vx;
            unit.matrix.t[1] = work->sparks[i].xform.position.vy;
            unit.matrix.t[2] = work->sparks[i].xform.position.vz;
            unit.super = work->sparks[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->sparks[i].head.duration / 2 < work->sparks[i].head.elapsed) {
                value = (work->sparks[i].head.duration - work->sparks[i].head.elapsed) * 2;
                duration = work->sparks[i].head.duration;
                work->sparks[i].head.color[0] = gModelVariant659_aConfigs[work->head.command].color[3] * value / duration;
                work->sparks[i].head.color[1] = gModelVariant659_aConfigs[work->head.command].color[4] * value / duration;
                work->sparks[i].head.color[2] = gModelVariant659_aConfigs[work->head.command].color[5] * value / duration;
            }
            setPolyFT4(&work->sprite);
            work->sprite.tpage = work->tpage[2];
            work->sprite.clut = work->clut[2];
            setSemiTrans(&work->sprite, 1);
            setUV4(&work->sprite, work->sparks[i].head.u, work->sparks[i].head.v,
                work->sparks[i].head.u + 31, work->sparks[i].head.v,
                work->sparks[i].head.u, work->sparks[i].head.v + 31,
                work->sparks[i].head.u + 31, work->sparks[i].head.v + 31);
            setRGB0(&work->sprite, work->sparks[i].head.color[0], work->sparks[i].head.color[1], work->sparks[i].head.color[2]);
            depth = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3],
                (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            if (work->sparks[i].head.u != 224) {
                work->sparks[i].head.u += 32;
            }
            work->sparks[i].head.elapsed += step;
            if (work->sparks[i].head.duration < work->sparks[i].head.elapsed) {
                work->sparks[i].head.active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->count * 3) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

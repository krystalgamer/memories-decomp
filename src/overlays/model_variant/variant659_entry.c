#include "../../types.h"
#include "variant659_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 659's entry: animated flare sprites spawned every third frame on a
 * per-command timetable; each finished flare releases a growing ring of
 * Gouraud strips and a camera-facing spark. The setup also prints the
 * work-area size. */

#define SPAWN_FLARE(threshold)                                                  \
    if (work->elapsed < threshold) {                                            \
        if (work->flags[1] == 0) {                                              \
            work->flares[work->flags[0]].active = 1;                            \
            work->flares[work->flags[0]].elapsed = 0;                           \
            work->flares[work->flags[0]].color[0] = D_8013D3E4[work->command].color[0]; \
            work->flares[work->flags[0]].color[1] = D_8013D3E4[work->command].color[1]; \
            work->flares[work->flags[0]].color[2] = D_8013D3E4[work->command].color[2]; \
            work->flares[work->flags[0]].u = 0;                                 \
            work->flares[work->flags[0]].v = 0;                                 \
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

s32 func_8013B004(u8 *context, s32 command)
{
    Model659State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model659ViewSnapshot view;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    /* VALUE also holds each flare's +1/-1 side and the 0x1000 ring scale;
     * a separate sign variable or literal scale changes the allocation. */
    s32 value, duration;
    u32 tex;
    PSXLONG depth;

    work = (Model659State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model659State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model659State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[2]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[3]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[5]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[6]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D320[0]);
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
        work->radius = D_8013D3E4[command].radius;
        work->height = D_8013D3E4[command].height;
        for (i = 0; i < work->count; i++) {
            work->flares[i].active = 0;
            work->flares[i].elapsed = 0;
            work->flares[i].duration = 32;
            work->flares[i].mode = 1;
            work->flares[i].coord = Model_GetCameraCoordinateUnit();
            value = (i % 2) * 2 - 1;
            work->flares[i].color[0] = D_8013D3E4[command].color[0];
            work->flares[i].color[1] = D_8013D3E4[command].color[1];
            work->flares[i].color[2] = D_8013D3E4[command].color[2];
            work->flares[i].u = 0;
            work->flares[i].v = 0;
            setVector(&work->flares[i].scale, 4096, 4096, 4096);
            setVector(&work->flares[i].unknown_3C, 0, 0, 0);
            setVector(&work->flares[i].rotation, 0, work->slot * 2048, value * ((i % 8) << 12) / 8);
            setVector(&work->flares[i].position,
                value * work->radius / 2 * rsin((i << 12) / work->count) / 4096,
                value * work->radius / 2 * rcos((i << 12) / work->count) / 4096,
                work->radius * rsin((i % 4) << 10) / 4096);
            if (work->command == 5) {
                work->flares[i].position.vy -= 250;
            } else {
                work->flares[i].position.vy -= 350;
            }
            work->flares[i].position.vz += (work->radius + 450) * work->direction;
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].duration = work->flares[i].duration;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013D3E4[command].color[3];
            work->rings[i].color[1] = D_8013D3E4[command].color[4];
            work->rings[i].color[2] = D_8013D3E4[command].color[5];
            work->rings[i].u = 64;
            work->rings[i].v = 88;
            value = 4096;
            setVector(&work->rings[i].scale, value, value, value);
            setVector(&work->rings[i].unknown_3C, 0, 0, 0);
            work->rings[i].rotation = work->flares[i].rotation;
            work->rings[i].position = work->flares[i].position;
            work->rings[i].position.vz += work->radius * -work->direction;
        }
        for (i = 0; i < work->count; i++) {
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].duration = work->flares[i].duration;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].color[0] = D_8013D3E4[command].color[3];
            work->sparks[i].color[1] = D_8013D3E4[command].color[4];
            work->sparks[i].color[2] = D_8013D3E4[command].color[5];
            work->sparks[i].u = 0;
            work->sparks[i].v = 216;
            value = 4096;
            setVector(&work->sparks[i].scale, value, value, value);
            setVector(&work->sparks[i].unknown_3C, 0, 0, 0);
            setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
            work->sparks[i].position = work->rings[i].position;
        }
        work->delay = D_8013D3E4[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        /* "* -1" keeps the negated radius loads apart from the positive ones. */
        setVector(&work->corners[0], 0, work->radius * -1, 0);
        setVector(&work->corners[1], 0, work->radius, 0);
        setVector(&work->corners[2], 0, work->radius * -1, work->radius * 4);
        setVector(&work->corners[3], 0, work->radius, work->radius * 4);
        for (i = 0; i < 9; i++) {
            setVector(&work->inner[i][0], work->radius / 8 * rsin(i << 9) / 4096,
                work->radius / 8 * rcos(i << 9) / 4096, 0);
            setVector(&work->outer[i][0], work->radius / 2 * rsin(i << 9) / 4096,
                work->radius / 2 * rcos(i << 9) / 4096, -work->radius / 2);
            setVector(&work->inner[i][1], work->radius / 8 * rsin(i << 9) / 4096,
                work->radius / 8 * rcos(i << 9) / 4096, 0);
            setVector(&work->outer[i][1], work->radius / 2 * rsin(i << 9) / 4096,
                work->radius / 2 * rcos(i << 9) / 4096, -work->radius);
        }
        value = work->radius / 2;
        setVector(&work->quad[0], -value, -value, 0);
        setVector(&work->quad[1], value, -value, 0);
        setVector(&work->quad[2], -value, value, 0);
        setVector(&work->quad[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
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
    view = *(Model659ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->flares[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            setVector(&work->flares[i].scale, 4096, 4096, 4096);
            ScaleMatrix(&unit.matrix, &work->flares[i].scale);
            RotMatrixZ(work->flares[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->flares[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->flares[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->flares[i].position.vx;
            unit.matrix.t[1] = work->flares[i].position.vy;
            unit.matrix.t[2] = work->flares[i].position.vz;
            unit.super = work->flares[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->flares[i].duration / 2 < work->flares[i].elapsed) {
                /* The divisor is reloaded into DURATION, as in 658. */
                value = (work->flares[i].duration - work->flares[i].elapsed) * 2;
                duration = work->flares[i].duration;
                work->flares[i].color[0] = D_8013D3E4[work->command].color[0] * value / duration;
                work->flares[i].color[1] = D_8013D3E4[work->command].color[1] * value / duration;
                work->flares[i].color[2] = D_8013D3E4[work->command].color[2] * value / duration;
            }
            setPolyFT4(&work->sprite);
            work->sprite.tpage = work->tpage[0];
            work->sprite.clut = work->clut[0];
            setSemiTrans(&work->sprite, 1);
            setUV4(&work->sprite, work->flares[i].u, work->flares[i].v + 87,
                work->flares[i].u + 63, work->flares[i].v + 87,
                work->flares[i].u, work->flares[i].v,
                work->flares[i].u + 63, work->flares[i].v);
            setRGB0(&work->sprite, work->flares[i].color[0], work->flares[i].color[1], work->flares[i].color[2]);
            depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
            }
            switch (work->flares[i].u) {
            case 0:
                if (work->flares[i].v == 0) {
                    work->flares[i].u = 64;
                }
                break;
            case 64:
            case 128:
                work->flares[i].u += 64;
                break;
            case 192:
                work->flares[i].u = 0;
                work->flares[i].v = 88;
                work->rings[i].active = 1;
                work->rings[i].elapsed = 0;
                work->rings[i].color[0] = D_8013D3E4[work->command].color[3];
                work->rings[i].color[1] = D_8013D3E4[work->command].color[4];
                work->rings[i].color[2] = D_8013D3E4[work->command].color[5];
                value = 4096;
                setVector(&work->rings[i].scale, value, value, value);
                work->sparks[i].active = 1;
                work->sparks[i].elapsed = 0;
                work->sparks[i].color[0] = D_8013D3E4[work->command].color[3];
                work->sparks[i].color[1] = D_8013D3E4[work->command].color[4];
                work->sparks[i].color[2] = D_8013D3E4[work->command].color[5];
                work->sparks[i].u = 0;
                break;
            }
            work->flares[i].elapsed += step;
            if (work->flares[i].duration < work->flares[i].elapsed) {
                work->flares[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = (work->rings[i].elapsed << 13) / work->rings[i].duration + 4096;
            setVector(&work->rings[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->rings[i].scale);
            RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->rings[i].position.vx;
            unit.matrix.t[1] = work->rings[i].position.vy;
            unit.matrix.t[2] = work->rings[i].position.vz;
            unit.super = work->rings[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                value = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                duration = work->rings[i].duration;
                work->rings[i].color[0] = D_8013D3E4[work->command].color[3] * value / duration;
                work->rings[i].color[1] = D_8013D3E4[work->command].color[4] * value / duration;
                work->rings[i].color[2] = D_8013D3E4[work->command].color[5] * value / duration;
            }
            for (k = 0; k < 2; k++) {
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->rings[i].u, work->rings[i].v,
                        work->rings[i].u + 31, work->rings[i].v,
                        work->rings[i].u, work->rings[i].v + 127,
                        work->rings[i].u + 31, work->rings[i].v + 127);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    setRGB3(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    depth = RotAverage4(&work->inner[j][k], &work->inner[j + 1][k],
                        &work->outer[j][k], &work->outer[j + 1][k],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
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
    for (i = 0; i < work->count; i++) {
        if (work->sparks[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
            RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->sparks[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
            RotMatrixY(work->sparks[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->sparks[i].position.vx;
            unit.matrix.t[1] = work->sparks[i].position.vy;
            unit.matrix.t[2] = work->sparks[i].position.vz;
            unit.super = work->sparks[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->sparks[i].duration / 2 < work->sparks[i].elapsed) {
                value = (work->sparks[i].duration - work->sparks[i].elapsed) * 2;
                duration = work->sparks[i].duration;
                work->sparks[i].color[0] = D_8013D3E4[work->command].color[3] * value / duration;
                work->sparks[i].color[1] = D_8013D3E4[work->command].color[4] * value / duration;
                work->sparks[i].color[2] = D_8013D3E4[work->command].color[5] * value / duration;
            }
            setPolyFT4(&work->sprite);
            work->sprite.tpage = work->tpage[2];
            work->sprite.clut = work->clut[2];
            setSemiTrans(&work->sprite, 1);
            setUV4(&work->sprite, work->sparks[i].u, work->sparks[i].v,
                work->sparks[i].u + 31, work->sparks[i].v,
                work->sparks[i].u, work->sparks[i].v + 31,
                work->sparks[i].u + 31, work->sparks[i].v + 31);
            setRGB0(&work->sprite, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
            depth = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3],
                (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
            }
            if (work->sparks[i].u != 224) {
                work->sparks[i].u += 32;
            }
            work->sparks[i].elapsed += step;
            if (work->sparks[i].duration < work->sparks[i].elapsed) {
                work->sparks[i].active = 0;
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

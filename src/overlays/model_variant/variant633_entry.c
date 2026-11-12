#include "../../types.h"
#include "variant633_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 633's entry: twelve arcing trails sweep a fan of spokes away from
 * a model point, each trail sheds a ring of sparks and a pair of glowing
 * rings, and spinning sprites drift away from the launch points. The setup
 * also prints the work-area size. */

#ifndef VERSION_FRENCH
static const char gModelVariant633_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0";
#endif

s32 ModelVariant633_HandleRequest(u8 *context, s32 command)
{
    Model633State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    SVECTOR rot;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, n, t, h, w, w2;
    s16 idx;
    u16 drop;
    s16 travel, lift;
    u32 tex;
    long depth;

    work = (Model633State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model633State));
#ifndef VERSION_FRENCH
        printf(gModelVariant633_szWorkSizeFormat, sizeof(Model633State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant633_Image);
        work->tpage = tex >> 16;
        work->clut = tex;
        switch (command) {
        case 0:
            work->spark_count = work->burst_count = work->trail_count = 12;
            break;
        case 1:
            work->spark_count = work->burst_count = work->trail_count = 12;
            break;
        case 2:
            work->spark_count = work->burst_count = work->trail_count = 5;
            break;
        }
        work->radius = gModelVariant633_aConfigs[command].radius;
        work->height = gModelVariant633_aConfigs[command].height;
        for (i = 0; i < work->trail_count; i++) {
            work->trails[i].head.duration = 0x34;
            work->trails[i].head.active = 0;
            work->trails[i].head.elapsed = 0;
            work->trails[i].head.mode = 1;
            work->trails[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->trails[i].head.color[0] = gModelVariant633_aConfigs[command].red;
            work->trails[i].head.color[1] = gModelVariant633_aConfigs[command].green;
            work->trails[i].head.color[2] = gModelVariant633_aConfigs[command].blue;
            work->trails[i].ticks = 0;
            work->trails[i].launched = 0;
            work->curve[i] = i * -50 - 0x226;
            work->depth[i] = 0x384;
            switch (command) {
            case 0:
                switch (i) {
                case 0:
                case 4:
                case 8:
                    work->peak[i] = work->depth[i] * 3 / 7;
                    break;
                case 1:
                case 5:
                case 9:
                    work->peak[i] = work->depth[i] * 4 / 9;
                    break;
                case 2:
                case 6:
                case 10:
                    work->peak[i] = work->depth[i] * 6 / 11;
                    break;
                case 3:
                case 7:
                case 11:
                    work->peak[i] = work->depth[i] * 2 / 5;
                    break;
                }
                break;
            case 1:
            case 2:
                switch (i) {
                case 0:
                case 5:
                case 10:
                    work->peak[i] = work->depth[i] * 3 / 7;
                    break;
                case 1:
                case 6:
                case 11:
                    work->peak[i] = work->depth[i] * 4 / 9;
                    break;
                case 2:
                case 7:
                    work->peak[i] = work->depth[i] * 6 / 11;
                    break;
                case 3:
                case 8:
                    work->peak[i] = work->depth[i] * 2 / 5;
                    break;
                case 4:
                case 9:
                    work->peak[i] = work->depth[i] * 3 / 5;
                    break;
                }
                break;
            }
            work->progress[i] = 0;
            work->base[i] = -(work->peak[i] * work->peak[i] / work->curve[i]);
            setVector(&work->trails[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->trails[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->trails[i].xform.rotation, 0, work->head.slot * 2048, ((i % 4) << 8) - 0x180);
                setVector(&work->trails[i].xform.position, 0, 0, (work->head.slot * 2 - 1) * 100);
                break;
            case 1:
                setVector(&work->trails[i].xform.rotation, 0, work->head.slot * 2048,
                    ((i % 2) << 11) + (i << 12) / work->trail_count);
                setVector(&work->trails[i].xform.position, 0, 0, 0);
                break;
            case 2:
                setVector(&work->trails[i].xform.rotation, 0, work->head.slot * 2048, ((i % 5) << 10) / 5 - 0x180);
                setVector(&work->trails[i].xform.position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].head.duration = 0x20;
            work->bursts[i].head.active = 0;
            work->bursts[i].head.elapsed = 0;
            work->bursts[i].head.mode = 1;
            work->bursts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].head.color[0] = gModelVariant633_aConfigs[command].red;
            work->bursts[i].head.color[1] = gModelVariant633_aConfigs[command].green;
            work->bursts[i].head.color[2] = gModelVariant633_aConfigs[command].blue;
            setVector(&work->bursts[i].xform.scale, 0, 0, 0);
            setVector(&work->bursts[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bursts[i].xform.position, 0, 0, (work->head.slot * 2 - 1) * 100);
                break;
            case 1:
            case 2:
                setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bursts[i].xform.position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->spark_count; i++) {
            for (k = 0; k < 3; k++) {
                work->sparks[k][i].head.duration = 0x18;
                work->sparks[k][i].head.active = 0;
                work->sparks[k][i].head.elapsed = 0;
                work->sparks[k][i].head.mode = 1;
                work->sparks[k][i].xform.coord = Model_GetCameraCoordinateUnit();
                work->sparks[k][i].head.color[0] = gModelVariant633_aConfigs[command].spark_red;
                work->sparks[k][i].head.color[1] = gModelVariant633_aConfigs[command].spark_green;
                work->sparks[k][i].head.color[2] = gModelVariant633_aConfigs[command].spark_blue;
                setVector(&work->sparks[k][i].xform.scale, 0x1000, 0x1000, 0x1000);
                n = (k << 12) / 3 + ((i % 2) << 11);
                setVector(&work->sparks[k][i].growth, rsin(n) * 20 / 4096, rcos(n) * 20 / 4096,
                    (work->head.slot * 2 - 1) * 5);
                switch (command) {
                case 0:
                    setVector(&work->sparks[k][i].xform.rotation, 0, work->head.slot * 2048, 0);
                    setVector(&work->sparks[k][i].xform.position, 0, 0, (work->head.slot * 2 - 1) * 100);
                    break;
                case 1:
                case 2:
                    setVector(&work->sparks[k][i].xform.rotation, 0, work->head.slot * 2048, 0);
                    setVector(&work->sparks[k][i].xform.position, 0, 0, 0);
                    break;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            for (k = 0; k < 4; k++) {
                work->flares[k][i].head.duration = 0x18;
                work->flares[k][i].head.active = 0;
                work->flares[k][i].head.elapsed = 0;
                work->flares[k][i].head.mode = 1;
                work->flares[k][i].xform.coord = Model_GetCameraCoordinateUnit();
                work->flares[k][i].head.color[0] = gModelVariant633_aConfigs[command].spark_red;
                work->flares[k][i].head.color[1] = gModelVariant633_aConfigs[command].spark_green;
                work->flares[k][i].head.color[2] = gModelVariant633_aConfigs[command].spark_blue;
                work->flares[k][i].ticks = 0;
                setVector(&work->flares[k][i].xform.scale, 0x1000, 0x1000, 0x1000);
                /* A block-scoped sign keeps retail's register split between the
                 * sign and the growth pointer. */
                {
                    s32 sign;
                    t = rsin(k << 10);
                    sign = (i + k) % 2 * 2 - 1;
                    setVector(&work->flares[k][i].growth, sign * (w = t * 20) / 4096, -30, sign * (w = rcos(k << 10) * 20) / 4096);
                }
                switch (command) {
                case 0:
                case 1:
                case 2:
                    setVector(&work->flares[k][i].xform.rotation, 0, work->head.slot * 2048, 0);
                    setVector(&work->flares[k][i].xform.position, 0, 0, 0);
                    break;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            work->glows[i].head.duration = 0x20;
            work->glows[i].head.active = 0;
            work->glows[i].head.elapsed = 0;
            work->glows[i].head.mode = 1;
            work->glows[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->glows[i].head.color[0] = gModelVariant633_aConfigs[command].red;
            work->glows[i].head.color[1] = gModelVariant633_aConfigs[command].green;
            work->glows[i].head.color[2] = gModelVariant633_aConfigs[command].blue;
            setVector(&work->glows[i].xform.scale, 0, 0, 0);
            setVector(&work->glows[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
                setVector(&work->glows[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->glows[i].xform.position, 0, 0, 0);
                break;
            }
        }
        work->delay = gModelVariant633_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->trail_count; i++) {
            for (k = 0; k < 9; k++) {
                if (i == 0) {
                    setVector(&work->spokes[k], work->radius * rsin(k << 9) / 4096,
                        work->radius * rcos(k << 9) / 4096, 0);
                }
                if (k == 0) {
                    setVector(&work->heads[0][i], 0, 0, 0);
                }
                PushMatrix();
                setVector(&rot, -0x200, 0, 0);
                RotMatrix(&rot, &m);
                ApplyMatrixSV(&m, &work->spokes[k], &work->grid[k][0][i]);
                PopMatrix();
            }
        }
        /* Each scaled term gets its own block-scoped temporary; reusing one
         * variable changes retail's register choice for the products. */
        for (i = 0; i < 32; i++) {
            s32 u, v;
            setVector(&work->ring_lower[i], work->radius * (u = rcos((i % 4) << 8) * 15) / 4096 * rsin(i << 7) / 4096,
                work->radius * (v = rsin(((i % 4) << 8) + 0x400) * 15) / 4096 * rcos(i << 7) / 4096,
                work->radius * -(rsin((i % 4) << 9) * 10) / 4096 - work->radius * 5);
        }
        setVector(&work->square[0], -(work->radius * 10), -(work->radius * 10), 0);
        setVector(&work->square[1], work->radius * 10, -(work->radius * 10), 0);
        setVector(&work->square[2], -(work->radius * 10), work->radius * 10, 0);
        setVector(&work->square[3], work->radius * 10, work->radius * 10, 0);
        /* The vy comma expression keeps the cosine, scale and absolute value
         * in retail's evaluation order. */
        for (i = 0; i < 32; i++) {
            s32 u, v, x;
            setVector(&work->ring_upper[i], work->radius * (u = rcos((i % 4) << 8) * 15) / 4096 * rsin(i << 7) / 4096,
                (x = rcos(((i % 4) << 9) + 0x71) * 10, w2 = work->radius * x / 4096, (w2 < 0 ? -w2 : w2) + work->radius * 5),
                work->radius * (v = rsin(((i % 4) << 8) + 0x400) * 15) / 4096 * rcos(i << 7) / 4096);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            idx = -1;
            switch (work->flags[0]) {
            case 0:
                if (work->head.elapsed == 0) {
                    idx = 0;
                    work->flags[0]++;
                }
                break;
            case 1:
                if (work->head.elapsed >= 15) {
                    idx = 2;
                    work->flags[0]++;
                }
                break;
            case 2:
                if (work->head.elapsed >= 31) {
                    idx = 1;
                    work->flags[0]++;
                }
                break;
            case 3:
                if (work->head.elapsed >= 47) {
                    idx = 3;
                    work->flags[0]++;
                }
                break;
            case 4:
                if (work->head.elapsed >= 63) {
                    idx = 4;
                    work->flags[0]++;
                }
                break;
            case 5:
                if (work->head.elapsed >= 79) {
                    idx = 6;
                    work->flags[0]++;
                }
                break;
            case 6:
                if (work->head.elapsed >= 93) {
                    idx = 5;
                    work->flags[0]++;
                }
                break;
            case 7:
                if (work->head.elapsed >= 109) {
                    idx = 7;
                    work->flags[0]++;
                }
                break;
            case 8:
                if (work->head.elapsed >= 125) {
                    idx = 8;
                    work->flags[0]++;
                }
                break;
            case 9:
                if (work->head.elapsed >= 141) {
                    idx = 10;
                    work->flags[0]++;
                }
                break;
            case 10:
                if (work->head.elapsed >= 155) {
                    idx = 9;
                    work->flags[0]++;
                }
                break;
            case 11:
                if (work->head.elapsed >= 169) {
                    idx = 11;
                    work->flags[0]++;
                }
                break;
            }
            if (idx >= 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 10);
                GsGetLwUnit(&unit, &ls);
                work->trails[idx].head.active = 1;
                work->bursts[idx].head.active = 1;
                work->trails[idx].xform.position.vx += ls.t[0];
                work->trails[idx].xform.position.vy += ls.t[1];
                work->trails[idx].xform.position.vz += ls.t[2];
                work->bursts[idx].xform.position.vx += ls.t[0];
                work->bursts[idx].xform.position.vy += ls.t[1];
                work->bursts[idx].xform.position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][idx].head.active = 1;
                    work->sparks[k][idx].xform.position.vx += ls.t[0];
                    work->sparks[k][idx].xform.position.vy += ls.t[1];
                    work->sparks[k][idx].xform.position.vz += ls.t[2];
                }
            }
            break;
        case 1:
            if (work->head.elapsed >= work->flags[0] * 8 && work->flags[0] < work->trail_count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0);
                GsGetLwUnit(&unit, &ls);
                work->trails[work->flags[0]].head.active = 1;
                work->trails[work->flags[0]].xform.position.vx += ls.t[0];
                work->trails[work->flags[0]].xform.position.vy += ls.t[1];
                work->trails[work->flags[0]].xform.position.vz += ls.t[2];
                work->bursts[work->flags[0]].head.active = 1;
                work->bursts[work->flags[0]].xform.position.vx += ls.t[0];
                work->bursts[work->flags[0]].xform.position.vy += ls.t[1];
                work->bursts[work->flags[0]].xform.position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][work->flags[0]].head.active = 1;
                    work->sparks[k][work->flags[0]].xform.position.vx += ls.t[0];
                    work->sparks[k][work->flags[0]].xform.position.vy += ls.t[1];
                    work->sparks[k][work->flags[0]].xform.position.vz += ls.t[2];
                }
                work->flags[0]++;
            }
            break;
        case 2:
            idx = -1;
            switch (work->flags[0]) {
            case 0:
                if (work->head.elapsed == 0) {
                    idx = 0;
                    work->flags[0]++;
                }
                break;
            case 1:
                if (work->head.elapsed >= 63) {
                    idx = 2;
                    work->flags[0]++;
                }
                break;
            case 2:
                if (work->head.elapsed >= 125) {
                    idx = 1;
                    work->flags[0]++;
                }
                break;
            case 3:
                if (work->head.elapsed >= 187) {
                    idx = 3;
                    work->flags[0]++;
                }
                break;
            case 4:
                if (work->head.elapsed >= 241) {
                    idx = 4;
                    work->flags[0]++;
                }
                break;
            }
            if (idx >= 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
                GsGetLwUnit(&unit, &ls);
                work->trails[idx].head.active = 1;
                work->bursts[idx].head.active = 1;
                work->trails[idx].xform.position.vx += ls.t[0];
                work->trails[idx].xform.position.vy += ls.t[1];
                work->trails[idx].xform.position.vz += ls.t[2];
                work->bursts[idx].xform.position.vx += ls.t[0];
                work->bursts[idx].xform.position.vy += ls.t[1];
                work->bursts[idx].xform.position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][idx].head.active = 1;
                    work->sparks[k][idx].xform.position.vx += ls.t[0];
                    work->sparks[k][idx].xform.position.vy += ls.t[1];
                    work->sparks[k][idx].xform.position.vz += ls.t[2];
                }
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->trail_count; i++) {
            if (work->trails[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->trails[i].xform.scale);
                RotMatrixZ(work->trails[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->trails[i].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->trails[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->trails[i].xform.position.vx;
                unit.matrix.t[1] = work->trails[i].xform.position.vy;
                unit.matrix.t[2] = work->trails[i].xform.position.vz;
                unit.super = work->trails[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->progress[i] < work->trails[i].head.duration - 18) {
                    work->progress[i] += step;
                    if (work->trails[i].head.duration - 18 < work->progress[i]) {
                        work->progress[i] = work->trails[i].head.duration - 18;
                    }
                } else {
                    if (work->trails[i].launched == 0) {
                        PushMatrix();
                        GsGetLwUnit(&unit, &m);
                        ApplyMatrix(&m, &work->heads[0][i], &work->flares[0][i].xform.position);
                        PopMatrix();
                        work->flares[0][i].xform.position.vx += work->trails[i].xform.position.vx;
                        work->flares[0][i].xform.position.vy += work->trails[i].xform.position.vy;
                        work->flares[0][i].xform.position.vz += work->trails[i].xform.position.vz;
                        for (k = 1; k < 4; k++) {
                            work->flares[k][i].xform.position = work->flares[0][i].xform.position;
                        }
                        work->flares[0][i].head.active = 1;
                        work->glows[i].xform.position = work->flares[0][i].xform.position;
                        work->glows[i].xform.position.vy -= work->radius * 10;
                        work->glows[i].head.active = 1;
                        work->trails[i].launched = 1;
                    }
                    result = 4;
                }
                for (k = work->trails[i].ticks + 1; k > 0; k--) {
                    for (t = 0; t < 9; t++) {
                        work->grid[t][k][i] = work->grid[t][k - 1][i];
                        if (t == 0) {
                            work->heads[k][i] = work->heads[k - 1][i];
                        }
                    }
                }
                travel = work->depth[i] * work->progress[i] / (work->trails[i].head.duration - 18);
                drop = travel;
                lift = work->base[i] + (work->peak[i] - travel) * (work->peak[i] - travel) / work->curve[i];
                h = lift;
                t = travel;
                setVector(&work->heads[0][i], 0, -h, -t);
                setVector(&rot,
                    -ratan2(work->heads[1][i].vy - work->heads[0][i].vy,
                        work->heads[1][i].vz - work->heads[0][i].vz),
                    0, 0);
                PushMatrix();
                RotMatrix(&rot, &m);
                for (k = 0; k < 9; k++) {
                    ApplyMatrixSV(&m, &work->spokes[k], &work->grid[k][0][i]);
                    work->grid[k][0][i].vy -= lift;
                    work->grid[k][0][i].vz -= drop;
                }
                PopMatrix();
                for (k = 0; k <= work->trails[i].ticks; k++) {
                    for (t = 0; t < 8; t++) {
                        setPolyG4(&work->shaded);
                        setRGB0(&work->shaded, work->trails[i].head.color[0] * (5 - k) / 5,
                            work->trails[i].head.color[1] * (5 - k) / 5, work->trails[i].head.color[2] * (5 - k) / 5);
                        setRGB1(&work->shaded, work->trails[i].head.color[0] * (5 - k) / 5,
                            work->trails[i].head.color[1] * (5 - k) / 5, work->trails[i].head.color[2] * (5 - k) / 5);
                        setRGB2(&work->shaded, work->trails[i].head.color[0] * (4 - k) / 5,
                            work->trails[i].head.color[1] * (4 - k) / 5, work->trails[i].head.color[2] * (4 - k) / 5);
                        setRGB3(&work->shaded, work->trails[i].head.color[0] * (4 - k) / 5,
                            work->trails[i].head.color[1] * (4 - k) / 5, work->trails[i].head.color[2] * (4 - k) / 5);
                        depth = RotAverage4(&work->grid[t][k][i], &work->grid[t + 1][k][i],
                            &work->grid[t][k + 1][i], &work->grid[t + 1][k + 1][i],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth,
                                work->trails[i].head.mode);
                        }
                    }
                }
                if (work->trails[i].ticks < 4) {
                    work->trails[i].ticks++;
                }
                work->trails[i].head.elapsed += step;
                if (work->trails[i].head.duration < work->trails[i].head.elapsed) {
                    work->trails[i].head.active = 0;
                    if (work->flags[4] == 0 && i == work->trail_count - 1) {
                        work->flags[4] = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
                RotMatrixY(work->bursts[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                unit.super = work->bursts[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->bursts[i].xform.scale.vx = (work->bursts[i].head.elapsed << 14) / work->bursts[i].head.duration;
                work->bursts[i].xform.scale.vy = (work->bursts[i].head.elapsed << 14) / work->bursts[i].head.duration;
                work->bursts[i].xform.scale.vz = (work->bursts[i].head.elapsed << 14) / work->bursts[i].head.duration;
                if (work->bursts[i].head.duration / 2 < work->bursts[i].head.elapsed) {
                    work->bursts[i].head.color[0] = gModelVariant633_aConfigs[work->head.command].spark_red * ((work->bursts[i].head.duration - work->bursts[i].head.elapsed) << 1)
                        / work->bursts[i].head.duration;
                    work->bursts[i].head.color[1] = gModelVariant633_aConfigs[work->head.command].spark_green * ((work->bursts[i].head.duration - work->bursts[i].head.elapsed) << 1)
                        / work->bursts[i].head.duration;
                    work->bursts[i].head.color[2] = gModelVariant633_aConfigs[work->head.command].spark_blue * ((work->bursts[i].head.duration - work->bursts[i].head.elapsed) << 1)
                        / work->bursts[i].head.duration;
                }
                for (k = 0; k < 32; k++) {
                    work->box.attribute = 0x50000000;
                    work->box.r = work->bursts[i].head.color[0];
                    work->box.g = work->bursts[i].head.color[1];
                    work->box.b = work->bursts[i].head.color[2];
                    depth = RotTransPers(&work->ring_lower[k], (long *)&work->box.x, &interpolation, &flag);
                    work->box.w = 1;
                    work->box.h = 1;
                    if (flag >= 0) {
                        GsSortBoxFill(&work->box, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->bursts[i].head.elapsed += step;
                if (work->bursts[i].head.duration < work->bursts[i].head.elapsed) {
                    work->bursts[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 3; i++) {
            for (k = 0; k < work->spark_count; k++) {
                if (work->sparks[i][k].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][k].xform.scale);
                    RotMatrixZ(work->sparks[i][k].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][k].xform.rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->head.slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][k].xform.rotation.vy - (*(s16 *)&view.bytes[2] - 0xC06), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][k].xform.position.vx;
                    unit.matrix.t[1] = work->sparks[i][k].xform.position.vy;
                    unit.matrix.t[2] = work->sparks[i][k].xform.position.vz;
                    unit.super = work->sparks[i][k].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->sparks[i][k].xform.position.vx += work->sparks[i][k].growth.vx;
                    work->sparks[i][k].xform.position.vy += work->sparks[i][k].growth.vy;
                    work->sparks[i][k].xform.position.vz += work->sparks[i][k].growth.vz;
                    if ((u16)(work->sparks[i][k].head.duration / 10) < work->sparks[i][k].head.elapsed) {
                        work->sparks[i][k].head.color[0] = gModelVariant633_aConfigs[work->head.command].spark_red
                            * ((work->sparks[i][k].head.duration - work->sparks[i][k].head.elapsed) * 5 << 1) / (work->sparks[i][k].head.duration * 9);
                        work->sparks[i][k].head.color[1] = gModelVariant633_aConfigs[work->head.command].spark_green
                            * ((work->sparks[i][k].head.duration - work->sparks[i][k].head.elapsed) * 5 << 1) / (work->sparks[i][k].head.duration * 9);
                        work->sparks[i][k].head.color[2] = gModelVariant633_aConfigs[work->head.command].spark_blue
                            * ((work->sparks[i][k].head.duration - work->sparks[i][k].head.elapsed) * 5 << 1) / (work->sparks[i][k].head.duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->sparks[i][k].head.color[0], work->sparks[i][k].head.color[1], work->sparks[i][k].head.color[2]);
                    setUV4(&work->flat, work->sparks[i][k].head.u, work->sparks[i][k].head.v,
                        work->sparks[i][k].head.u + 31, work->sparks[i][k].head.v,
                        work->sparks[i][k].head.u, work->sparks[i][k].head.v + 31,
                        work->sparks[i][k].head.u + 31, work->sparks[i][k].head.v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (long *)&work->flat.x0, (long *)&work->flat.x1,
                        (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->sparks[i][k].head.u += 32;
                    if (work->sparks[i][k].head.u == 128) {
                        work->sparks[i][k].head.u = 0;
                    }
                    work->sparks[i][k].head.elapsed += step;
                    if (work->sparks[i][k].head.duration < work->sparks[i][k].head.elapsed) {
                        work->sparks[i][k].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < 4; i++) {
            for (k = 0; k < work->trail_count; k++) {
                if (work->flares[i][k].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->flares[i][k].xform.scale);
                    RotMatrixZ(work->flares[i][k].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->flares[i][k].xform.rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->head.slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->flares[i][k].xform.rotation.vy - (*(s16 *)&view.bytes[2] - 0xC06), &unit.matrix);
                    unit.matrix.t[0] = work->flares[i][k].xform.position.vx;
                    unit.matrix.t[1] = work->flares[i][k].xform.position.vy;
                    unit.matrix.t[2] = work->flares[i][k].xform.position.vz;
                    unit.super = work->flares[i][k].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->flares[i][k].xform.position.vx += work->flares[i][k].growth.vx;
                    work->flares[i][k].xform.position.vy += work->flares[i][k].growth.vy;
                    work->flares[i][k].xform.position.vz += work->flares[i][k].growth.vz;
                    if ((u16)(work->flares[i][k].head.duration / 10) < work->flares[i][k].head.elapsed) {
                        work->flares[i][k].head.color[0] = gModelVariant633_aConfigs[work->head.command].spark_red
                            * ((work->flares[i][k].head.duration - work->flares[i][k].head.elapsed) * 5 << 1) / (work->flares[i][k].head.duration * 9);
                        work->flares[i][k].head.color[1] = gModelVariant633_aConfigs[work->head.command].spark_green
                            * ((work->flares[i][k].head.duration - work->flares[i][k].head.elapsed) * 5 << 1) / (work->flares[i][k].head.duration * 9);
                        work->flares[i][k].head.color[2] = gModelVariant633_aConfigs[work->head.command].spark_blue
                            * ((work->flares[i][k].head.duration - work->flares[i][k].head.elapsed) * 5 << 1) / (work->flares[i][k].head.duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->flares[i][k].head.color[0], work->flares[i][k].head.color[1], work->flares[i][k].head.color[2]);
                    setUV4(&work->flat, work->flares[i][k].head.u, work->flares[i][k].head.v,
                        work->flares[i][k].head.u + 31, work->flares[i][k].head.v,
                        work->flares[i][k].head.u, work->flares[i][k].head.v + 31,
                        work->flares[i][k].head.u + 31, work->flares[i][k].head.v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (long *)&work->flat.x0, (long *)&work->flat.x1,
                        (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->flares[i][k].head.u += 32;
                    if (work->flares[i][k].head.u == 128) {
                        work->flares[i][k].head.u = 0;
                    }
                    work->flares[i][k].head.elapsed += step;
                    if (work->flares[i][k].head.duration < work->flares[i][k].head.elapsed) {
                        work->flares[i][k].head.active = 0;
                    }
                    if (i < 3 && work->flares[i][k].ticks == 1) {
                        work->flares[i + 1][k].head.active = 1;
                    }
                    work->flares[i][k].ticks++;
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            if (work->glows[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->glows[i].xform.scale);
                RotMatrixY(work->glows[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->glows[i].xform.position.vx;
                unit.matrix.t[1] = work->glows[i].xform.position.vy;
                unit.matrix.t[2] = work->glows[i].xform.position.vz;
                unit.super = work->glows[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->glows[i].xform.scale.vx = (work->glows[i].head.elapsed << 14) / work->glows[i].head.duration;
                work->glows[i].xform.scale.vy = (work->glows[i].head.elapsed << 14) / work->glows[i].head.duration;
                work->glows[i].xform.scale.vz = (work->glows[i].head.elapsed << 14) / work->glows[i].head.duration;
                if (work->glows[i].head.duration / 2 < work->glows[i].head.elapsed) {
                    work->glows[i].head.color[0] = gModelVariant633_aConfigs[work->head.command].spark_red * ((work->glows[i].head.duration - work->glows[i].head.elapsed) << 1)
                        / work->glows[i].head.duration;
                    work->glows[i].head.color[1] = gModelVariant633_aConfigs[work->head.command].spark_green * ((work->glows[i].head.duration - work->glows[i].head.elapsed) << 1)
                        / work->glows[i].head.duration;
                    work->glows[i].head.color[2] = gModelVariant633_aConfigs[work->head.command].spark_blue * ((work->glows[i].head.duration - work->glows[i].head.elapsed) << 1)
                        / work->glows[i].head.duration;
                }
                for (k = 0; k < 32; k++) {
                    work->box.attribute = 0x50000000;
                    work->box.r = work->glows[i].head.color[0];
                    work->box.g = work->glows[i].head.color[1];
                    work->box.b = work->glows[i].head.color[2];
                    depth = RotTransPers(&work->ring_upper[k], (long *)&work->box.x, &interpolation, &flag);
                    work->box.w = 1;
                    work->box.h = 1;
                    if (flag >= 0) {
                        GsSortBoxFill(&work->box, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->glows[i].head.elapsed += step;
                if (work->glows[i].head.duration < work->glows[i].head.elapsed) {
                    work->glows[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        if (work->flags[4] == 1) {
            result = 1;
            work->flags[4] = 2;
        }
        work->head.elapsed += step;
        if (inactive == work->trail_count * 6 + work->burst_count + work->spark_count * 3) {
            if (work->head.command == 2) {
                if (work->head.elapsed >= 0x10F) {
                    result = 2;
                }
            } else {
                result = 2;
            }
        }
    }
    return result;
}

#include "../../types.h"
#include "variant633_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 633's entry: twelve arcing trails sweep a fan of spokes away from
 * a model point, each trail sheds a ring of sparks and a pair of glowing
 * rings, and spinning sprites drift away from the launch points. The setup
 * also prints the work-area size. */

#ifndef VERSION_FRENCH
static const char D_8013E1E0[24] = "E_DATA size = %x byte\n\0";
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model633State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model633ViewSnapshot view;
    SVECTOR rot;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, n, t, h, w, w2;
    s16 idx;
    u16 drop;
    s16 travel, lift;
    u32 tex;
    PSXLONG depth;

    work = (Model633State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model633State));
#ifndef VERSION_FRENCH
        printf(D_8013E1E0, sizeof(Model633State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E29C);
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
        work->radius = D_8013E2B8[command].radius;
        work->height = D_8013E2B8[command].height;
        for (i = 0; i < work->trail_count; i++) {
            work->trails[i].duration = 0x34;
            work->trails[i].active = 0;
            work->trails[i].elapsed = 0;
            work->trails[i].mode = 1;
            work->trails[i].coord = Model_GetCameraCoordinateUnit();
            work->trails[i].red = D_8013E2B8[command].red;
            work->trails[i].green = D_8013E2B8[command].green;
            work->trails[i].blue = D_8013E2B8[command].blue;
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
            setVector(&work->trails[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->trails[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->trails[i].rotation, 0, work->slot * 2048, ((i % 4) << 8) - 0x180);
                setVector(&work->trails[i].position, 0, 0, (work->slot * 2 - 1) * 100);
                break;
            case 1:
                setVector(&work->trails[i].rotation, 0, work->slot * 2048,
                    ((i % 2) << 11) + (i << 12) / work->trail_count);
                setVector(&work->trails[i].position, 0, 0, 0);
                break;
            case 2:
                setVector(&work->trails[i].rotation, 0, work->slot * 2048, ((i % 5) << 10) / 5 - 0x180);
                setVector(&work->trails[i].position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].duration = 0x20;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].red = D_8013E2B8[command].red;
            work->bursts[i].green = D_8013E2B8[command].green;
            work->bursts[i].blue = D_8013E2B8[command].blue;
            setVector(&work->bursts[i].scale, 0, 0, 0);
            setVector(&work->bursts[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i].position, 0, 0, (work->slot * 2 - 1) * 100);
                break;
            case 1:
            case 2:
                setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i].position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->spark_count; i++) {
            for (k = 0; k < 3; k++) {
                work->sparks[k][i].duration = 0x18;
                work->sparks[k][i].active = 0;
                work->sparks[k][i].elapsed = 0;
                work->sparks[k][i].mode = 1;
                work->sparks[k][i].coord = Model_GetCameraCoordinateUnit();
                work->sparks[k][i].red = D_8013E2B8[command].spark_red;
                work->sparks[k][i].green = D_8013E2B8[command].spark_green;
                work->sparks[k][i].blue = D_8013E2B8[command].spark_blue;
                setVector(&work->sparks[k][i].scale, 0x1000, 0x1000, 0x1000);
                n = (k << 12) / 3 + ((i % 2) << 11);
                setVector(&work->sparks[k][i].growth, rsin(n) * 20 / 4096, rcos(n) * 20 / 4096,
                    (work->slot * 2 - 1) * 5);
                switch (command) {
                case 0:
                    setVector(&work->sparks[k][i].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->sparks[k][i].position, 0, 0, (work->slot * 2 - 1) * 100);
                    break;
                case 1:
                case 2:
                    setVector(&work->sparks[k][i].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->sparks[k][i].position, 0, 0, 0);
                    break;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            for (k = 0; k < 4; k++) {
                work->flares[k][i].duration = 0x18;
                work->flares[k][i].active = 0;
                work->flares[k][i].elapsed = 0;
                work->flares[k][i].mode = 1;
                work->flares[k][i].coord = Model_GetCameraCoordinateUnit();
                work->flares[k][i].red = D_8013E2B8[command].spark_red;
                work->flares[k][i].green = D_8013E2B8[command].spark_green;
                work->flares[k][i].blue = D_8013E2B8[command].spark_blue;
                work->flares[k][i].ticks = 0;
                setVector(&work->flares[k][i].scale, 0x1000, 0x1000, 0x1000);
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
                    setVector(&work->flares[k][i].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->flares[k][i].position, 0, 0, 0);
                    break;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            work->glows[i].duration = 0x20;
            work->glows[i].active = 0;
            work->glows[i].elapsed = 0;
            work->glows[i].mode = 1;
            work->glows[i].coord = Model_GetCameraCoordinateUnit();
            work->glows[i].red = D_8013E2B8[command].red;
            work->glows[i].green = D_8013E2B8[command].green;
            work->glows[i].blue = D_8013E2B8[command].blue;
            setVector(&work->glows[i].scale, 0, 0, 0);
            setVector(&work->glows[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
                setVector(&work->glows[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->glows[i].position, 0, 0, 0);
                break;
            }
        }
        work->delay = D_8013E2B8[command].delay;
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
        switch (work->command) {
        case 0:
            idx = -1;
            switch (work->flags[0]) {
            case 0:
                if (work->elapsed == 0) {
                    idx = 0;
                    work->flags[0]++;
                }
                break;
            case 1:
                if (work->elapsed >= 15) {
                    idx = 2;
                    work->flags[0]++;
                }
                break;
            case 2:
                if (work->elapsed >= 31) {
                    idx = 1;
                    work->flags[0]++;
                }
                break;
            case 3:
                if (work->elapsed >= 47) {
                    idx = 3;
                    work->flags[0]++;
                }
                break;
            case 4:
                if (work->elapsed >= 63) {
                    idx = 4;
                    work->flags[0]++;
                }
                break;
            case 5:
                if (work->elapsed >= 79) {
                    idx = 6;
                    work->flags[0]++;
                }
                break;
            case 6:
                if (work->elapsed >= 93) {
                    idx = 5;
                    work->flags[0]++;
                }
                break;
            case 7:
                if (work->elapsed >= 109) {
                    idx = 7;
                    work->flags[0]++;
                }
                break;
            case 8:
                if (work->elapsed >= 125) {
                    idx = 8;
                    work->flags[0]++;
                }
                break;
            case 9:
                if (work->elapsed >= 141) {
                    idx = 10;
                    work->flags[0]++;
                }
                break;
            case 10:
                if (work->elapsed >= 155) {
                    idx = 9;
                    work->flags[0]++;
                }
                break;
            case 11:
                if (work->elapsed >= 169) {
                    idx = 11;
                    work->flags[0]++;
                }
                break;
            }
            if (idx >= 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 10);
                GsGetLwUnit(&unit, &ls);
                work->trails[idx].active = 1;
                work->bursts[idx].active = 1;
                work->trails[idx].position.vx += ls.t[0];
                work->trails[idx].position.vy += ls.t[1];
                work->trails[idx].position.vz += ls.t[2];
                work->bursts[idx].position.vx += ls.t[0];
                work->bursts[idx].position.vy += ls.t[1];
                work->bursts[idx].position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][idx].active = 1;
                    work->sparks[k][idx].position.vx += ls.t[0];
                    work->sparks[k][idx].position.vy += ls.t[1];
                    work->sparks[k][idx].position.vz += ls.t[2];
                }
            }
            break;
        case 1:
            if (work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->trail_count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0);
                GsGetLwUnit(&unit, &ls);
                work->trails[work->flags[0]].active = 1;
                work->trails[work->flags[0]].position.vx += ls.t[0];
                work->trails[work->flags[0]].position.vy += ls.t[1];
                work->trails[work->flags[0]].position.vz += ls.t[2];
                work->bursts[work->flags[0]].active = 1;
                work->bursts[work->flags[0]].position.vx += ls.t[0];
                work->bursts[work->flags[0]].position.vy += ls.t[1];
                work->bursts[work->flags[0]].position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][work->flags[0]].active = 1;
                    work->sparks[k][work->flags[0]].position.vx += ls.t[0];
                    work->sparks[k][work->flags[0]].position.vy += ls.t[1];
                    work->sparks[k][work->flags[0]].position.vz += ls.t[2];
                }
                work->flags[0]++;
            }
            break;
        case 2:
            idx = -1;
            switch (work->flags[0]) {
            case 0:
                if (work->elapsed == 0) {
                    idx = 0;
                    work->flags[0]++;
                }
                break;
            case 1:
                if (work->elapsed >= 63) {
                    idx = 2;
                    work->flags[0]++;
                }
                break;
            case 2:
                if (work->elapsed >= 125) {
                    idx = 1;
                    work->flags[0]++;
                }
                break;
            case 3:
                if (work->elapsed >= 187) {
                    idx = 3;
                    work->flags[0]++;
                }
                break;
            case 4:
                if (work->elapsed >= 241) {
                    idx = 4;
                    work->flags[0]++;
                }
                break;
            }
            if (idx >= 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
                GsGetLwUnit(&unit, &ls);
                work->trails[idx].active = 1;
                work->bursts[idx].active = 1;
                work->trails[idx].position.vx += ls.t[0];
                work->trails[idx].position.vy += ls.t[1];
                work->trails[idx].position.vz += ls.t[2];
                work->bursts[idx].position.vx += ls.t[0];
                work->bursts[idx].position.vy += ls.t[1];
                work->bursts[idx].position.vz += ls.t[2];
                for (k = 0; k < 3; k++) {
                    work->sparks[k][idx].active = 1;
                    work->sparks[k][idx].position.vx += ls.t[0];
                    work->sparks[k][idx].position.vy += ls.t[1];
                    work->sparks[k][idx].position.vz += ls.t[2];
                }
            }
            break;
        }
        PushMatrix();
        view = *(Model633ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->trail_count; i++) {
            if (work->trails[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->trails[i].scale);
                RotMatrixZ(work->trails[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->trails[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->trails[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->trails[i].position.vx;
                unit.matrix.t[1] = work->trails[i].position.vy;
                unit.matrix.t[2] = work->trails[i].position.vz;
                unit.super = work->trails[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->progress[i] < work->trails[i].duration - 18) {
                    work->progress[i] += step;
                    if (work->trails[i].duration - 18 < work->progress[i]) {
                        work->progress[i] = work->trails[i].duration - 18;
                    }
                } else {
                    if (work->trails[i].launched == 0) {
                        PushMatrix();
                        GsGetLwUnit(&unit, &m);
                        ApplyMatrix(&m, &work->heads[0][i], &work->flares[0][i].position);
                        PopMatrix();
                        work->flares[0][i].position.vx += work->trails[i].position.vx;
                        work->flares[0][i].position.vy += work->trails[i].position.vy;
                        work->flares[0][i].position.vz += work->trails[i].position.vz;
                        for (k = 1; k < 4; k++) {
                            work->flares[k][i].position = work->flares[0][i].position;
                        }
                        work->flares[0][i].active = 1;
                        work->glows[i].position = work->flares[0][i].position;
                        work->glows[i].position.vy -= work->radius * 10;
                        work->glows[i].active = 1;
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
                travel = work->depth[i] * work->progress[i] / (work->trails[i].duration - 18);
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
                        setRGB0(&work->shaded, work->trails[i].red * (5 - k) / 5,
                            work->trails[i].green * (5 - k) / 5, work->trails[i].blue * (5 - k) / 5);
                        setRGB1(&work->shaded, work->trails[i].red * (5 - k) / 5,
                            work->trails[i].green * (5 - k) / 5, work->trails[i].blue * (5 - k) / 5);
                        setRGB2(&work->shaded, work->trails[i].red * (4 - k) / 5,
                            work->trails[i].green * (4 - k) / 5, work->trails[i].blue * (4 - k) / 5);
                        setRGB3(&work->shaded, work->trails[i].red * (4 - k) / 5,
                            work->trails[i].green * (4 - k) / 5, work->trails[i].blue * (4 - k) / 5);
                        depth = RotAverage4(&work->grid[t][k][i], &work->grid[t + 1][k][i],
                            &work->grid[t][k + 1][i], &work->grid[t + 1][k + 1][i],
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&work->shaded, func_80058F10(), (u16)depth,
                                work->trails[i].mode);
                        }
                    }
                }
                if (work->trails[i].ticks < 4) {
                    work->trails[i].ticks++;
                }
                work->trails[i].elapsed += step;
                if (work->trails[i].duration < work->trails[i].elapsed) {
                    work->trails[i].active = 0;
                    if (work->flags[4] == 0 && i == work->trail_count - 1) {
                        work->flags[4] = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixY(work->bursts[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->bursts[i].scale.vx = (work->bursts[i].elapsed << 14) / work->bursts[i].duration;
                work->bursts[i].scale.vy = (work->bursts[i].elapsed << 14) / work->bursts[i].duration;
                work->bursts[i].scale.vz = (work->bursts[i].elapsed << 14) / work->bursts[i].duration;
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    work->bursts[i].red = D_8013E2B8[work->command].spark_red * ((work->bursts[i].duration - work->bursts[i].elapsed) << 1)
                        / work->bursts[i].duration;
                    work->bursts[i].green = D_8013E2B8[work->command].spark_green * ((work->bursts[i].duration - work->bursts[i].elapsed) << 1)
                        / work->bursts[i].duration;
                    work->bursts[i].blue = D_8013E2B8[work->command].spark_blue * ((work->bursts[i].duration - work->bursts[i].elapsed) << 1)
                        / work->bursts[i].duration;
                }
                for (k = 0; k < 32; k++) {
                    work->box.attribute = 0x50000000;
                    work->box.r = work->bursts[i].red;
                    work->box.g = work->bursts[i].green;
                    work->box.b = work->bursts[i].blue;
                    depth = RotTransPers(&work->ring_lower[k], (PSXLONG *)&work->box.x, &interpolation, &flag);
                    work->box.w = 1;
                    work->box.h = 1;
                    if (flag >= 0) {
                        GsSortBoxFill(&work->box, func_80058F10(), (u16)depth);
                    }
                }
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 3; i++) {
            for (k = 0; k < work->spark_count; k++) {
                if (work->sparks[i][k].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][k].scale);
                    RotMatrixZ(work->sparks[i][k].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][k].rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][k].rotation.vy - (*(s16 *)&view.bytes[2] - 0xC06), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][k].position.vx;
                    unit.matrix.t[1] = work->sparks[i][k].position.vy;
                    unit.matrix.t[2] = work->sparks[i][k].position.vz;
                    unit.super = work->sparks[i][k].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->sparks[i][k].position.vx += work->sparks[i][k].growth.vx;
                    work->sparks[i][k].position.vy += work->sparks[i][k].growth.vy;
                    work->sparks[i][k].position.vz += work->sparks[i][k].growth.vz;
                    if ((u16)(work->sparks[i][k].duration / 10) < work->sparks[i][k].elapsed) {
                        work->sparks[i][k].red = D_8013E2B8[work->command].spark_red
                            * ((work->sparks[i][k].duration - work->sparks[i][k].elapsed) * 5 << 1) / (work->sparks[i][k].duration * 9);
                        work->sparks[i][k].green = D_8013E2B8[work->command].spark_green
                            * ((work->sparks[i][k].duration - work->sparks[i][k].elapsed) * 5 << 1) / (work->sparks[i][k].duration * 9);
                        work->sparks[i][k].blue = D_8013E2B8[work->command].spark_blue
                            * ((work->sparks[i][k].duration - work->sparks[i][k].elapsed) * 5 << 1) / (work->sparks[i][k].duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->sparks[i][k].red, work->sparks[i][k].green, work->sparks[i][k].blue);
                    setUV4(&work->flat, work->sparks[i][k].u, work->sparks[i][k].v,
                        work->sparks[i][k].u + 31, work->sparks[i][k].v,
                        work->sparks[i][k].u, work->sparks[i][k].v + 31,
                        work->sparks[i][k].u + 31, work->sparks[i][k].v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->sparks[i][k].u += 32;
                    if (work->sparks[i][k].u == 128) {
                        work->sparks[i][k].u = 0;
                    }
                    work->sparks[i][k].elapsed += step;
                    if (work->sparks[i][k].duration < work->sparks[i][k].elapsed) {
                        work->sparks[i][k].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < 4; i++) {
            for (k = 0; k < work->trail_count; k++) {
                if (work->flares[i][k].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->flares[i][k].scale);
                    RotMatrixZ(work->flares[i][k].rotation.vz, &unit.matrix);
                    RotMatrixX(work->flares[i][k].rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->flares[i][k].rotation.vy - (*(s16 *)&view.bytes[2] - 0xC06), &unit.matrix);
                    unit.matrix.t[0] = work->flares[i][k].position.vx;
                    unit.matrix.t[1] = work->flares[i][k].position.vy;
                    unit.matrix.t[2] = work->flares[i][k].position.vz;
                    unit.super = work->flares[i][k].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->flares[i][k].position.vx += work->flares[i][k].growth.vx;
                    work->flares[i][k].position.vy += work->flares[i][k].growth.vy;
                    work->flares[i][k].position.vz += work->flares[i][k].growth.vz;
                    if ((u16)(work->flares[i][k].duration / 10) < work->flares[i][k].elapsed) {
                        work->flares[i][k].red = D_8013E2B8[work->command].spark_red
                            * ((work->flares[i][k].duration - work->flares[i][k].elapsed) * 5 << 1) / (work->flares[i][k].duration * 9);
                        work->flares[i][k].green = D_8013E2B8[work->command].spark_green
                            * ((work->flares[i][k].duration - work->flares[i][k].elapsed) * 5 << 1) / (work->flares[i][k].duration * 9);
                        work->flares[i][k].blue = D_8013E2B8[work->command].spark_blue
                            * ((work->flares[i][k].duration - work->flares[i][k].elapsed) * 5 << 1) / (work->flares[i][k].duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->flares[i][k].red, work->flares[i][k].green, work->flares[i][k].blue);
                    setUV4(&work->flat, work->flares[i][k].u, work->flares[i][k].v,
                        work->flares[i][k].u + 31, work->flares[i][k].v,
                        work->flares[i][k].u, work->flares[i][k].v + 31,
                        work->flares[i][k].u + 31, work->flares[i][k].v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->flares[i][k].u += 32;
                    if (work->flares[i][k].u == 128) {
                        work->flares[i][k].u = 0;
                    }
                    work->flares[i][k].elapsed += step;
                    if (work->flares[i][k].duration < work->flares[i][k].elapsed) {
                        work->flares[i][k].active = 0;
                    }
                    if (i < 3 && work->flares[i][k].ticks == 1) {
                        work->flares[i + 1][k].active = 1;
                    }
                    work->flares[i][k].ticks++;
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->trail_count; i++) {
            if (work->glows[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->glows[i].scale);
                RotMatrixY(work->glows[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->glows[i].position.vx;
                unit.matrix.t[1] = work->glows[i].position.vy;
                unit.matrix.t[2] = work->glows[i].position.vz;
                unit.super = work->glows[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->glows[i].scale.vx = (work->glows[i].elapsed << 14) / work->glows[i].duration;
                work->glows[i].scale.vy = (work->glows[i].elapsed << 14) / work->glows[i].duration;
                work->glows[i].scale.vz = (work->glows[i].elapsed << 14) / work->glows[i].duration;
                if (work->glows[i].duration / 2 < work->glows[i].elapsed) {
                    work->glows[i].red = D_8013E2B8[work->command].spark_red * ((work->glows[i].duration - work->glows[i].elapsed) << 1)
                        / work->glows[i].duration;
                    work->glows[i].green = D_8013E2B8[work->command].spark_green * ((work->glows[i].duration - work->glows[i].elapsed) << 1)
                        / work->glows[i].duration;
                    work->glows[i].blue = D_8013E2B8[work->command].spark_blue * ((work->glows[i].duration - work->glows[i].elapsed) << 1)
                        / work->glows[i].duration;
                }
                for (k = 0; k < 32; k++) {
                    work->box.attribute = 0x50000000;
                    work->box.r = work->glows[i].red;
                    work->box.g = work->glows[i].green;
                    work->box.b = work->glows[i].blue;
                    depth = RotTransPers(&work->ring_upper[k], (PSXLONG *)&work->box.x, &interpolation, &flag);
                    work->box.w = 1;
                    work->box.h = 1;
                    if (flag >= 0) {
                        GsSortBoxFill(&work->box, func_80058F10(), (u16)depth);
                    }
                }
                work->glows[i].elapsed += step;
                if (work->glows[i].duration < work->glows[i].elapsed) {
                    work->glows[i].active = 0;
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
        work->elapsed += step;
        if (inactive == work->trail_count * 6 + work->burst_count + work->spark_count * 3) {
            if (work->command == 2) {
                if (work->elapsed >= 0x10F) {
                    result = 2;
                }
            } else {
                result = 2;
            }
        }
    }
    return result;
}

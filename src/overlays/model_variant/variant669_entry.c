#include "../../types.h"
#include "variant669_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 669's entry: rings of sprites that grow and drift toward a target
 * while dropping fading echoes, each ring later releasing a camera-facing
 * burst (a sprite, two shards, four spinning flashes and a cone of strips),
 * and up to two streaks whose edges follow a pair of model points and leave
 * a ribbon of Gouraud quads. The setup also prints the work-area size. */

#define TRACK_POINT(index, point)                                              \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);          \
    GsGetLwUnit(&unit, &ls);                                                   \
    (point).vx = ls.t[0];                                                      \
    (point).vy = ls.t[1];                                                      \
    (point).vz = ls.t[2]

#define START_RING(index)                                                      \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);          \
    GsGetLwUnit(&unit, &ls);                                                   \
    work->rings[work->flags[1]].active = 1;                                    \
    setVector(&work->rings[work->flags[1]].position, ls.t[0], ls.t[1], ls.t[2]); \
    work->flags[1]++;                                                          \
    if (work->flags[1] == work->ring_count) {                                  \
        work->flags[2] = 1;                                                    \
    }

#define START_STREAKS()                                                        \
    if (work->elapsed == 0) {                                                  \
        for (i = 0; i < work->streak_count; i++) {                             \
            work->streaks[i].active = 1;                                       \
        }                                                                      \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model669State *work;
    MATRIX ls;
    MATRIX m;
    s32 sel[2];
    GsCOORDUNIT unit;
    Model669ViewSnapshot view;
    SVECTOR point[4];
    SVECTOR mid[2];
    /* Never referenced, but it keeps the original 0x10-byte frame slot. */
    VECTOR unused;
    u8 red[4], green[4], blue[4];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value;
    s32 amount, next, ahead;
    u32 tex;
    u8 r, g, b;
    PSXLONG depth;

    work = (Model669State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model669State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model669State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->unknown_E41[0] = 128;
        work->unknown_E41[1] = 128;
        work->unknown_E41[2] = 128;
        work->unknown_E45[0] = 0;
        work->unknown_E45[1] = 0;
        work->unknown_E45[2] = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013F05C[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013F05C[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013F05C[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 2:
        case 3:
        case 6:
            work->ring_count = 1;
            work->echo_count = 16;
            work->streak_count = 1;
            work->trail_length = 32;
            break;
        case 1:
            work->ring_count = 2;
            work->echo_count = 16;
            work->streak_count = 1;
            work->trail_length = 32;
            break;
        case 4:
            work->ring_count = 1;
            work->echo_count = 0;
            work->streak_count = 1;
            work->trail_length = 32;
            break;
        case 5:
            work->ring_count = 1;
            work->echo_count = 0;
            work->streak_count = 0;
            work->trail_length = 0;
            break;
        case 7:
            work->ring_count = 1;
            work->echo_count = 16;
            work->streak_count = 2;
            work->trail_length = 32;
            break;
        case 8:
            work->ring_count = 8;
            work->echo_count = 16;
            work->streak_count = 1;
            work->trail_length = 32;
            break;
        }
        work->radius = D_8013F0B0[command].radius;
        work->height = D_8013F0B0[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].duration = 32;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013F0B0[command].color[0];
            work->rings[i].color[1] = D_8013F0B0[command].color[1];
            work->rings[i].color[2] = D_8013F0B0[command].color[2];
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, work->direction * 1350);
            switch (command) {
            case 0:
                setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0x555);
                break;
            case 1:
                if (i == 0) {
                    setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0x555);
                } else {
                    setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
                }
                break;
            case 3:
                setVector(&work->rings[i].rotation, 0, work->slot * 2048, -0x555);
                break;
            case 7:
                setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0x400);
                break;
            default:
                setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
                break;
            }
            setVector(&work->rings[i].position, 0, 0, 0);
            work->rings[i].echo = 0;
            work->rings[i].released = 0;
        }
        /* The ring index is j and the echo index i here, which keeps i in
         * the register the other loops give it. */
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->echo_count; i++) {
                work->echoes[i][j].active = 0;
                work->echoes[i][j].elapsed = 0;
                work->echoes[i][j].duration = 16;
                work->echoes[i][j].mode = 1;
                work->echoes[i][j].coord = Model_GetCameraCoordinateUnit();
                work->echoes[i][j].color[0] = D_8013F0B0[command].color[0];
                work->echoes[i][j].color[1] = D_8013F0B0[command].color[1];
                work->echoes[i][j].color[2] = D_8013F0B0[command].color[2];
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
            if (command == 1) {
                work->bursts[i].duration = 32;
            } else {
                work->bursts[i].duration = 48;
            }
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013F0B0[command].color[3];
            work->bursts[i].color[1] = D_8013F0B0[command].color[4];
            work->bursts[i].color[2] = D_8013F0B0[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->streak_count; i++) {
            work->streaks[i].active = 0;
            work->streaks[i].elapsed = 0;
            switch (command) {
            case 0:
                work->streaks[i].duration = 236;
                break;
            case 1:
                work->streaks[i].duration = 256;
                break;
            case 2:
                work->streaks[i].duration = 100;
                break;
            case 3:
                work->streaks[i].duration = 240;
                break;
            case 4:
                work->streaks[i].duration = 292;
                break;
            case 6:
                work->streaks[i].duration = 120;
                break;
            case 7:
                work->streaks[i].duration = 132;
                break;
            case 8:
                work->streaks[i].duration = 74;
                break;
            }
            work->streaks[i].mode = 1;
            work->streaks[i].coord = Model_GetCameraCoordinateUnit();
            work->streaks[i].color[0] = D_8013F0B0[command].color[0];
            work->streaks[i].color[1] = D_8013F0B0[command].color[1];
            work->streaks[i].color[2] = D_8013F0B0[command].color[2];
            work->streaks[i].u = 0;
            work->streaks[i].v = 0;
            work->streaks[i].done = 0;
            work->streaks[i].count = 0;
            setVector(&work->streaks[i].scale, 4096, 4096, 4096);
            setVector(&work->streaks[i].target, 0, 0, 0);
            setVector(&work->streaks[i].rotation, 0, 0, 0);
            setVector(&work->streaks[i].position, 0, 0, 0);
        }
        work->delay = D_8013F0B0[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 33; i++) {
            setVector(&work->inner[i], 0, work->radius * rsin(i * 64 + 1024) / 4096,
                work->radius + work->radius * rcos(i * 64 + 1024) / 4096);
            setVector(&work->outer[i], 0, work->radius * rsin(i * 64 + 1024) / 4096,
                work->radius + (work->radius * rcos(i * 64 + 1024) / 4096 - work->height * rsin(i << 6) / 4096));
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        /* Reloading the radius here, rather than reusing value, keeps the
         * original copy through a scratch register. */
        value = work->radius / 2;
        amount = work->radius;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, amount);
            setVector(&work->rim[i], amount * rsin(i << 8) / 4096, amount * rcos(i << 8) / 4096, amount * 5 / 4);
        }
        /* The radius term first: it fixes the order of the two loads. */
        value = work->radius / 3;
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
        /* Recomputed rather than reused, as the original does. */
        value = work->radius * 3 / 2;
        amount = work->radius * 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->cone_inner[i], value * rsin(i << 8) / 4096, 0, value * rcos(i << 8) / 4096);
            setVector(&work->cone_outer[i], amount * rsin(i << 8) / 4096, 0, amount * rcos(i << 8) / 4096);
            setVector(&work->cone_middle[i], (work->cone_inner[i].vx + work->cone_outer[i].vx) / 2,
                (work->cone_inner[i].vy + work->cone_outer[i].vy) / 2,
                (work->cone_inner[i].vz + work->cone_outer[i].vz) / 2);
        }
        for (i = 0; i < work->streak_count; i++) {
            for (j = 0; j < work->trail_length * 2; j++) {
                for (k = 0; k < 2; k++) {
                    setVector(&work->trail[k][j][i], 0, 0, 0);
                }
            }
        }
        value = work->radius;
        setVector(&work->echo_corners[0], -value, -value, 0);
        setVector(&work->echo_corners[1], value, -value, 0);
        setVector(&work->echo_corners[2], -value, value, 0);
        setVector(&work->echo_corners[3], value, value, 0);
        for (i = 0; i < 2; i++) {
            setVector(&work->prev[i], 0, 0, 0);
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < work->streak_count; j++) {
                setVector(&work->heads[i][j], 0, 0, 0);
            }
        }
        setVector(&work->offset, 160, -80, 10);
    }
    for (i = 0; i < work->streak_count; i++) {
        if (work->streaks[i].done == 0) {
            for (j = work->trail_length * 2 - 1; j >= 2; j--) {
                for (k = 0; k < 2; k++) {
                    work->trail[k][j][i] = work->trail[k][j - 2][i];
                }
            }
        }
    }
    for (i = 0; i < work->streak_count; i++) {
        if (work->streaks[i].done == 0) {
            switch (work->command) {
            case 0:
            case 1:
                TRACK_POINT(0x17, point[0]);
                TRACK_POINT(0x19, point[1]);
                break;
            case 2:
                TRACK_POINT(0x17, point[0]);
                TRACK_POINT(0x18, point[1]);
                break;
            case 3:
                TRACK_POINT(0xB, point[0]);
                TRACK_POINT(0x12, point[1]);
                break;
            case 4:
                TRACK_POINT(0xD, point[0]);
                PushMatrix();
                ApplyMatrixSV(&ls, &work->offset, &point[1]);
                PopMatrix();
                point[1].vx += ls.t[0];
                point[1].vy += ls.t[1];
                point[1].vz += ls.t[2];
                break;
            case 6:
                TRACK_POINT(0xB, point[0]);
                TRACK_POINT(0xC, point[1]);
                break;
            case 7:
                TRACK_POINT(i * 8 + 8, point[0]);
                TRACK_POINT(i * 8 + 10, point[1]);
                break;
            case 8:
                TRACK_POINT(0xD, point[0]);
                TRACK_POINT(0xE, point[1]);
                break;
            }
            for (j = 0; j < 2; j++) {
                setVector(&work->trail[j][0][i],
                    (point[j].vx - work->heads[j][i].vx) * 3 / 4 + work->heads[j][i].vx,
                    (point[j].vy - work->heads[j][i].vy) * 3 / 4 + work->heads[j][i].vy,
                    (point[j].vz - work->heads[j][i].vz) * 3 / 4 + work->heads[j][i].vz);
                setVector(&work->trail[j][1][i],
                    (point[j].vx - work->heads[j][i].vx) / 4 + work->heads[j][i].vx,
                    (point[j].vy - work->heads[j][i].vy) / 4 + work->heads[j][i].vy,
                    (point[j].vz - work->heads[j][i].vz) / 4 + work->heads[j][i].vz);
                work->heads[j][i] = point[j];
            }
        }
    }
    if (work->flags[3] == 0) {
        work->flags[3]++;
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else if (work->flags[3] != 0) {
        switch (work->command) {
        case 0:
            START_STREAKS();
            if (work->flags[1] + 4 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(3);
            }
            break;
        case 1:
            START_STREAKS();
            if (work->flags[1] * 60 + 4 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(3);
            }
            break;
        case 2:
            START_STREAKS();
            if (work->flags[1] + 18 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(3);
            }
            break;
        case 3:
            START_STREAKS();
            if (work->flags[1] + 44 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(2);
            }
            break;
        case 4:
            START_STREAKS();
            if (work->elapsed >= 189 && work->flags[1] == 0) {
                setVector(&work->bursts[0].position, 0, -350, work->direction * 450);
                work->bursts[0].active = 1;
                work->flags[1] = 1;
                work->flags[2] = 1;
            }
            break;
        case 5:
            if (work->elapsed == 0 && work->flags[1] < work->ring_count) {
                work->bursts[work->flags[1]].active = 1;
                setVector(&work->bursts[work->flags[1]].position, 0, -350, work->direction * 450);
                work->flags[1]++;
                if (work->flags[1] == work->ring_count) {
                    work->flags[2] = 1;
                }
            }
            break;
        case 6:
            START_STREAKS();
            if (work->flags[1] + 12 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(3);
            }
            break;
        case 7:
            START_STREAKS();
            if (work->flags[1] + 44 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(4);
            }
            break;
        case 8:
            START_STREAKS();
            if (work->flags[1] * 40 + 54 < work->elapsed && work->flags[1] < work->ring_count) {
                START_RING(10);
            }
            break;
        }
        PushMatrix();
        view = *(Model669ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->rings[i].elapsed << 14) / work->rings[i].duration + 4096;
                setVector(&work->rings[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
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
                    work->rings[i].color[0] = D_8013F0B0[work->command].color[0] * value / amount;
                    work->rings[i].color[1] = D_8013F0B0[work->command].color[1] * value / amount;
                    work->rings[i].color[2] = D_8013F0B0[work->command].color[2] * value / amount;
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
                setVector(&work->echoes[work->rings[i].echo][i].position, unit.matrix.t[0], unit.matrix.t[1],
                    unit.matrix.t[2]);
                work->rings[i].echo++;
                if (work->rings[i].echo >= work->echo_count) {
                    work->rings[i].echo = 0;
                }
                if (work->rings[i].elapsed > D_8013F0B0[work->command].release && work->rings[i].released == 0) {
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
                    /* Each lit vertex is set before its black partner. */
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
                    RotMatrixZ(work->echoes[i][j].rotation.vz, &unit.matrix);
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
                                &work->outer[k + 1],
                                (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
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
                    work->bursts[i].color[0] = D_8013F0B0[work->command].color[3] * value / amount;
                    work->bursts[i].color[1] = D_8013F0B0[work->command].color[4] * value / amount;
                    work->bursts[i].color[2] = D_8013F0B0[work->command].color[5] * value / amount;
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
                        &work->flash_corners[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
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
                            &work->cone_middle[k + 1],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
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
                        setRGB0(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1],
                            work->bursts[i].color[2]);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1],
                            work->bursts[i].color[2]);
                        setRGB3(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->cone_middle[k], &work->cone_outer[k], &work->cone_middle[k + 1],
                            &work->cone_outer[k + 1],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                result = 4;
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                    if (i == work->ring_count - 1) {
                        result = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->streak_count; i++) {
            if (work->streaks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->streaks[i].scale);
                unit.matrix.t[0] = work->streaks[i].position.vx;
                unit.matrix.t[1] = work->streaks[i].position.vy;
                unit.matrix.t[2] = work->streaks[i].position.vz;
                unit.super = work->streaks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->streaks[i].duration * 4 / 5 < work->streaks[i].elapsed) {
                    value = (work->streaks[i].duration - work->streaks[i].elapsed) * 5;
                    amount = work->streaks[i].duration;
                    work->streaks[i].color[0] = D_8013F0B0[work->command].color[0] * value / amount;
                    work->streaks[i].color[1] = D_8013F0B0[work->command].color[1] * value / amount;
                    work->streaks[i].color[2] = D_8013F0B0[work->command].color[2] * value / amount;
                }
                for (j = 0; j < work->streaks[i].count * 2 - 1; j++) {
                    amount = j + 1;
                    value = work->streaks[i].count * 4 - 2;
                    for (k = 0; k < 2; k++) {
                        if ((k & 1) == 0) {
                            sel[0] = 1;
                            sel[1] = 0;
                        } else {
                            sel[0] = 0;
                            sel[1] = 1;
                        }
                        mid[k].vx = (work->trail[sel[0]][j + 1][i].vx - work->trail[sel[1]][j + 1][i].vx) * amount / value
                            + work->trail[sel[1]][j + 1][i].vx;
                        mid[k].vy = (work->trail[sel[0]][j + 1][i].vy - work->trail[sel[1]][j + 1][i].vy) * amount / value
                            + work->trail[sel[1]][j + 1][i].vy;
                        mid[k].vz = (work->trail[sel[0]][j + 1][i].vz - work->trail[sel[1]][j + 1][i].vz) * amount / value
                            + work->trail[sel[1]][j + 1][i].vz;
                    }
                    if (j == 0) {
                        for (k = 0; k < 2; k++) {
                            work->prev[k].vx = work->heads[k][i].vx;
                            work->prev[k].vy = work->heads[k][i].vy;
                            work->prev[k].vz = work->heads[k][i].vz;
                        }
                    }
                    point[0].vx = (work->prev[0].vx + work->prev[1].vx) / 2;
                    point[0].vy = (work->prev[0].vy + work->prev[1].vy) / 2;
                    point[0].vz = (work->prev[0].vz + work->prev[1].vz) / 2;
                    point[1].vx = (mid[0].vx + mid[1].vx) / 2;
                    point[1].vy = (mid[0].vy + mid[1].vy) / 2;
                    point[1].vz = (mid[0].vz + mid[1].vz) / 2;
                    next = j + 1;
                    amount = work->streaks[i].count * 2 - next;
                    value = work->streaks[i].count * 2 - 1;
                    red[0] = work->streaks[i].color[0] * amount / value;
                    green[0] = work->streaks[i].color[1] * amount / value;
                    blue[0] = work->streaks[i].color[2] * amount / value;
                    red[2] = red[0] / 2;
                    green[2] = green[0] / 2;
                    blue[2] = blue[0] / 2;
                    /* Adding the 2 through a variable keeps j + 2 as its own term
                     * instead of letting it fold into the count product. */
                    ahead = 2;
                    amount = work->streaks[i].count * 2 - (j + ahead);
                    red[1] = work->streaks[i].color[0] * amount / value;
                    green[1] = work->streaks[i].color[1] * amount / value;
                    blue[1] = work->streaks[i].color[2] * amount / value;
                    red[3] = red[1] / 2;
                    green[3] = green[1] / 2;
                    blue[3] = blue[1] / 2;
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[2], green[2], blue[2]);
                    setRGB1(&work->quad, red[0], green[0], blue[0]);
                    setRGB2(&work->quad, red[3], green[3], blue[3]);
                    setRGB3(&work->quad, red[1], green[1], blue[1]);
                    depth = RotAverage4(&work->prev[0], &point[0], &mid[0], &point[1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->streaks[i].mode);
                    }
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[0], green[0], blue[0]);
                    setRGB1(&work->quad, red[2], green[2], blue[2]);
                    setRGB2(&work->quad, red[1], green[1], blue[1]);
                    setRGB3(&work->quad, red[3], green[3], blue[3]);
                    depth = RotAverage4(&point[0], &work->prev[1], &point[1], &mid[1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->streaks[i].mode);
                    }
                    for (k = 0; k < 2; k++) {
                        work->prev[k] = mid[k];
                    }
                }
                if (work->streaks[i].done == 0 && work->streaks[i].count < work->trail_length) {
                    work->streaks[i].count++;
                }
                work->streaks[i].elapsed += step;
                if (work->streaks[i].duration < work->streaks[i].elapsed) {
                    work->streaks[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->ring_count * (work->echo_count + 2) + work->streak_count && work->flags[2] == 1) {
            result = 2;
        }
    }
    return result;
}

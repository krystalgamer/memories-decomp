#include "../../types.h"
#include "variant655_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 655's entry: up to eight streaks whose two edges follow a pair of
 * model points and leave a fading ribbon of Gouraud quads behind them, then
 * up to three bursts (a growing sprite and a fan of strips) with their rings
 * of spinning sparks, released on a per-command timetable. The setup also
 * prints the work-area size. */

#ifndef VERSION_FRENCH
static const char D_8013FBE0[24] = "E_DATA size = %x byte\n\0\0";
#endif

#define TRACK_POINT(index, point)                                              \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);          \
    GsGetLwUnit(&unit, &ls);                                                   \
    (point).vx = ls.t[0];                                                      \
    (point).vy = ls.t[1];                                                      \
    (point).vz = ls.t[2]

s32 func_8013B004(u8 *context, s32 command)
{
    Model655State *work;
    MATRIX ls;
    s32 sel[2];
    GsCOORDUNIT unit;
    Model655ViewSnapshot view;
    SVECTOR point[2];
    SVECTOR mid[2];
    /* Never referenced, but it keeps the original 0x10-byte frame slot. */
    VECTOR unused;
    u8 red[4], green[4], blue[4];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value;
    s32 scale, m, ahead;
    u32 tex;
    s32 duration;
    PSXLONG depth;

    work = (Model655State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model655State));
#ifndef VERSION_FRENCH
        printf(D_8013FBE0, sizeof(Model655State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FD94[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FD94[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 1:
        case 12:
        case 22:
        case 24:
            work->rows = 1;
            work->cols = 32;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 2:
        case 10:
            work->rows = 2;
            work->cols = 16;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 3:
        case 11:
            work->rows = 1;
            work->cols = 16;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 4:
            work->rows = 4;
            work->cols = 16;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 5:
            work->rows = 8;
            work->cols = 16;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 6:
            work->rows = 1;
            work->cols = 32;
            work->burst_count = 3;
            work->spark_count = 8;
            break;
        case 7:
        case 13:
        case 19:
            work->rows = 2;
            work->cols = 16;
            work->burst_count = 2;
            work->spark_count = 8;
            break;
        case 8:
        case 9:
        case 15:
        case 21:
        case 23:
            work->rows = 0;
            work->cols = 0;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 14:
            work->rows = 2;
            work->cols = 32;
            work->burst_count = 1;
            work->spark_count = 8;
            break;
        case 16:
            work->rows = 0;
            work->cols = 0;
            work->burst_count = 3;
            work->spark_count = 8;
            break;
        case 17:
            work->rows = 6;
            work->cols = 16;
            work->burst_count = 3;
            work->spark_count = 8;
            break;
        case 18:
            work->rows = 1;
            work->cols = 32;
            work->burst_count = 2;
            work->spark_count = 8;
            break;
        case 20:
            work->rows = 0;
            work->cols = 0;
            work->burst_count = 2;
            work->spark_count = 8;
            break;
        }
        work->radius = D_8013FDCC[command].radius;
        work->height = D_8013FDCC[command].height;
        for (i = 0; i < work->rows; i++) {
            work->streaks[i].active = 0;
            work->streaks[i].elapsed = 0;
            switch (command) {
            case 0:
                work->streaks[i].duration = 118;
                break;
            case 1:
                work->streaks[i].duration = 296;
                break;
            case 2:
                work->streaks[i].duration = 96;
                break;
            case 3:
                work->streaks[i].duration = 102;
                break;
            case 4:
                work->streaks[i].duration = 112;
                break;
            case 5:
                work->streaks[i].duration = 90;
                break;
            case 6:
                work->streaks[i].duration = 232;
                break;
            case 7:
                work->streaks[i].duration = 150;
                break;
            case 10:
                work->streaks[i].duration = 202;
                break;
            case 11:
                work->streaks[i].duration = 128;
                break;
            case 12:
                work->streaks[i].duration = 168;
                break;
            case 13:
                work->streaks[i].duration = 282;
                break;
            case 14:
                work->streaks[i].duration = 452;
                break;
            case 17:
                work->streaks[i].duration = 212;
                break;
            case 18:
                work->streaks[i].duration = 152;
                break;
            case 19:
                work->streaks[i].duration = 104;
                break;
            case 22:
                work->streaks[i].duration = 60;
                break;
            case 24:
                work->streaks[i].duration = 144;
                break;
            }
            work->streaks[i].mode = 1;
            work->streaks[i].coord = Model_GetCameraCoordinateUnit();
            work->streaks[i].color[0] = D_8013FDCC[command].color[0];
            work->streaks[i].color[1] = D_8013FDCC[command].color[1];
            work->streaks[i].color[2] = D_8013FDCC[command].color[2];
            work->streaks[i].u = 0;
            work->streaks[i].v = 0;
            work->streaks[i].done = 0;
            work->streaks[i].count = 0;
            setVector(&work->streaks[i].scale, 4096, 4096, 4096);
            setVector(&work->streaks[i].unknown_40, 0, 0, 0);
            setVector(&work->streaks[i].rotation, 0, 0, 0);
            setVector(&work->streaks[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 48;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013FDCC[command].color[3];
            work->bursts[i].color[1] = D_8013FDCC[command].color[4];
            work->bursts[i].color[2] = D_8013FDCC[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].unknown_40, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 9:
                setVector(&work->bursts[i].position, 0, 80, 0);
                break;
            case 12:
                setVector(&work->bursts[i].position, 0, 120, -(work->direction * 50));
                break;
            case 14:
                setVector(&work->bursts[i].position, 0, 100, 0);
                break;
            default:
                setVector(&work->bursts[i].position, 0, 0, 0);
                break;
            }
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        for (j = 0; j < work->burst_count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = 48;
                work->sparks[i][j].mode = 1;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = D_8013FDCC[command].color[3];
                work->sparks[i][j].color[1] = D_8013FDCC[command].color[4];
                work->sparks[i][j].color[2] = D_8013FDCC[command].color[5];
                work->sparks[i][j].u = 0;
                work->sparks[i][j].v = 64;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].unknown_40, 0, 0, work->radius * 10);
                setVector(&work->sparks[i][j].rotation,
                    (i << 12) / work->spark_count + j * 113,
                    work->slot * 2048 + (i << 12) / work->spark_count + j * 113,
                    (i << 12) / work->spark_count + j * 113);
                switch (command) {
                case 9:
                    setVector(&work->sparks[i][j].position, 0, 80, 0);
                    break;
                case 12:
                    setVector(&work->sparks[i][j].position, 0, 120, -(work->direction * 50));
                    break;
                case 14:
                    setVector(&work->sparks[i][j].position, 0, 100, 0);
                    break;
                default:
                    setVector(&work->sparks[i][j].position, 0, 0, 0);
                    break;
                }
                work->sparks[i][j].position.vy -= 350;
                work->sparks[i][j].position.vz += work->direction * 450;
            }
        }
        work->delay = D_8013FDCC[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->rows; i++) {
            for (j = 0; j < work->cols * 2; j++) {
                for (k = 0; k < 2; k++) {
                    setVector(&work->trail[k][j][i], 0, 0, 0);
                }
            }
        }
        switch (command) {
        case 0:
        case 1:
            setVector(&work->offset, 0, 0, work->height);
            break;
        case 3:
            setVector(&work->offset, -5, -work->height, 80);
            break;
        }
        for (i = 0; i < 2; i++) {
            setVector(&work->prev[i], 0, 0, 0);
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < work->rows; j++) {
                setVector(&work->heads[i][j], 0, 0, 0);
            }
        }
        setVector(&work->corners[0], work->radius * -2, work->radius * -2, 0);
        setVector(&work->corners[1], work->radius * 2, work->radius * -2, 0);
        setVector(&work->corners[2], work->radius * -2, work->radius * 2, 0);
        setVector(&work->corners[3], work->radius * 2, work->radius * 2, 0);
        setVector(&work->corners[4], work->radius * -1, work->radius / 2, 0);
        setVector(&work->corners[5], work->radius, work->radius / 2, 0);
        setVector(&work->corners[6], work->radius * -1, work->radius * -2, 0);
        setVector(&work->corners[7], work->radius, work->radius * -2, 0);
        for (i = 0; i < 16; i++) {
            for (j = 0; j < 2; j++) {
                /* The tripled sines go through block-local variables, which
                 * keeps the 3 off the radius. */
                s32 swell_x, swell_y;

                setVector(&work->ring[j][i], work->radius * rsin(i * 256 + j * 64) / 4096,
                    work->radius * rcos(i * 256 + j * 64) / 4096, 0);
                setVector(&work->outer[j][i], work->radius * (swell_x = rsin(i * 256 + j * 64) * 3) / 4096,
                    work->radius * (swell_y = rcos(i * 256 + j * 64) * 3) / 4096, 0);
            }
        }
    }
    for (i = 0; i < work->rows; i++) {
        if (work->streaks[i].done == 0) {
            for (j = work->cols * 2 - 1; j >= 2; j--) {
                for (k = 0; k < 2; k++) {
                    work->trail[k][j][i] = work->trail[k][j - 2][i];
                }
            }
        }
    }
    for (i = 0; i < work->rows; i++) {
        if (work->streaks[i].done == 0) {
            switch (work->command) {
            case 0:
            case 1:
                switch (i) {
                case 0:
                    TRACK_POINT(0x12, point[0]);
                    PushMatrix();
                    ApplyMatrixSV(&ls, &work->offset, &point[1]);
                    point[1].vx += point[0].vx;
                    point[1].vy += point[0].vy;
                    point[1].vz += point[0].vz;
                    PopMatrix();
                    break;
                }
                break;
            case 2:
                switch (i) {
                case 0:
                    TRACK_POINT(17, point[0]);
                    TRACK_POINT(18, point[1]);
                    break;
                case 1:
                    TRACK_POINT(8, point[0]);
                    TRACK_POINT(9, point[1]);
                    break;
                }
                break;
            case 3:
                switch (i) {
                case 0:
                    TRACK_POINT(0xE, point[0]);
                    PushMatrix();
                    ApplyMatrixSV(&ls, &work->offset, &point[1]);
                    point[1].vx += point[0].vx;
                    point[1].vy += point[0].vy;
                    point[1].vz += point[0].vz;
                    PopMatrix();
                    break;
                }
                break;
            case 4:
                switch (i) {
                case 0:
                    TRACK_POINT(6, point[0]);
                    TRACK_POINT(7, point[1]);
                    break;
                case 1:
                    TRACK_POINT(7, point[0]);
                    TRACK_POINT(8, point[1]);
                    break;
                case 2:
                    TRACK_POINT(9, point[0]);
                    TRACK_POINT(10, point[1]);
                    break;
                case 3:
                    TRACK_POINT(11, point[0]);
                    TRACK_POINT(12, point[1]);
                    break;
                }
                break;
            case 5:
                /* The row and side stay in variables across both lookups; the
                 * side shares the fade divisor's saved register. */
                value = i / 2;
                scale = i % 2;
                TRACK_POINT(value * 3 + 7 + scale, point[0]);
                TRACK_POINT(scale + value * 3 + 8, point[1]);
                break;
            case 6:
                TRACK_POINT(23, point[0]);
                TRACK_POINT(24, point[1]);
                break;
            case 7:
                switch (i) {
                case 0:
                    TRACK_POINT(10, point[0]);
                    TRACK_POINT(11, point[1]);
                    break;
                case 1:
                    TRACK_POINT(14, point[0]);
                    TRACK_POINT(15, point[1]);
                    break;
                }
                break;
            case 10:
                switch (i) {
                case 0:
                    TRACK_POINT(6, point[0]);
                    TRACK_POINT(7, point[1]);
                    break;
                case 1:
                    TRACK_POINT(9, point[0]);
                    TRACK_POINT(10, point[1]);
                    break;
                }
                break;
            case 11:
                switch (i) {
                case 0:
                    TRACK_POINT(37, point[0]);
                    TRACK_POINT(38, point[1]);
                    break;
                }
                break;
            case 12:
                switch (i) {
                case 0:
                    TRACK_POINT(20, point[0]);
                    TRACK_POINT(24, point[1]);
                    break;
                }
                break;
            case 13:
                switch (i) {
                case 0:
                    TRACK_POINT(17, point[0]);
                    TRACK_POINT(19, point[1]);
                    break;
                case 1:
                    TRACK_POINT(22, point[0]);
                    TRACK_POINT(24, point[1]);
                    break;
                }
                break;
            case 14:
                switch (i) {
                case 0:
                    TRACK_POINT(11, point[0]);
                    TRACK_POINT(12, point[1]);
                    break;
                case 1:
                    TRACK_POINT(14, point[0]);
                    TRACK_POINT(15, point[1]);
                    break;
                }
                break;
            case 17:
                /* Written this way the remainder is not folded back into i. */
                value = i % 3 * 4 + 9 + i / 3 * 12;
                TRACK_POINT(value, point[0]);
                TRACK_POINT(value + 1, point[1]);
                break;
            case 18:
                TRACK_POINT(9, point[0]);
                TRACK_POINT(20, point[1]);
                break;
            case 19:
                value = i * 2 + 20;
                TRACK_POINT(value, point[0]);
                TRACK_POINT(value + 1, point[1]);
                break;
            case 22:
                TRACK_POINT(20, point[0]);
                TRACK_POINT(21, point[1]);
                break;
            case 24:
                TRACK_POINT(10, point[0]);
                TRACK_POINT(11, point[1]);
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
    if (work->flags[0] == 0) {
        work->flags[0]++;
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else if (work->flags[0] != 0) {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 6) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 1:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 86) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 2:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 38) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 3:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 40) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 4:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 46) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 5:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 20) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 6:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 60) {
                work->bursts[0].active = 1;
                work->bursts[0].duration = 32;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                    work->sparks[j][0].duration = 32;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 86) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 4;
            }
            if (work->flags[3] == 0 && work->elapsed > 126) {
                work->bursts[2].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][2].active = 1;
                }
                work->flags[3] = 1;
                result = 1;
            }
            break;
        case 7:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 58) {
                work->bursts[0].active = 1;
                work->bursts[0].duration = 24;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                    work->sparks[j][0].duration = 24;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 74) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 1;
            }
            break;
        case 8:
        case 9:
        case 21:
        case 23:
            if (work->elapsed == 0) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                result = 1;
            }
            break;
        case 10:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 108) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 11:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 26) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 12:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 70) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 13:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 98) {
                work->bursts[0].active = 1;
                work->bursts[0].duration = 32;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                    work->sparks[j][0].duration = 32;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 120) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 1;
            }
            break;
        case 14:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 252) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 15:
            if (work->elapsed == 0) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                result = 1;
            }
            break;
        case 16:
            if (work->flags[1] == 0 && work->elapsed == 0) {
                work->bursts[0].active = 1;
                work->bursts[0].duration = 28;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                    work->sparks[j][0].duration = 28;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 20) {
                work->bursts[1].active = 1;
                work->bursts[1].duration = 28;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                    work->sparks[j][1].duration = 28;
                }
                work->flags[2] = 1;
                result = 4;
            }
            if (work->flags[3] == 0 && work->elapsed > 40) {
                work->bursts[2].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][2].active = 1;
                }
                work->flags[3] = 1;
                result = 1;
            }
            break;
        case 17:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 62) {
                work->bursts[0].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 100) {
                work->bursts[1].active = 1;
                work->bursts[1].duration = 32;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                    work->sparks[j][1].duration = 32;
                }
                work->flags[2] = 1;
                result = 4;
            }
            if (work->flags[3] == 0 && work->elapsed > 124) {
                work->bursts[2].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][2].active = 1;
                }
                work->flags[3] = 1;
                result = 1;
            }
            break;
        case 18:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 44) {
                work->bursts[0].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 84) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 1;
            }
            break;
        case 19:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 32) {
                work->bursts[0].active = 1;
                work->bursts[0].duration = 34;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                    work->sparks[j][0].duration = 34;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 60) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 1;
            }
            break;
        case 20:
            if (work->flags[1] == 0 && work->elapsed == 0) {
                work->bursts[0].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][0].active = 1;
                }
                work->flags[1] = 1;
                result = 4;
            }
            if (work->flags[2] == 0 && work->elapsed > 44) {
                work->bursts[1].active = 1;
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j][1].active = 1;
                }
                work->flags[2] = 1;
                result = 1;
            }
            break;
        case 22:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 26) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 24:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->streaks[i].active = 1;
                }
            }
            if (work->flags[1] == 0 && work->elapsed > 74) {
                for (i = 0; i < work->burst_count; i++) {
                    work->bursts[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model655ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->rows; i++) {
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
                duration = work->streaks[i].duration;
                if (duration * 4 / 5 < work->streaks[i].elapsed) {
                    value = (duration - work->streaks[i].elapsed) * 5;
                    /* The divisor is a copy held in a saved register. */
                    scale = duration;
                    work->streaks[i].color[0] = D_8013FDCC[work->command].color[0] * value / scale;
                    work->streaks[i].color[1] = D_8013FDCC[work->command].color[1] * value / scale;
                    work->streaks[i].color[2] = D_8013FDCC[work->command].color[2] * value / scale;
                }
                for (j = 0; j < work->streaks[i].count * 2 - 1; j++) {
                    scale = j + 1;
                    value = work->streaks[i].count * 4 - 2;
                    for (k = 0; k < 2; k++) {
                        if ((k & 1) == 0) {
                            sel[0] = 1;
                            sel[1] = 0;
                        } else {
                            sel[0] = 0;
                            sel[1] = 1;
                        }
                        mid[k].vx = (work->trail[sel[0]][j + 1][i].vx - work->trail[sel[1]][j + 1][i].vx) * scale / value
                            + work->trail[sel[1]][j + 1][i].vx;
                        mid[k].vy = (work->trail[sel[0]][j + 1][i].vy - work->trail[sel[1]][j + 1][i].vy) * scale / value
                            + work->trail[sel[1]][j + 1][i].vy;
                        mid[k].vz = (work->trail[sel[0]][j + 1][i].vz - work->trail[sel[1]][j + 1][i].vz) * scale / value
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
                    m = j + 1;
                    scale = work->streaks[i].count * 2 - m;
                    value = work->streaks[i].count * 2 - 1;
                    red[0] = work->streaks[i].color[0] * scale / value;
                    green[0] = work->streaks[i].color[1] * scale / value;
                    blue[0] = work->streaks[i].color[2] * scale / value;
                    red[2] = red[0] / 2;
                    green[2] = green[0] / 2;
                    blue[2] = blue[0] / 2;
                    /* Adding the 2 through a variable keeps j + 2 as its own term
                     * instead of letting it fold into the count product. */
                    ahead = 2;
                    scale = work->streaks[i].count * 2 - (j + ahead);
                    red[1] = work->streaks[i].color[0] * scale / value;
                    green[1] = work->streaks[i].color[1] * scale / value;
                    blue[1] = work->streaks[i].color[2] * scale / value;
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
                if (work->streaks[i].done == 0 && work->streaks[i].count < work->cols) {
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
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = work->bursts[i].elapsed * 0xAAA / work->bursts[i].duration + 0x800;
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
                    scale = work->bursts[i].duration;
                    work->bursts[i].color[0] = D_8013FDCC[work->command].color[3] * value / scale;
                    work->bursts[i].color[1] = D_8013FDCC[work->command].color[4] * value / scale;
                    work->bursts[i].color[2] = D_8013FDCC[work->command].color[5] * value / scale;
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
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = work->bursts[i].elapsed * 5 * 4096 / work->bursts[i].duration;
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
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->bursts[i].u + 127, work->bursts[i].v + 64,
                        work->bursts[i].u + 127, work->bursts[i].v + 95,
                        work->bursts[i].u, work->bursts[i].v + 64,
                        work->bursts[i].u, work->bursts[i].v + 95);
                    setRGB0(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    setRGB1(&work->strip, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->outer[0][j], &work->outer[1][j], &work->ring[0][j], &work->ring[1][j],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
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
        for (j = 0; j < work->burst_count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->sparks[i][j].active == 1) {
                    if (work->sparks[i][j].duration / 2 < work->sparks[i][j].elapsed) {
                        value = (work->sparks[i][j].duration - work->sparks[i][j].elapsed) * 2;
                        scale = work->sparks[i][j].duration;
                        work->sparks[i][j].color[0] = D_8013FDCC[work->command].color[3] * value / scale;
                        work->sparks[i][j].color[1] = D_8013FDCC[work->command].color[4] * value / scale;
                        work->sparks[i][j].color[2] = D_8013FDCC[work->command].color[5] * value / scale;
                    }
                    for (k = 0; k < 8; k++) {
                        unit.flg = 0;
                        unit.matrix = D_800FE168;
                        value = 0x800;
                        scale = work->sparks[i][j].elapsed * 5 * 4096 / work->sparks[i][j].duration + value;
                        setVector(&work->sparks[i][j].scale, value, scale, value);
                        ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                        RotMatrixZ(work->sparks[i][j].rotation.vz + k * 512, &unit.matrix);
                        RotMatrixX(work->sparks[i][j].rotation.vx, &unit.matrix);
                        RotMatrixY(work->sparks[i][j].rotation.vy, &unit.matrix);
                        unit.matrix.t[0] = work->sparks[i][j].position.vx;
                        unit.matrix.t[1] = work->sparks[i][j].position.vy;
                        unit.matrix.t[2] = work->sparks[i][j].position.vz;
                        unit.super = work->sparks[i][j].coord;
                        GsGetLsUnit(&unit, &ls);
                        GsSetLsMatrix(&ls);
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[1];
                        work->sprite.clut = work->clut[1];
                        setSemiTrans(&work->sprite, 1);
                        setUV4(&work->sprite, work->sparks[i][j].u, work->sparks[i][j].v,
                            work->sparks[i][j].u + 127, work->sparks[i][j].v,
                            work->sparks[i][j].u, work->sparks[i][j].v + 31,
                            work->sparks[i][j].u + 127, work->sparks[i][j].v + 31);
                        setRGB0(&work->sprite, work->sparks[i][j].color[0], work->sparks[i][j].color[1],
                            work->sparks[i][j].color[2]);
                        depth = RotAverage4(&work->corners[4], &work->corners[6], &work->corners[5], &work->corners[7],
                            (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                        }
                    }
                    work->sparks[i][j].elapsed += step;
                    if (work->sparks[i][j].duration < work->sparks[i][j].elapsed) {
                        work->sparks[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->rows + work->burst_count * (work->spark_count + 1)) {
            result = 2;
        }
    }
    return result;
}

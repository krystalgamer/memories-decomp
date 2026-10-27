#include "../../types.h"
#include "variant662_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 662's entry: a rippling tube of strips reaching from in front of
 * the slot to a tracked model point, a dome of strips at the point, and,
 * once the tube is fully grown, a widening cone of strips and eight
 * camera-facing animated sparks around it. The setup also prints the
 * work-area size. */

#define AIM_AT_POINT(index)                                                                     \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);                         \
    GsGetLwUnit(&unit, &ls);                                                                  \
    distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)           \
        + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));            \
    for (i = 0; i < work->tube_count; i++) {                                                  \
        setVector(&work->tubes[i].position, ls.t[0], ls.t[1], ls.t[2]);                       \
        setVector(&work->tubes[i].rotation, ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450), \
            ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);          \
        work->cones[i].rotation = work->tubes[i].rotation;                                    \
        /* A separate variable for the cone's offset keeps it in s0. */                    \
        reach = work->direction * work->height / -5;                                          \
        setVector(&work->cones[i].position, reach * rsin(work->cones[i].rotation.vy) / 4096,    \
            work->direction * reach * rsin(work->cones[i].rotation.vx) / 4096 - 350,           \
            reach * rcos(work->cones[i].rotation.vy) / 4096 + work->direction * 450);           \
        work->domes[i].rotation = work->tubes[i].rotation;                                    \
        work->domes[i].position = work->tubes[i].position;                                    \
        for (j = 0; j < work->spark_count; j++) {                                             \
            PushMatrix();                                                                     \
            offset = work->sparks[j][i].offset;                                               \
            m = D_800FE168;                                                                   \
            RotMatrixZ(work->tubes[i].rotation.vz, &m);                                       \
            RotMatrixX(work->tubes[i].rotation.vx, &m);                                       \
            RotMatrixY(work->tubes[i].rotation.vy, &m);                                       \
            ApplyMatrixLV(&m, &offset, &work->sparks[j][i].position);                         \
            /* The "+= 0" keeps retail's dead load of vx. */                                   \
            applyVector(&work->sparks[j][i].position, 0, -350, work->direction * 450, +=);        \
            PopMatrix();                                                                      \
        }                                                                                     \
    }

#define STRIP_UV(element, j, k)                                                                 \
    setPolyGT4(&work->strip);                                                                 \
    work->strip.tpage = work->tpage[0];                                                       \
    work->strip.clut = work->clut[0];                                                         \
    setSemiTrans(&work->strip, 1);                                                            \
    uv[0] = (k % 4) * 63 / 4;                                                                 \
    uv[1] = (k % 4 + 1) * 63 / 4;                                                             \
    uv[2] = (j % 2) << 5;                                                                     \
    uv[3] = (j % 2 + 1) << 5;                                                                 \
    setUV4(&work->strip, (element).u + uv[0], (element).v + uv[3],                            \
        (element).u + uv[1], (element).v + uv[3],                                             \
        (element).u + uv[0], (element).v + uv[2],                                             \
        (element).u + uv[1], (element).v + uv[2])

s32 func_8013B004(u8 *context, s32 command)
{
    Model662State *work;
    MATRIX ls;
    MATRIX m;
    s32 uv[4];
    GsCOORDUNIT unit;
    Model662ViewSnapshot view;
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, i, step, inactive, distance;
    s32 j, k;
    s32 value;
    s32 duration;
    s32 reach;
    u8 r, g, b;
    u32 tex;
    PSXLONG depth;

    work = (Model662State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model662State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model662State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013F534[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013F534[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            work->tube_count = 1;
            work->spark_count = 8;
            break;
        }
        work->radius = D_8013F588[command].radius;
        work->height = D_8013F588[command].height;
        for (i = 0; i < work->tube_count; i++) {
            work->tubes[i].active = 0;
            work->tubes[i].elapsed = 0;
            switch (command) {
            case 0:
                work->tubes[i].duration = 238;
                break;
            case 1:
                work->tubes[i].duration = 332;
                break;
            case 2:
                work->tubes[i].duration = 90;
                break;
            case 3:
                work->tubes[i].duration = 174;
                break;
            case 4:
                work->tubes[i].duration = 56;
                break;
            case 5:
                work->tubes[i].duration = 240;
                break;
            case 6:
                work->tubes[i].duration = 68;
                break;
            case 7:
                work->tubes[i].duration = 296;
                break;
            }
            work->tubes[i].mode = 1;
            work->tubes[i].coord = Model_GetCameraCoordinateUnit();
            work->tubes[i].color[0] = D_8013F588[command].color[0];
            work->tubes[i].color[1] = D_8013F588[command].color[1];
            work->tubes[i].color[2] = D_8013F588[command].color[2];
            work->tubes[i].u = 0;
            work->tubes[i].v = 0;
            work->triggered[i] = 0;
            setVector(&work->tubes[i].scale, 4096, 4096, 4096);
            setVector(&work->tubes[i].offset, 0, 0, 0);
            setVector(&work->tubes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->tubes[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->tube_count; i++) {
            work->cones[i].active = 0;
            work->cones[i].elapsed = 0;
            work->cones[i].duration = work->tubes[i].duration * 3 / 4;
            work->cones[i].mode = 1;
            work->cones[i].coord = Model_GetCameraCoordinateUnit();
            work->cones[i].color[0] = D_8013F588[command].color[3];
            work->cones[i].color[1] = D_8013F588[command].color[4];
            work->cones[i].color[2] = D_8013F588[command].color[5];
            work->cones[i].u = 0;
            work->cones[i].v = 0;
            setVector(&work->cones[i].scale, 4096, 4096, 4096);
            setVector(&work->cones[i].offset, 0, 0, 0);
            setVector(&work->cones[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->cones[i].position, 0, 0, 0);
            work->cones[i].position.vy -= 350;
            work->cones[i].position.vz += work->direction * 450;
        }
        for (i = 0; i < work->tube_count; i++) {
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            work->domes[i].mode = 1;
            work->domes[i].duration = work->tubes[i].duration;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].color[0] = D_8013F588[command].color[3];
            work->domes[i].color[1] = D_8013F588[command].color[4];
            work->domes[i].color[2] = D_8013F588[command].color[5];
            work->domes[i].u = 0;
            work->domes[i].v = 0;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].offset, 0, 0, 0);
            setVector(&work->domes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->domes[i].position, 0, 0, 0);
        }
        for (j = 0; j < work->tube_count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = work->tubes[j].duration * 3 / 4;
                work->sparks[i][j].mode = 1;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = D_8013F588[command].color[6];
                work->sparks[i][j].color[1] = D_8013F588[command].color[7];
                work->sparks[i][j].color[2] = D_8013F588[command].color[8];
                work->sparks[i][j].u = (i * i % 8) << 5;
                work->sparks[i][j].v = 128;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                if (i == 0) {
                    setVector(&work->sparks[i][j].offset, 0, 0, 0);
                } else {
                    setVector(&work->sparks[i][j].offset,
                        work->radius * (rsin(((i - 1) << 12) / (work->spark_count - 1)) << 1) / 4096,
                        work->radius * (rcos(((i - 1) << 12) / (work->spark_count - 1)) << 1) / 4096,
                        work->radius * ((i % 2) * 2 - 1) * rsin(((i - 1) << 12) / (work->spark_count - 1)) / 4096
                            - work->radius);
                }
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        work->delay = D_8013F588[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->tube_count; i++) {
            for (j = 0; j < 5; j++) {
                for (k = 0; k < 9; k++) {
                    /* Written "radius * 3 * rsin", GCC folds this into retail's
                     * rsin * 3 product. */
                    setVector(&work->dome_points[k][j][i],
                        work->radius * 3 * rsin(j << 8) / 4096 * rsin(k << 9) / 4096,
                        work->radius * 3 * rsin(j << 8) / 4096 * rcos(k << 9) / 4096,
                        work->radius * -(j * 5) / 4);
                }
            }
        }
        setVector(&work->corners[0], work->radius * -2, work->radius * -2, 0);
        setVector(&work->corners[1], work->radius * 2, work->radius * -2, 0);
        setVector(&work->corners[2], work->radius * -2, work->radius * 2, 0);
        setVector(&work->corners[3], work->radius * 2, work->radius * 2, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        if (work->elapsed == 0) {
            for (i = 0; i < work->tube_count; i++) {
                work->tubes[i].active = 1;
                work->domes[i].active = 1;
                work->flags[1] = 1;
            }
        }
        break;
    case 2:
        if (work->elapsed == 0) {
            work->cones[0].active = 1;
            work->cones[0].position.vz -= work->direction * 500;
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][0].active = 1;
                PushMatrix();
                offset = work->sparks[i][0].offset;
                m = D_800FE168;
                RotMatrixY(work->slot * 2048, &m);
                ApplyMatrixLV(&m, &offset, &work->sparks[i][0].position);
                applyVector(&work->sparks[i][0].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
            work->flags[1] = 1;
        }
        break;
    }
    switch (work->command) {
    case 0:
        AIM_AT_POINT(7);
        break;
    case 1:
        AIM_AT_POINT(6);
        break;
    case 3:
        AIM_AT_POINT(7);
        break;
    case 4:
        AIM_AT_POINT(0);
        break;
    case 5:
        AIM_AT_POINT(0x18);
        break;
    case 6:
        AIM_AT_POINT(0xB);
        break;
    case 7:
        AIM_AT_POINT(0x11);
        break;
    default:
        distance = 0;
        break;
    }
    PushMatrix();
    view = *(Model662ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->tube_count; i++) {
        if (work->tubes[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->tubes[i].elapsed < work->tubes[i].duration / 4) {
                value = (work->tubes[i].elapsed << 14) / work->tubes[i].duration;
            } else {
                value = 4096;
                if (work->triggered[i] == 0) {
                    work->cones[i].active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].active = 1;
                    }
                    work->triggered[i] = 1;
                }
            }
            setVector(&work->tubes[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->tubes[i].scale);
            RotMatrixZ(work->tubes[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->tubes[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->tubes[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->tubes[i].position.vx;
            unit.matrix.t[1] = work->tubes[i].position.vy;
            unit.matrix.t[2] = work->tubes[i].position.vz;
            unit.super = work->tubes[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->tubes[i].duration * 9 / 10 < work->tubes[i].elapsed) {
                value = (work->tubes[i].duration - work->tubes[i].elapsed) * 10;
                duration = work->tubes[i].duration;
                work->tubes[i].color[0] = D_8013F588[work->command].color[0] * value / duration;
                work->tubes[i].color[1] = D_8013F588[work->command].color[1] * value / duration;
                work->tubes[i].color[2] = D_8013F588[work->command].color[2] * value / duration;
            }
            /* k runs the segments and j the ring points, so j keeps the
             * register it has in every other loop. */
            for (k = 0; k < 17; k++) {
                for (j = 0; j < 9; j++) {
                    /* The quarter radius is computed once, before both calls. */
                    value = work->radius / 4;
                    setVector(&work->tube_points[j][k][i],
                        (work->radius + value * rsin(work->tubes[i].elapsed * 100 + k * -500) / 4096)
                            * rsin(j << 9) / 4096,
                        (work->radius + value * rsin(work->tubes[i].elapsed * 100 + k * -500) / 4096)
                            * rcos(j << 9) / 4096,
                        distance * -k / 16);
                }
            }
            work->tubes[i].v = work->tubes[i].elapsed * 4 % 64;
            for (j = 0; j < 16; j++) {
                for (k = 0; k < 8; k++) {
                    STRIP_UV(work->tubes[i], j, k);
                    if (j == 0) {
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                    } else {
                        setRGB0(&work->strip, work->tubes[i].color[0], work->tubes[i].color[1], work->tubes[i].color[2]);
                        setRGB1(&work->strip, work->tubes[i].color[0], work->tubes[i].color[1], work->tubes[i].color[2]);
                    }
                    if (j == 15) {
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                    } else {
                        setRGB2(&work->strip, work->tubes[i].color[0], work->tubes[i].color[1], work->tubes[i].color[2]);
                        setRGB3(&work->strip, work->tubes[i].color[0], work->tubes[i].color[1], work->tubes[i].color[2]);
                    }
                    depth = RotAverage4(&work->tube_points[k][j][i], &work->tube_points[k + 1][j][i],
                        &work->tube_points[k][j + 1][i], &work->tube_points[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->tubes[i].elapsed += step;
            if (work->tubes[i].duration < work->tubes[i].elapsed) {
                work->tubes[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->tube_count; i++) {
        if (work->cones[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            /* Here and below, an if/else rather than a preset scale keeps
             * retail's spill slots and frame size. */
            if (work->cones[i].elapsed < work->cones[i].duration / 2) {
                value = (work->cones[i].elapsed << 13) / work->cones[i].duration + 0x400;
            } else {
                value = 0x1400;
            }
            setVector(&work->cones[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->cones[i].scale);
            RotMatrixZ(work->cones[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->cones[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->cones[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->cones[i].position.vx;
            unit.matrix.t[1] = work->cones[i].position.vy;
            unit.matrix.t[2] = work->cones[i].position.vz;
            unit.super = work->cones[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->cones[i].duration * 9 / 10 < work->cones[i].elapsed) {
                value = (work->cones[i].duration - work->cones[i].elapsed) * 10;
                duration = work->cones[i].duration;
                work->cones[i].color[0] = D_8013F588[work->command].color[3] * value / duration;
                work->cones[i].color[1] = D_8013F588[work->command].color[4] * value / duration;
                work->cones[i].color[2] = D_8013F588[work->command].color[5] * value / duration;
            }
            for (j = 0; j < 9; j++) {
                /* The cone's depth shares duration's register (s0). */
                duration = work->height * -rsin(j * 341 * 2 / 8) / 4096;
                for (k = 0; k < 17; k++) {
                    value = work->radius * j / 8 * rsin(work->cones[i].elapsed * 100 + (k << 10)) / 4096;
                    setVector(&work->cone_points[k][j][i], (work->height + value) * j / 8 * rsin(k << 8) / 4096,
                        (work->height + value) * j / 8 * rcos(k << 8) / 4096, duration + value);
                }
            }
            work->cones[i].v = work->cones[i].elapsed * 4 % 64;
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 16; k++) {
                    STRIP_UV(work->cones[i], j, k);
                    if (j >= 4) {
                        value = (8 - j) * 2;
                        r = work->cones[i].color[0] * value / 8;
                        g = work->cones[i].color[1] * value / 8;
                        b = work->cones[i].color[2] * value / 8;
                    } else if (j == 0) {
                        /* Chained from r, as retail copies it. */
                        b = g = r = 0;
                    } else {
                        r = work->cones[i].color[0];
                        g = work->cones[i].color[1];
                        b = work->cones[i].color[2];
                    }
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    if (j >= 4) {
                        value = (7 - j) * 2;
                        r = work->cones[i].color[0] * value / 8;
                        g = work->cones[i].color[1] * value / 8;
                        b = work->cones[i].color[2] * value / 8;
                    } else {
                        r = work->cones[i].color[0];
                        g = work->cones[i].color[1];
                        b = work->cones[i].color[2];
                    }
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->cone_points[k][j][i], &work->cone_points[k + 1][j][i],
                        &work->cone_points[k][j + 1][i], &work->cone_points[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->cones[i].elapsed += step;
            if (work->cones[i].duration < work->cones[i].elapsed) {
                work->cones[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->tube_count; i++) {
        if (work->domes[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->domes[i].elapsed < work->domes[i].duration / 4) {
                value = (work->domes[i].elapsed << 14) / work->domes[i].duration;
            } else {
                value = 4096;
            }
            setVector(&work->domes[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->domes[i].scale);
            RotMatrixZ(work->domes[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->domes[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->domes[i].position.vx;
            unit.matrix.t[1] = work->domes[i].position.vy;
            unit.matrix.t[2] = work->domes[i].position.vz;
            unit.super = work->domes[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->domes[i].duration * 9 / 10 < work->domes[i].elapsed) {
                value = (work->domes[i].duration - work->domes[i].elapsed) * 10;
                duration = work->domes[i].duration;
                work->domes[i].color[0] = D_8013F588[work->command].color[3] * value / duration;
                work->domes[i].color[1] = D_8013F588[work->command].color[4] * value / duration;
                work->domes[i].color[2] = D_8013F588[work->command].color[5] * value / duration;
            }
            work->domes[i].v = work->domes[i].elapsed * 4 % 64;
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 8; k++) {
                    STRIP_UV(work->domes[i], j, k);
                    value = 4 - j;
                    r = work->domes[i].color[0] * value / 4;
                    g = work->domes[i].color[1] * value / 4;
                    b = work->domes[i].color[2] * value / 4;
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    value = 3 - j;
                    r = work->domes[i].color[0] * value / 4;
                    g = work->domes[i].color[1] * value / 4;
                    b = work->domes[i].color[2] * value / 4;
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->dome_points[k][j][i], &work->dome_points[k + 1][j][i],
                        &work->dome_points[k][j + 1][i], &work->dome_points[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->domes[i].elapsed += step;
            if (work->domes[i].duration < work->domes[i].elapsed) {
                work->domes[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->tube_count; j++) {
        for (i = 0; i < work->spark_count; i++) {
            if (work->sparks[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->sparks[i][j].elapsed < work->sparks[i][j].duration / 4) {
                    value = (work->sparks[i][j].elapsed << 14) / work->sparks[i][j].duration;
                } else {
                    value = 4096;
                }
                setVector(&work->sparks[i][j].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sparks[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->sparks[i][j].position.vx;
                unit.matrix.t[1] = work->sparks[i][j].position.vy;
                unit.matrix.t[2] = work->sparks[i][j].position.vz;
                unit.super = work->sparks[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i][j].duration * 9 / 10 < work->sparks[i][j].elapsed) {
                    value = (work->sparks[i][j].duration - work->sparks[i][j].elapsed) * 10;
                    duration = work->sparks[i][j].duration;
                    work->sparks[i][j].color[0] = D_8013F588[work->command].color[6] * value / duration;
                    work->sparks[i][j].color[1] = D_8013F588[work->command].color[7] * value / duration;
                    work->sparks[i][j].color[2] = D_8013F588[work->command].color[8] * value / duration;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[1];
                work->sprite.clut = work->clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->sparks[i][j].u, work->sparks[i][j].v,
                    work->sparks[i][j].u + 31, work->sparks[i][j].v,
                    work->sparks[i][j].u, work->sparks[i][j].v + 31,
                    work->sparks[i][j].u + 31, work->sparks[i][j].v + 31);
                setRGB0(&work->sprite, work->sparks[i][j].color[0], work->sparks[i][j].color[1],
                    work->sparks[i][j].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->sparks[i][j].u += 32;
                if (work->sparks[i][j].u > 224) {
                    work->sparks[i][j].u = 0;
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
    if (work->flags[1] == 1) {
        if (inactive == work->tube_count * 3 + work->tube_count * work->spark_count) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

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
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, index);                         \
    GsGetLwUnit(&unit, &ls);                                                                  \
    distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)           \
        + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));            \
    for (i = 0; i < work->tube_count; i++) {                                                  \
        setVector(&work->tubes[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);                       \
        setVector(&work->tubes[i].xform.rotation, ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450), \
            ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->head.slot * 2048, 0);          \
        work->cones[i].xform.rotation = work->tubes[i].xform.rotation;                                    \
        /* A separate variable for the cone's offset keeps it in s0. */                    \
        reach = work->direction * work->place.height / -5;                                          \
        setVector(&work->cones[i].xform.position, reach * rsin(work->cones[i].xform.rotation.vy) / 4096,    \
            work->direction * reach * rsin(work->cones[i].xform.rotation.vx) / 4096 - 350,           \
            reach * rcos(work->cones[i].xform.rotation.vy) / 4096 + work->direction * 450);           \
        work->domes[i].xform.rotation = work->tubes[i].xform.rotation;                                    \
        work->domes[i].xform.position = work->tubes[i].xform.position;                                    \
        for (j = 0; j < work->spark_count; j++) {                                             \
            PushMatrix();                                                                     \
            offset = work->sparks[j][i].offset;                                               \
            m = GsIDMATRIX;                                                                   \
            RotMatrixZ(work->tubes[i].xform.rotation.vz, &m);                                       \
            RotMatrixX(work->tubes[i].xform.rotation.vx, &m);                                       \
            RotMatrixY(work->tubes[i].xform.rotation.vy, &m);                                       \
            ApplyMatrixLV(&m, &offset, &work->sparks[j][i].xform.position);                         \
            /* The "+= 0" keeps retail's dead load of vx. */                                   \
            applyVector(&work->sparks[j][i].xform.position, 0, -350, work->direction * 450, +=);        \
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
    setUV4(&work->strip, (element).head.u + uv[0], (element).head.v + uv[3],                            \
        (element).head.u + uv[1], (element).head.v + uv[3],                                             \
        (element).head.u + uv[0], (element).head.v + uv[2],                                             \
        (element).head.u + uv[1], (element).head.v + uv[2])

s32 ModelVariant662_HandleRequest(u8 *context, s32 command)
{
    Model662State *work;
    MATRIX ls;
    MATRIX m;
    s32 uv[4];
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    VECTOR offset;
    long interpolation, flag;
    s32 result, i, step, inactive, distance;
    s32 j, k;
    s32 value;
    s32 duration;
    s32 reach;
    u8 r, g, b;
    u32 tex;
    long depth;

    work = (Model662State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model662State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model662State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant662_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant662_aImages[0]);
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
        work->place.radius = gModelVariant662_aConfigs[command].radius;
        work->place.height = gModelVariant662_aConfigs[command].height;
        for (i = 0; i < work->tube_count; i++) {
            work->tubes[i].head.active = 0;
            work->tubes[i].head.elapsed = 0;
            switch (command) {
            case 0:
                work->tubes[i].head.duration = 238;
                break;
            case 1:
                work->tubes[i].head.duration = 332;
                break;
            case 2:
                work->tubes[i].head.duration = 90;
                break;
            case 3:
                work->tubes[i].head.duration = 174;
                break;
            case 4:
                work->tubes[i].head.duration = 56;
                break;
            case 5:
                work->tubes[i].head.duration = 240;
                break;
            case 6:
                work->tubes[i].head.duration = 68;
                break;
            case 7:
                work->tubes[i].head.duration = 296;
                break;
            }
            work->tubes[i].head.mode = 1;
            work->tubes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->tubes[i].head.color[0] = gModelVariant662_aConfigs[command].color[0];
            work->tubes[i].head.color[1] = gModelVariant662_aConfigs[command].color[1];
            work->tubes[i].head.color[2] = gModelVariant662_aConfigs[command].color[2];
            work->tubes[i].head.u = 0;
            work->tubes[i].head.v = 0;
            work->triggered[i] = 0;
            setVector(&work->tubes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->tubes[i].offset, 0, 0, 0);
            setVector(&work->tubes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->tubes[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->tube_count; i++) {
            work->cones[i].head.active = 0;
            work->cones[i].head.elapsed = 0;
            work->cones[i].head.duration = work->tubes[i].head.duration * 3 / 4;
            work->cones[i].head.mode = 1;
            work->cones[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->cones[i].head.color[0] = gModelVariant662_aConfigs[command].color[3];
            work->cones[i].head.color[1] = gModelVariant662_aConfigs[command].color[4];
            work->cones[i].head.color[2] = gModelVariant662_aConfigs[command].color[5];
            work->cones[i].head.u = 0;
            work->cones[i].head.v = 0;
            setVector(&work->cones[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->cones[i].offset, 0, 0, 0);
            setVector(&work->cones[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->cones[i].xform.position, 0, 0, 0);
            work->cones[i].xform.position.vy -= 350;
            work->cones[i].xform.position.vz += work->direction * 450;
        }
        for (i = 0; i < work->tube_count; i++) {
            work->domes[i].head.active = 0;
            work->domes[i].head.elapsed = 0;
            work->domes[i].head.mode = 1;
            work->domes[i].head.duration = work->tubes[i].head.duration;
            work->domes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i].head.color[0] = gModelVariant662_aConfigs[command].color[3];
            work->domes[i].head.color[1] = gModelVariant662_aConfigs[command].color[4];
            work->domes[i].head.color[2] = gModelVariant662_aConfigs[command].color[5];
            work->domes[i].head.u = 0;
            work->domes[i].head.v = 0;
            setVector(&work->domes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->domes[i].offset, 0, 0, 0);
            setVector(&work->domes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->domes[i].xform.position, 0, 0, 0);
        }
        for (j = 0; j < work->tube_count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][j].head.active = 0;
                work->sparks[i][j].head.elapsed = 0;
                work->sparks[i][j].head.duration = work->tubes[j].head.duration * 3 / 4;
                work->sparks[i][j].head.mode = 1;
                work->sparks[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].head.color[0] = gModelVariant662_aConfigs[command].color[6];
                work->sparks[i][j].head.color[1] = gModelVariant662_aConfigs[command].color[7];
                work->sparks[i][j].head.color[2] = gModelVariant662_aConfigs[command].color[8];
                work->sparks[i][j].head.u = (i * i % 8) << 5;
                work->sparks[i][j].head.v = 128;
                setVector(&work->sparks[i][j].xform.scale, 4096, 4096, 4096);
                if (i == 0) {
                    setVector(&work->sparks[i][j].offset, 0, 0, 0);
                } else {
                    setVector(&work->sparks[i][j].offset,
                        work->place.radius * (rsin(((i - 1) << 12) / (work->spark_count - 1)) << 1) / 4096,
                        work->place.radius * (rcos(((i - 1) << 12) / (work->spark_count - 1)) << 1) / 4096,
                        work->place.radius * ((i % 2) * 2 - 1) * rsin(((i - 1) << 12) / (work->spark_count - 1)) / 4096
                            - work->place.radius);
                }
                setVector(&work->sparks[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->sparks[i][j].xform.position, 0, 0, 0);
            }
        }
        work->place.delay = gModelVariant662_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->tube_count; i++) {
            for (j = 0; j < 5; j++) {
                for (k = 0; k < 9; k++) {
                    /* Written "radius * 3 * rsin", GCC folds this into retail's
                     * rsin * 3 product. */
                    setVector(&work->dome_points[k][j][i],
                        work->place.radius * 3 * rsin(j << 8) / 4096 * rsin(k << 9) / 4096,
                        work->place.radius * 3 * rsin(j << 8) / 4096 * rcos(k << 9) / 4096,
                        work->place.radius * -(j * 5) / 4);
                }
            }
        }
        setVector(&work->corners[0], work->place.radius * -2, work->place.radius * -2, 0);
        setVector(&work->corners[1], work->place.radius * 2, work->place.radius * -2, 0);
        setVector(&work->corners[2], work->place.radius * -2, work->place.radius * 2, 0);
        setVector(&work->corners[3], work->place.radius * 2, work->place.radius * 2, 0);
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
        return result;
    }
    switch (work->head.command) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        if (work->head.elapsed == 0) {
            for (i = 0; i < work->tube_count; i++) {
                work->tubes[i].head.active = 1;
                work->domes[i].head.active = 1;
                work->flags[1] = 1;
            }
        }
        break;
    case 2:
        if (work->head.elapsed == 0) {
            work->cones[0].head.active = 1;
            work->cones[0].xform.position.vz -= work->direction * 500;
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][0].head.active = 1;
                PushMatrix();
                offset = work->sparks[i][0].offset;
                m = GsIDMATRIX;
                RotMatrixY(work->head.slot * 2048, &m);
                ApplyMatrixLV(&m, &offset, &work->sparks[i][0].xform.position);
                applyVector(&work->sparks[i][0].xform.position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
            work->flags[1] = 1;
        }
        break;
    }
    switch (work->head.command) {
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
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->tube_count; i++) {
        if (work->tubes[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            if (work->tubes[i].head.elapsed < work->tubes[i].head.duration / 4) {
                value = (work->tubes[i].head.elapsed << 14) / work->tubes[i].head.duration;
            } else {
                value = 4096;
                if (work->triggered[i] == 0) {
                    work->cones[i].head.active = 1;
                    for (j = 0; j < work->spark_count; j++) {
                        work->sparks[j][i].head.active = 1;
                    }
                    work->triggered[i] = 1;
                }
            }
            setVector(&work->tubes[i].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->tubes[i].xform.scale);
            RotMatrixZ(work->tubes[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->tubes[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->tubes[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->tubes[i].xform.position.vx;
            unit.matrix.t[1] = work->tubes[i].xform.position.vy;
            unit.matrix.t[2] = work->tubes[i].xform.position.vz;
            unit.super = work->tubes[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->tubes[i].head.duration * 9 / 10 < work->tubes[i].head.elapsed) {
                value = (work->tubes[i].head.duration - work->tubes[i].head.elapsed) * 10;
                duration = work->tubes[i].head.duration;
                work->tubes[i].head.color[0] = gModelVariant662_aConfigs[work->head.command].color[0] * value / duration;
                work->tubes[i].head.color[1] = gModelVariant662_aConfigs[work->head.command].color[1] * value / duration;
                work->tubes[i].head.color[2] = gModelVariant662_aConfigs[work->head.command].color[2] * value / duration;
            }
            /* k runs the segments and j the ring points, so j keeps the
             * register it has in every other loop. */
            for (k = 0; k < 17; k++) {
                for (j = 0; j < 9; j++) {
                    /* The quarter radius is computed once, before both calls. */
                    value = work->place.radius / 4;
                    setVector(&work->tube_points[j][k][i],
                        (work->place.radius + value * rsin(work->tubes[i].head.elapsed * 100 + k * -500) / 4096)
                            * rsin(j << 9) / 4096,
                        (work->place.radius + value * rsin(work->tubes[i].head.elapsed * 100 + k * -500) / 4096)
                            * rcos(j << 9) / 4096,
                        distance * -k / 16);
                }
            }
            work->tubes[i].head.v = work->tubes[i].head.elapsed * 4 % 64;
            for (j = 0; j < 16; j++) {
                for (k = 0; k < 8; k++) {
                    STRIP_UV(work->tubes[i], j, k);
                    if (j == 0) {
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                    } else {
                        setRGB0(&work->strip, work->tubes[i].head.color[0], work->tubes[i].head.color[1], work->tubes[i].head.color[2]);
                        setRGB1(&work->strip, work->tubes[i].head.color[0], work->tubes[i].head.color[1], work->tubes[i].head.color[2]);
                    }
                    if (j == 15) {
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                    } else {
                        setRGB2(&work->strip, work->tubes[i].head.color[0], work->tubes[i].head.color[1], work->tubes[i].head.color[2]);
                        setRGB3(&work->strip, work->tubes[i].head.color[0], work->tubes[i].head.color[1], work->tubes[i].head.color[2]);
                    }
                    depth = RotAverage4(&work->tube_points[k][j][i], &work->tube_points[k + 1][j][i],
                        &work->tube_points[k][j + 1][i], &work->tube_points[k + 1][j + 1][i],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
            }
            work->tubes[i].head.elapsed += step;
            if (work->tubes[i].head.duration < work->tubes[i].head.elapsed) {
                work->tubes[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->tube_count; i++) {
        if (work->cones[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            /* Here and below, an if/else rather than a preset scale keeps
             * retail's spill slots and frame size. */
            if (work->cones[i].head.elapsed < work->cones[i].head.duration / 2) {
                value = (work->cones[i].head.elapsed << 13) / work->cones[i].head.duration + 0x400;
            } else {
                value = 0x1400;
            }
            setVector(&work->cones[i].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->cones[i].xform.scale);
            RotMatrixZ(work->cones[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->cones[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->cones[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->cones[i].xform.position.vx;
            unit.matrix.t[1] = work->cones[i].xform.position.vy;
            unit.matrix.t[2] = work->cones[i].xform.position.vz;
            unit.super = work->cones[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->cones[i].head.duration * 9 / 10 < work->cones[i].head.elapsed) {
                value = (work->cones[i].head.duration - work->cones[i].head.elapsed) * 10;
                duration = work->cones[i].head.duration;
                work->cones[i].head.color[0] = gModelVariant662_aConfigs[work->head.command].color[3] * value / duration;
                work->cones[i].head.color[1] = gModelVariant662_aConfigs[work->head.command].color[4] * value / duration;
                work->cones[i].head.color[2] = gModelVariant662_aConfigs[work->head.command].color[5] * value / duration;
            }
            for (j = 0; j < 9; j++) {
                /* The cone's depth shares duration's register (s0). */
                duration = work->place.height * -rsin(j * 341 * 2 / 8) / 4096;
                for (k = 0; k < 17; k++) {
                    value = work->place.radius * j / 8 * rsin(work->cones[i].head.elapsed * 100 + (k << 10)) / 4096;
                    setVector(&work->cone_points[k][j][i], (work->place.height + value) * j / 8 * rsin(k << 8) / 4096,
                        (work->place.height + value) * j / 8 * rcos(k << 8) / 4096, duration + value);
                }
            }
            work->cones[i].head.v = work->cones[i].head.elapsed * 4 % 64;
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 16; k++) {
                    STRIP_UV(work->cones[i], j, k);
                    if (j >= 4) {
                        value = (8 - j) * 2;
                        r = work->cones[i].head.color[0] * value / 8;
                        g = work->cones[i].head.color[1] * value / 8;
                        b = work->cones[i].head.color[2] * value / 8;
                    } else if (j == 0) {
                        /* Chained from r, as retail copies it. */
                        b = g = r = 0;
                    } else {
                        r = work->cones[i].head.color[0];
                        g = work->cones[i].head.color[1];
                        b = work->cones[i].head.color[2];
                    }
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    if (j >= 4) {
                        value = (7 - j) * 2;
                        r = work->cones[i].head.color[0] * value / 8;
                        g = work->cones[i].head.color[1] * value / 8;
                        b = work->cones[i].head.color[2] * value / 8;
                    } else {
                        r = work->cones[i].head.color[0];
                        g = work->cones[i].head.color[1];
                        b = work->cones[i].head.color[2];
                    }
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->cone_points[k][j][i], &work->cone_points[k + 1][j][i],
                        &work->cone_points[k][j + 1][i], &work->cone_points[k + 1][j + 1][i],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
            }
            work->cones[i].head.elapsed += step;
            if (work->cones[i].head.duration < work->cones[i].head.elapsed) {
                work->cones[i].head.active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->tube_count; i++) {
        if (work->domes[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            if (work->domes[i].head.elapsed < work->domes[i].head.duration / 4) {
                value = (work->domes[i].head.elapsed << 14) / work->domes[i].head.duration;
            } else {
                value = 4096;
            }
            setVector(&work->domes[i].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
            RotMatrixZ(work->domes[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->domes[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->domes[i].xform.position.vx;
            unit.matrix.t[1] = work->domes[i].xform.position.vy;
            unit.matrix.t[2] = work->domes[i].xform.position.vz;
            unit.super = work->domes[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->domes[i].head.duration * 9 / 10 < work->domes[i].head.elapsed) {
                value = (work->domes[i].head.duration - work->domes[i].head.elapsed) * 10;
                duration = work->domes[i].head.duration;
                work->domes[i].head.color[0] = gModelVariant662_aConfigs[work->head.command].color[3] * value / duration;
                work->domes[i].head.color[1] = gModelVariant662_aConfigs[work->head.command].color[4] * value / duration;
                work->domes[i].head.color[2] = gModelVariant662_aConfigs[work->head.command].color[5] * value / duration;
            }
            work->domes[i].head.v = work->domes[i].head.elapsed * 4 % 64;
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 8; k++) {
                    STRIP_UV(work->domes[i], j, k);
                    value = 4 - j;
                    r = work->domes[i].head.color[0] * value / 4;
                    g = work->domes[i].head.color[1] * value / 4;
                    b = work->domes[i].head.color[2] * value / 4;
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    value = 3 - j;
                    r = work->domes[i].head.color[0] * value / 4;
                    g = work->domes[i].head.color[1] * value / 4;
                    b = work->domes[i].head.color[2] * value / 4;
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->dome_points[k][j][i], &work->dome_points[k + 1][j][i],
                        &work->dome_points[k][j + 1][i], &work->dome_points[k + 1][j + 1][i],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
            }
            work->domes[i].head.elapsed += step;
            if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                work->domes[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->tube_count; j++) {
        for (i = 0; i < work->spark_count; i++) {
            if (work->sparks[i][j].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->sparks[i][j].head.elapsed < work->sparks[i][j].head.duration / 4) {
                    value = (work->sparks[i][j].head.elapsed << 14) / work->sparks[i][j].head.duration;
                } else {
                    value = 4096;
                }
                setVector(&work->sparks[i][j].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->sparks[i][j].xform.scale);
                RotMatrixZ(work->sparks[i][j].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i][j].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sparks[i][j].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->sparks[i][j].xform.position.vx;
                unit.matrix.t[1] = work->sparks[i][j].xform.position.vy;
                unit.matrix.t[2] = work->sparks[i][j].xform.position.vz;
                unit.super = work->sparks[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i][j].head.duration * 9 / 10 < work->sparks[i][j].head.elapsed) {
                    value = (work->sparks[i][j].head.duration - work->sparks[i][j].head.elapsed) * 10;
                    duration = work->sparks[i][j].head.duration;
                    work->sparks[i][j].head.color[0] = gModelVariant662_aConfigs[work->head.command].color[6] * value / duration;
                    work->sparks[i][j].head.color[1] = gModelVariant662_aConfigs[work->head.command].color[7] * value / duration;
                    work->sparks[i][j].head.color[2] = gModelVariant662_aConfigs[work->head.command].color[8] * value / duration;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[1];
                work->sprite.clut = work->clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->sparks[i][j].head.u, work->sparks[i][j].head.v,
                    work->sparks[i][j].head.u + 31, work->sparks[i][j].head.v,
                    work->sparks[i][j].head.u, work->sparks[i][j].head.v + 31,
                    work->sparks[i][j].head.u + 31, work->sparks[i][j].head.v + 31);
                setRGB0(&work->sprite, work->sparks[i][j].head.color[0], work->sparks[i][j].head.color[1],
                    work->sparks[i][j].head.color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->sparks[i][j].head.u += 32;
                if (work->sparks[i][j].head.u > 224) {
                    work->sparks[i][j].head.u = 0;
                }
                work->sparks[i][j].head.elapsed += step;
                if (work->sparks[i][j].head.duration < work->sparks[i][j].head.elapsed) {
                    work->sparks[i][j].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->head.elapsed += step;
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

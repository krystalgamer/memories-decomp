#include "../../types.h"
#include "variant632_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 632's entry: 32 camera-facing flares (two mirrored sets for command
 * 1) flash on a disc in front of the slot and fade, a dotted sphere grows
 * there, and commands 0 and 3 draw a swipe trail of ten shaded strips behind
 * two points tracked on the model over the last eleven frames. The setup also
 * prints the work-area size. */

/* The quad corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic: that is retail's three loads
 * of the radius and the zero-extension before the third negation. */
static inline s32 Model632_Negate(s32 value)
{
    return -value;
}

s32 ModelVariant632_HandleRequest(u8 *context, s32 command)
{
    Model632State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    SVECTOR corners[4];
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    u32 tex;
    long depth;
    s32 value;

    work = (Model632State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model632State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model632State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant632_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant632_aImages[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->flare_count = 32;
            work->sphere_count = 1;
            work->copies = 1;
            break;
        case 1:
            work->flare_count = 32;
            work->sphere_count = 0;
            work->copies = 2;
            break;
        case 2:
        case 3:
        case 4:
            work->flare_count = 32;
            work->sphere_count = 1;
            work->copies = 1;
            break;
        }
        work->radius = gModelVariant632_aConfigs[command].radius;
        work->unknown_0C = gModelVariant632_aConfigs[command].unknown_04;
        for (i = 0; i < work->flare_count; i++) {
            work->flares[i][0].head.active = 0;
            work->flares[i][0].head.elapsed = 0;
            switch (command) {
            case 0:
                work->flares[i][0].head.duration = 16;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                work->flares[i][0].head.duration = 32;
                break;
            }
            work->flares[i][0].head.mode = 1;
            work->flares[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->flares[i][0].head.color[0] = gModelVariant632_aConfigs[command].red;
            work->flares[i][0].head.color[1] = gModelVariant632_aConfigs[command].green;
            work->flares[i][0].head.color[2] = gModelVariant632_aConfigs[command].blue;
            work->flares[i][0].head.u = 0;
            work->flares[i][0].head.v = 16;
            setVector(&work->flares[i][0].xform.scale, 4096, 4096, 4096);
            setVector(&work->flares[i][0].growth, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                setVector(&work->flares[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->flares[i][0].xform.position,
                    work->radius / 2 * (i % 3 + 1) * rsin(i * 4096 / work->flare_count) / 4096,
                    work->radius / 2 * ((work->flare_count / 2 - i) % 3) * (i % 2 * 2 - 1),
                    work->radius / 2 * (i % 3 + 1) * rcos(i * 4096 / work->flare_count) / 4096 * (i % 2 * 2 - 1));
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->flares[i][j] = work->flares[i][0];
                if (j / 2 * 2 == j - 1) {
                    s32 swap = work->flares[i][j].xform.position.vx;
                    work->flares[i][j].xform.position.vx = work->flares[i][j].xform.position.vz;
                    work->flares[i][j].xform.position.vz = swap;
                }
                work->flares[i][j].xform.position.vy -= 350;
                work->flares[i][j].xform.position.vz += (work->head.slot * 2 - 1) * 450;
            }
            work->flares[i][0].xform.position.vy -= 350;
            work->flares[i][0].xform.position.vz += (work->head.slot * 2 - 1) * 450;
        }
        for (i = 0; i < work->sphere_count; i++) {
            work->spheres[i].head.duration = 32;
            work->spheres[i].head.active = 0;
            work->spheres[i].head.elapsed = 0;
            work->spheres[i].head.mode = 1;
            work->spheres[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->spheres[i].head.color[0] = gModelVariant632_aConfigs[command].sphere_red;
            work->spheres[i].head.color[1] = gModelVariant632_aConfigs[command].sphere_green;
            work->spheres[i].head.color[2] = gModelVariant632_aConfigs[command].sphere_blue;
            setVector(&work->spheres[i].xform.scale, 0, 0, 0);
            setVector(&work->spheres[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                setVector(&work->spheres[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->spheres[i].xform.position, 0, 0, 0);
                break;
            }
            work->spheres[i].xform.position.vy -= 350;
            work->spheres[i].xform.position.vz += (work->head.slot * 2 - 1) * 450;
        }
        for (i = 0; i < 10; i++) {
            work->strips[i].head.duration = 32;
            work->strips[i].head.active = 0;
            work->strips[i].head.elapsed = 0;
            work->strips[i].head.mode = 1;
            work->strips[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->strips[i].head.color[0] = gModelVariant632_aConfigs[command].red;
            work->strips[i].head.color[1] = gModelVariant632_aConfigs[command].green;
            work->strips[i].head.color[2] = gModelVariant632_aConfigs[command].blue;
            setVector(&work->strips[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->strips[i].growth, 0, 0, 0);
            setVector(&work->strips[i].xform.rotation, 0, 0, 0);
            setVector(&work->strips[i].xform.position, 0, 0, 0);
        }
        work->delay = gModelVariant632_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->square[0], Model632_Negate(work->radius), Model632_Negate(work->radius), 0);
        setVector(&work->square[1], work->radius, Model632_Negate(work->radius), 0);
        setVector(&work->square[2], Model632_Negate(work->radius), work->radius, 0);
        setVector(&work->square[3], work->radius, work->radius, 0);
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                /* `5 * radius * rcos()`: other groupings fold the 5 onto
                 * the radius. */
                setVector(&work->grid[j][i],
                    SquareRoot0(5 * 5 * work->radius * work->radius
                        - 5 * work->radius * rcos(i << 8) / 4096 * (5 * work->radius * rcos(i << 8) / 4096))
                        * rsin(j << 9) / 4096,
                    5 * work->radius * rcos(i << 8) / 4096,
                    SquareRoot0(5 * 5 * work->radius * work->radius
                        - 5 * work->radius * rcos(i << 8) / 4096 * (5 * work->radius * rcos(i << 8) / 4096))
                        * rcos(j << 9) / 4096);
            }
        }
    }
    switch (work->head.command) {
    case 0:
        for (i = 10; i > 0; i--) {
            work->trail[i].first = work->trail[i - 1].first;
            work->trail[i].second = work->trail[i - 1].second;
        }
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 15);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].first, ls.t[0], ls.t[1], ls.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 18);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].second, ls.t[0], ls.t[1], ls.t[2]);
        break;
    case 3:
        for (i = 10; i > 0; i--) {
            work->trail[i].first = work->trail[i - 1].first;
            work->trail[i].second = work->trail[i - 1].second;
        }
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 16);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].first, ls.t[0], ls.t[1], ls.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 17);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].second, ls.t[0], ls.t[1], ls.t[2]);
        break;
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed >= 109 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].head.active = 1;
                }
                work->spheres[0].head.active = 1;
                work->flags[0] = 1;
            }
            if (work->flags[2] < 10) {
                work->strips[work->flags[2]].head.active = 1;
                work->flags[2]++;
            }
            if (work->head.elapsed >= 141) {
                for (i = 0; i < 10; i++) {
                    work->strips[i].head.active = 0;
                }
            }
            break;
        case 1:
            if (work->head.elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].head.active = 1;
                }
                work->flags[0] = 1;
            }
            if (work->head.elapsed >= 29 && work->flags[1] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][1].head.active = 1;
                }
                work->flags[1] = 1;
            }
            break;
        case 2:
            if (work->head.elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].head.active = 1;
                }
                work->spheres[0].head.active = 1;
                work->flags[0] = 1;
            }
            break;
        case 3:
            if (work->head.elapsed >= 51 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].head.active = 1;
                }
                work->spheres[0].head.active = 1;
                work->flags[0] = 1;
            }
            if (work->flags[2] < 10) {
                work->strips[work->flags[2]].head.active = 1;
                work->flags[2]++;
            }
            if (work->head.elapsed >= 181) {
                for (i = 0; i < 10; i++) {
                    work->strips[i].head.active = 0;
                }
            }
            break;
        case 4:
            if (work->head.elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].head.active = 1;
                }
                work->spheres[0].head.active = 1;
                work->flags[0] = 1;
            }
            break;
        }
        inactive = 0;
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        for (i = 0; i < work->flare_count; i++) {
            for (j = 0; j < work->copies; j++) {
                if (work->flares[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->flares[i][j].xform.scale);
                    RotMatrixZ(work->flares[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->flares[i][j].xform.rotation.vx
                        - (*(s16 *)&view.bytes[4] - 0x1000) * (work->head.slot * 2 - 1), &unit.matrix);
                    /* A quarter turn added through a variable, so the rotation loads
                     * before the view yaw. */
                    value = 0xC06;
                    RotMatrixY(work->flares[i][j].xform.rotation.vy + value - *(s16 *)&view.bytes[2], &unit.matrix);
                    unit.matrix.t[0] = work->flares[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->flares[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->flares[i][j].xform.position.vz;
                    unit.super = work->flares[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    /* `* 5 << 1` keeps the 10 on the remaining time: a plain `* 10`
                     * is folded onto the colour instead. */
                    if ((u16)(work->flares[i][j].head.duration / 10) < work->flares[i][j].head.elapsed) {
                        work->flares[i][j].head.color[0] = gModelVariant632_aConfigs[work->head.command].red
                            * ((work->flares[i][j].head.duration - work->flares[i][j].head.elapsed) * 5 << 1)
                            / (work->flares[i][j].head.duration * 9);
                        work->flares[i][j].head.color[1] = gModelVariant632_aConfigs[work->head.command].green
                            * ((work->flares[i][j].head.duration - work->flares[i][j].head.elapsed) * 5 << 1)
                            / (work->flares[i][j].head.duration * 9);
                        work->flares[i][j].head.color[2] = gModelVariant632_aConfigs[work->head.command].blue
                            * ((work->flares[i][j].head.duration - work->flares[i][j].head.elapsed) * 5 << 1)
                            / (work->flares[i][j].head.duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[0];
                    work->flat.clut = work->clut[0];
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->flares[i][j].head.color[0], work->flares[i][j].head.color[1], work->flares[i][j].head.color[2]);
                    setUV4(&work->flat, work->flares[i][j].head.u, work->flares[i][j].head.v,
                        work->flares[i][j].head.u + 31, work->flares[i][j].head.v,
                        work->flares[i][j].head.u, work->flares[i][j].head.v + 31,
                        work->flares[i][j].head.u + 31, work->flares[i][j].head.v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (long *)&work->flat.x0, (long *)&work->flat.x1,
                        (long *)&work->flat.x2, (long *)&work->flat.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->flares[i][j].head.u += 32;
                    if (work->flares[i][j].head.u == 128) {
                        work->flares[i][j].head.u = 0;
                    }
                    work->flares[i][j].head.elapsed += step;
                    if (work->flares[i][j].head.duration < work->flares[i][j].head.elapsed) {
                        work->flares[i][j].head.active = 0;
                        if (work->flags[4] == 0 && j == work->copies - 1) {
                            work->flags[4] = 1;
                        }
                    }
                    if (work->flags[4] == 0) {
                        result = 4;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->sphere_count; i++) {
            if (work->spheres[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->spheres[i].xform.scale);
                RotMatrixY(work->spheres[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->spheres[i].xform.position.vx;
                unit.matrix.t[1] = work->spheres[i].xform.position.vy;
                unit.matrix.t[2] = work->spheres[i].xform.position.vz;
                unit.super = work->spheres[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                /* Member stores rather than setVector, so all three
                 * quotients are computed before the first store. */
                work->spheres[i].xform.scale.vx = (work->spheres[i].head.elapsed << 12) / work->spheres[i].head.duration;
                work->spheres[i].xform.scale.vy = (work->spheres[i].head.elapsed << 12) / work->spheres[i].head.duration;
                work->spheres[i].xform.scale.vz = (work->spheres[i].head.elapsed << 12) / work->spheres[i].head.duration;
                if (work->spheres[i].head.duration / 2 < work->spheres[i].head.elapsed) {
                    work->spheres[i].head.color[0] = gModelVariant632_aConfigs[work->head.command].sphere_red
                        * ((work->spheres[i].head.duration - work->spheres[i].head.elapsed) << 1) / work->spheres[i].head.duration;
                    work->spheres[i].head.color[1] = gModelVariant632_aConfigs[work->head.command].sphere_green
                        * ((work->spheres[i].head.duration - work->spheres[i].head.elapsed) << 1) / work->spheres[i].head.duration;
                    work->spheres[i].head.color[2] = gModelVariant632_aConfigs[work->head.command].sphere_blue
                        * ((work->spheres[i].head.duration - work->spheres[i].head.elapsed) << 1) / work->spheres[i].head.duration;
                }
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 9; k++) {
                        if (((k == 0 || k == 8) && j == 0) || (k != 0 && k != 8)) {
                            work->dot.attribute = 0x50000000;
                            work->dot.r = work->spheres[i].head.color[0];
                            work->dot.g = work->spheres[i].head.color[1];
                            work->dot.b = work->spheres[i].head.color[2];
                            depth = RotTransPers(&work->grid[j][k], (long *)&work->dot.x0, &interpolation, &flag);
                            work->dot.x1 = work->dot.x0;
                            work->dot.y1 = work->dot.y0;
                            if (flag >= 0) {
                                GsSortLine(&work->dot, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                }
                work->spheres[i].head.elapsed += step;
                if (work->spheres[i].head.duration < work->spheres[i].head.elapsed) {
                    work->spheres[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 10; i++) {
            if (work->strips[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->strips[i].xform.scale);
                RotMatrixZ(work->strips[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->strips[i].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->strips[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->strips[i].xform.position.vx;
                unit.matrix.t[1] = work->strips[i].xform.position.vy;
                unit.matrix.t[2] = work->strips[i].xform.position.vz;
                unit.super = work->strips[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyGT4(&work->strip);
                setRGB0(&work->strip, 0, 0, 0);
                setRGB2(&work->strip, 0, 0, 0);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                /* The first angle is held in `value` across the calls, while
                 * the second is written out and partly recomputed. */
                value = (i << 10) / 10;
                setRGB1(&work->strip, work->strips[i].head.color[0] * rcos(value) / 4096,
                    work->strips[i].head.color[1] * rcos(value) / 4096,
                    work->strips[i].head.color[2] * rcos(value) / 4096);
                setRGB3(&work->strip, work->strips[i].head.color[0] * rcos(((i + 1) << 10) / 10) / 4096,
                    work->strips[i].head.color[1] * rcos(((i + 1) << 10) / 10) / 4096,
                    work->strips[i].head.color[2] * rcos(((i + 1) << 10) / 10) / 4096);
                setUV4(&work->strip, work->strips[i].head.u, work->strips[i].head.v + 1,
                    work->strips[i].head.u + 127, work->strips[i].head.v + 1,
                    work->strips[i].head.u, work->strips[i].head.v + 15,
                    work->strips[i].head.u + 127, work->strips[i].head.v + 15);
                setVector(&corners[0],
                    work->trail[i].first.vx
                        + (work->trail[i].second.vx - work->trail[i].first.vx) / 2 * rsin(value) / 4096,
                    work->trail[i].first.vy
                        + (work->trail[i].second.vy - work->trail[i].first.vy) / 2 * rsin(value) / 4096,
                    work->trail[i].first.vz
                        + (work->trail[i].second.vz - work->trail[i].first.vz) / 2 * rsin(value) / 4096);
                setVector(&corners[1],
                    work->trail[i].second.vx
                        + (work->trail[i].first.vx - work->trail[i].second.vx) / 2 * rsin(value) / 4096,
                    work->trail[i].second.vy
                        + (work->trail[i].first.vy - work->trail[i].second.vy) / 2 * rsin(value) / 4096,
                    work->trail[i].second.vz
                        + (work->trail[i].first.vz - work->trail[i].second.vz) / 2 * rsin(value) / 4096);
                setVector(&corners[2],
                    work->trail[i + 1].first.vx
                        + (work->trail[i + 1].second.vx - work->trail[i + 1].first.vx) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096,
                    work->trail[i + 1].first.vy
                        + (work->trail[i + 1].second.vy - work->trail[i + 1].first.vy) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096,
                    work->trail[i + 1].first.vz
                        + (work->trail[i + 1].second.vz - work->trail[i + 1].first.vz) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096);
                setVector(&corners[3],
                    work->trail[i + 1].second.vx
                        + (work->trail[i + 1].first.vx - work->trail[i + 1].second.vx) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096,
                    work->trail[i + 1].second.vy
                        + (work->trail[i + 1].first.vy - work->trail[i + 1].second.vy) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096,
                    work->trail[i + 1].second.vz
                        + (work->trail[i + 1].first.vz - work->trail[i + 1].second.vz) / 2
                            * rsin(((i + 1) << 10) / 10) / 4096);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                    (long *)&work->strip.x0, (long *)&work->strip.x1,
                    (long *)&work->strip.x2, (long *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->strips[i].head.elapsed += step;
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
        if (inactive == work->flare_count * work->copies + work->sphere_count + 10) {
            result = 2;
        }
    }
    return result;
}

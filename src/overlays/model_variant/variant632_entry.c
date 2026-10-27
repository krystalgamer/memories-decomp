#include "../../types.h"
#include "variant632_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

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

s32 func_8013B004(u8 *context, s32 command)
{
    Model632State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model632ViewSnapshot view;
    SVECTOR corners[4];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    u32 tex;
    PSXLONG depth;
    s32 value;

    work = (Model632State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model632State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model632State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CFB0[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CFB0[0]);
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
        work->radius = D_8013CFE8[command].radius;
        work->unknown_0C = D_8013CFE8[command].unknown_04;
        for (i = 0; i < work->flare_count; i++) {
            work->flares[i][0].active = 0;
            work->flares[i][0].elapsed = 0;
            switch (command) {
            case 0:
                work->flares[i][0].duration = 16;
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                work->flares[i][0].duration = 32;
                break;
            }
            work->flares[i][0].mode = 1;
            work->flares[i][0].coord = Model_GetCameraCoordinateUnit();
            work->flares[i][0].red = D_8013CFE8[command].red;
            work->flares[i][0].green = D_8013CFE8[command].green;
            work->flares[i][0].blue = D_8013CFE8[command].blue;
            work->flares[i][0].u = 0;
            work->flares[i][0].v = 16;
            setVector(&work->flares[i][0].scale, 4096, 4096, 4096);
            setVector(&work->flares[i][0].unknown_3C, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                setVector(&work->flares[i][0].rotation, 0, work->slot * 2048, 0);
                setVector(&work->flares[i][0].position,
                    work->radius / 2 * (i % 3 + 1) * rsin(i * 4096 / work->flare_count) / 4096,
                    work->radius / 2 * ((work->flare_count / 2 - i) % 3) * (i % 2 * 2 - 1),
                    work->radius / 2 * (i % 3 + 1) * rcos(i * 4096 / work->flare_count) / 4096 * (i % 2 * 2 - 1));
                break;
            }
            for (j = 1; j < work->copies; j++) {
                work->flares[i][j] = work->flares[i][0];
                if (j / 2 * 2 == j - 1) {
                    s32 swap = work->flares[i][j].position.vx;
                    work->flares[i][j].position.vx = work->flares[i][j].position.vz;
                    work->flares[i][j].position.vz = swap;
                }
                work->flares[i][j].position.vy -= 350;
                work->flares[i][j].position.vz += (work->slot * 2 - 1) * 450;
            }
            work->flares[i][0].position.vy -= 350;
            work->flares[i][0].position.vz += (work->slot * 2 - 1) * 450;
        }
        for (i = 0; i < work->sphere_count; i++) {
            work->spheres[i].duration = 32;
            work->spheres[i].active = 0;
            work->spheres[i].elapsed = 0;
            work->spheres[i].mode = 1;
            work->spheres[i].coord = Model_GetCameraCoordinateUnit();
            work->spheres[i].red = D_8013CFE8[command].sphere_red;
            work->spheres[i].green = D_8013CFE8[command].sphere_green;
            work->spheres[i].blue = D_8013CFE8[command].sphere_blue;
            setVector(&work->spheres[i].scale, 0, 0, 0);
            setVector(&work->spheres[i].unknown_3C, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                setVector(&work->spheres[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->spheres[i].position, 0, 0, 0);
                break;
            }
            work->spheres[i].position.vy -= 350;
            work->spheres[i].position.vz += (work->slot * 2 - 1) * 450;
        }
        for (i = 0; i < 10; i++) {
            work->strips[i].duration = 32;
            work->strips[i].active = 0;
            work->strips[i].elapsed = 0;
            work->strips[i].mode = 1;
            work->strips[i].coord = Model_GetCameraCoordinateUnit();
            work->strips[i].red = D_8013CFE8[command].red;
            work->strips[i].green = D_8013CFE8[command].green;
            work->strips[i].blue = D_8013CFE8[command].blue;
            setVector(&work->strips[i].scale, 4096, 4096, 4096);
            setVector(&work->strips[i].unknown_3C, 0, 0, 0);
            setVector(&work->strips[i].rotation, 0, 0, 0);
            setVector(&work->strips[i].position, 0, 0, 0);
        }
        work->delay = D_8013CFE8[command].delay;
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
    switch (work->command) {
    case 0:
        for (i = 10; i > 0; i--) {
            work->trail[i].first = work->trail[i - 1].first;
            work->trail[i].second = work->trail[i - 1].second;
        }
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 15);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].first, ls.t[0], ls.t[1], ls.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 18);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].second, ls.t[0], ls.t[1], ls.t[2]);
        break;
    case 3:
        for (i = 10; i > 0; i--) {
            work->trail[i].first = work->trail[i - 1].first;
            work->trail[i].second = work->trail[i - 1].second;
        }
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 16);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].first, ls.t[0], ls.t[1], ls.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 17);
        GsGetLwUnit(&unit, &ls);
        setVector(&work->trail[0].second, ls.t[0], ls.t[1], ls.t[2]);
        break;
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed >= 109 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].active = 1;
                }
                work->spheres[0].active = 1;
                work->flags[0] = 1;
            }
            if (work->flags[2] < 10) {
                work->strips[work->flags[2]].active = 1;
                work->flags[2]++;
            }
            if (work->elapsed >= 141) {
                for (i = 0; i < 10; i++) {
                    work->strips[i].active = 0;
                }
            }
            break;
        case 1:
            if (work->elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].active = 1;
                }
                work->flags[0] = 1;
            }
            if (work->elapsed >= 29 && work->flags[1] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][1].active = 1;
                }
                work->flags[1] = 1;
            }
            break;
        case 2:
            if (work->elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].active = 1;
                }
                work->spheres[0].active = 1;
                work->flags[0] = 1;
            }
            break;
        case 3:
            if (work->elapsed >= 51 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].active = 1;
                }
                work->spheres[0].active = 1;
                work->flags[0] = 1;
            }
            if (work->flags[2] < 10) {
                work->strips[work->flags[2]].active = 1;
                work->flags[2]++;
            }
            if (work->elapsed >= 181) {
                for (i = 0; i < 10; i++) {
                    work->strips[i].active = 0;
                }
            }
            break;
        case 4:
            if (work->elapsed == 0 && work->flags[0] == 0) {
                for (i = 0; i < work->flare_count; i++) {
                    work->flares[i][0].active = 1;
                }
                work->spheres[0].active = 1;
                work->flags[0] = 1;
            }
            break;
        }
        inactive = 0;
        PushMatrix();
        view = *(Model632ViewSnapshot *)Model_GetViewMetricsBuffer();
        for (i = 0; i < work->flare_count; i++) {
            for (j = 0; j < work->copies; j++) {
                if (work->flares[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->flares[i][j].scale);
                    RotMatrixZ(work->flares[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->flares[i][j].rotation.vx
                        - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1), &unit.matrix);
                    /* A quarter turn added through a variable, so the rotation loads
                     * before the view yaw. */
                    value = 0xC06;
                    RotMatrixY(work->flares[i][j].rotation.vy + value - *(s16 *)&view.bytes[2], &unit.matrix);
                    unit.matrix.t[0] = work->flares[i][j].position.vx;
                    unit.matrix.t[1] = work->flares[i][j].position.vy;
                    unit.matrix.t[2] = work->flares[i][j].position.vz;
                    unit.super = work->flares[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    /* `* 5 << 1` keeps the 10 on the remaining time: a plain `* 10`
                     * is folded onto the colour instead. */
                    if ((u16)(work->flares[i][j].duration / 10) < work->flares[i][j].elapsed) {
                        work->flares[i][j].red = D_8013CFE8[work->command].red
                            * ((work->flares[i][j].duration - work->flares[i][j].elapsed) * 5 << 1)
                            / (work->flares[i][j].duration * 9);
                        work->flares[i][j].green = D_8013CFE8[work->command].green
                            * ((work->flares[i][j].duration - work->flares[i][j].elapsed) * 5 << 1)
                            / (work->flares[i][j].duration * 9);
                        work->flares[i][j].blue = D_8013CFE8[work->command].blue
                            * ((work->flares[i][j].duration - work->flares[i][j].elapsed) * 5 << 1)
                            / (work->flares[i][j].duration * 9);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[0];
                    work->flat.clut = work->clut[0];
                    setSemiTrans(&work->flat, 1);
                    setRGB0(&work->flat, work->flares[i][j].red, work->flares[i][j].green, work->flares[i][j].blue);
                    setUV4(&work->flat, work->flares[i][j].u, work->flares[i][j].v,
                        work->flares[i][j].u + 31, work->flares[i][j].v,
                        work->flares[i][j].u, work->flares[i][j].v + 31,
                        work->flares[i][j].u + 31, work->flares[i][j].v + 31);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->flares[i][j].u += 32;
                    if (work->flares[i][j].u == 128) {
                        work->flares[i][j].u = 0;
                    }
                    work->flares[i][j].elapsed += step;
                    if (work->flares[i][j].duration < work->flares[i][j].elapsed) {
                        work->flares[i][j].active = 0;
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
            if (work->spheres[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->spheres[i].scale);
                RotMatrixY(work->spheres[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->spheres[i].position.vx;
                unit.matrix.t[1] = work->spheres[i].position.vy;
                unit.matrix.t[2] = work->spheres[i].position.vz;
                unit.super = work->spheres[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                /* Member stores rather than setVector, so all three
                 * quotients are computed before the first store. */
                work->spheres[i].scale.vx = (work->spheres[i].elapsed << 12) / work->spheres[i].duration;
                work->spheres[i].scale.vy = (work->spheres[i].elapsed << 12) / work->spheres[i].duration;
                work->spheres[i].scale.vz = (work->spheres[i].elapsed << 12) / work->spheres[i].duration;
                if (work->spheres[i].duration / 2 < work->spheres[i].elapsed) {
                    work->spheres[i].red = D_8013CFE8[work->command].sphere_red
                        * ((work->spheres[i].duration - work->spheres[i].elapsed) << 1) / work->spheres[i].duration;
                    work->spheres[i].green = D_8013CFE8[work->command].sphere_green
                        * ((work->spheres[i].duration - work->spheres[i].elapsed) << 1) / work->spheres[i].duration;
                    work->spheres[i].blue = D_8013CFE8[work->command].sphere_blue
                        * ((work->spheres[i].duration - work->spheres[i].elapsed) << 1) / work->spheres[i].duration;
                }
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 9; k++) {
                        if (((k == 0 || k == 8) && j == 0) || (k != 0 && k != 8)) {
                            work->dot.attribute = 0x50000000;
                            work->dot.r = work->spheres[i].red;
                            work->dot.g = work->spheres[i].green;
                            work->dot.b = work->spheres[i].blue;
                            depth = RotTransPers(&work->grid[j][k], (PSXLONG *)&work->dot.x0, &interpolation, &flag);
                            work->dot.x1 = work->dot.x0;
                            work->dot.y1 = work->dot.y0;
                            if (flag >= 0) {
                                GsSortLine(&work->dot, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                }
                work->spheres[i].elapsed += step;
                if (work->spheres[i].duration < work->spheres[i].elapsed) {
                    work->spheres[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 10; i++) {
            if (work->strips[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->strips[i].scale);
                RotMatrixZ(work->strips[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->strips[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->strips[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->strips[i].position.vx;
                unit.matrix.t[1] = work->strips[i].position.vy;
                unit.matrix.t[2] = work->strips[i].position.vz;
                unit.super = work->strips[i].coord;
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
                setRGB1(&work->strip, work->strips[i].red * rcos(value) / 4096,
                    work->strips[i].green * rcos(value) / 4096,
                    work->strips[i].blue * rcos(value) / 4096);
                setRGB3(&work->strip, work->strips[i].red * rcos(((i + 1) << 10) / 10) / 4096,
                    work->strips[i].green * rcos(((i + 1) << 10) / 10) / 4096,
                    work->strips[i].blue * rcos(((i + 1) << 10) / 10) / 4096);
                setUV4(&work->strip, work->strips[i].u, work->strips[i].v + 1,
                    work->strips[i].u + 127, work->strips[i].v + 1,
                    work->strips[i].u, work->strips[i].v + 15,
                    work->strips[i].u + 127, work->strips[i].v + 15);
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
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
                work->strips[i].elapsed += step;
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
        if (inactive == work->flare_count * work->copies + work->sphere_count + 10) {
            result = 2;
        }
    }
    return result;
}

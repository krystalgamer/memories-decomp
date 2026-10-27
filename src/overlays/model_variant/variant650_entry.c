#include "../../types.h"
#include "variant650_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 650's entry: spinning quads arc out of a slot position, then a
 * textured sphere and a fan of gouraud triangles open and fade. Command 2
 * opens half of the spheres and fans at once and the rest later. The setup
 * also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model650State *work;
    MATRIX ls;
    MATRIX spin;
    GsCOORDUNIT unit;
    Model650ViewSnapshot view;
    SVECTOR corners[4];
    SVECTOR centre;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, n;
    u32 tex;
    PSXLONG depth;
    s32 d;
    u16 t;

    work = (Model650State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model650State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model650State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB20[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB20[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->quad_count = 8;
            work->sphere_count = 1;
            work->fan_count = 2;
            break;
        case 1:
        case 3:
            work->quad_count = 0;
            work->sphere_count = 1;
            work->fan_count = 2;
            break;
        case 2:
            work->quad_count = 0;
            work->sphere_count = 2;
            work->fan_count = 4;
            break;
        }
        work->radius = D_8013CB58[command].radius;
        work->unknown_10 = D_8013CB58[command].unknown_04;
        for (i = 0; i < work->quad_count; i++) {
            work->quads[i].active = 0;
            work->quads[i].elapsed = 0;
            work->quads[i].duration = 0x40;
            work->quads[i].mode = 1;
            work->quads[i].coord = Model_GetCameraCoordinateUnit();
            work->quads[i].red = D_8013CB58[command].red;
            work->quads[i].green = D_8013CB58[command].green;
            work->quads[i].blue = D_8013CB58[command].blue;
            work->quads[i].u = 0;
            work->quads[i].v = 0x40;
            work->sideways[i].amplitude = (i % 3 + 1) * (i % 2 * 2 - 1);
            work->sideways[i].divisor = i % 3 + i + 5;
            work->vertical[i].amplitude = i % 5 + 1;
            work->vertical[i].divisor = i % 5 + i + 5;
            setVector(&work->quads[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->quads[i].growth, 0, 0, (i % 4 + 1) * 0xA000 / 360 * (i % 2 * 2 - 1));
            setVector(&work->quads[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->quads[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->sphere_count; i++) {
            work->spheres[i].duration = 0x40;
            work->spheres[i].active = 0;
            work->spheres[i].elapsed = 0;
            work->spheres[i].mode = 1;
            work->spheres[i].coord = Model_GetCameraCoordinateUnit();
            work->spheres[i].red = D_8013CB58[command].sphere_red;
            work->spheres[i].green = D_8013CB58[command].sphere_green;
            work->spheres[i].blue = D_8013CB58[command].sphere_blue;
            work->spheres[i].u = 0;
            work->spheres[i].v = 0;
            setVector(&work->spheres[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->spheres[i].growth, 0, 0, 0);
            setVector(&work->spheres[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->spheres[i].position, 0, 0, 0);
            work->spheres[i].position.vy -= 350;
            work->spheres[i].position.vz += work->direction * 450;
        }
        for (i = 0; i < work->fan_count; i++) {
            work->fans[i].duration = 0x40;
            work->fans[i].active = 0;
            work->fans[i].elapsed = 0;
            work->fans[i].mode = 1;
            work->fans[i].coord = Model_GetCameraCoordinateUnit();
            work->fans[i].red = D_8013CB58[command].sphere_red;
            work->fans[i].green = D_8013CB58[command].sphere_green;
            work->fans[i].blue = D_8013CB58[command].sphere_blue;
            work->fans[i].u = 0;
            work->fans[i].v = 0;
            setVector(&work->fans[i].scale, 0, 0, 0);
            setVector(&work->fans[i].growth, 0, 0, 0);
            setVector(&work->fans[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->fans[i].position, 0, 0, 0);
            work->fans[i].position.vy -= 350;
            work->fans[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013CB58[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        d = work->radius >> 1;
        setVector(&work->square[0], -d, -(d * 96) / 64, 0);
        setVector(&work->square[1], d, -(d * 96) / 64, 0);
        setVector(&work->square[2], -d, d * 96 / 64, 0);
        setVector(&work->square[3], d, d * 96 / 64, 0);
        for (i = 0; i < 9; i++) {
            for (k = 0; k < 17; k++) {
                setVector(&work->grid[k][i],
                    SquareRoot0(work->radius * 100 * work->radius
                        - work->radius * 10 * rcos(i << 8) / 4096 * (work->radius * 10 * rcos(i << 8) / 4096))
                        * rsin(k << 8) / 4096,
                    work->radius * 10 * rcos(i << 8) / 4096,
                    SquareRoot0(work->radius * 100 * work->radius
                        - work->radius * 10 * rcos(i << 8) / 4096 * (work->radius * 10 * rcos(i << 8) / 4096))
                        * rcos(k << 8) / 4096);
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], work->radius * 15 * rsin(i << 8) / 4096,
                work->radius * 15 * rcos(i << 8) / 4096, 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 10);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->quad_count; i++) {
                    work->quads[i].active = 1;
                    applyVector(&work->quads[i].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
            }
            break;
        case 1:
        case 3:
            if (work->elapsed == 0) {
                for (i = 0; i < work->sphere_count; i++) {
                    work->spheres[i].active = 1;
                }
                for (i = 0; i < work->fan_count; i++) {
                    work->fans[i].active = 1;
                }
            }
            break;
        case 2:
            if (work->elapsed == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 15);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->sphere_count >> 1; i++) {
                    work->spheres[i].active = 1;
                    setVector(&work->spheres[i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                for (i = 0; i < work->fan_count >> 1; i++) {
                    setVector(&work->fans[i].position, ls.t[0], ls.t[1], ls.t[2]);
                    work->fans[i].active = 1;
                }
            }
            if (work->flags[0] == 0 && work->elapsed >= 0x87) {
                work->flags[0] = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                for (i = work->sphere_count / 2; i < work->sphere_count; i++) {
                    work->spheres[i].active = 1;
                    setVector(&work->spheres[i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                for (i = work->fan_count / 2; i < work->fan_count; i++) {
                    work->fans[i].active = 1;
                    setVector(&work->fans[i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
            }
            break;
        }
        PushMatrix();
        view = *(Model650ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->quad_count; i++) {
            if (work->quads[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->quads[i].scale);
                RotMatrixZ(work->quads[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->quads[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->quads[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->quads[i].position.vx
                    + work->direction
                        * (work->sideways[i].amplitude * work->quads[i].elapsed * 9 / 10
                            * (work->quads[i].elapsed * 9 / 10 - work->quads[i].duration)
                            / work->sideways[i].divisor);
                unit.matrix.t[1] = work->quads[i].position.vy
                    + work->vertical[i].amplitude * work->quads[i].elapsed * 10 / 9
                        * (work->quads[i].elapsed * 10 / 9 - work->quads[i].duration)
                        / work->vertical[i].divisor;
                unit.matrix.t[2] = work->quads[i].position.vz
                    + (work->direction * 450 - work->quads[i].position.vz) * work->quads[i].elapsed
                        / work->quads[i].duration;
                unit.super = work->quads[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setUV4(&work->flat, work->quads[i].u, work->quads[i].v, work->quads[i].u + 63, work->quads[i].v,
                    work->quads[i].u, work->quads[i].v + 96, work->quads[i].u + 63, work->quads[i].v + 96);
                setRGB0(&work->flat, work->quads[i].red, work->quads[i].green, work->quads[i].blue);
                PushMatrix();
                spin = D_800FE168;
                RotMatrixZ(work->quads[i].elapsed * work->quads[i].growth.vz, &spin);
                for (k = 0; k < 4; k++) {
                    ApplyMatrixSV(&spin, &work->square[k], &corners[k]);
                }
                PopMatrix();
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                work->quads[i].elapsed += step;
                if (work->quads[i].duration < work->quads[i].elapsed) {
                    work->quads[i].active = 0;
                    if (i == 0) {
                        for (k = 0; k < work->sphere_count; k++) {
                            work->spheres[k].active = 1;
                        }
                        for (k = 0; k < work->fan_count; k++) {
                            work->fans[k].active = 1;
                        }
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->sphere_count; i++) {
            if (work->spheres[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                d = (work->spheres[i].elapsed << 12) / work->spheres[i].duration + 0x400;
                setVector(&work->spheres[i].scale, d, d, d);
                ScaleMatrix(&unit.matrix, &work->spheres[i].scale);
                RotMatrixZ(work->spheres[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->spheres[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->spheres[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->spheres[i].position.vx;
                unit.matrix.t[1] = work->spheres[i].position.vy;
                unit.matrix.t[2] = work->spheres[i].position.vz;
                unit.super = work->spheres[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                t = work->spheres[i].duration;
                if (t >> 1 < work->spheres[i].elapsed) {
                    d = (t - work->spheres[i].elapsed) * 2;
                    n = t;
                    work->spheres[i].red = D_8013CB58[work->command].sphere_red * d / n;
                    work->spheres[i].green = D_8013CB58[work->command].sphere_green * d / n;
                    work->spheres[i].blue = D_8013CB58[work->command].sphere_blue * d / n;
                }
                for (k = 0; k < 8; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyFT4(&work->flat);
                        work->flat.tpage = work->tpage[1];
                        work->flat.clut = work->clut[1];
                        setSemiTrans(&work->flat, 1);
                        setUV4(&work->flat, work->spheres[i].u, work->spheres[i].v, work->spheres[i].u + 254,
                            work->spheres[i].v, work->spheres[i].u, work->spheres[i].v + 32,
                            work->spheres[i].u + 254, work->spheres[i].v + 32);
                        setRGB0(&work->flat, work->spheres[i].red, work->spheres[i].green, work->spheres[i].blue);
                        depth = RotAverage4(&work->grid[j][k], &work->grid[j + 1][k], &work->grid[j][k + 1],
                            &work->grid[j + 1][k + 1], (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                            (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->spheres[i].v += step * 2;
                if (work->spheres[i].v >= 32) {
                    work->spheres[i].v = 0;
                }
                work->spheres[i].elapsed += step;
                if (work->spheres[i].duration < work->spheres[i].elapsed) {
                    work->spheres[i].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->fan_count; i++) {
            if (work->fans[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                d = (work->fans[i].elapsed << 12) / work->fans[i].duration + 0x400;
                setVector(&work->fans[i].scale, d, d, d);
                ScaleMatrix(&unit.matrix, &work->fans[i].scale);
                RotMatrixZ(work->fans[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->fans[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->fans[i].position.vx;
                unit.matrix.t[1] = work->fans[i].position.vy;
                unit.matrix.t[2] = work->fans[i].position.vz;
                unit.super = work->fans[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                t = work->fans[i].duration;
                if (t >> 1 < work->fans[i].elapsed) {
                    d = (t - work->fans[i].elapsed) * 2;
                    n = t;
                    work->fans[i].red = D_8013CB58[work->command].sphere_red * d / n;
                    work->fans[i].green = D_8013CB58[work->command].sphere_green * d / n;
                    work->fans[i].blue = D_8013CB58[work->command].sphere_blue * d / n;
                }
                setVector(&centre, 0, 0, 0);
                for (k = 0; k < 16; k++) {
                    setPolyG3(&work->fan);
                    setRGB0(&work->fan, work->fans[i].red, work->fans[i].green, work->fans[i].blue);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&centre, &work->ring[k], &work->ring[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->fan, func_80058F10(), (u16)depth,
                            work->fans[i].mode);
                    }
                }
                work->fans[i].elapsed += step;
                if (work->fans[i].duration < work->fans[i].elapsed) {
                    work->fans[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (work->command == 2) {
            if (work->elapsed >= 0x8D && inactive == work->quad_count + work->sphere_count + work->fan_count) {
                result = 1;
                if (work->flags[4] == 1) {
                    result = 2;
                }
                work->flags[4] = 1;
            }
        } else if (inactive == work->quad_count + work->sphere_count + work->fan_count) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

#include "../../types.h"
#include "variant627_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 627's entry: two shaded rings and ten fans of lines that open out
 * of a cone in front of the active slot, each growing and fading over the
 * row's lifetime. The setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model627State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    SVECTOR mid0, mid1, mid;
    PSXLONG interpolation, flag, flag2, xy;
    s32 result, i, step, inactive;
    s32 j;
    s32 near, far, angle;
    s32 low, high;
    PSXLONG depth;

    work = (Model627State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model627State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model627State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        if (command < 7) {
            if (command >= 0) {
                work->ring_count = 2;
                work->line_count = 10;
            }
        }
        work->inner = D_8013C384[command].inner;
        work->outer = D_8013C384[command].outer;
        work->height = D_8013C384[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].duration = D_8013C384[command].duration;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].red = D_8013C384[command].red;
            work->rings[i].green = D_8013C384[command].green;
            work->rings[i].blue = D_8013C384[command].blue;
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 0x1000 - (i << 12) / work->ring_count,
                0x1000 - (i << 12) / work->ring_count, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            if (command < 7) if (command >= 0) {
                setVector(&work->rings[i].position, 0, 50, 0);
                work->rings[i].position.vy -= 350;
                work->rings[i].position.vz += (work->slot * 2 - 1) * 450;
                setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            }
        }
        for (i = 0; i < work->line_count; i++) {
            work->lines[i].active = 0;
            work->lines[i].elapsed = 0;
            work->lines[i].duration = D_8013C384[command].duration;
            work->lines[i].mode = 1;
            work->lines[i].coord = Model_GetCameraCoordinateUnit();
            work->lines[i].red = D_8013C384[command].red;
            work->lines[i].green = D_8013C384[command].green;
            work->lines[i].blue = D_8013C384[command].blue;
            setVector(&work->lines[i].scale, 0x1000 - (i << 12) / work->line_count,
                0x1000 - (i << 12) / work->line_count, 0x1000);
            setVector(&work->lines[i].growth, 0, 0, 0);
            if (command < 7) if (command >= 0) {
                setVector(&work->lines[i].position, 0, 50, 0);
                work->lines[i].position.vy -= 350;
                work->lines[i].position.vz += (work->slot * 2 - 1) * 450;
                setVector(&work->lines[i].rotation, 0, work->slot * 2048, (i << 12) / work->line_count);
            }
        }
        work->delay = D_8013C384[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (work->elapsed == 0) {
                for (i = 0; i < work->ring_count; i++) {
                    work->rings[i].active = 1;
                }
                for (i = 0; i < work->line_count; i++) {
                    work->lines[i].active = 1;
                }
            }
            break;
        }
        PushMatrix();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                    if (D_8013C384[work->command].red / (work->rings[i].duration / 2 - 1) * step < work->rings[i].red) {
                        work->rings[i].red -= D_8013C384[work->command].red / (work->rings[i].duration / 2 - 1) * step;
                    } else {
                        work->rings[i].red = 0;
                    }
                    if (D_8013C384[work->command].green / (work->rings[i].duration / 2 - 1) * step < work->rings[i].green) {
                        work->rings[i].green -= D_8013C384[work->command].green / (work->rings[i].duration / 2 - 1) * step;
                    } else {
                        work->rings[i].green = 0;
                    }
                    if (D_8013C384[work->command].blue / (work->rings[i].duration / 2 - 1) * step < work->rings[i].blue) {
                        work->rings[i].blue -= D_8013C384[work->command].blue / (work->rings[i].duration / 2 - 1) * step;
                    } else {
                        work->rings[i].blue = 0;
                    }
                }
                near = (s16)((work->rings[i].growth.vz * work->outer
                    + (work->height - work->rings[i].growth.vz) * work->inner) * 2 / work->height) / 2;
                far = (s16)(((work->rings[i].growth.vz + work->rings[i].growth.vy) * (work->outer - work->inner)
                    + work->outer * work->height) * 2 / work->height) / 2;
                for (j = 0; j < 17; j++) {
                    angle = j << 8;
                    setVector(&work->rim[j], near * rsin(angle) / 4096, near * rcos(angle) / 4096,
                        work->rings[i].growth.vz + -work->height / 2);
                    setVector(&work->edge[j], far * rsin(angle) / 4096, far * rcos(angle) / 4096,
                        work->rings[i].growth.vy + (work->rings[i].growth.vz + work->height / 2));
                }
                work->rings[i].growth.vz += step * 5;
                work->rings[i].growth.vy += step * 10;
                for (j = 0; j < 16; j++) {
                    mid0.vx = (work->rim[j].vx + work->edge[j].vx) / 2;
                    mid0.vy = (work->rim[j].vy + work->edge[j].vy) / 2;
                    mid0.vz = (work->rim[j].vz + work->edge[j].vz) / 2;
                    mid1.vx = (work->rim[j + 1].vx + work->edge[j + 1].vx) / 2;
                    mid1.vy = (work->rim[j + 1].vy + work->edge[j + 1].vy) / 2;
                    mid1.vz = (work->rim[j + 1].vz + work->edge[j + 1].vz) / 2;
                    setPolyG4(&work->quad);
                    setSemiTrans(&work->quad, 1);
                    setRGB0(&work->quad, 0, 0, 0);
                    setRGB1(&work->quad, 0, 0, 0);
                    setRGB2(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB3(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    depth = RotAverage4(&work->rim[j], &work->rim[j + 1], &mid0, &mid1,
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->rings[i].mode);
                    }
                    setPolyG4(&work->quad);
                    setSemiTrans(&work->quad, 1);
                    setRGB2(&work->quad, 0, 0, 0);
                    setRGB3(&work->quad, 0, 0, 0);
                    setRGB0(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB1(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    depth = RotAverage4(&mid0, &mid1, &work->edge[j], &work->edge[j + 1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->rings[i].mode);
                    }
                }
                work->rings[i].u += 0x40;
                if (work->rings[i].u == 0xC0) {
                    work->rings[i].u = 0;
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->line_count; i++) {
            if (work->lines[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->lines[i].scale);
                RotMatrixZ(work->lines[i].rotation.vz, &unit.matrix);
                RotMatrixY(work->lines[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->lines[i].position.vx;
                unit.matrix.t[1] = work->lines[i].position.vy;
                unit.matrix.t[2] = work->lines[i].position.vz;
                unit.super = work->lines[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->lines[i].duration / 2 < work->lines[i].elapsed) {
                    if (D_8013C384[work->command].red / (work->lines[i].duration / 2 - 1) * step < work->lines[i].red) {
                        work->lines[i].red -= D_8013C384[work->command].red / (work->lines[i].duration / 2 - 1) * step;
                    } else {
                        work->lines[i].red = 0;
                    }
                    if (D_8013C384[work->command].green / (work->lines[i].duration / 2 - 1) * step < work->lines[i].green) {
                        work->lines[i].green -= D_8013C384[work->command].green / (work->lines[i].duration / 2 - 1) * step;
                    } else {
                        work->lines[i].green = 0;
                    }
                    if (D_8013C384[work->command].blue / (work->lines[i].duration / 2 - 1) * step < work->lines[i].blue) {
                        work->lines[i].blue -= D_8013C384[work->command].blue / (work->lines[i].duration / 2 - 1) * step;
                    } else {
                        work->lines[i].blue = 0;
                    }
                }
                low = (s16)((work->lines[i].growth.vz * work->outer
                    + (work->height - work->lines[i].growth.vz) * work->inner) * 2 / work->height) / 2;
                high = (s16)(((work->lines[i].growth.vz + work->lines[i].growth.vy) * (work->outer - work->inner)
                    + work->outer * work->height) * 2 / work->height) / 2;
                for (j = 0; j < 32; j++) {
                    angle = j << 7;
                    setVector(&work->spokes[j][0], low * rsin(angle) / 4096, low * rcos(angle) / 4096,
                        work->lines[i].growth.vz + -work->height / 2);
                    setVector(&work->spokes[j][1], high * rsin(angle) / 4096, high * rcos(angle) / 4096,
                        work->lines[i].growth.vy + (work->lines[i].growth.vz + work->height / 2));
                }
                work->lines[i].growth.vz += (i + 10) * step;
                work->lines[i].growth.vy += (i + 25) * step;
                for (j = 0; j < 32; j++) {
                    work->line.attribute = 0x50000000;
                    work->line.r0 = 0;
                    work->line.g0 = 0;
                    work->line.b0 = 0;
                    work->line.r1 = work->lines[i].red;
                    work->line.g1 = work->lines[i].green;
                    work->line.b1 = work->lines[i].blue;
                    RotTransPers(&work->spokes[j][0], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                    RotTransPers(&work->spokes[j][1], (PSXLONG *)&work->line.x1, &interpolation, &flag2);
                    mid.vx = (work->spokes[j][0].vx + work->spokes[j][1].vx) / 2;
                    mid.vy = (work->spokes[j][0].vy + work->spokes[j][1].vy) / 2;
                    mid.vz = (work->spokes[j][0].vz + work->spokes[j][1].vz) / 2;
                    depth = RotTransPers(&mid, &xy, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                    }
                }
                work->lines[i].elapsed += step;
                if (work->lines[i].duration < work->lines[i].elapsed) {
                    work->lines[i].active = 0;
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
        if (work->elapsed == 0) {
            result = 4;
            work->flags[4] = 1;
        }
        work->elapsed += step;
        if (inactive == work->ring_count + work->line_count) {
            result = 2;
        }
    }
    return result;
}

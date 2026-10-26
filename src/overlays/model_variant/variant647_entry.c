#include "../../types.h"
#include "variant647_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 647's entry: one or two bursts of rings, each set up from a command
 * row; every burst grows from the camera unit for 64 frames and
 * fades out in its second half. The setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model647State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model647ViewSnapshot view;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, fade;
    s32 angle, turn;
    s32 size;
    u32 tex;
    u16 duration;
    PSXLONG depth;

    work = (Model647State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model647State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model647State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013BF5C);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013BF5C);
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
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 16:
        case 17:
        case 18:
            work->count = 1;
            break;
        case 14:
        case 15:
            work->count = 2;
            break;
        }
        work->radius = D_8013BF78[command].radius;
        work->height = D_8013BF78[command].height;
        for (i = 0; i < work->count; i++) {
            work->bursts[i].duration = 64;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].unknown_4C = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].red = D_8013BF78[command].red;
            work->bursts[i].green = D_8013BF78[command].green;
            work->bursts[i].blue = D_8013BF78[command].blue;
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 0, 0, 0);
            setVector(&work->bursts[i].unknown_80, 0, 0, 0);
            switch (command) {
            case 9:
            case 10:
                setVector(&work->bursts[i].rotation, 0x200, work->slot * 2048, 0);
                break;
            case 15:
            case 18:
                setVector(&work->bursts[i].rotation, -0x400, work->slot * 2048, 0);
                break;
            default:
                setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
                break;
            }
            switch (command) {
            case 0:
            case 5:
            case 6:
            case 12:
                setVector(&work->bursts[i].position, 0, 50, 0);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 7:
            case 8:
            case 11:
            case 13:
            case 16:
            case 17:
                setVector(&work->bursts[i].position, 0, 0, 0);
                break;
            case 9:
                setVector(&work->bursts[i].position, 0, -50, -(work->direction * 200));
                break;
            case 10:
                setVector(&work->bursts[i].position, 0, -50, -(work->direction * 150));
                break;
            case 14:
                setVector(&work->bursts[i].position, i * 150 - 75, 50, 0);
                break;
            case 15:
                setVector(&work->bursts[i].position, 0, 100, -(work->direction * 70));
                break;
            case 18:
                setVector(&work->bursts[i].position, 0, 200, 0);
                break;
            }
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013BF78[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 17; j++) {
                angle = (i << 7) + (j << 8);
                setVector(&work->inner[j][i], work->radius * (i * 2 + 2) / 4 * rsin(angle) / 4096,
                    work->radius * (i * 2 + 2) / 4 * rcos(angle) / 4096, 0);
                setVector(&work->outer[j][i], work->radius * (i * 6 + 6) / 4 * rsin(angle) / 4096,
                    work->radius * (i * 6 + 6) / 4 * rcos(angle) / 4096, work->height * (i - 4) / 4);
            }
        }
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 17; j++) {
                turn = (i << 7) + (j << 8);
                setVector(&work->top[j][i], work->radius * (i * 2 + 2) / 8 * rsin(turn) / 4096,
                    work->radius * (i * 2 + 2) / 8 * rcos(turn) / 4096, 0);
                turn = ((i + 1) << 7) + (j << 8);
                setVector(&work->bottom[j][i], work->radius * ((i + 1) << 3) / 8 * rsin(turn) / 4096,
                    work->radius * ((i + 1) << 3) / 8 * rcos(turn) / 4096,
                    work->height * -((8 - i) * 2) / 8 - work->height);
            }
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
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 16:
        case 17:
        case 18:
            if (work->elapsed == 0) {
                for (i = 0; i < work->count; i++) {
                    work->bursts[i].active = 1;
                }
                result = 1;
            }
            break;
        case 15:
            if (work->elapsed == 0) {
                work->bursts[0].active = 1;
                result = 4;
            }
            if (work->flags[0] == 0 && work->elapsed >= 41) {
                result = work->flags[0] = work->bursts[1].active = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model647ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                size = work->bursts[i].elapsed * 0x6000 / work->bursts[i].duration;
                setVector(&work->bursts[i].scale, size, size, size);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixX(work->bursts[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                duration = work->bursts[i].duration;
                if (duration / 2 < work->bursts[i].elapsed) {
                    fade = (duration - work->bursts[i].elapsed) * 2;
                    work->bursts[i].red = D_8013BF78[work->command].red * fade / duration;
                    work->bursts[i].green = D_8013BF78[work->command].green * fade / duration;
                    work->bursts[i].blue = D_8013BF78[work->command].blue * fade / duration;
                }
                for (k = 0; k < 4; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->quad);
                        work->quad.tpage = work->tpage[0];
                        work->quad.clut = work->clut[0];
                        setSemiTrans(&work->quad, 1);
                        setUV4(&work->quad, work->bursts[i].u, work->bursts[i].v, work->bursts[i].u + 31, work->bursts[i].v,
                            work->bursts[i].u, work->bursts[i].v + 127, work->bursts[i].u + 31, work->bursts[i].v + 127);
                        setRGB0(&work->quad, 0, 0, 0);
                        setRGB1(&work->quad, 0, 0, 0);
                        setRGB2(&work->quad, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                        setRGB3(&work->quad, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                        depth = RotAverage4(&work->inner[j][k], &work->inner[j + 1][k],
                            &work->outer[j][k], &work->outer[j + 1][k],
                            (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                            (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                        }
                    }
                }
                for (k = 0; k < 8; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT3(&work->triangle);
                        work->triangle.tpage = work->tpage[0];
                        work->triangle.clut = work->clut[0];
                        setSemiTrans(&work->triangle, 1);
                        setUV3(&work->triangle, work->bursts[i].u, work->bursts[i].v, work->bursts[i].u + 31, work->bursts[i].v,
                            work->bursts[i].u + 16, work->bursts[i].v + 127);
                        setRGB2(&work->triangle, 0, 0, 0);
                        setRGB0(&work->triangle, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                        setRGB1(&work->triangle, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                        depth = RotAverage3(&work->top[j][k], &work->top[j + 1][k],
                            &work->bottom[j][k],
                            (PSXLONG *)&work->triangle.x0, (PSXLONG *)&work->triangle.x1,
                            (PSXLONG *)&work->triangle.x2, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->triangle, func_80058F10(), (u16)depth);
                        }
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
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->count) {
            result = 2;
        }
    }
    return result;
}

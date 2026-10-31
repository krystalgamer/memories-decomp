#include "../../types.h"
#include "variant650_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 650's entry: up to eight spinning sprites thrown along arcs from a
 * model joint, then a scrolling textured sphere and a Gouraud-triangle disc
 * that grow at a joint and fade. The setup also prints the work-area size. */

/* The work-size format string; North American retail pads it with zero bytes. */
#ifndef VERSION_FRENCH
#ifndef MODEL650_STRING_PAD
#define MODEL650_STRING_PAD ""
#endif
static const char D_8013CB08[24] = "E_DATA size = %x byte\n\0" MODEL650_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model650State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model650ViewSnapshot view;
    SVECTOR points[4];
    SVECTOR origin;
    PSXLONG interpolation, flag;
    s32 result, i, j, k, step, inactive;
    s32 value, duration;
    u32 tex;
    PSXLONG depth;

    work = (Model650State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model650State));
#ifndef VERSION_FRENCH
        printf(D_8013CB08, sizeof(Model650State));
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
            work->sprite_count = 8;
            work->shell_count = 1;
            work->fan_count = 2;
            break;
        case 1:
        case 3:
            work->shell_count = 1;
            work->sprite_count = 0;
            work->fan_count = 2;
            break;
        case 2:
            work->sprite_count = 0;
            work->shell_count = 2;
            work->fan_count = 4;
            break;
        }
        work->radius = D_8013CB58[command].radius;
        work->height = D_8013CB58[command].height;
        for (i = 0; i < work->sprite_count; i++) {
            work->sprites[i].active = 0;
            work->sprites[i].elapsed = 0;
            work->sprites[i].duration = 64;
            work->sprites[i].mode = 1;
            work->sprites[i].coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].color[0] = D_8013CB58[command].color[0];
            work->sprites[i].color[1] = D_8013CB58[command].color[1];
            work->sprites[i].color[2] = D_8013CB58[command].color[2];
            work->sprites[i].u = 0;
            work->sprites[i].v = 64;
            work->arc_x[i][0] = (i % 3 + 1) * ((i % 2) * 2 - 1);
            work->arc_x[i][1] = i % 3 + i + 5;
            work->arc_y[i][0] = i % 5 + 1;
            work->arc_y[i][1] = i % 5 + i + 5;
            setVector(&work->sprites[i].scale, 4096, 4096, 4096);
            setVector(&work->sprites[i].spin, 0, 0, (i % 4 + 1) * 40960 / 360 * ((i % 2) * 2 - 1));
            setVector(&work->sprites[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sprites[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->shell_count; i++) {
            work->shells[i].duration = 64;
            work->shells[i].active = 0;
            work->shells[i].elapsed = 0;
            work->shells[i].mode = 1;
            work->shells[i].coord = Model_GetCameraCoordinateUnit();
            work->shells[i].color[0] = D_8013CB58[command].color[3];
            work->shells[i].color[1] = D_8013CB58[command].color[4];
            work->shells[i].color[2] = D_8013CB58[command].color[5];
            work->shells[i].u = 0;
            work->shells[i].v = 0;
            setVector(&work->shells[i].scale, 4096, 4096, 4096);
            setVector(&work->shells[i].spin, 0, 0, 0);
            setVector(&work->shells[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->shells[i].position, 0, 0, 0);
            work->shells[i].position.vy -= 350;
            work->shells[i].position.vz += work->direction * 450;
        }
        for (i = 0; i < work->fan_count; i++) {
            work->fans[i].duration = 64;
            work->fans[i].active = 0;
            work->fans[i].elapsed = 0;
            work->fans[i].mode = 1;
            work->fans[i].coord = Model_GetCameraCoordinateUnit();
            work->fans[i].color[0] = D_8013CB58[command].color[3];
            work->fans[i].color[1] = D_8013CB58[command].color[4];
            work->fans[i].color[2] = D_8013CB58[command].color[5];
            work->fans[i].u = 0;
            work->fans[i].v = 0;
            setVector(&work->fans[i].scale, 0, 0, 0);
            setVector(&work->fans[i].spin, 0, 0, 0);
            setVector(&work->fans[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->fans[i].position, 0, 0, 0);
            work->fans[i].position.vy -= 350;
            work->fans[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013CB58[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius / 2;
        setVector(&work->corners[0], -value, -(value * 96) / 64, 0);
        setVector(&work->corners[1], value, -(value * 96) / 64, 0);
        setVector(&work->corners[2], -value, value * 96 / 64, 0);
        setVector(&work->corners[3], value, value * 96 / 64, 0);
        /* radius * -(c * -10) keeps retail's radius * (c * 10): written as
         * radius * (c * 10), the front end reassociates it to (radius * 10) * c. */
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->shell[j][i],
                    SquareRoot0(work->radius * 10 * (work->radius * 10)
                        - work->radius * -(rcos(i << 8) * -10) / 4096 * (work->radius * -(rcos(i << 8) * -10) / 4096))
                        * rsin(j << 8) / 4096,
                    work->radius * -(rcos(i << 8) * -10) / 4096,
                    SquareRoot0(work->radius * 10 * (work->radius * 10)
                        - work->radius * -(rcos(i << 8) * -10) / 4096 * (work->radius * -(rcos(i << 8) * -10) / 4096))
                        * rcos(j << 8) / 4096);
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->circle[i], work->radius * -(rsin(i << 8) * -15) / 4096,
                work->radius * -(rcos(i << 8) * -15) / 4096, 0);
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
                for (i = 0; i < work->sprite_count; i++) {
                    work->sprites[i].active = 1;
                    applyVector(&work->sprites[i].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
            }
            break;
        case 1:
        case 3:
            if (work->elapsed == 0) {
                for (i = 0; i < work->shell_count; i++) {
                    work->shells[i].active = 1;
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
                for (i = 0; i < work->shell_count / 2; i++) {
                    work->shells[i].active = 1;
                    setVector(&work->shells[i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                for (i = 0; i < work->fan_count / 2; i++) {
                    setVector(&work->fans[i].position, ls.t[0], ls.t[1], ls.t[2]);
                    work->fans[i].active = 1;
                }
            }
            if (work->flags[0] == 0 && work->elapsed >= 135) {
                work->flags[0] = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                for (i = work->shell_count / 2; i < work->shell_count; i++) {
                    work->shells[i].active = 1;
                    setVector(&work->shells[i].position, ls.t[0], ls.t[1], ls.t[2]);
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
        for (i = 0; i < work->sprite_count; i++) {
            if (work->sprites[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sprites[i].scale);
                RotMatrixZ(work->sprites[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sprites[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->sprites[i].position.vx + work->direction
                    * (work->arc_x[i][0] * work->sprites[i].elapsed * 9 / 10
                        * (work->sprites[i].elapsed * 9 / 10 - work->sprites[i].duration) / work->arc_x[i][1]);
                unit.matrix.t[1] = work->sprites[i].position.vy
                    + work->arc_y[i][0] * work->sprites[i].elapsed * 10 / 9
                        * (work->sprites[i].elapsed * 10 / 9 - work->sprites[i].duration) / work->arc_y[i][1];
                unit.matrix.t[2] = work->sprites[i].position.vz
                    + (work->direction * 450 - work->sprites[i].position.vz) * work->sprites[i].elapsed
                        / work->sprites[i].duration;
                unit.super = work->sprites[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setUV4(&work->sprite, work->sprites[i].u, work->sprites[i].v,
                    work->sprites[i].u + 63, work->sprites[i].v,
                    work->sprites[i].u, work->sprites[i].v + 96,
                    work->sprites[i].u + 63, work->sprites[i].v + 96);
                setRGB0(&work->sprite, work->sprites[i].color[0], work->sprites[i].color[1], work->sprites[i].color[2]);
                PushMatrix();
                m = D_800FE168;
                RotMatrixZ(work->sprites[i].elapsed * work->sprites[i].spin.vz, &m);
                for (j = 0; j < 4; j++) {
                    ApplyMatrixSV(&m, &work->corners[j], &points[j]);
                }
                PopMatrix();
                depth = RotAverage4(&points[0], &points[1], &points[2], &points[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->sprites[i].elapsed += step;
                if (work->sprites[i].duration < work->sprites[i].elapsed) {
                    work->sprites[i].active = 0;
                    if (i == 0) {
                        for (j = 0; j < work->shell_count; j++) {
                            work->shells[j].active = 1;
                        }
                        for (j = 0; j < work->fan_count; j++) {
                            work->fans[j].active = 1;
                        }
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->shell_count; i++) {
            if (work->shells[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->shells[i].elapsed << 12) / work->shells[i].duration + 0x400;
                setVector(&work->shells[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->shells[i].scale);
                RotMatrixZ(work->shells[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->shells[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->shells[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->shells[i].position.vx;
                unit.matrix.t[1] = work->shells[i].position.vy;
                unit.matrix.t[2] = work->shells[i].position.vz;
                unit.super = work->shells[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->shells[i].duration / 2 < work->shells[i].elapsed) {
                    value = (work->shells[i].duration - work->shells[i].elapsed) * 2;
                    duration = work->shells[i].duration;
                    work->shells[i].color[0] = D_8013CB58[work->command].color[3] * value / duration;
                    work->shells[i].color[1] = D_8013CB58[work->command].color[4] * value / duration;
                    work->shells[i].color[2] = D_8013CB58[work->command].color[5] * value / duration;
                }
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 16; k++) {
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[1];
                        work->sprite.clut = work->clut[1];
                        setSemiTrans(&work->sprite, 1);
                        setUV4(&work->sprite, work->shells[i].u, work->shells[i].v,
                            work->shells[i].u + 254, work->shells[i].v,
                            work->shells[i].u, work->shells[i].v + 32,
                            work->shells[i].u + 254, work->shells[i].v + 32);
                        setRGB0(&work->sprite, work->shells[i].color[0], work->shells[i].color[1], work->shells[i].color[2]);
                        depth = RotAverage4(&work->shell[k][j], &work->shell[k + 1][j],
                            &work->shell[k][j + 1], &work->shell[k + 1][j + 1],
                            (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->shells[i].v += step * 2;
                if (work->shells[i].v >= 32) {
                    work->shells[i].v = 0;
                }
                work->shells[i].elapsed += step;
                if (work->shells[i].duration < work->shells[i].elapsed) {
                    work->shells[i].active = 0;
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
                value = (work->fans[i].elapsed << 12) / work->fans[i].duration + 0x400;
                setVector(&work->fans[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->fans[i].scale);
                RotMatrixZ(work->fans[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->fans[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->fans[i].position.vx;
                unit.matrix.t[1] = work->fans[i].position.vy;
                unit.matrix.t[2] = work->fans[i].position.vz;
                unit.super = work->fans[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->fans[i].duration / 2 < work->fans[i].elapsed) {
                    value = (work->fans[i].duration - work->fans[i].elapsed) * 2;
                    duration = work->fans[i].duration;
                    work->fans[i].color[0] = D_8013CB58[work->command].color[3] * value / duration;
                    work->fans[i].color[1] = D_8013CB58[work->command].color[4] * value / duration;
                    work->fans[i].color[2] = D_8013CB58[work->command].color[5] * value / duration;
                }
                setVector(&origin, 0, 0, 0);
                for (j = 0; j < 16; j++) {
                    setPolyG3(&work->fan);
                    setRGB0(&work->fan, work->fans[i].color[0], work->fans[i].color[1], work->fans[i].color[2]);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&origin, &work->circle[j], &work->circle[j + 1],
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
            if (work->elapsed > 140 && inactive == work->sprite_count + work->shell_count + work->fan_count) {
                result = 1;
                if (work->flags[4] == 1) {
                    result = 2;
                }
                work->flags[4] = 1;
            }
        } else if (inactive == work->sprite_count + work->shell_count + work->fan_count) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

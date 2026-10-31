#include "../../types.h"
#include "variant652_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 652's entry: rings of fanned triangles pulse out of a point
 * between two model joints, then up to four shaded domes grow round the
 * effect origin, shed rising sparks row by row and end in a screen flash.
 * The setup also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x9A. */
#ifndef VERSION_FRENCH
#ifndef MODEL652_STRING_PAD
#define MODEL652_STRING_PAD "\x9A"
#endif
static const char D_8013D780[24] = "E_DATA size = %x byte\n\0" MODEL652_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model652State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model652ViewSnapshot view;
    SVECTOR origin;
    SVECTOR points[4];
    VECTOR center;
    VECTOR scale;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, duration, n;
    u32 tex;
    PSXLONG depth;

    work = (Model652State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model652State));
#ifndef VERSION_FRENCH
        printf(D_8013D780, sizeof(Model652State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->cursor = 0;
        work->other_slot = work->slot == 0;
        Model_CopySlotPosition(work->other_slot, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D798[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D798[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D798[2]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
            work->ring_count = 64;
            work->dome_count = 4;
            work->spark_count = 3;
            break;
        }
        work->radius = D_8013D7EC[command].radius;
        work->height = D_8013D7EC[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].duration = 128;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013D7EC[command].color[0];
            work->rings[i].color[1] = D_8013D7EC[command].color[1];
            work->rings[i].color[2] = D_8013D7EC[command].color[2];
            work->rings[i].u = 0;
            work->rings[i].v = 32;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].spin, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i].duration = 128;
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            work->domes[i].mode = 1;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].color[0] = D_8013D7EC[command].color[3];
            work->domes[i].color[1] = D_8013D7EC[command].color[4];
            work->domes[i].color[2] = D_8013D7EC[command].color[5];
            work->domes[i].u = 0;
            work->domes[i].v = 160;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].spin, 8, 0, 0);
            setVector(&work->domes[i].rotation, 0, (i << 12) / work->dome_count + work->slot * 2048, 0);
            if (i == 0) {
                setVector(&work->domes[i].position, 0, 0, 0);
            } else {
                setVector(&work->domes[i].position,
                    work->radius * (rsin(work->slot * 2048 + (i - 1) * 4096 / (work->dome_count - 1)) << 1) / 4096,
                    0,
                    work->radius * (rcos(work->slot * 2048 + (i - 1) * 4096 / (work->dome_count - 1)) << 1) / 4096);
            }
            applyVector(&work->domes[i].position, work->origin.vx, 0, work->origin.vz, +=);
        }
        for (j = 0; j < work->dome_count; j++) {
            for (k = 0; k < 8; k++) {
                for (i = 0; i < work->spark_count; i++) {
                    work->sparks[i][k][j].duration = 32;
                    work->sparks[i][k][j].active = 0;
                    work->sparks[i][k][j].elapsed = 0;
                    work->sparks[i][k][j].mode = 1;
                    work->sparks[i][k][j].coord = Model_GetCameraCoordinateUnit();
                    work->sparks[i][k][j].color[0] = D_8013D7EC[command].color[0];
                    work->sparks[i][k][j].color[1] = D_8013D7EC[command].color[1];
                    work->sparks[i][k][j].color[2] = D_8013D7EC[command].color[2];
                    work->sparks[i][k][j].u = 0;
                    work->sparks[i][k][j].v = 0;
                    setVector(&work->sparks[i][k][j].scale,
                        work->radius * (rsin((4096 / work->spark_count >> 1) + (k << 9) + (i << 12) / work->spark_count) << 1) / 4096,
                        0,
                        work->radius * (rcos((4096 / work->spark_count >> 1) + (k << 9) + (i << 12) / work->spark_count) << 1) / 4096);
                    setVector(&work->sparks[i][k][j].spin,
                        work->radius * (rsin((k << 9) + (i << 12) / work->spark_count) << 1) / 4096,
                        0,
                        work->radius * (rcos((k << 9) + (i << 12) / work->spark_count) << 1) / 4096);
                    setVector(&work->sparks[i][k][j].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->sparks[i][k][j].position, 0,
                        work->height * ~k / 8 + work->height / 16, 0);
                    applyVector(&work->sparks[i][k][j].position, work->domes[j].position.vx,
                        work->domes[j].position.vy, work->domes[j].position.vz, +=);
                }
            }
        }
        work->flash.duration = 64;
        work->flash.active = 0;
        work->flash.elapsed = 0;
        work->flash.mode = 1;
        work->flash.color[0] = D_8013D7EC[command].color[0];
        work->flash.color[1] = D_8013D7EC[command].color[1];
        work->flash.color[2] = D_8013D7EC[command].color[2];
        work->delay = D_8013D7EC[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->rim[i][0], work->radius * rsin(i << 8) / 4096, work->radius * rcos(i << 8) / 4096, 0);
            work->rim_uv[i][0] = rsin(i << 8) * 63 / 4096 + 64;
            work->rim_uv[i][1] = rcos(i << 8) * 63 / 4096 + 64;
        }
        for (k = 0; k < 9; k++) {
            for (i = 0; i < 17; i++) {
                setVector(&work->dome[i][k],
                    (work->radius + ((k + i) % 2 * 2 - 1) * work->radius / 5 * ((k + i) % 4 + 1)) * (8 - k) / 8 * rsin(i << 8) / 4096,
                    work->height * -k / 8,
                    (work->radius + ((k + i) % 2 * 2 - 1) * work->radius / 5 * ((k + i) % 4 + 1)) * (8 - k) / 8 * rcos(i << 8) / 4096);
                for (j = 0; j < 3; j++) {
                    switch (j) {
                    case 0:
                        value = D_8013D7EC[command].color[3] + ((k + i) % 2 * 2 - 1) * D_8013D7EC[command].color[3] * ((k + i) % 3 + 1);
                        break;
                    case 1:
                        value = D_8013D7EC[command].color[4] + ((k + i) % 2 * 2 - 1) * D_8013D7EC[command].color[4] * ((k + i) % 3 + 1);
                        break;
                    case 2:
                        value = D_8013D7EC[command].color[5] + ((k + i) % 2 * 2 - 1) * D_8013D7EC[command].color[5] * ((k + i) % 3 + 1);
                        break;
                    default:
                        value = 0;
                        break;
                    }
                    if (value > 255) {
                        value = 255;
                    }
                    if (value < 0) {
                        value = 0;
                    }
                    work->shade[j][i][k] = value;
                }
            }
            work->dome[16][k] = work->dome[0][k];
            for (j = 0; j < 3; j++) {
                work->shade[j][16][k] = work->shade[j][0][k];
            }
        }
        setVector(&work->corners[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->corners[3], work->radius / 2, work->radius / 2, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed < 280) {
            if ((work->cursor & 3) == 0) {
                work->rings[work->cursor / 4].active = 1;
                work->rings[work->cursor / 4].elapsed = 0;
                work->rings[work->cursor / 4].color[0] = D_8013D7EC[work->command].color[0];
                work->rings[work->cursor / 4].color[1] = D_8013D7EC[work->command].color[1];
                work->rings[work->cursor / 4].color[2] = D_8013D7EC[work->command].color[2];
            }
            work->cursor++;
            if (work->cursor >= work->ring_count * 4) {
                work->cursor = 0;
            }
        } else if (work->flags[0] == 0) {
            for (i = 0; i < work->ring_count; i++) {
                work->rings[i].active = 0;
            }
            for (i = 0; i < work->dome_count; i++) {
                work->domes[i].active = 1;
            }
            work->flash.active = 1;
            work->flags[0] = 1;
        }
        break;
    case 1:
        if (work->elapsed < 150) {
            if ((work->cursor & 3) == 0) {
                work->rings[work->cursor / 4].active = 1;
                work->rings[work->cursor / 4].elapsed = 0;
                work->rings[work->cursor / 4].color[0] = D_8013D7EC[work->command].color[0];
                work->rings[work->cursor / 4].color[1] = D_8013D7EC[work->command].color[1];
                work->rings[work->cursor / 4].color[2] = D_8013D7EC[work->command].color[2];
            }
            work->cursor++;
            if (work->cursor >= work->ring_count * 4) {
                work->cursor = 0;
            }
        } else if (work->flags[0] == 0) {
            for (i = 0; i < work->ring_count; i++) {
                work->rings[i].active = 0;
            }
            for (i = 0; i < work->dome_count; i++) {
                work->domes[i].active = 1;
            }
            work->flash.active = 1;
            work->flags[0] = 1;
        }
        break;
    }
    PushMatrix();
    view = *(Model652ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    switch (work->command) {
    case 0:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 6);
        GsGetLwUnit(&unit, &ls);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 13);
        GsGetLwUnit(&unit, &m);
        center.vx = (ls.t[0] + m.t[0]) / 2;
        center.vy = (ls.t[1] + m.t[1]) / 2;
        center.vz = (ls.t[2] + m.t[2]) / 2;
        break;
    case 1:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 19);
        GsGetLwUnit(&unit, &ls);
        center.vx = ls.t[0];
        center.vy = ls.t[1];
        center.vz = ls.t[2];
        break;
    }
    for (i = 0; i < work->ring_count; i++) {
        if (work->rings[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = ((work->rings[i].duration - work->rings[i].elapsed) << 14) / work->rings[i].duration;
            setVector(&work->rings[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->rings[i].scale);
            RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
            RotMatrixY(work->rings[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->rings[i].position.vx + center.vx;
            unit.matrix.t[1] = work->rings[i].position.vy + center.vy;
            unit.matrix.t[2] = work->rings[i].position.vz + center.vz;
            unit.super = work->rings[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->rings[i].elapsed < work->rings[i].duration) {
                value = work->rings[i].elapsed;
                duration = work->rings[i].duration;
                work->rings[i].color[0] = D_8013D7EC[work->command].color[0] * value / duration;
                work->rings[i].color[1] = D_8013D7EC[work->command].color[1] * value / duration;
                work->rings[i].color[2] = D_8013D7EC[work->command].color[2] * value / duration;
            }
            setVector(&origin, 0, 0, 0);
            m = D_800FE168;
            RotMatrixZ(work->rings[i].elapsed * 10, &m);
            for (k = 0; k < 17; k++) {
                PushMatrix();
                ApplyMatrixSV(&m, &work->rim[k][0], &work->rim[k][1]);
                PopMatrix();
            }
            for (k = 0; k < 16; k++) {
                setPolyGT3(&work->fan);
                work->fan.tpage = work->tpage[0];
                work->fan.clut = work->clut[0];
                setSemiTrans(&work->fan, 1);
                work->fan.u0 = work->rings[i].u + 64;
                work->fan.v0 = work->rings[i].v + 64;
                work->fan.u1 = work->rings[i].u + work->rim_uv[k][0];
                work->fan.v1 = work->rings[i].v + work->rim_uv[k][1];
                work->fan.u2 = work->rings[i].u + work->rim_uv[k + 1][0];
                work->fan.v2 = work->rings[i].v + work->rim_uv[k + 1][1];
                setRGB0(&work->fan, 0, 0, 0);
                setRGB1(&work->fan, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                setRGB2(&work->fan, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                depth = RotAverage3(&origin, &work->rim[k][1], &work->rim[k + 1][1],
                    (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
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
    for (i = 0; i < work->dome_count; i++) {
        if (work->domes[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->domes[i].elapsed < work->domes[i].duration / 8) {
                value = (work->domes[i].elapsed << 15) / work->domes[i].duration;
            } else {
                value = 4096;
            }
            setVector(&work->domes[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->domes[i].scale);
            RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->domes[i].position.vx;
            unit.matrix.t[1] = work->domes[i].position.vy;
            unit.matrix.t[2] = work->domes[i].position.vz;
            unit.super = work->domes[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < work->domes[i].spin.vx; j++) {
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->domes[i].u, work->domes[i].v,
                        work->domes[i].u + 63, work->domes[i].v,
                        work->domes[i].u, work->domes[i].v + 63,
                        work->domes[i].u + 63, work->domes[i].v + 63);
                    if (j == 0) {
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                    } else {
                        setRGB0(&work->strip, work->shade[0][k][j], work->shade[1][k][j], work->shade[2][k][j]);
                        setRGB1(&work->strip, work->shade[0][k + 1][j], work->shade[1][k + 1][j],
                            work->shade[2][k + 1][j]);
                    }
                    setRGB2(&work->strip, work->shade[0][k][j + 1], work->shade[1][k][j + 1], work->shade[2][k][j + 1]);
                    setRGB3(&work->strip, work->shade[0][k + 1][j + 1], work->shade[1][k + 1][j + 1],
                        work->shade[2][k + 1][j + 1]);
                    depth = RotAverage4(&work->dome[k][j], &work->dome[k + 1][j],
                        &work->dome[k][j + 1], &work->dome[k + 1][j + 1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            if (work->domes[i].duration * 2 / 3 < work->domes[i].elapsed) {
                if (work->domes[i].spin.vx > 0) {
                    for (k = 0; k < work->spark_count; k++) {
                        work->sparks[k][work->domes[i].spin.vx - 1][i].active = 1;
                    }
                }
                work->domes[i].spin.vx--;
                if (work->domes[i].spin.vx < 0) {
                    work->domes[i].spin.vx = 0;
                }
            }
            work->domes[i].elapsed += step;
            if (i == 0) {
                result = 4;
            }
            if (work->domes[i].duration < work->domes[i].elapsed) {
                work->domes[i].active = 0;
                if (i == 0) {
                    result = 1;
                }
            }
        } else {
            inactive++;
        }
    }
    setVector(&scale, 4096, 4096, 4096);
    for (j = 0; j < work->dome_count; j++) {
        for (k = 0; k < 8; k++) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->sparks[i][k][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &scale);
                    RotMatrixZ(work->sparks[i][k][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][k][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->sparks[i][k][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][k][j].position.vx + work->sparks[i][k][j].spin.vx * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.matrix.t[1] = work->sparks[i][k][j].position.vy + work->sparks[i][k][j].spin.vy * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.matrix.t[2] = work->sparks[i][k][j].position.vz + work->sparks[i][k][j].spin.vz * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.super = work->sparks[i][k][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    duration = work->sparks[i][k][j].duration;
                    if (duration * 2 / 3 < work->sparks[i][k][j].elapsed) {
                        value = (duration - work->sparks[i][k][j].elapsed) * 3;
                        work->sparks[i][k][j].color[0] = D_8013D7EC[work->command].color[0] * value / duration;
                        work->sparks[i][k][j].color[1] = D_8013D7EC[work->command].color[1] * value / duration;
                        work->sparks[i][k][j].color[2] = D_8013D7EC[work->command].color[2] * value / duration;
                    }
                    m = D_800FE168;
                    RotMatrixZ((i + k + j) * 50 * ((i + k + j) % 2 * 2 - 1), &m);
                    for (n = 0; n < 4; n++) {
                        PushMatrix();
                        ApplyMatrixSV(&m, &work->corners[n], &points[n]);
                        PopMatrix();
                    }
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[1];
                    work->sprite.clut = work->clut[1];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, 64, work->sparks[i][k][j].v + 160, 127, work->sparks[i][k][j].v + 160, 64, work->sparks[i][k][j].v + 224, 127, work->sparks[i][k][j].v + 224);
                    setRGB0(&work->sprite, work->sparks[i][k][j].color[0], work->sparks[i][k][j].color[1], work->sparks[i][k][j].color[2]);
                    depth = RotAverage4(&points[0], &points[1], &points[2], &points[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &scale);
                    RotMatrixZ(work->sparks[i][k][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][k][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->sparks[i][k][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][k][j].position.vx + work->sparks[i][k][j].scale.vx * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.matrix.t[1] = work->sparks[i][k][j].position.vy + work->sparks[i][k][j].scale.vy * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.matrix.t[2] = work->sparks[i][k][j].position.vz + work->sparks[i][k][j].scale.vz * work->sparks[i][k][j].elapsed / work->sparks[i][k][j].duration;
                    unit.super = work->sparks[i][k][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->sparks[i][k][j].u, work->sparks[i][k][j].v, work->sparks[i][k][j].u + 31, work->sparks[i][k][j].v, work->sparks[i][k][j].u, work->sparks[i][k][j].v + 31, work->sparks[i][k][j].u + 31, work->sparks[i][k][j].v + 31);
                    setRGB0(&work->sprite, work->sparks[i][k][j].color[0], work->sparks[i][k][j].color[1], work->sparks[i][k][j].color[2]);
                    depth = RotAverage4(&points[0], &points[1], &points[2], &points[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    work->sparks[i][k][j].u += 32;
                    if (work->sparks[i][k][j].u > 224) {
                        work->sparks[i][k][j].u = 0;
                    }
                    work->sparks[i][k][j].elapsed += step;
                    if (work->sparks[i][k][j].duration < work->sparks[i][k][j].elapsed) {
                        work->sparks[i][k][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
    }
    if (work->flash.active == 1) {
        if (work->flash.elapsed < work->flash.duration) {
            value = work->flash.duration - work->flash.elapsed;
            duration = work->flash.duration;
            work->flash.color[0] = D_8013D7EC[work->command].color[0] * value / duration;
            work->flash.color[1] = D_8013D7EC[work->command].color[1] * value / duration;
            work->flash.color[2] = D_8013D7EC[work->command].color[2] * value / duration;
        }
        setPolyF4(&work->flash_poly);
        setRGB0(&work->flash_poly, work->flash.color[0], work->flash.color[1], work->flash.color[2]);
#ifdef VERSION_FRENCH
        setXY4(&work->flash_poly, 0, 0, 320, 0, 0, 256, 320, 256);
#else
        setXY4(&work->flash_poly, 0, 0, 320, 0, 0, 240, 320, 240);
#endif
        Graphics_SubmitDrawModePacket((u32 *)&work->flash_poly, func_80058F10(), 0, work->flash.mode);
        work->flash.elapsed += step;
        if (work->flash.duration < work->flash.elapsed) {
            work->flash.active = 0;
        }
    } else {
        inactive++;
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->ring_count + work->dome_count + work->spark_count * (work->dome_count << 3) + 1) {
        result = 2;
    }
    return result;
}

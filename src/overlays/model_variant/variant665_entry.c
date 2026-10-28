#include "../../types.h"
#include "variant665_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 665's entry: rings of shaded strips fly out of the slot while
 * quads rise from model points, sparks fall from another point, a burst
 * quad flashes and fans of strips spin around the slot. The ambient light
 * fades down first and back up once everything has finished. The setup
 * also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x14 in slot zero and 0x8F in slot one. */
#ifndef MODEL665_STRING_PAD
#define MODEL665_STRING_PAD "\x14"
#endif
#ifndef VERSION_FRENCH
static const char D_8013E4EC[24] = "E_DATA size = %x byte\n\0" MODEL665_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model665State *work;
    MATRIX ls;
    MATRIX m;
    s32 z[4];
    GsCOORDUNIT unit;
    Model665ViewSnapshot view;
    SVECTOR p[4];
    VECTOR target;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, n, b, spawn;
    u32 h;
    u32 tex;
    u16 depth;

    work = (Model665State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model665State));
#ifndef VERSION_FRENCH
        printf(D_8013E4EC, sizeof(Model665State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->focal = 100;
        work->strip_u[1] = 0x30;
        work->strip_u[2] = 0x60;
        work->strip_u[3] = 0x90;
        work->elapsed = 0;
        work->fade_in = 0;
        work->fade_out = 0;
        work->fade_state = 0;
        work->strip_u[0] = 0;
        work->strip_v[0] = 0;
        work->strip_v[1] = 0;
        work->strip_v[2] = 0;
        work->strip_v[3] = 0;
        work->strip_u[4] = 0xC0;
        work->strip_v[4] = 0;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[6]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[7]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[8]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[3]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[0]);
        work->tpage[4] = tex >> 16;
        work->clut[4] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E504[9]);
        work->tpage[5] = tex >> 16;
        work->clut[5] = tex;
        switch (command) {
        case 0:
            work->ring_count = 32;
            work->rows = 8;
            work->cols = 4;
            work->spark_count = 32;
            work->fan_count = 1;
            break;
        case 1:
            work->ring_count = 32;
            work->rows = 8;
            work->cols = 4;
            work->spark_count = 32;
            work->fan_count = 7;
            break;
        }
        work->radius = D_8013E61C[command].radius;
        work->height = D_8013E61C[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].duration = 0x80;
            work->rings[i].color[0] = D_8013E61C[command].ring_color[0];
            work->rings[i].color[1] = D_8013E61C[command].ring_color[1];
            work->rings[i].color[2] = D_8013E61C[command].ring_color[2];
            setVector(&work->rings[i].end, 0, 0, 100);
            setVector(&work->rings[i].start, 0, 0, 1000);
        }
        for (j = 0; j < work->cols; j++) {
            for (i = 0; i < work->rows; i++) {
                work->rising[i][j].duration = 0x30;
                work->rising[i][j].active = 0;
                work->rising[i][j].elapsed = 0;
                work->rising[i][j].mode = 1;
                work->rising[i][j].coord = Model_GetCameraCoordinateUnit();
                work->rising[i][j].color[0] = D_8013E61C[command].rise_color[0];
                work->rising[i][j].color[1] = D_8013E61C[command].rise_color[1];
                work->rising[i][j].color[2] = D_8013E61C[command].rise_color[2];
                n = ((i + j) % 3 << 12) / 3;
                work->rising[i][j].u = 0;
                work->rising[i][j].v = 0x50;
                setVector(&work->rising[i][j].scale, 0x1000, 0x1000, 0x1000);
                setVector(&work->rising[i][j].growth, rsin(n) * 100 / 4096, rcos(n) * 100 / 4096,
                    -(work->direction * 400));
                setVector(&work->rising[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->rising[i][j].position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i].duration = 0x20;
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].color[0] = D_8013E61C[command].spark_color[0];
            work->sparks[i].color[1] = D_8013E61C[command].spark_color[1];
            work->sparks[i].color[2] = D_8013E61C[command].spark_color[2];
            work->sparks[i].u = 0;
            work->sparks[i].v = 0x80;
            setVector(&work->sparks[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i].growth, 0, 0, 0);
            setVector(&work->sparks[i].rotation, 0, 0, 0);
            setVector(&work->sparks[i].position, 0, 0, 0);
        }
        work->burst.duration = 0x6E;
        work->burst.active = 0;
        work->burst.elapsed = 0;
        work->burst.mode = 1;
        work->burst.coord = Model_GetCameraCoordinateUnit();
        work->burst.color[0] = D_8013E61C[command].spark_color[0];
        work->burst.color[1] = D_8013E61C[command].spark_color[1];
        work->burst.color[2] = D_8013E61C[command].spark_color[2];
        work->burst.u = 0xB0;
        work->burst.v = 0x80;
        setVector(&work->burst.scale, 0x1000, 0x1000, 0x1000);
        setVector(&work->burst.growth, 0, 0, 0);
        setVector(&work->burst.rotation, 0, work->slot * 2048, 0);
        setVector(&work->burst.position, 0, 0, 0);
        for (i = 0; i < work->fan_count; i++) {
            work->fans[i].active = 0;
            work->fans[i].elapsed = 0;
            work->fans[i].mode = 1;
            work->fans[i].coord = Model_GetCameraCoordinateUnit();
            work->fans[i].color[0] = D_8013E61C[command].spark_color[0];
            work->fans[i].color[1] = D_8013E61C[command].spark_color[1];
            work->fans[i].color[2] = D_8013E61C[command].spark_color[2];
            work->fans[i].u = 0x30;
            work->fans[i].v = 0x80;
            setVector(&work->fans[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->fans[i].growth, 0, 0, 0);
            setVector(&work->fans[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                work->fans[i].duration = 0x20;
                setVector(&work->fans[i].position, 0, -100, -(work->direction * 100));
                break;
            case 1:
                work->fans[i].duration = 0x18;
                setVector(&work->fans[i].position, work->direction * 100, -100, 0);
                break;
            default:
                work->fans[i].duration = 0x20;
                setVector(&work->fans[i].position, 0, 0, 0);
                break;
            }
            work->fans[i].position.vy -= 350;
            work->fans[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013E61C[command].delay;
        for (i = 0; i < 7; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_inner[i], rsin(i << 8) * 320 / 4096, rcos(i << 8) * 320 / 4096, 0);
            setVector(&work->ring_outer[i], rsin((i << 8) + 0x400) * 320 / 4096,
                rcos((i << 8) + 0x400) * 320 / 4096, work->height);
        }
        n = work->radius / 4;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        for (i = 0; i < 2; i++) {
            h = work->radius;
            n = h / 32 * (i % 2 * 2 - 1);
            b = (u16)(h / 6) * (i % 2 * 2 - 1);
            setVector(&work->petals[0][i], n, 0, h / 2);
            setVector(&work->petals[1][i], b, 0, -(s32)h / 2);
            setVector(&work->petals[2][i], n * 4, -(s32)h / 2, 0);
            setVector(&work->petals[3][i], b * 4, -(s32)h / 2, -(s32)h);
            setVector(&work->petals[4][i], (work->petals[0][i].vx + work->petals[2][i].vx) / 2,
                (work->petals[0][i].vy + work->petals[2][i].vy) / 2,
                (work->petals[0][i].vz + work->petals[2][i].vz) / 2);
            setVector(&work->petals[5][i], (work->petals[1][i].vx + work->petals[3][i].vx) / 2,
                (work->petals[1][i].vy + work->petals[3][i].vy) / 2,
                (work->petals[1][i].vz + work->petals[3][i].vz) / 2);
        }
        n = work->radius / 2;
        setVector(&work->burst_square[0], 0, -n, -n);
        setVector(&work->burst_square[1], 0, n, -n);
        setVector(&work->burst_square[2], 0, -n, n);
        setVector(&work->burst_square[3], 0, n, n);
        n = work->radius / 16;
        b = work->radius;
        for (i = 0; i < 17; i++) {
            setVector(&work->disc_inner[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
            setVector(&work->disc_outer[i], b * rsin(i << 8) / 4096, b * rcos(i << 8) / 4096, 0);
        }
        n = work->radius / 3;
        setVector(&work->fan_square[0], -n, -n, 0);
        setVector(&work->fan_square[1], n, -n, 0);
        setVector(&work->fan_square[2], -n, n, 0);
        setVector(&work->fan_square[3], n, n, 0);
    }
    if (work->fade_state == 0) {
        work->fade_in += step;
        if (D_8013E61C[work->command].fade < work->fade_in) {
            work->fade_in = D_8013E61C[work->command].fade;
            work->fade_state = 1;
        }
        n = 0x800 - work->fade_in * 0x1800 / D_8013E61C[work->command].fade;
        if (n < -0x1000) {
            n = -0x1000;
        }
        Model_SetSlotAmbient(2, n, n, n);
    }
    if (work->fade_state == 1) {
        Model_SetSlotAmbient(2, -0x1000, -0x1000, -0x1000);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->flags[0] * 8 + 6 < work->elapsed && work->flags[0] < work->rows) {
                for (i = 0; i < work->cols; i++) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i + 31);
                    GsGetLwUnit(&unit, &ls);
                    work->rising[work->flags[0]][i].active = 1;
                    setVector(&work->rising[work->flags[0]][i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                work->flags[0]++;
            }
            if (work->elapsed >= 0x75 && work->elapsed < 0x116) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
                GsGetLwUnit(&unit, &ls);
                work->sparks[work->flags[1]].active = 1;
                work->sparks[work->flags[1]].elapsed = 0;
                work->sparks[work->flags[1]].color[0] = D_8013E61C[work->command].spark_color[0];
                work->sparks[work->flags[1]].color[1] = D_8013E61C[work->command].spark_color[1];
                work->sparks[work->flags[1]].color[2] = D_8013E61C[work->command].spark_color[2];
                setVector(&work->sparks[work->flags[1]].position, ls.t[0], ls.t[1] + 70, ls.t[2]);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
                work->sparks[work->flags[1]].rotation.vy = unit.rot.vy + 0x800 - work->slot * 2048;
                work->flags[1]++;
                if (work->flags[1] >= work->spark_count) {
                    work->flags[1] = 0;
                }
            }
            if (work->elapsed >= 0xA1 && work->flags[5] == 0) {
                work->fans[0].active = 1;
                work->flags[5] = 1;
            }
            Model_SetFrameStepOverride(1);
            spawn = 0;
            break;
        case 1:
            if (work->flags[0] * 10 + 6 < work->elapsed && work->flags[0] < work->rows) {
                for (i = 0; i < work->cols; i++) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i + 31);
                    GsGetLwUnit(&unit, &ls);
                    work->rising[work->flags[0]][i].active = 1;
                    setVector(&work->rising[work->flags[0]][i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                work->flags[0]++;
            }
            if ((work->elapsed >= 0x79 && work->elapsed < 0xC8)
                || (work->elapsed >= 0x141 && work->elapsed < 0x182)) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
                GsGetLwUnit(&unit, &ls);
                work->sparks[work->flags[1]].active = 1;
                work->sparks[work->flags[1]].elapsed = 0;
                work->sparks[work->flags[1]].color[0] = D_8013E61C[work->command].spark_color[0];
                work->sparks[work->flags[1]].color[1] = D_8013E61C[work->command].spark_color[1];
                work->sparks[work->flags[1]].color[2] = D_8013E61C[work->command].spark_color[2];
                setVector(&work->sparks[work->flags[1]].position, ls.t[0], ls.t[1] + 70, ls.t[2]);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
                work->sparks[work->flags[1]].rotation.vy = unit.rot.vy + 0x800 - work->slot * 2048;
                work->flags[1]++;
                if (work->flags[1] >= work->spark_count) {
                    work->flags[1] = 0;
                }
            }
            if (work->elapsed >= work->flags[5] * 10 + 0xDC && work->flags[5] < work->fan_count) {
                work->fans[work->flags[5]].active = 1;
                work->flags[5]++;
            }
            if (work->elapsed >= 0xC9 && work->flags[6] == 0) {
                work->burst.active = 1;
                work->flags[6] = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
            GsGetLwUnit(&unit, &ls);
            setVector(&target, ls.t[0], ls.t[1], ls.t[2]);
            Model_SetFrameStepOverride(1);
            spawn = 0;
            break;
        default:
            setVector(&target, 0, 0, 0);
            spawn = 1;
            break;
        }
        if (spawn == 0 && work->elapsed < D_8013E61C[work->command].ring_until) {
            if (work->elapsed == 0 || work->flags[3] >= 13) {
                work->rings[work->flags[2]].active = 1;
                work->rings[work->flags[2]].elapsed = 0;
                work->rings[work->flags[2]].color[0] = D_8013E61C[work->command].ring_color[0];
                work->rings[work->flags[2]].color[1] = D_8013E61C[work->command].ring_color[1];
                work->rings[work->flags[2]].color[2] = D_8013E61C[work->command].ring_color[2];
                work->flags[2]++;
                if (work->ring_count - 1 < work->flags[2]) {
                    work->flags[2] = 0;
                }
                work->flags[3] = 0;
            }
            work->flags[3] += step;
        }
        PushMatrix();
        view = *(Model665ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                if (work->rings[i].elapsed < work->rings[i].duration / 2) {
                    n = work->rings[i].elapsed * 2;
                    b = work->rings[i].duration;
                    work->rings[i].color[0] = D_8013E61C[work->command].ring_color[0] * n / b;
                    work->rings[i].color[1] = D_8013E61C[work->command].ring_color[1] * n / b;
                    work->rings[i].color[2] = D_8013E61C[work->command].ring_color[2] * n / b;
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    n = (i + j) % 5;
                    setUV4(&work->strip, work->strip_u[n], work->strip_v[n] + 79, work->strip_u[n],
                        work->strip_v[n], work->strip_u[n] + 47, work->strip_v[n] + 79,
                        work->strip_u[n] + 47, work->strip_v[n]);
                    setRGB0(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    setRGB2(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    PushMatrix();
                    n = i % 2;
                    m = D_800FE168;
                    RotMatrixZ(work->rings[i].elapsed * 20 + (n << 7), &m);
                    ApplyMatrixSV(&m, &work->ring_inner[j], &p[0]);
                    ApplyMatrixSV(&m, &work->ring_outer[j], &p[1]);
                    ApplyMatrixSV(&m, &work->ring_inner[j + 1], &p[2]);
                    ApplyMatrixSV(&m, &work->ring_outer[j + 1], &p[3]);
                    PopMatrix();
                    n = work->rings[i].start.vz + (work->rings[i].end.vz - work->rings[i].start.vz) * work->rings[i].elapsed / work->rings[i].duration;
                    z[0] = p[0].vz + n;
                    z[1] = p[1].vz + n;
                    z[2] = p[2].vz + n;
                    z[3] = p[3].vz + n;
                    if (z[0] != 0 && z[1] != 0 && z[2] != 0 && z[3] != 0) {
                        work->strip.x0 = (work->rings[i].start.vx + p[0].vx) * work->focal / z[0] + 160;
                        work->strip.y0 = (work->rings[i].start.vy + p[0].vy) * work->focal / z[0] + 120;
                        work->strip.x1 = (work->rings[i].start.vx + p[1].vx) * work->focal / z[1] + 160;
                        work->strip.y1 = (work->rings[i].start.vy + p[1].vy) * work->focal / z[1] + 120;
                        work->strip.x2 = (work->rings[i].start.vx + p[2].vx) * work->focal / z[2] + 160;
                        work->strip.y2 = (work->rings[i].start.vy + p[2].vy) * work->focal / z[2] + 120;
                        work->strip.x3 = (work->rings[i].start.vx + p[3].vx) * work->focal / z[3] + 160;
                        work->strip.y3 = (work->rings[i].start.vy + p[3].vy) * work->focal / z[3] + 120;
                        GsSortPoly(&work->strip, func_80058F10(), 0xFFE);
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
        for (j = 0; j < work->cols; j++) {
            for (i = 0; i < work->rows; i++) {
                if (work->rising[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = (work->rising[i][j].elapsed << 12) / work->rising[i][j].duration + 0x1000;
                    setVector(&work->rising[i][j].scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &work->rising[i][j].scale);
                    RotMatrixZ(work->rising[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->rising[i][j].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->rising[i][j].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                    unit.matrix.t[0] = work->rising[i][j].position.vx + work->rising[i][j].growth.vx * work->rising[i][j].elapsed / work->rising[i][j].duration;
                    unit.matrix.t[1] = work->rising[i][j].position.vy + work->rising[i][j].growth.vy * work->rising[i][j].elapsed / work->rising[i][j].duration;
                    unit.matrix.t[2] = work->rising[i][j].position.vz + work->rising[i][j].growth.vz * work->rising[i][j].elapsed / work->rising[i][j].duration;
                    unit.super = work->rising[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rising[i][j].duration / 2 < work->rising[i][j].elapsed) {
                        n = (work->rising[i][j].duration - work->rising[i][j].elapsed) * 2;
                        b = work->rising[i][j].duration;
                        work->rising[i][j].color[0] = D_8013E61C[work->command].rise_color[0] * n / b;
                        work->rising[i][j].color[1] = D_8013E61C[work->command].rise_color[1] * n / b;
                        work->rising[i][j].color[2] = D_8013E61C[work->command].rise_color[2] * n / b;
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->rising[i][j].u, work->rising[i][j].v, work->rising[i][j].u + 31, work->rising[i][j].v, work->rising[i][j].u, work->rising[i][j].v + 31, work->rising[i][j].u + 31, work->rising[i][j].v + 31);
                    setRGB0(&work->flat, work->rising[i][j].color[0], work->rising[i][j].color[1], work->rising[i][j].color[2]);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    if (work->rising[i][j].u < 0xC0) {
                        work->rising[i][j].u += 32;
                    }
                    work->rising[i][j].elapsed += step;
                    if (work->rising[i][j].duration < work->rising[i][j].elapsed) {
                        work->rising[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->spark_count; i++) {
            if (work->sparks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->sparks[i].elapsed << 14) / work->sparks[i].duration + 0x800;
                setVector(&work->sparks[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                RotMatrixY(work->sparks[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->sparks[i].position.vx;
                unit.matrix.t[1] = work->sparks[i].position.vy;
                unit.matrix.t[2] = work->sparks[i].position.vz;
                unit.super = work->sparks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                n = work->sparks[i].duration - work->sparks[i].elapsed;
                b = work->sparks[i].duration;
                work->sparks[i].color[0] = D_8013E61C[work->command].spark_color[0] * n / b;
                work->sparks[i].color[1] = D_8013E61C[work->command].spark_color[1] * n / b;
                work->sparks[i].color[2] = D_8013E61C[work->command].spark_color[2] * n / b;
                for (j = 0; j < 2; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[2];
                    work->strip.clut = work->clut[2];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->sparks[i].u, work->sparks[i].v + 127, work->sparks[i].u + 47, work->sparks[i].v + 127, work->sparks[i].u, work->sparks[i].v + 64,
                        work->sparks[i].u + 47, work->sparks[i].v + 64);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                    setRGB3(&work->strip, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                    depth = RotAverage4(&work->petals[0][j], &work->petals[1][j], &work->petals[4][j],
                        &work->petals[5][j], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[2];
                    work->flat.clut = work->clut[2];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->sparks[i].u, work->sparks[i].v + 64, work->sparks[i].u + 47, work->sparks[i].v + 64, work->sparks[i].u, work->sparks[i].v, work->sparks[i].u + 47, work->sparks[i].v);
                    setRGB0(&work->flat, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                    depth = RotAverage4(&work->petals[4][j], &work->petals[5][j], &work->petals[2][j],
                        &work->petals[3][j], (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                }
                work->sparks[i].elapsed += step;
                if (work->sparks[i].duration < work->sparks[i].elapsed) {
                    work->sparks[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        if (work->burst.active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->burst.elapsed < work->burst.duration / 4) {
                n = (work->burst.elapsed << 13) / work->burst.duration + 0x800;
            } else {
                n = 0x1000;
            }
            setVector(&work->burst.scale, n, n, n);
            ScaleMatrix(&unit.matrix, &work->burst.scale);
            RotMatrixX(work->burst.rotation.vx, &unit.matrix);
            RotMatrixY(work->burst.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = target.vx + work->burst.position.vx;
            unit.matrix.t[1] = target.vy + work->burst.position.vy;
            unit.matrix.t[2] = target.vz + work->burst.position.vz;
            unit.super = work->burst.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->burst.elapsed > work->burst.duration * 3 / 4) {
                n = (work->burst.duration - work->burst.elapsed) * 4;
                b = work->burst.duration;
                work->burst.color[0] = D_8013E61C[work->command].spark_color[0] * n / b;
                work->burst.color[1] = D_8013E61C[work->command].spark_color[1] * n / b;
                work->burst.color[2] = D_8013E61C[work->command].spark_color[2] * n / b;
            }
            setPolyFT4(&work->flat);
            work->flat.tpage = work->tpage[3];
            work->flat.clut = work->clut[3];
            setSemiTrans(&work->flat, 1);
            setUV4(&work->flat, work->burst.u + 63, work->burst.v, work->burst.u, work->burst.v,
                work->burst.u + 63, work->burst.v + 63, work->burst.u, work->burst.v + 63);
            setRGB0(&work->flat, work->burst.color[0], work->burst.color[1], work->burst.color[2]);
            depth = RotAverage4(&work->burst_square[0], &work->burst_square[1], &work->burst_square[2],
                &work->burst_square[3], (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
            }
            work->burst.rotation.vx = work->burst.elapsed * 300;
            work->burst.elapsed += step;
            if (work->burst.duration < work->burst.elapsed) {
                work->burst.active = 0;
            }
        } else {
            inactive++;
        }
        for (i = 0; i < work->fan_count; i++) {
            if (work->fans[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->fans[i].elapsed << 14) / work->fans[i].duration + 0x800;
                setVector(&work->fans[i].scale, n, n, n);
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
                if (work->fans[i].duration / 2 < work->fans[i].elapsed) {
                    n = (work->fans[i].duration - work->fans[i].elapsed) * 2;
                    b = work->fans[i].duration;
                    work->fans[i].color[0] = D_8013E61C[work->command].spark_color[0] * n / b;
                    work->fans[i].color[1] = D_8013E61C[work->command].spark_color[1] * n / b;
                    work->fans[i].color[2] = D_8013E61C[work->command].spark_color[2] * n / b;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[5];
                work->flat.clut = work->clut[5];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->fans[i].u + 64, work->fans[i].v, work->fans[i].u + 127, work->fans[i].v, work->fans[i].u + 64, work->fans[i].v + 63, work->fans[i].u + 127,
                    work->fans[i].v + 63);
                setRGB0(&work->flat, work->fans[i].color[0], work->fans[i].color[1], work->fans[i].color[2]);
                depth = RotAverage4(&work->fan_square[0], &work->fan_square[1], &work->fan_square[2],
                    &work->fan_square[3], (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[4];
                    work->strip.clut = work->clut[4];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->fans[i].u, work->fans[i].v, work->fans[i].u + 63, work->fans[i].v, work->fans[i].u, work->fans[i].v + 63, work->fans[i].u + 63, work->fans[i].v + 63);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->fans[i].color[0], work->fans[i].color[1], work->fans[i].color[2]);
                    setRGB3(&work->strip, work->fans[i].color[0], work->fans[i].color[1], work->fans[i].color[2]);
                    depth = RotAverage4(&work->disc_inner[j], &work->disc_inner[j + 1], &work->disc_outer[j],
                        &work->disc_outer[j + 1], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                result = 4;
                work->fans[i].elapsed += step;
                if (work->fans[i].duration < work->fans[i].elapsed) {
                    /* Both arms are the same. The extra branch only makes this
                     * loop large enough that GCC leaves the scale address
                     * above unreduced, as the original does. */
                    if (i >= 0) {
                        work->fans[i].active = 0;
                    } else {
                        work->fans[i].active = 0;
                    }
                    if (i == work->fan_count - 1) {
                        result = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->ring_count + work->rows * work->cols + work->spark_count + 1 + work->fan_count) {
            if (work->flags[4] == 1) {
                result = 2;
                Model_SetSlotAmbient(2, 0x800, 0x800, 0x800);
            }
            if (work->fade_state == 1) {
                work->fade_out += step;
                if (D_8013E61C[work->command].fade < work->fade_out) {
                    work->fade_out = D_8013E61C[work->command].fade;
                    work->fade_state = 2;
                    work->flags[4] = 1;
                }
                n = work->fade_out * 0x1800 / D_8013E61C[work->command].fade - 0x1000;
                if (n > 0x800) {
                    n = 0x800;
                }
                Model_SetSlotAmbient(2, n, n, n);
            }
        }
    }
    return result;
}

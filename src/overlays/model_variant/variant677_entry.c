#include "../../types.h"
#include "variant677_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 677's entry: sprites appear at model joints and rise while they
 * spin; each one that finishes becomes a growing halo with five bursts
 * flung outwards and a spinning disc of shaded fans, while sparks drift
 * away from the joints. The setup also prints the work-area size. */

#ifndef VERSION_FRENCH
/* The format string's trailing pad byte differs between the slot images. */
#ifndef MODEL677_STRING_PAD
#define MODEL677_STRING_PAD "\x0C"
#endif
static const char D_8013D74C[24] = "E_DATA size = %x byte\n\0" MODEL677_STRING_PAD;
#endif

/* Fades COLOR[0..2] of element E from the second command colour. */
#define MODEL677_FADE(e, value, divisor)                                         \
    (e).color[0] = D_8013D898[work->command].color[3] * (value) / (divisor); \
    (e).color[1] = D_8013D898[work->command].color[4] * (value) / (divisor); \
    (e).color[2] = D_8013D898[work->command].color[5] * (value) / (divisor)

s32 func_8013B004(u8 *context, s32 command)
{
    Model677State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model677ViewSnapshot view;
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, sign, angle;
    u32 tex;
    PSXLONG depth;
    s16 corner;

    work = (Model677State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model677State));
#ifndef VERSION_FRENCH
        printf(D_8013D74C, sizeof(Model677State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        for (i = 0; i < 8; i++) {
            work->halo_u[i] = i % 4 * 64;
            work->halo_v[i] = i / 4 * 64 - 128;
        }
        /* Scheduling barrier: the empty loop after the early corner
         * assignment keeps the D_8013D764 high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * its single use before register allocation. */
        corner = 0;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[3]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[6]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[8]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[9]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[10]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D764[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        switch (command) {
        case 0:
            work->columns = 2;
            work->rows = 8;
            break;
        case 1:
            work->columns = 1;
            work->rows = 5;
            break;
        }
        work->radius = D_8013D898[command].radius;
        work->height = D_8013D898[command].height;
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            switch (command) {
            case 0:
            case 1:
                setVector(&offset, work->radius * 2, 0, -(work->radius * work->direction));
                RotMatrixZ((i << 12) / 5, &m);
                break;
            }
            ApplyMatrixLV(&m, &offset, &work->drift[i]);
            PopMatrix();
        }
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            switch (command) {
            case 0:
            case 1:
                setVector(&offset, 0, work->radius * 5, 0);
                RotMatrixZ((i << 11) / 5, &m);
                RotMatrixY((i % 3 << 12) / 3, &m);
                break;
            }
            ApplyMatrixLV(&m, &offset, &work->spread[i]);
            PopMatrix();
        }
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->rows; i++) {
                work->sprites[i][j].duration = 32;
                work->sprites[i][j].active = 0;
                work->sprites[i][j].elapsed = 0;
                work->sprites[i][j].mode = 1;
                work->sprites[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sprites[i][j].color[0] = D_8013D898[command].color[0];
                work->sprites[i][j].color[1] = D_8013D898[command].color[1];
                work->sprites[i][j].color[2] = D_8013D898[command].color[2];
                work->sprites[i][j].u = 0;
                work->sprites[i][j].v = 0;
                setVector(&work->sprites[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sprites[i][j].target, 0, 0, work->direction * 450);
                setVector(&work->sprites[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sprites[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->rows; i++) {
                work->halos[i][j].duration = 48;
                work->halos[i][j].active = 0;
                work->halos[i][j].elapsed = 0;
                work->halos[i][j].mode = 1;
                work->halos[i][j].coord = Model_GetCameraCoordinateUnit();
                work->halos[i][j].color[0] = D_8013D898[command].color[3];
                work->halos[i][j].color[1] = D_8013D898[command].color[4];
                work->halos[i][j].color[2] = D_8013D898[command].color[5];
                work->halos[i][j].u = 0;
                work->halos[i][j].v = 0;
                setVector(&work->halos[i][j].scale, 4096, 4096, 4096);
                setVector(&work->halos[i][j].target, 0, 0, 0);
                setVector(&work->halos[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->halos[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->rows; i++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = 32;
                work->sparks[i][j].mode = 1;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = D_8013D898[command].color[3];
                work->sparks[i][j].color[1] = D_8013D898[command].color[4];
                work->sparks[i][j].color[2] = D_8013D898[command].color[5];
                work->sparks[i][j].u = 0;
                work->sparks[i][j].v = 32;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].target, 0, 0, 0);
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        work->delay = D_8013D898[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->sprite[0], -value, -value, corner);
        setVector(&work->sprite[1], value, -value, 0);
        setVector(&work->sprite[2], -value, value, 0);
        setVector(&work->sprite[3], value, value, 0);
        value = work->radius * 2;
        setVector(&work->halo[0], -value, -value, 0);
        setVector(&work->halo[1], value, -value, 0);
        setVector(&work->halo[2], -value, value, 0);
        setVector(&work->halo[3], value, value, 0);
        value = work->radius >> 1;
        setVector(&work->spark[0], -value, -value, 0);
        setVector(&work->spark[1], value, -value, 0);
        setVector(&work->spark[2], -value, value, 0);
        setVector(&work->spark[3], value, value, 0);
        value = work->radius * 2;
        sign = work->radius * 4;
        for (i = 0; i < 17; i++) {
            setVector(&work->inner[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->outer[i], sign * rsin(i << 8) / 4096, sign * rcos(i << 8) / 4096, 0);
            setVector(&work->middle[i], (work->inner[i].vx + work->outer[i].vx) / 2,
                (work->inner[i].vy + work->outer[i].vy) / 2, (work->inner[i].vz + work->outer[i].vz) / 2);
            setVector(&work->inner_xz[i], value * rsin(i << 8) / 4096, 0, value * rcos(i << 8) / 4096);
            setVector(&work->outer_xz[i], sign * rsin(i << 8) / 4096, 0, sign * rcos(i << 8) / 4096);
            setVector(&work->middle_xz[i], (work->inner_xz[i].vx + work->outer_xz[i].vx) / 2,
                (work->inner_xz[i].vy + work->outer_xz[i].vy) / 2,
                (work->inner_xz[i].vz + work->outer_xz[i].vz) / 2);
        }
        value = work->radius;
        setVector(&work->burst[0], -value, -value, 0);
        setVector(&work->burst[1], value, -value, 0);
        setVector(&work->burst[2], -value, value, 0);
        setVector(&work->burst[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->flags[0] < work->rows && work->elapsed >= work->flags[0] * 20) {
            for (i = 0; i < work->columns; i++) {
                work->sprites[work->flags[0]][i].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i * 2 + 3);
                GsGetLwUnit(&unit, &ls);
                (work->sprites + work->flags[0])[0][i].position.vx = ls.t[0];
                (work->sprites + work->flags[0])[0][i].position.vy = ls.t[1];
                (work->sprites + work->flags[0])[0][i].position.vz = ls.t[2];
                work->sparks[work->flags[0]][i].active = 1;
                work->sparks[work->flags[0]][i].position = work->sprites[work->flags[0]][i].position;
            }
            work->flags[0]++;
        }
        break;
    case 1:
        if (work->flags[0] < work->rows && work->elapsed >= work->flags[0] * 20) {
            for (i = 0; i < work->columns; i++) {
                work->sprites[work->flags[0]][i].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 22);
                GsGetLwUnit(&unit, &ls);
                (work->sprites + work->flags[0])[0][i].position.vx = ls.t[0];
                (work->sprites + work->flags[0])[0][i].position.vy = ls.t[1];
                (work->sprites + work->flags[0])[0][i].position.vz = ls.t[2];
                work->sparks[work->flags[0]][i].active = 1;
                work->sparks[work->flags[0]][i].position = work->sprites[work->flags[0]][i].position;
            }
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(Model677ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (j = 0; j < work->columns; j++) {
        for (i = 0; i < work->rows; i++) {
            if (work->sprites[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sprites[i][j].scale);
                RotMatrixZ(work->sprites[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->sprites[i][j].rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->sprites[i][j].position.vx;
                unit.matrix.t[1] = work->sprites[i][j].position.vy;
                unit.matrix.t[2] = work->sprites[i][j].position.vz
                    + (work->sprites[i][j].target.vz - work->sprites[i][j].position.vz)
                        * work->sprites[i][j].elapsed / work->sprites[i][j].duration;
                unit.super = work->sprites[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[0];
                work->quad.clut = work->clut[0];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->sprites[i][j].u, work->sprites[i][j].v,
                    work->sprites[i][j].u + 31, work->sprites[i][j].v,
                    work->sprites[i][j].u, work->sprites[i][j].v + 31,
                    work->sprites[i][j].u + 31, work->sprites[i][j].v + 31);
                setRGB0(&work->quad, work->sprites[i][j].color[0], work->sprites[i][j].color[1],
                    work->sprites[i][j].color[2]);
                depth = RotAverage4(&work->sprite[0], &work->sprite[1], &work->sprite[2], &work->sprite[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                if ((work->sprites[i][j].u += 32) >= 96) {
                    work->sprites[i][j].u = 0;
                }
                work->sprites[i][j].elapsed += step;
                if (work->sprites[i][j].duration < work->sprites[i][j].elapsed) {
                    work->sprites[i][j].active = 0;
                    work->halos[i][j].active = 1;
                    setVector(&work->halos[i][j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->columns; j++) {
        for (i = 0; i < work->rows; i++) {
            if (work->halos[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->halos[i][j].elapsed << 13) / work->halos[i][j].duration + 0x1000;
                setVector(&work->halos[i][j].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->halos[i][j].scale);
                RotMatrixZ(work->halos[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->halos[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->halos[i][j].rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->halos[i][j].position.vx;
                unit.matrix.t[1] = work->halos[i][j].position.vy;
                unit.matrix.t[2] = work->halos[i][j].position.vz;
                unit.super = work->halos[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->halos[i][j].elapsed > (u32)work->halos[i][j].duration / 2) {
                    value = (work->halos[i][j].duration - work->halos[i][j].elapsed) * 2;
                    sign = work->halos[i][j].duration;
                    MODEL677_FADE(work->halos[i][j], value, sign);
                }
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->halo_u[work->halos[i][j].u], work->halo_v[work->halos[i][j].u],
                    work->halo_u[work->halos[i][j].u] + 63, work->halo_v[work->halos[i][j].u],
                    work->halo_u[work->halos[i][j].u], work->halo_v[work->halos[i][j].u] + 63,
                    work->halo_u[work->halos[i][j].u] + 63, work->halo_v[work->halos[i][j].u] + 63);
                setRGB0(&work->quad, work->halos[i][j].color[0], work->halos[i][j].color[1],
                    work->halos[i][j].color[2]);
                depth = RotAverage4(&work->halo[0], &work->halo[1], &work->halo[2], &work->halo[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                value = j % 2 * 2 - 1;
                for (k = 0; k < 5; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->halos[i][j].scale);
                    RotMatrixZ(work->halos[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->halos[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                    angle = 0xC06;
                    RotMatrixY(work->halos[i][j].rotation.vy + angle - view.yaw, &unit.matrix);
                    unit.matrix.t[0] = work->halos[i][j].position.vx
                        + value * work->spread[k].vx * work->halos[i][j].elapsed / work->halos[i][j].duration;
                    unit.matrix.t[1] = work->halos[i][j].position.vy
                        + value * work->spread[k].vy * work->halos[i][j].elapsed / work->halos[i][j].duration;
                    unit.matrix.t[2] = work->halos[i][j].position.vz
                        + value * work->spread[k].vz * work->halos[i][j].elapsed / work->halos[i][j].duration;
                    unit.super = work->halos[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->quad);
                    work->quad.tpage = work->tpage[2];
                    work->quad.clut = work->clut[2];
                    setSemiTrans(&work->quad, 1);
                    setUV4(&work->quad, work->halos[i][j].v, 32,
                        work->halos[i][j].v + 31, 32,
                        work->halos[i][j].v, 63,
                        work->halos[i][j].v + 31, 63);
                    setRGB0(&work->quad, work->halos[i][j].color[0], work->halos[i][j].color[1],
                        work->halos[i][j].color[2]);
                    depth = RotAverage4(&work->burst[0], &work->burst[1], &work->burst[2], &work->burst[3],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->halos[i][j].elapsed << 14) / work->halos[i][j].duration;
                setVector(&work->halos[i][j].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->halos[i][j].scale);
                RotMatrixZ(i * 500 + j * 700, &unit.matrix);
                RotMatrixX((i + j) * 600, &unit.matrix);
                RotMatrixY(i * 700 + j * 500, &unit.matrix);
                unit.matrix.t[0] = work->halos[i][j].position.vx;
                unit.matrix.t[1] = work->halos[i][j].position.vy;
                unit.matrix.t[2] = work->halos[i][j].position.vz;
                unit.super = work->halos[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->fan);
                    setUV4(&work->fan, 0, 64, 31, 64, 0, 127, 31, 127);
                    setRGB0(&work->fan, 0, 0, 0);
                    setRGB1(&work->fan, 0, 0, 0);
                    work->fan.tpage = work->tpage[3];
                    work->fan.clut = work->clut[3];
                    setSemiTrans(&work->fan, 1);
                    setRGB2(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                        work->halos[i][j].color[2]);
                    setRGB3(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                        work->halos[i][j].color[2]);
                    depth = RotAverage4(&work->inner[k], &work->inner[k + 1], &work->middle[k], &work->middle[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1,
                        (PSXLONG *)&work->fan.x2, (PSXLONG *)&work->fan.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                    setPolyGT4(&work->fan);
                    setUV4(&work->fan, 0, 64, 31, 64, 0, 127, 31, 127);
                    setRGB0(&work->fan, 0, 0, 0);
                    setRGB1(&work->fan, 0, 0, 0);
                    work->fan.tpage = work->tpage[3];
                    work->fan.clut = work->clut[3];
                    setSemiTrans(&work->fan, 1);
                    setRGB2(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                        work->halos[i][j].color[2]);
                    setRGB3(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                        work->halos[i][j].color[2]);
                    depth = RotAverage4(&work->outer[k], &work->outer[k + 1], &work->middle[k], &work->middle[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1,
                        (PSXLONG *)&work->fan.x2, (PSXLONG *)&work->fan.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                    if (work->command == 1) {
                        setPolyGT4(&work->fan);
                        setUV4(&work->fan, 0, 64, 31, 64, 0, 127, 31, 127);
                        setRGB0(&work->fan, 0, 0, 0);
                        setRGB1(&work->fan, 0, 0, 0);
                        work->fan.tpage = work->tpage[3];
                        work->fan.clut = work->clut[3];
                        setSemiTrans(&work->fan, 1);
                        setRGB2(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                            work->halos[i][j].color[2]);
                        setRGB3(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                            work->halos[i][j].color[2]);
                        depth = RotAverage4(&work->inner_xz[k], &work->inner_xz[k + 1], &work->middle_xz[k],
                            &work->middle_xz[k + 1],
                            (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1,
                            (PSXLONG *)&work->fan.x2, (PSXLONG *)&work->fan.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                        }
                        setPolyGT4(&work->fan);
                        setUV4(&work->fan, 0, 64, 31, 64, 0, 127, 31, 127);
                        setRGB0(&work->fan, 0, 0, 0);
                        setRGB1(&work->fan, 0, 0, 0);
                        work->fan.tpage = work->tpage[3];
                        work->fan.clut = work->clut[3];
                        setSemiTrans(&work->fan, 1);
                        setRGB2(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                            work->halos[i][j].color[2]);
                        setRGB3(&work->fan, work->halos[i][j].color[0], work->halos[i][j].color[1],
                            work->halos[i][j].color[2]);
                        depth = RotAverage4(&work->outer_xz[k], &work->outer_xz[k + 1], &work->middle_xz[k],
                            &work->middle_xz[k + 1],
                            (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1,
                            (PSXLONG *)&work->fan.x2, (PSXLONG *)&work->fan.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                        }
                    }
                }
                if (work->halos[i][j].u < 7) {
                    work->halos[i][j].u++;
                }
                if (work->halos[i][j].v < 224) {
                    work->halos[i][j].v += 32;
                }
                work->halos[i][j].elapsed += step;
                if (work->halos[i][j].duration < work->halos[i][j].elapsed) {
                    work->halos[i][j].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->columns; j++) {
        for (i = 0; i < work->rows; i++) {
            if (work->sparks[i][j].active == 1) {
                value = (work->sparks[i][j].elapsed << 12) / work->sparks[i][j].duration + 0x1000;
                setVector(&work->sparks[i][j].scale, value, value, value);
                if (work->sparks[i][j].elapsed > (u32)work->sparks[i][j].duration / 2) {
                    value = (work->sparks[i][j].duration - work->sparks[i][j].elapsed) * 2;
                    sign = work->sparks[i][j].duration;
                    MODEL677_FADE(work->sparks[i][j], value, sign);
                }
                for (k = 0; k < 5; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                    RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                    angle = 0xC06;
                    RotMatrixY(work->sparks[i][j].rotation.vy + angle - view.yaw, &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx
                        + work->drift[k].vx * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy
                        + work->drift[k].vy * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz
                        + work->drift[k].vz * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->quad);
                    work->quad.tpage = work->tpage[2];
                    work->quad.clut = work->clut[2];
                    setSemiTrans(&work->quad, 1);
                    setUV4(&work->quad, work->sparks[i][j].u, work->sparks[i][j].v,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v,
                        work->sparks[i][j].u, work->sparks[i][j].v + 31,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v + 31);
                    setRGB0(&work->quad, work->sparks[i][j].color[0], work->sparks[i][j].color[1],
                        work->sparks[i][j].color[2]);
                    depth = RotAverage4(&work->spark[0], &work->spark[1], &work->spark[2], &work->spark[3],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                    }
                }
                if (work->sparks[i][j].u < 224) {
                    work->sparks[i][j].u += 32;
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
    if (inactive == work->columns * (3 * work->rows)) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

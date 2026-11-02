#include "../../types.h"
#include "variant667_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 667's entry: up to two ribbons of Gouraud quads trail between two
 * model points while a row of sprites, a run of shaded fans and up to
 * thirty-two textured domes appear on a timetable around the effect. The
 * domes rise from the opposing slot and the last one ends in a ring of
 * strips. The setup also prints the work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL667_STRING_PAD
#define MODEL667_STRING_PAD "\x02"
#endif
static const char gModelVariant667_D2DFC[24] = "E_DATA size = %x byte\n\0" MODEL667_STRING_PAD;
#endif

#define TRACK_POINT(index, point)                                              \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);          \
    GsGetLwUnit(&unit, &ls);                                                   \
    (point).vx = ls.t[0];                                                      \
    (point).vy = ls.t[1];                                                      \
    (point).vz = ls.t[2]

s32 ModelVariant667_HandleRequest(u8 *context, s32 command)
{
    Model667State *work;
    MATRIX ls;
    s32 sel[2];
    GsCOORDUNIT unit;
    Model667ViewSnapshot view;
    SVECTOR point[2];
    SVECTOR mid[2];
    /* Never referenced, but it keeps the original 0x10-byte frame slot. */
    VECTOR unused;
    SVECTOR apex;
    u8 red[4], green[4], blue[4];
    VECTOR center;
    VECTOR from;
    VECTOR to;
    VECTOR size;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value;
    s32 scale, m, ahead, angle, arc;
    u32 tex;
    u32 radius;
    s32 t;
    s32 lo, hi;
    s32 duration;
    PSXLONG depth;

    work = (Model667State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model667State));
#ifndef VERSION_FRENCH
        printf(gModelVariant667_D2DFC, sizeof(Model667State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        for (i = 0, k = 0; i < 3; i++, k += 3) {
            j = 0;
            hi = k;
            lo = i << 5;
            for (; j < 3; j++) {
                work->cell_u[hi + j] = j << 5;
                work->cell_v[hi + j] = lo;
            }
        }
        k = work->slot == 0;
        /* An empty loop: its notes keep the scheduler from hoisting the origin
         * argument into the slot load's delay slot. */
        do {
        } while (0);
        Model_CopySlotPosition(k, (u16 *)work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant667_D2E14[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant667_D2E14[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant667_D2E14[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->rows = 1;
            work->cols = 32;
            work->sprite_count = 16;
            work->fan_count = 16;
            work->dome_count = 32;
            break;
        case 1:
            work->rows = 0;
            work->cols = 0;
            work->sprite_count = 16;
            work->fan_count = 0;
            work->dome_count = 32;
            break;
        }
        work->radius = gModelVariant667_D2E68[command].radius;
        work->height = gModelVariant667_D2E68[command].height;
        for (i = 0; i < work->rows; i++) {
            work->ribbons[i].active = 0;
            work->ribbons[i].elapsed = 0;
            if (command == 0) {
                work->ribbons[i].duration = 472;
            }
            work->ribbons[i].mode = 1;
            work->ribbons[i].coord = Model_GetCameraCoordinateUnit();
            work->ribbons[i].color[0] = gModelVariant667_D2E68[command].color[0];
            work->ribbons[i].color[1] = gModelVariant667_D2E68[command].color[1];
            work->ribbons[i].color[2] = gModelVariant667_D2E68[command].color[2];
            work->ribbons[i].u = 0;
            work->ribbons[i].v = 0;
            work->ribbons[i].done = 0;
            work->ribbons[i].count = 0;
            setVector(&work->ribbons[i].scale, 4096, 4096, 4096);
            setVector(&work->ribbons[i].unknown_40, 0, 0, 0);
            setVector(&work->ribbons[i].rotation, 0, 0, 0);
            setVector(&work->ribbons[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->sprite_count; i++) {
            work->sprites[i].duration = 48;
            work->sprites[i].active = 0;
            work->sprites[i].elapsed = 0;
            work->sprites[i].mode = 1;
            work->sprites[i].coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].color[0] = gModelVariant667_D2E68[command].color[3];
            work->sprites[i].color[1] = gModelVariant667_D2E68[command].color[4];
            work->sprites[i].color[2] = gModelVariant667_D2E68[command].color[5];
            work->sprites[i].u = i % 9;
            setVector(&work->sprites[i].scale, 4096, 4096, 4096);
            setVector(&work->sprites[i].unknown_40, 0, 0, 0);
            setVector(&work->sprites[i].rotation, 0, work->slot * 2048, 0);
            scale = (i % 2) * 2 - 1;
            value = work->height;
            angle = (i % 8) << 9;
            scale *= value;
            setVector(&work->sprites[i].position, scale * rsin(angle) / 4096,
                scale * rcos(((i % 11) << 11) / 11) / 4096, value * rcos(angle) / 4096);
        }
        for (i = 0; i < work->fan_count; i++) {
            work->fans[i].duration = 250;
            work->fans[i].active = 0;
            work->fans[i].elapsed = 0;
            work->fans[i].mode = 1;
            work->fans[i].coord = Model_GetCameraCoordinateUnit();
            work->fans[i].color[0] = gModelVariant667_D2E68[command].color[0];
            work->fans[i].color[1] = gModelVariant667_D2E68[command].color[1];
            work->fans[i].color[2] = gModelVariant667_D2E68[command].color[2];
            setVector(&work->fans[i].scale, 4096, 4096, 4096);
            setVector(&work->fans[i].unknown_40, 0, 0, 0);
            setVector(&work->fans[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->fans[i].position, 0, 0, 0);
            work->fans[i].done = 0;
        }
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i].duration = 32;
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            work->domes[i].mode = 1;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].color[0] = gModelVariant667_D2E68[command].color[3];
            work->domes[i].color[1] = gModelVariant667_D2E68[command].color[4];
            work->domes[i].color[2] = gModelVariant667_D2E68[command].color[5];
            work->domes[i].u = i % 9;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].unknown_40, 0, -1400, 0);
            scale = (i % 2) * 2 - 1;
            arc = i << 12;
            setVector(&work->domes[i].rotation, arc / work->dome_count + 0x111, (i % 8) << 9,
                scale * arc / work->dome_count + 0x111);
            value = work->height * rsin((i % 8) << 9) / 4096;
            setVector(&work->domes[i].position,
                work->origin[0] + scale * value * rsin(arc / work->dome_count) / 4096,
                work->origin[1] + 700,
                work->origin[2] + value * rcos(arc / work->dome_count) / 4096);
        }
        work->delay = gModelVariant667_D2E68[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->rows; i++) {
            for (j = 0; j < work->cols * 2; j++) {
                for (k = 0; k < 2; k++) {
                    setVector(&work->trail[k][j][i], 0, 0, 0);
                }
            }
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        for (i = 0; i < 2; i++) {
            setVector(&work->prev[i], 0, 0, 0);
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < work->rows; j++) {
                setVector(&work->heads[i][j], 0, 0, 0);
            }
        }
        value = work->radius / 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->fan_rim[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
        }
        t = work->radius;
        value = ((t << 1) + t) / 2;
        radius = t;
        t = -value;
        work->cap[0].vz = 0;
        work->cap[1].vx = value;
        work->cap[1].vz = 0;
        work->cap[2].vy = value;
        work->cap[2].vz = 0;
        work->cap[3].vx = value;
        work->cap[3].vy = value;
        work->cap[3].vz = 0;
        value = radius / 2;
        scale = value * 4;
        lo = scale - value;
        hi = value + scale;
        k = value * 8;
        work->cap[4].vz = scale;
        work->cap[5].vz = scale;
        work->cap[6].vz = scale;
        work->cap[7].vz = scale;
        value = radius;
        /* The halfword radius makes this zero; reusing radius orders the
         * zero after the copy above. */
        radius = work->radius >> 16;
        work->cap[4].vy = radius;
        work->cap[5].vy = radius;
        scale = value * 2;
        work->cap[0].vx = t;
        work->cap[0].vy = t;
        work->cap[1].vy = t;
        work->cap[2].vx = t;
        work->cap[4].vx = lo;
        work->cap[5].vx = hi;
        work->cap[6].vx = lo;
        work->cap[6].vy = k;
        work->cap[7].vx = hi;
        work->cap[7].vy = k;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->outer[i], scale * rsin(i << 8) / 4096, scale * rcos(i << 8) / 4096, 0);
        }
    }
    for (i = 0; i < work->rows; i++) {
        if (work->ribbons[i].done == 0) {
            for (j = work->cols * 2 - 1; j >= 2; j--) {
                for (k = 0; k < 2; k++) {
                    work->trail[k][j][i] = work->trail[k][j - 2][i];
                }
            }
        }
    }
    for (i = 0; i < work->rows; i++) {
        if (work->ribbons[i].done == 0) {
            if (work->command == 0) {
                TRACK_POINT(0x18, point[0]);
                from.vx = ls.t[0];
                from.vy = ls.t[1];
                from.vz = ls.t[2];
                TRACK_POINT(0x19, point[1]);
                center.vx = (point[0].vx + point[1].vx) / 2;
                center.vy = (point[0].vy + point[1].vy) / 2;
                center.vz = (point[0].vz + point[1].vz) / 2;
                to.vx = ls.t[0];
                to.vy = ls.t[1];
                to.vz = ls.t[2];
            } else {
                setVector(&center, 0, 0, 0);
                setVector(&from, 0, 0, 0);
                setVector(&to, 0, 0, 0);
            }
            for (j = 0; j < 2; j++) {
                setVector(&work->trail[j][0][i],
                    (point[j].vx - work->heads[j][i].vx) * 3 / 4 + work->heads[j][i].vx,
                    (point[j].vy - work->heads[j][i].vy) * 3 / 4 + work->heads[j][i].vy,
                    (point[j].vz - work->heads[j][i].vz) * 3 / 4 + work->heads[j][i].vz);
                setVector(&work->trail[j][1][i],
                    (point[j].vx - work->heads[j][i].vx) / 4 + work->heads[j][i].vx,
                    (point[j].vy - work->heads[j][i].vy) / 4 + work->heads[j][i].vy,
                    (point[j].vz - work->heads[j][i].vz) / 4 + work->heads[j][i].vz);
                work->heads[j][i] = point[j];
            }
        }
    }
    if (work->flags[0] == 0) {
        work->flags[0]++;
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else if (work->flags[0] != 0) {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->ribbons[i].active = 1;
                }
            }
            if (work->flags[1] * 10 + 40 < work->elapsed && work->flags[1] < work->sprite_count) {
                work->sprites[work->flags[1]].active = 1;
                work->flags[1]++;
                for (i = 0; i < work->fan_count; i++) {
                    work->fans[i].active = 1;
                }
            }
            if (work->elapsed >= 293 && work->flags[2] < work->dome_count) {
                work->domes[work->flags[2]].active = 1;
                work->flags[2]++;
            }
            break;
        case 1:
            TRACK_POINT(2, center);
            if (work->elapsed >= work->flags[1] * 10 && work->flags[1] < work->sprite_count) {
                work->sprites[work->flags[1]].active = 1;
                work->flags[1]++;
            }
            if (work->elapsed >= 183 && work->flags[2] < work->dome_count) {
                work->domes[work->flags[2]].active = 1;
                work->flags[2]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model667ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->rows; i++) {
            if (work->ribbons[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->ribbons[i].scale);
                unit.matrix.t[0] = work->ribbons[i].position.vx;
                unit.matrix.t[1] = work->ribbons[i].position.vy;
                unit.matrix.t[2] = work->ribbons[i].position.vz;
                unit.super = work->ribbons[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->ribbons[i].duration * 4 / 5 < work->ribbons[i].elapsed) {
                    value = (work->ribbons[i].duration - work->ribbons[i].elapsed) * 5;
                    scale = work->ribbons[i].duration;
                    work->ribbons[i].color[0] = gModelVariant667_D2E68[work->command].color[0] * value / scale;
                    work->ribbons[i].color[1] = gModelVariant667_D2E68[work->command].color[1] * value / scale;
                    work->ribbons[i].color[2] = gModelVariant667_D2E68[work->command].color[2] * value / scale;
                }
                for (j = 0; j < work->ribbons[i].count * 2 - 1; j++) {
                    scale = j + 1;
                    value = work->ribbons[i].count * 4 - 2;
                    for (k = 0; k < 2; k++) {
                        if ((k & 1) == 0) {
                            sel[0] = 1;
                            sel[1] = 0;
                        } else {
                            sel[0] = 0;
                            sel[1] = 1;
                        }
                        mid[k].vx = (work->trail[sel[0]][j + 1][i].vx - work->trail[sel[1]][j + 1][i].vx) * scale / value
                            + work->trail[sel[1]][j + 1][i].vx;
                        mid[k].vy = (work->trail[sel[0]][j + 1][i].vy - work->trail[sel[1]][j + 1][i].vy) * scale / value
                            + work->trail[sel[1]][j + 1][i].vy;
                        mid[k].vz = (work->trail[sel[0]][j + 1][i].vz - work->trail[sel[1]][j + 1][i].vz) * scale / value
                            + work->trail[sel[1]][j + 1][i].vz;
                    }
                    if (j == 0) {
                        for (k = 0; k < 2; k++) {
                            work->prev[k].vx = work->heads[k][i].vx;
                            work->prev[k].vy = work->heads[k][i].vy;
                            work->prev[k].vz = work->heads[k][i].vz;
                        }
                    }
                    point[0].vx = (work->prev[0].vx + work->prev[1].vx) / 2;
                    point[0].vy = (work->prev[0].vy + work->prev[1].vy) / 2;
                    point[0].vz = (work->prev[0].vz + work->prev[1].vz) / 2;
                    point[1].vx = (mid[0].vx + mid[1].vx) / 2;
                    point[1].vy = (mid[0].vy + mid[1].vy) / 2;
                    point[1].vz = (mid[0].vz + mid[1].vz) / 2;
                    m = j + 1;
                    scale = work->ribbons[i].count * 2 - m;
                    value = work->ribbons[i].count * 2 - 1;
                    red[0] = work->ribbons[i].color[0] * scale / value;
                    green[0] = work->ribbons[i].color[1] * scale / value;
                    blue[0] = work->ribbons[i].color[2] * scale / value;
                    red[2] = red[0] / 2;
                    green[2] = green[0] / 2;
                    blue[2] = blue[0] / 2;
                    ahead = 2;
                    scale = work->ribbons[i].count * 2 - (j + ahead);
                    red[1] = work->ribbons[i].color[0] * scale / value;
                    green[1] = work->ribbons[i].color[1] * scale / value;
                    blue[1] = work->ribbons[i].color[2] * scale / value;
                    red[3] = red[1] / 2;
                    green[3] = green[1] / 2;
                    blue[3] = blue[1] / 2;
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[2], green[2], blue[2]);
                    setRGB1(&work->quad, red[0], green[0], blue[0]);
                    setRGB2(&work->quad, red[3], green[3], blue[3]);
                    setRGB3(&work->quad, red[1], green[1], blue[1]);
                    depth = RotAverage4(&work->prev[0], &point[0], &mid[0], &point[1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->ribbons[i].mode);
                    }
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[0], green[0], blue[0]);
                    setRGB1(&work->quad, red[2], green[2], blue[2]);
                    setRGB2(&work->quad, red[1], green[1], blue[1]);
                    setRGB3(&work->quad, red[3], green[3], blue[3]);
                    depth = RotAverage4(&point[0], &work->prev[1], &point[1], &mid[1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, func_80058F10(), (u16)depth,
                            work->ribbons[i].mode);
                    }
                    for (k = 0; k < 2; k++) {
                        work->prev[k] = mid[k];
                    }
                }
                if (work->ribbons[i].done == 0 && work->ribbons[i].count < work->cols) {
                    work->ribbons[i].count++;
                }
                work->ribbons[i].elapsed += step;
                if (work->ribbons[i].duration < work->ribbons[i].elapsed) {
                    work->ribbons[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->sprite_count; i++) {
            if (work->sprites[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->sprites[i].duration / 2 < work->sprites[i].elapsed) {
                    value = ((work->sprites[i].duration - work->sprites[i].elapsed) << 13) / work->sprites[i].duration;
                } else {
                    value = 4096;
                }
                setVector(&work->sprites[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->sprites[i].scale);
                RotMatrixZ(work->sprites[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sprites[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = center.vx + work->sprites[i].position.vx
                    * (work->sprites[i].duration - work->sprites[i].elapsed) / work->sprites[i].duration;
                unit.matrix.t[1] = center.vy + work->sprites[i].position.vy
                    * (work->sprites[i].duration - work->sprites[i].elapsed) / work->sprites[i].duration;
                unit.matrix.t[2] = center.vz + work->sprites[i].position.vz
                    * (work->sprites[i].duration - work->sprites[i].elapsed) / work->sprites[i].duration;
                unit.super = work->sprites[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sprites[i].elapsed < work->sprites[i].duration / 4) {
                    scale = work->sprites[i].duration;
                    value = work->sprites[i].elapsed * 4;
                    work->sprites[i].color[0] = gModelVariant667_D2E68[work->command].color[3] * value / scale;
                    work->sprites[i].color[1] = gModelVariant667_D2E68[work->command].color[4] * value / scale;
                    work->sprites[i].color[2] = gModelVariant667_D2E68[work->command].color[5] * value / scale;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->cell_u[work->sprites[i].u], work->cell_v[work->sprites[i].u],
                    work->cell_u[work->sprites[i].u] + 31, work->cell_v[work->sprites[i].u],
                    work->cell_u[work->sprites[i].u], work->cell_v[work->sprites[i].u] + 31,
                    work->cell_u[work->sprites[i].u] + 31, work->cell_v[work->sprites[i].u] + 31);
                setRGB0(&work->sprite, work->sprites[i].color[0], work->sprites[i].color[1], work->sprites[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->sprites[i].elapsed += step;
                if (work->sprites[i].duration < work->sprites[i].elapsed) {
                    work->sprites[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->fan_count; i++) {
            if (work->fans[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->fans[i].elapsed < work->fans[i].duration / 2) {
                    value = (work->fans[i].elapsed << 13) / work->fans[i].duration;
                } else {
                    value = 4096;
                }
                /* The fan flickers: even frames draw it at half size. */
                if ((work->fans[i].done & 1) == 0) {
                    value /= 2;
                }
                setVector(&work->fans[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->fans[i].scale);
                RotMatrixZ(work->fans[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->fans[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = from.vx + (to.vx - from.vx) * i / work->fan_count;
                unit.matrix.t[1] = from.vy + (to.vy - from.vy) * i / work->fan_count;
                unit.matrix.t[2] = from.vz + (to.vz - from.vz) * i / work->fan_count;
                unit.super = work->fans[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                value = rsin((i << 11) / work->fan_count);
                red[0] = work->fans[i].color[0] * value / 4096;
                green[0] = work->fans[i].color[1] * value / 4096;
                blue[0] = work->fans[i].color[2] * value / 4096;
                setVector(&apex, 0, 0, 0);
                for (j = 0; j < 16; j++) {
                    setPolyG3(&work->fan);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    setRGB0(&work->fan, red[0], green[0], blue[0]);
                    depth = RotAverage3(&apex, &work->fan_rim[j], &work->fan_rim[j + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->fan, func_80058F10(), (u16)depth,
                            work->fans[i].mode);
                    }
                }
                work->fans[i].done++;
                work->fans[i].elapsed += step;
                if (work->fans[i].duration < work->fans[i].elapsed) {
                    work->fans[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->dome_count; i++) {
            if (work->domes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].position.vx;
                unit.matrix.t[1] = work->domes[i].position.vy
                    + work->domes[i].unknown_40.vy * work->domes[i].elapsed / work->domes[i].duration;
                unit.matrix.t[2] = work->domes[i].position.vz;
                unit.super = work->domes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].elapsed < work->domes[i].duration / 4) {
                    value = work->domes[i].elapsed * 4;
                    scale = work->domes[i].duration;
                    work->domes[i].color[0] = gModelVariant667_D2E68[work->command].color[3] * value / scale;
                    work->domes[i].color[1] = gModelVariant667_D2E68[work->command].color[4] * value / scale;
                    work->domes[i].color[2] = gModelVariant667_D2E68[work->command].color[5] * value / scale;
                } else if (work->domes[i].elapsed > work->domes[i].duration * 3 / 4) {
                    value = (work->domes[i].duration - work->domes[i].elapsed) * 4;
                    scale = work->domes[i].duration;
                    work->domes[i].color[0] = gModelVariant667_D2E68[work->command].color[3] * value / scale;
                    work->domes[i].color[1] = gModelVariant667_D2E68[work->command].color[4] * value / scale;
                    work->domes[i].color[2] = gModelVariant667_D2E68[work->command].color[5] * value / scale;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->cell_u[work->domes[i].u], work->cell_v[work->domes[i].u],
                    work->cell_u[work->domes[i].u] + 31, work->cell_v[work->domes[i].u],
                    work->cell_u[work->domes[i].u], work->cell_v[work->domes[i].u] + 31,
                    work->cell_u[work->domes[i].u] + 31, work->cell_v[work->domes[i].u] + 31);
                setRGB0(&work->sprite, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                depth = RotAverage4(&work->cap[0], &work->cap[1], &work->cap[2], &work->cap[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[1];
                work->sprite.clut = work->clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, 0, 255, 31, 255, 0, 128, 31, 128);
                setRGB0(&work->sprite, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                depth = RotAverage4(&work->cap[4], &work->cap[5], &work->cap[6], &work->cap[7],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                if (work->domes[i].duration / 2 < work->domes[i].elapsed) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->domes[i].elapsed << 13) / work->domes[i].duration;
                    setVector(&size, value, value, value);
                    ScaleMatrix(&unit.matrix, &size);
                    RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
                    RotMatrixZ(work->domes[i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->domes[i].rotation.vx, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i].position.vx;
                    unit.matrix.t[1] = work->origin[1];
                    unit.matrix.t[2] = work->domes[i].position.vz;
                    unit.super = work->domes[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[2];
                        work->strip.clut = work->clut[2];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, 32, 128, 63, 128, 32, 255, 63, 255);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                        setRGB3(&work->strip, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                        depth = RotAverage4(&work->ring[j], &work->ring[j + 1], &work->outer[j], &work->outer[j + 1],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                    result = 4;
                }
                work->domes[i].elapsed += step;
                if (work->domes[i].duration < work->domes[i].elapsed) {
                    work->domes[i].active = 0;
                    if (i == work->dome_count - 1) {
                        result = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->rows + work->sprite_count + work->fan_count + work->dome_count) {
            result = 2;
        }
    }
    return result;
}

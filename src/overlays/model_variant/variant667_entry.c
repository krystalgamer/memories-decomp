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
static const char gModelVariant667_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL667_STRING_PAD;
#endif

#define TRACK_POINT(index, point)                                              \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, index);          \
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
    ModelVariantViewSnapshot view;
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
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value;
    s32 scale, m, ahead, angle, arc;
    u32 tex;
    u32 radius;
    s32 t;
    s32 lo, hi;
    s32 duration;
    long depth;

    work = (Model667State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model667State));
#ifndef VERSION_FRENCH
        printf(gModelVariant667_szWorkSizeFormat, sizeof(Model667State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        for (i = 0, k = 0; i < 3; i++, k += 3) {
            j = 0;
            hi = k;
            lo = i << 5;
            for (; j < 3; j++) {
                work->cell_u[hi + j] = j << 5;
                work->cell_v[hi + j] = lo;
            }
        }
        k = work->head.slot == 0;
        /* An empty loop: its notes keep the scheduler from hoisting the origin
         * argument into the slot load's delay slot. */
        do {
        } while (0);
        Model_CopySlotPosition(k, (u16 *)work->origin);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant667_aImages[0]);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant667_aImages[2]);
        work->textures.tpage[1] = tex >> 16;
        work->textures.clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant667_aImages[1]);
        work->textures.tpage[2] = tex >> 16;
        work->textures.clut[2] = tex;
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
        work->place.radius = gModelVariant667_aConfigs[command].radius;
        work->place.height = gModelVariant667_aConfigs[command].height;
        for (i = 0; i < work->rows; i++) {
            work->ribbons[i].head.active = 0;
            work->ribbons[i].head.elapsed = 0;
            if (command == 0) {
                work->ribbons[i].head.duration = 472;
            }
            work->ribbons[i].head.mode = 1;
            work->ribbons[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->ribbons[i].head.color[0] = gModelVariant667_aConfigs[command].color[0];
            work->ribbons[i].head.color[1] = gModelVariant667_aConfigs[command].color[1];
            work->ribbons[i].head.color[2] = gModelVariant667_aConfigs[command].color[2];
            work->ribbons[i].head.u = 0;
            work->ribbons[i].head.v = 0;
            work->ribbons[i].done = 0;
            work->ribbons[i].count = 0;
            setVector(&work->ribbons[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->ribbons[i].unknown_40, 0, 0, 0);
            setVector(&work->ribbons[i].xform.rotation, 0, 0, 0);
            setVector(&work->ribbons[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->sprite_count; i++) {
            work->sprites[i].head.duration = 48;
            work->sprites[i].head.active = 0;
            work->sprites[i].head.elapsed = 0;
            work->sprites[i].head.mode = 1;
            work->sprites[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].head.color[0] = gModelVariant667_aConfigs[command].color[3];
            work->sprites[i].head.color[1] = gModelVariant667_aConfigs[command].color[4];
            work->sprites[i].head.color[2] = gModelVariant667_aConfigs[command].color[5];
            work->sprites[i].head.u = i % 9;
            setVector(&work->sprites[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->sprites[i].unknown_40, 0, 0, 0);
            setVector(&work->sprites[i].xform.rotation, 0, work->head.slot * 2048, 0);
            scale = (i % 2) * 2 - 1;
            value = work->place.height;
            angle = (i % 8) << 9;
            scale *= value;
            setVector(&work->sprites[i].xform.position, scale * rsin(angle) / 4096,
                scale * rcos(((i % 11) << 11) / 11) / 4096, value * rcos(angle) / 4096);
        }
        for (i = 0; i < work->fan_count; i++) {
            work->fans[i].head.duration = 250;
            work->fans[i].head.active = 0;
            work->fans[i].head.elapsed = 0;
            work->fans[i].head.mode = 1;
            work->fans[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->fans[i].head.color[0] = gModelVariant667_aConfigs[command].color[0];
            work->fans[i].head.color[1] = gModelVariant667_aConfigs[command].color[1];
            work->fans[i].head.color[2] = gModelVariant667_aConfigs[command].color[2];
            setVector(&work->fans[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->fans[i].unknown_40, 0, 0, 0);
            setVector(&work->fans[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->fans[i].xform.position, 0, 0, 0);
            work->fans[i].done = 0;
        }
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i].head.duration = 32;
            work->domes[i].head.active = 0;
            work->domes[i].head.elapsed = 0;
            work->domes[i].head.mode = 1;
            work->domes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i].head.color[0] = gModelVariant667_aConfigs[command].color[3];
            work->domes[i].head.color[1] = gModelVariant667_aConfigs[command].color[4];
            work->domes[i].head.color[2] = gModelVariant667_aConfigs[command].color[5];
            work->domes[i].head.u = i % 9;
            setVector(&work->domes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->domes[i].unknown_40, 0, -1400, 0);
            scale = (i % 2) * 2 - 1;
            arc = i << 12;
            setVector(&work->domes[i].xform.rotation, arc / work->dome_count + 0x111, (i % 8) << 9,
                scale * arc / work->dome_count + 0x111);
            value = work->place.height * rsin((i % 8) << 9) / 4096;
            setVector(&work->domes[i].xform.position,
                work->origin[0] + scale * value * rsin(arc / work->dome_count) / 4096,
                work->origin[1] + 700,
                work->origin[2] + value * rcos(arc / work->dome_count) / 4096);
        }
        work->place.delay = gModelVariant667_aConfigs[command].delay;
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
        value = work->place.radius;
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
        value = work->place.radius / 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->fan_rim[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
        }
        t = work->place.radius;
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
        radius = work->place.radius >> 16;
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
            if (work->head.command == 0) {
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
    if (work->place.delay > 0) {
        work->place.delay -= step;
    } else if (work->flags[0] != 0) {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->rows; i++) {
                    work->ribbons[i].head.active = 1;
                }
            }
            if (work->flags[1] * 10 + 40 < work->head.elapsed && work->flags[1] < work->sprite_count) {
                work->sprites[work->flags[1]].head.active = 1;
                work->flags[1]++;
                for (i = 0; i < work->fan_count; i++) {
                    work->fans[i].head.active = 1;
                }
            }
            if (work->head.elapsed >= 293 && work->flags[2] < work->dome_count) {
                work->domes[work->flags[2]].head.active = 1;
                work->flags[2]++;
            }
            break;
        case 1:
            TRACK_POINT(2, center);
            if (work->head.elapsed >= work->flags[1] * 10 && work->flags[1] < work->sprite_count) {
                work->sprites[work->flags[1]].head.active = 1;
                work->flags[1]++;
            }
            if (work->head.elapsed >= 183 && work->flags[2] < work->dome_count) {
                work->domes[work->flags[2]].head.active = 1;
                work->flags[2]++;
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->rows; i++) {
            if (work->ribbons[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->ribbons[i].xform.scale);
                unit.matrix.t[0] = work->ribbons[i].xform.position.vx;
                unit.matrix.t[1] = work->ribbons[i].xform.position.vy;
                unit.matrix.t[2] = work->ribbons[i].xform.position.vz;
                unit.super = work->ribbons[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->ribbons[i].head.duration * 4 / 5 < work->ribbons[i].head.elapsed) {
                    value = (work->ribbons[i].head.duration - work->ribbons[i].head.elapsed) * 5;
                    scale = work->ribbons[i].head.duration;
                    work->ribbons[i].head.color[0] = gModelVariant667_aConfigs[work->head.command].color[0] * value / scale;
                    work->ribbons[i].head.color[1] = gModelVariant667_aConfigs[work->head.command].color[1] * value / scale;
                    work->ribbons[i].head.color[2] = gModelVariant667_aConfigs[work->head.command].color[2] * value / scale;
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
                    red[0] = work->ribbons[i].head.color[0] * scale / value;
                    green[0] = work->ribbons[i].head.color[1] * scale / value;
                    blue[0] = work->ribbons[i].head.color[2] * scale / value;
                    red[2] = red[0] / 2;
                    green[2] = green[0] / 2;
                    blue[2] = blue[0] / 2;
                    ahead = 2;
                    scale = work->ribbons[i].count * 2 - (j + ahead);
                    red[1] = work->ribbons[i].head.color[0] * scale / value;
                    green[1] = work->ribbons[i].head.color[1] * scale / value;
                    blue[1] = work->ribbons[i].head.color[2] * scale / value;
                    red[3] = red[1] / 2;
                    green[3] = green[1] / 2;
                    blue[3] = blue[1] / 2;
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[2], green[2], blue[2]);
                    setRGB1(&work->quad, red[0], green[0], blue[0]);
                    setRGB2(&work->quad, red[3], green[3], blue[3]);
                    setRGB3(&work->quad, red[1], green[1], blue[1]);
                    depth = RotAverage4(&work->prev[0], &point[0], &mid[0], &point[1],
                        (long *)&work->quad.x0, (long *)&work->quad.x1,
                        (long *)&work->quad.x2, (long *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth,
                            work->ribbons[i].head.mode);
                    }
                    setPolyG4(&work->quad);
                    setRGB0(&work->quad, red[0], green[0], blue[0]);
                    setRGB1(&work->quad, red[2], green[2], blue[2]);
                    setRGB2(&work->quad, red[1], green[1], blue[1]);
                    setRGB3(&work->quad, red[3], green[3], blue[3]);
                    depth = RotAverage4(&point[0], &work->prev[1], &point[1], &mid[1],
                        (long *)&work->quad.x0, (long *)&work->quad.x1,
                        (long *)&work->quad.x2, (long *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth,
                            work->ribbons[i].head.mode);
                    }
                    for (k = 0; k < 2; k++) {
                        work->prev[k] = mid[k];
                    }
                }
                if (work->ribbons[i].done == 0 && work->ribbons[i].count < work->cols) {
                    work->ribbons[i].count++;
                }
                work->ribbons[i].head.elapsed += step;
                if (work->ribbons[i].head.duration < work->ribbons[i].head.elapsed) {
                    work->ribbons[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->sprite_count; i++) {
            if (work->sprites[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->sprites[i].head.duration / 2 < work->sprites[i].head.elapsed) {
                    value = ((work->sprites[i].head.duration - work->sprites[i].head.elapsed) << 13) / work->sprites[i].head.duration;
                } else {
                    value = 4096;
                }
                setVector(&work->sprites[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->sprites[i].xform.scale);
                RotMatrixZ(work->sprites[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sprites[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = center.vx + work->sprites[i].xform.position.vx
                    * (work->sprites[i].head.duration - work->sprites[i].head.elapsed) / work->sprites[i].head.duration;
                unit.matrix.t[1] = center.vy + work->sprites[i].xform.position.vy
                    * (work->sprites[i].head.duration - work->sprites[i].head.elapsed) / work->sprites[i].head.duration;
                unit.matrix.t[2] = center.vz + work->sprites[i].xform.position.vz
                    * (work->sprites[i].head.duration - work->sprites[i].head.elapsed) / work->sprites[i].head.duration;
                unit.super = work->sprites[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sprites[i].head.elapsed < work->sprites[i].head.duration / 4) {
                    scale = work->sprites[i].head.duration;
                    value = work->sprites[i].head.elapsed * 4;
                    work->sprites[i].head.color[0] = gModelVariant667_aConfigs[work->head.command].color[3] * value / scale;
                    work->sprites[i].head.color[1] = gModelVariant667_aConfigs[work->head.command].color[4] * value / scale;
                    work->sprites[i].head.color[2] = gModelVariant667_aConfigs[work->head.command].color[5] * value / scale;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->textures.tpage[0];
                work->sprite.clut = work->textures.clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->cell_u[work->sprites[i].head.u], work->cell_v[work->sprites[i].head.u],
                    work->cell_u[work->sprites[i].head.u] + 31, work->cell_v[work->sprites[i].head.u],
                    work->cell_u[work->sprites[i].head.u], work->cell_v[work->sprites[i].head.u] + 31,
                    work->cell_u[work->sprites[i].head.u] + 31, work->cell_v[work->sprites[i].head.u] + 31);
                setRGB0(&work->sprite, work->sprites[i].head.color[0], work->sprites[i].head.color[1], work->sprites[i].head.color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->sprites[i].head.elapsed += step;
                if (work->sprites[i].head.duration < work->sprites[i].head.elapsed) {
                    work->sprites[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->fan_count; i++) {
            if (work->fans[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->fans[i].head.elapsed < work->fans[i].head.duration / 2) {
                    value = (work->fans[i].head.elapsed << 13) / work->fans[i].head.duration;
                } else {
                    value = 4096;
                }
                /* The fan flickers: even frames draw it at half size. */
                if ((work->fans[i].done & 1) == 0) {
                    value /= 2;
                }
                setVector(&work->fans[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->fans[i].xform.scale);
                RotMatrixZ(work->fans[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].xform.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->fans[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = from.vx + (to.vx - from.vx) * i / work->fan_count;
                unit.matrix.t[1] = from.vy + (to.vy - from.vy) * i / work->fan_count;
                unit.matrix.t[2] = from.vz + (to.vz - from.vz) * i / work->fan_count;
                unit.super = work->fans[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                value = rsin((i << 11) / work->fan_count);
                red[0] = work->fans[i].head.color[0] * value / 4096;
                green[0] = work->fans[i].head.color[1] * value / 4096;
                blue[0] = work->fans[i].head.color[2] * value / 4096;
                setVector(&apex, 0, 0, 0);
                for (j = 0; j < 16; j++) {
                    setPolyG3(&work->fan);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    setRGB0(&work->fan, red[0], green[0], blue[0]);
                    depth = RotAverage3(&apex, &work->fan_rim[j], &work->fan_rim[j + 1],
                        (long *)&work->fan.x0, (long *)&work->fan.x1, (long *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->fan, Graphics_GetDefaultOrderingTable(), (u16)depth,
                            work->fans[i].head.mode);
                    }
                }
                work->fans[i].done++;
                work->fans[i].head.elapsed += step;
                if (work->fans[i].head.duration < work->fans[i].head.elapsed) {
                    work->fans[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->dome_count; i++) {
            if (work->domes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].xform.position.vx;
                unit.matrix.t[1] = work->domes[i].xform.position.vy
                    + work->domes[i].unknown_40.vy * work->domes[i].head.elapsed / work->domes[i].head.duration;
                unit.matrix.t[2] = work->domes[i].xform.position.vz;
                unit.super = work->domes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].head.elapsed < work->domes[i].head.duration / 4) {
                    value = work->domes[i].head.elapsed * 4;
                    scale = work->domes[i].head.duration;
                    work->domes[i].head.color[0] = gModelVariant667_aConfigs[work->head.command].color[3] * value / scale;
                    work->domes[i].head.color[1] = gModelVariant667_aConfigs[work->head.command].color[4] * value / scale;
                    work->domes[i].head.color[2] = gModelVariant667_aConfigs[work->head.command].color[5] * value / scale;
                } else if (work->domes[i].head.elapsed > work->domes[i].head.duration * 3 / 4) {
                    value = (work->domes[i].head.duration - work->domes[i].head.elapsed) * 4;
                    scale = work->domes[i].head.duration;
                    work->domes[i].head.color[0] = gModelVariant667_aConfigs[work->head.command].color[3] * value / scale;
                    work->domes[i].head.color[1] = gModelVariant667_aConfigs[work->head.command].color[4] * value / scale;
                    work->domes[i].head.color[2] = gModelVariant667_aConfigs[work->head.command].color[5] * value / scale;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->textures.tpage[0];
                work->sprite.clut = work->textures.clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->cell_u[work->domes[i].head.u], work->cell_v[work->domes[i].head.u],
                    work->cell_u[work->domes[i].head.u] + 31, work->cell_v[work->domes[i].head.u],
                    work->cell_u[work->domes[i].head.u], work->cell_v[work->domes[i].head.u] + 31,
                    work->cell_u[work->domes[i].head.u] + 31, work->cell_v[work->domes[i].head.u] + 31);
                setRGB0(&work->sprite, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                depth = RotAverage4(&work->cap[0], &work->cap[1], &work->cap[2], &work->cap[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->textures.tpage[1];
                work->sprite.clut = work->textures.clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, 0, 255, 31, 255, 0, 128, 31, 128);
                setRGB0(&work->sprite, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                depth = RotAverage4(&work->cap[4], &work->cap[5], &work->cap[6], &work->cap[7],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    value = (work->domes[i].head.elapsed << 13) / work->domes[i].head.duration;
                    setVector(&size, value, value, value);
                    ScaleMatrix(&unit.matrix, &size);
                    RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
                    RotMatrixZ(work->domes[i].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->domes[i].xform.rotation.vx, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i].xform.position.vx;
                    unit.matrix.t[1] = work->origin[1];
                    unit.matrix.t[2] = work->domes[i].xform.position.vz;
                    unit.super = work->domes[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->textures.tpage[2];
                        work->strip.clut = work->textures.clut[2];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, 32, 128, 63, 128, 32, 255, 63, 255);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB3(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        depth = RotAverage4(&work->ring[j], &work->ring[j + 1], &work->outer[j], &work->outer[j + 1],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                    result = 4;
                }
                work->domes[i].head.elapsed += step;
                if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                    work->domes[i].head.active = 0;
                    if (i == work->dome_count - 1) {
                        result = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->rows + work->sprite_count + work->fan_count + work->dome_count) {
            result = 2;
        }
    }
    return result;
}

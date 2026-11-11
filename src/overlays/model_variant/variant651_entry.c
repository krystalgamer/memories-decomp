#include "../../types.h"
#include "variant651_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 651's entry: shaded strip rings spin out of a model point, a dome
 * of strips grows over the slot row by row, and each finished row sends up
 * a spinning cone of strips. The setup also prints the work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL651_STRING_PAD
#define MODEL651_STRING_PAD "\x8C"
#endif
static const char gModelVariant651_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL651_STRING_PAD;
#endif

s32 ModelVariant651_HandleRequest(u8 *context, s32 command)
{
    Model651State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    SVECTOR v[2];
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, n, d, t;
    s32 x, y;
    u8 u0, u1;
    u32 tex;
    long depth;

    work = (Model651State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model651State));
#ifndef VERSION_FRENCH
        printf(gModelVariant651_szWorkSizeFormat, sizeof(Model651State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant651_Image);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        switch (command) {
        case 0:
            work->ring_count = 4;
            work->dome_count = 1;
            break;
        case 1:
            work->ring_count = 0;
            work->dome_count = 0;
            break;
        }
        work->place.radius = gModelVariant651_aConfigs[command].radius;
        work->place.height = gModelVariant651_aConfigs[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].head.duration = 0x40;
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant651_aConfigs[command].red;
            work->rings[i].head.color[1] = gModelVariant651_aConfigs[command].green;
            work->rings[i].head.color[2] = gModelVariant651_aConfigs[command].blue;
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 0;
            setVector(&work->rings[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, (i << 12) / work->ring_count);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i].head.duration = 0x80;
            work->domes[i].head.active = 0;
            work->domes[i].head.elapsed = 0;
            work->domes[i].head.mode = 1;
            work->domes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i].head.color[0] = gModelVariant651_aConfigs[command].red;
            work->domes[i].head.color[1] = gModelVariant651_aConfigs[command].green;
            work->domes[i].head.color[2] = gModelVariant651_aConfigs[command].blue;
            work->domes[i].head.u = 0;
            work->domes[i].head.v = 0;
            setVector(&work->domes[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->domes[i].growth, 0, 0, (work->place.height / 2 + 450) * work->direction);
            setVector(&work->domes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->domes[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < 8; i++) {
            work->cones[i].head.duration = 0x30;
            work->cones[i].head.active = 0;
            work->cones[i].head.elapsed = 0;
            work->cones[i].head.mode = 1;
            work->cones[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->cones[i].head.color[0] = gModelVariant651_aConfigs[command].red;
            work->cones[i].head.color[1] = gModelVariant651_aConfigs[command].green;
            work->cones[i].head.color[2] = gModelVariant651_aConfigs[command].blue;
            work->cones[i].head.u = 0;
            work->cones[i].head.v = 0;
            setVector(&work->cones[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->cones[i].growth, 0, 0, 0);
            setVector(&work->cones[i].xform.rotation, 0, work->head.slot * 2048, i << 9);
            setVector(&work->cones[i].xform.position, 0, 0, 0);
            work->cones[i].xform.position.vy -= 350;
            work->cones[i].xform.position.vz += work->direction * 450;
        }
        work->place.delay = gModelVariant651_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i][0], work->place.radius * rsin(i << 8) / 4096,
                work->place.radius * rcos(i << 8) / 4096, 0);
            setVector(&work->ring[i][1], work->place.radius * (rsin(i << 8) << 3) / 4096,
                work->place.radius * (rcos(i << 8) << 3) / 4096, -work->place.height / 2);
        }
        /* The row, angle and half counters reuse k, j and t; the original
         * register allocation depends on that sharing. */
        for (k = 0; k < 8; k++) {
            for (i = 0; i < 17; i++) {
                v[0].vx = work->place.radius * rsin((k + 1) * 96 + 256) / 4096 * rsin(j = i << 8) / 4096;
                v[0].vy = work->place.radius * rsin((k + 1) * 96 + 256) / 4096 * rcos(j) / 4096;
                v[0].vz = work->place.height / 2 - work->place.height * k / 8;
                x = rsin((k + 1) * 96 + 256);
                v[1].vx = work->place.radius * ((x << 2) + x) / 4096 * rsin(j) / 4096;
                y = rsin((k + 1) * 96 + 256);
                v[1].vy = work->place.radius * ((y << 2) + y) / 4096 * rcos(j) / 4096;
                v[1].vz = work->place.height / 2 - work->place.height * k / 8 - work->place.height / 4;
                ls = GsIDMATRIX;
                RotMatrixZ(k << 9, &ls);
                for (t = 0; t < 2; t++) {
                    PushMatrix();
                    ApplyMatrixSV(&ls, &v[t], &work->dome[i][t][k]);
                    PopMatrix();
                }
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->outer[i][0], work->place.radius * (rsin(i << 8) << 1) / 4096,
                work->place.radius * (rcos(i << 8) << 1) / 4096, 0);
            setVector(&work->outer[i][1], work->place.radius * (rsin(i << 8) << 2) / 4096,
                work->place.radius * (rcos(i << 8) << 2) / 4096, work->place.height / 15);
        }
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->flags[0] / 4 < work->ring_count) {
                if ((work->flags[0] & 3) == 0) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
                    GsGetLwUnit(&unit, &ls);
                    work->rings[work->flags[0] / 4].head.active = 1;
                    applyVector(&work->rings[work->flags[0] / 4].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            if (work->head.elapsed >= 11 && work->flags[1] == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
                GsGetLwUnit(&unit, &ls);
                work->domes[0].head.active = 1;
                applyVector(&work->domes[0].xform.position, ls.t[0], ls.t[1], ls.t[2], +=);
                n = work->place.height * work->domes[0].head.duration
                    / ((ABS(work->domes[0].growth.vz) + ABS(work->domes[0].xform.position.vz)) * 8);
                for (i = 0; i < 8; i++) {
                    work->row_timer[i] = n * (i + 2);
                }
                work->flags[1] = 1;
            }
            break;
        case 1:
            if (work->flags[0] < 7) {
                if (work->head.elapsed >= work->flags[0] * 16) {
                    work->cones[work->flags[0]].head.active = 1;
                    /* The zero x/y adds keep the original's dead loads. */
                    applyVector(&work->cones[work->flags[0]].xform.position, 0, 0, work->direction * 100, +=);
                    if (work->flags[0] == 6) {
                        work->flags[1] = result = 1;
                    }
                    work->flags[0]++;
                }
                if (work->flags[1] != 1) {
                    result = 4;
                }
            }
            break;
        }
        PushMatrix();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                n = (work->rings[i].head.elapsed << 12) / work->rings[i].head.duration;
                setVector(&work->rings[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
                RotMatrixY(work->rings[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].xform.position.vx;
                unit.matrix.t[1] = work->rings[i].xform.position.vy;
                unit.matrix.t[2] = work->rings[i].xform.position.vz;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->rings[i].xform.rotation.vz -= step * 10;
                if (work->rings[i].head.duration / 2 < work->rings[i].head.elapsed) {
                    n = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 2;
                    d = work->rings[i].head.duration;
                    work->rings[i].head.color[0] = gModelVariant651_aConfigs[work->head.command].red * n / d;
                    work->rings[i].head.color[1] = gModelVariant651_aConfigs[work->head.command].green * n / d;
                    work->rings[i].head.color[2] = gModelVariant651_aConfigs[work->head.command].blue * n / d;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->textures.tpage[0];
                    work->quad.clut = work->textures.clut[0];
                    setSemiTrans(&work->quad, 1);
                    u0 = work->rings[i].head.u + k * 127 / 16;
                    u1 = work->rings[i].head.u + (k + 1) * 127 / 16;
                    setUV4(&work->quad, u0, work->rings[i].head.v, u1, work->rings[i].head.v,
                        u0, work->rings[i].head.v + 63, u1, work->rings[i].head.v + 63);
                    setRGB0(&work->quad, 0, 0, 0);
                    setRGB1(&work->quad, 0, 0, 0);
                    setRGB2(&work->quad, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    setRGB3(&work->quad, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    depth = RotAverage4(&work->ring[k][0], &work->ring[k + 1][0], &work->ring[k][1],
                        &work->ring[k + 1][1], (long *)&work->quad.x0, (long *)&work->quad.x1,
                        (long *)&work->quad.x2, (long *)&work->quad.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->rings[i].head.elapsed += step;
                if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                    work->rings[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->dome_count; i++) {
            if (work->domes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->domes[i].head.elapsed < (u16)(work->domes[i].head.duration / 3)) {
                    n = (work->domes[i].head.elapsed << 12) / (u16)(work->domes[i].head.duration / 3);
                } else {
                    n = 0x1000;
                }
                setVector(&work->domes[i].xform.scale, n, n, n);
                work->domes[i].xform.scale.vz += work->domes[i].xform.scale.vz / 8 * rsin(work->domes[i].head.elapsed * 100) / 4096;
                ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                RotMatrixZ(work->domes[i].xform.rotation.vz, &unit.matrix);
                RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].xform.position.vx;
                unit.matrix.t[1] = work->domes[i].xform.position.vy;
                unit.matrix.t[2] = work->domes[i].xform.position.vz
                    + (work->domes[i].growth.vz - work->domes[i].xform.position.vz) * work->domes[i].head.elapsed
                        / work->domes[i].head.duration;
                unit.super = work->domes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->domes[i].xform.rotation.vz += step * 20;
                for (t = 0; t < 8; t++) {
                    if (work->row_timer[t] < work->domes[i].head.duration) {
                        for (k = 0; k < 16; k++) {
                            setPolyGT4(&work->quad);
                            work->quad.tpage = work->textures.tpage[0];
                            work->quad.clut = work->textures.clut[0];
                            setSemiTrans(&work->quad, 1);
                            u0 = work->domes[i].head.u + k * 127 / 16;
                            u1 = work->domes[i].head.u + (k + 1) * 127 / 16;
                            setUV4(&work->quad, u0, work->domes[i].head.v, u1, work->domes[i].head.v,
                                u0, work->domes[i].head.v + 63, u1, work->domes[i].head.v + 63);
                            setRGB0(&work->quad, 0, 0, 0);
                            setRGB1(&work->quad, 0, 0, 0);
                            setRGB2(&work->quad, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                            setRGB3(&work->quad, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                            depth = RotAverage4(&work->dome[k][0][t], &work->dome[k + 1][0][t],
                                &work->dome[k][1][t], &work->dome[k + 1][1][t], (long *)&work->quad.x0,
                                (long *)&work->quad.x1, (long *)&work->quad.x2,
                                (long *)&work->quad.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    if ((work->domes[i].head.duration < work->row_timer[t]
                            || work->domes[i].head.duration < (work->row_timer[t] += step))
                        && work->cones[t].growth.vx == 0) {
                        work->cones[t].head.active = 1;
                        work->cones[t].xform.position.vy = unit.matrix.t[1];
                        work->cones[t].growth.vx = 1;
                        if (t == 0) {
                            result = 1;
                        } else if (result != 1) {
                            result = 4;
                        }
                    }
                }
                work->domes[i].head.elapsed += step;
                if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                    work->domes[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 8; i++) {
            if (work->cones[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                n = (work->cones[i].head.elapsed * 5 << 12) / work->cones[i].head.duration + 0x1000;
                setVector(&work->cones[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->cones[i].xform.scale);
                RotMatrixZ(work->cones[i].xform.rotation.vz, &unit.matrix);
                RotMatrixY(work->cones[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->cones[i].xform.position.vx;
                unit.matrix.t[1] = work->cones[i].xform.position.vy;
                unit.matrix.t[2] = work->cones[i].xform.position.vz;
                unit.super = work->cones[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->cones[i].head.duration / 2 < work->cones[i].head.elapsed) {
                    n = (work->cones[i].head.duration - work->cones[i].head.elapsed) * 2;
                    d = work->cones[i].head.duration;
                    work->cones[i].head.color[0] = gModelVariant651_aConfigs[work->head.command].red * n / d;
                    work->cones[i].head.color[1] = gModelVariant651_aConfigs[work->head.command].green * n / d;
                    work->cones[i].head.color[2] = gModelVariant651_aConfigs[work->head.command].blue * n / d;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->textures.tpage[0];
                    work->quad.clut = work->textures.clut[0];
                    setSemiTrans(&work->quad, 1);
                    u0 = work->cones[i].head.u + k % 4 * 127 / 4;
                    u1 = work->cones[i].head.u + (k % 4 + 1) * 127 / 4;
                    setUV4(&work->quad, u0, work->cones[i].head.v, u1, work->cones[i].head.v,
                        u0, work->cones[i].head.v + 63, u1, work->cones[i].head.v + 63);
                    setRGB0(&work->quad, 0, 0, 0);
                    setRGB1(&work->quad, 0, 0, 0);
                    setRGB2(&work->quad, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                    setRGB3(&work->quad, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                    depth = RotAverage4(&work->outer[k][0], &work->outer[k + 1][0], &work->outer[k][1],
                        &work->outer[k + 1][1], (long *)&work->quad.x0, (long *)&work->quad.x1,
                        (long *)&work->quad.x2, (long *)&work->quad.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->cones[i].head.elapsed += step;
                if (work->cones[i].head.duration < work->cones[i].head.elapsed) {
                    work->cones[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->ring_count + work->dome_count + 8) {
            result = 2;
        }
    }
    return result;
}

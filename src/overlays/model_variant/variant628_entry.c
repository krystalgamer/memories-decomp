#include "../../types.h"
#include "variant628_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 628's entry: a sprite ball arcs away from a model joint while a
 * ring of sparks bursts out of it. Where the ball lands, two wobbling
 * Gouraud domes rise and shed a trail of spinning ring bursts. The setup
 * also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x26 in slot zero and 0x00 in slot one. */
#ifndef MODEL628_STRING_PAD
#define MODEL628_STRING_PAD "&"
#endif
#ifndef VERSION_FRENCH
static const char gModelVariant628_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL628_STRING_PAD;
#endif

s32 ModelVariant628_HandleRequest(u8 *context, s32 command)
{
    Model628State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    SVECTOR rotation;
    SVECTOR mid[2];
    VECTOR vec;
    long interpolation, flag;
    s32 result, i, j, step, inactive;
    s32 k, l, fade, n, shade, h;
    u8 color;
    u32 tex;
    long depth;

    work = (Model628State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model628State));
#ifndef VERSION_FRENCH
        printf(gModelVariant628_szWorkSizeFormat, sizeof(Model628State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant628_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant628_aImages[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant628_aImages[2]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 1;
            work->spark_count = 8;
            work->dome_count = 2;
            work->burst_count = 32;
            break;
        }
        work->radius = gModelVariant628_aConfigs[command].radius;
        work->height = gModelVariant628_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->balls[i].head.active = 0;
            work->balls[i].head.elapsed = 0;
            work->balls[i].head.duration = 0x20;
            work->balls[i].head.mode = 1;
            work->balls[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->balls[i].head.color[0] = gModelVariant628_aConfigs[command].red;
            work->balls[i].head.color[1] = gModelVariant628_aConfigs[command].green;
            work->balls[i].head.color[2] = gModelVariant628_aConfigs[command].blue;
            work->balls[i].head.u = 0;
            work->balls[i].head.v = 0x60;
            setVector(&work->balls[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->balls[i].xform.rotation, 0, work->head.slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->balls[i].velocity, -20, 2, 7);
                if (i == 0) {
                    setVector(&work->balls[i].xform.position, 0, 0, 0);
                }
                break;
            }
        }
        PushMatrix();
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i][0].head.active = 0;
            work->sparks[i][0].head.elapsed = 0;
            work->sparks[i][0].head.duration = 0x40;
            work->sparks[i][0].head.mode = 1;
            work->sparks[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->sparks[i][0].head.color[0] = 0x80;
            work->sparks[i][0].head.color[1] = 0x80;
            work->sparks[i][0].head.color[2] = 0x80;
            work->sparks[i][0].head.u = 0;
            work->sparks[i][0].head.v = 0;
            setVector(&work->sparks[i][0].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&rotation, (work->head.slot * 2 - 1) * 0x28000 / 360, 0, 0);
            setVector(&vec, rsin((i << 12) / work->spark_count) * 10 / 4096,
                rcos((i << 12) / work->spark_count) * 10 / 4096, -((work->head.slot * 2 - 1) * 10));
            RotMatrix(&rotation, &m);
            ApplyMatrixLV(&m, &vec, &work->sparks[i][0].velocity);
            switch (command) {
            case 0:
                if (i == 0) {
                    setVector(&work->sparks[i][0].xform.position, 0, 0, 0);
                }
                break;
            }
        }
        PopMatrix();
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i][0].head.active = 0;
            work->domes[i][0].head.elapsed = 0;
            work->domes[i][0].head.duration = 0x80;
            work->domes[i][0].head.mode = 1;
            work->domes[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i][0].head.color[0] = gModelVariant628_aConfigs[command].red;
            work->domes[i][0].head.color[1] = gModelVariant628_aConfigs[command].green;
            work->domes[i][0].head.color[2] = gModelVariant628_aConfigs[command].blue;
            work->domes[i][0].head.u = 0;
            work->domes[i][0].head.v = 0x20;
            work->domes[i][0].stage = 0;
            setVector(&work->domes[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->domes[i][0].velocity, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->domes[i][0].xform.position, 0, 0, 0);
                switch (i) {
                case 0:
                    setVector(&work->domes[i][0].xform.scale, 0x1000, 0, 0x1000);
                    break;
                case 1:
                    setVector(&work->domes[i][0].xform.scale, 0x1800, 0, 0x1800);
                    break;
                }
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i][0].head.active = 0;
            work->bursts[i][0].head.elapsed = 0;
            work->bursts[i][0].head.duration = 0x30;
            work->bursts[i][0].head.mode = 1;
            work->bursts[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i][0].head.color[0] = gModelVariant628_aConfigs[command].red;
            work->bursts[i][0].head.color[1] = gModelVariant628_aConfigs[command].green;
            work->bursts[i][0].head.color[2] = gModelVariant628_aConfigs[command].blue;
            work->bursts[i][0].head.u = 0;
            work->bursts[i][0].head.v = 0x20;
            setVector(&work->bursts[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->bursts[i][0].velocity, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->bursts[i][0].xform.position, 0, 0, 0);
                setVector(&work->bursts[i][0].xform.scale, 0x1000, 0x1000, 0x1000);
                break;
            }
        }
        work->delay = gModelVariant628_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->square[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->square[3], work->radius / 2, work->radius / 2, 0);
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->dome[j][i],
                    (work->radius * 2 - work->radius * i / 8) * rsin(j << 8) / 4096,
                    /* One read of the height feeds both terms. */
                    (h = work->height, -h + h * i / 8),
                    (work->radius * 2 - work->radius * i / 8) * rcos(j << 8) / 4096);
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i].inner, work->radius * rsin(i << 8) / 4096, 0,
                work->radius * rcos(i << 8) / 4096);
            setVector(&work->ring[i].outer, ((work->radius * 3) >> 1) * rsin((i << 8) + 0xE3) / 4096, -300,
                ((work->radius * 3) >> 1) * rcos((i << 8) + 0xE3) / 4096);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed != 0) {
                break;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0xC);
            GsGetLwUnit(&unit, &ls);
            work->balls[0].head.active = 1;
            work->balls[0].xform.position.vx += ls.t[0];
            work->balls[0].xform.position.vy += ls.t[1];
            work->balls[0].xform.position.vz += ls.t[2] * -(work->head.slot * 2 + -1);
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][0].head.active = 1;
                work->sparks[i][0].xform.position.vx += ls.t[0];
                work->sparks[i][0].xform.position.vy += ls.t[1];
                work->sparks[i][0].xform.position.vz += ls.t[2];
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->balls[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->balls[i].xform.scale);
                RotMatrixX(work->balls[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * (work->head.slot * 2 - 1),
                    &unit.matrix);
                RotMatrixY(work->balls[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->balls[i].xform.position.vx;
                unit.matrix.t[1] = work->balls[i].velocity.vx * -work->balls[i].velocity.vz
                        * work->balls[i].head.elapsed * work->balls[i].head.elapsed
                        / (work->balls[i].head.duration * (work->balls[i].velocity.vy << 1))
                    + work->balls[i].velocity.vx * work->balls[i].head.elapsed + work->balls[i].xform.position.vy;
                unit.matrix.t[2] = ((work->balls[i].xform.position.vz + 450) * work->balls[i].head.elapsed
                        / work->balls[i].head.duration - work->balls[i].xform.position.vz) * (work->head.slot * 2 - 1);
                unit.super = work->balls[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->flat);
                setSemiTrans(&work->flat, 1);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setUV4(&work->flat, work->balls[i].head.u, work->balls[i].head.v, work->balls[i].head.u + 31,
                    work->balls[i].head.v, work->balls[i].head.u, work->balls[i].head.v + 31,
                    work->balls[i].head.u + 31, work->balls[i].head.v + 31);
                setRGB0(&work->flat, work->balls[i].head.color[0], work->balls[i].head.color[1], work->balls[i].head.color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (long *)&work->flat.x0, (long *)&work->flat.x1,
                    (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->balls[i].head.u += 0x20;
                if (work->balls[i].head.u == 0x60) {
                    work->balls[i].head.u = 0;
                }
                work->balls[i].head.elapsed += step;
                if (work->balls[i].head.duration < work->balls[i].head.elapsed) {
                    work->balls[i].head.active = 0;
                    for (j = 0; j < work->dome_count; j++) {
                        work->domes[j][i].head.active = 1;
                        work->domes[j][i].xform.position.vx = unit.matrix.t[0];
                        work->domes[j][i].xform.position.vy = unit.matrix.t[1];
                        work->domes[j][i].xform.position.vz = unit.matrix.t[2];
                    }
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->sparks[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].xform.scale);
                    RotMatrixX(work->sparks[i][j].xform.rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->head.slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][j].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].xform.position.vz;
                    unit.super = work->sparks[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->sparks[i][j].xform.position.vx += work->sparks[i][j].velocity.vx;
                    work->sparks[i][j].xform.position.vy += work->sparks[i][j].velocity.vy;
                    work->sparks[i][j].xform.position.vz += work->sparks[i][j].velocity.vz;
                    if (work->sparks[i][j].head.duration / 2 < work->sparks[i][j].head.elapsed) {
                        shade = ((work->sparks[i][j].head.duration - work->sparks[i][j].head.elapsed) << 8)
                            / work->sparks[i][j].head.duration;
                        work->sparks[i][j].head.color[0] = shade;
                        work->sparks[i][j].head.color[1] = shade;
                        work->sparks[i][j].head.color[2] = shade;
                    }
                    setPolyFT4(&work->flat);
                    setSemiTrans(&work->flat, 1);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setUV4(&work->flat, work->sparks[i][j].head.u, work->sparks[i][j].head.v, work->sparks[i][j].head.u + 31,
                        work->sparks[i][j].head.v, work->sparks[i][j].head.u, work->sparks[i][j].head.v + 31,
                        work->sparks[i][j].head.u + 31, work->sparks[i][j].head.v + 31);
                    setRGB0(&work->flat, work->sparks[i][j].head.color[0], work->sparks[i][j].head.color[1],
                        work->sparks[i][j].head.color[2]);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (long *)&work->flat.x0, (long *)&work->flat.x1,
                        (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->sparks[i][j].head.u += 0x20;
                    if (work->sparks[i][j].head.u == 0xE0) {
                        work->sparks[i][j].head.u = 0;
                    }
                    work->sparks[i][j].head.elapsed += step;
                    if (work->sparks[i][j].head.duration < work->sparks[i][j].head.elapsed) {
                        work->sparks[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->dome_count; i++) {
                /* Holding the flag in n, which later takes a third of the
                 * duration, pins that value in a saved register. */
                n = work->domes[i][j].head.active;
                if (n == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->domes[i][j].xform.scale);
                    RotMatrixY(work->domes[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->domes[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->domes[i][j].xform.position.vz;
                    unit.super = work->domes[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    switch (i) {
                    case 0:
                        if (work->domes[i][j].head.elapsed < 20) {
                            work->domes[i][j].xform.scale.vy += step * 0xCC;
                            if (work->domes[i][j].xform.scale.vy > 0x1000) {
                                work->domes[i][j].xform.scale.vy = 0x1000;
                            }
                        } else {
                            work->domes[i][j].xform.scale.vy = rsin(work->domes[i][j].stage * 1000 + 0x800) / 32 + 0x1000;
                            work->domes[i][j].stage += step;
                        }
                        break;
                    case 1:
                        if (work->domes[i][j].head.elapsed < 20) {
                            work->domes[i][j].xform.scale.vy += step * 0x88;
                            if (work->domes[i][j].xform.scale.vy > 0xAAA) {
                                work->domes[i][j].xform.scale.vy = 0xAAA;
                            }
                        } else {
                            work->domes[i][j].xform.scale.vy = rsin(work->domes[i][j].stage * 1000) / 32 + 0xAAA;
                            work->domes[i][j].stage += step;
                        }
                        break;
                    }
                    n = work->domes[i][j].head.duration / 3;
                    if (work->domes[i][j].head.duration * 2 / 3 < work->domes[i][j].head.elapsed) {
                        fade = gModelVariant628_aConfigs[work->head.command].red / (n - 1) * step;
                        if (fade < work->domes[i][j].head.color[0]) {
                            work->domes[i][j].head.color[0] -= fade;
                        } else {
                            work->domes[i][j].head.color[0] = 0;
                        }
                        fade = gModelVariant628_aConfigs[work->head.command].green / (n - 1) * step;
                        if (fade < work->domes[i][j].head.color[1]) {
                            work->domes[i][j].head.color[1] -= fade;
                        } else {
                            work->domes[i][j].head.color[1] = 0;
                        }
                        fade = gModelVariant628_aConfigs[work->head.command].blue / (n - 1) * step;
                        if (fade < work->domes[i][j].head.color[2]) {
                            work->domes[i][j].head.color[2] -= fade;
                        } else {
                            work->domes[i][j].head.color[2] = 0;
                        }
                    }
                    PushMatrix();
                    for (k = 0; k < 9; k++) {
                        m = GsIDMATRIX;
                        RotMatrixY(rsin(work->domes[i][j].head.elapsed * 80 + k * 500) / 15, &m);
                        for (l = 0; l < 17; l++) {
                            ApplyMatrixSV(&m, &work->dome[l][k], &work->spun[l][k]);
                        }
                    }
                    PopMatrix();
                    for (k = 0; k < 8; k++) {
                        for (l = 0; l < 16; l++) {
                            setPolyGT4(&work->shaded);
                            setSemiTrans(&work->shaded, 1);
                            work->shaded.tpage = work->tpage[2];
                            work->shaded.clut = work->clut[2];
                            setUV4(&work->shaded, work->domes[i][j].head.u, work->domes[i][j].head.v + k * 2,
                                work->domes[i][j].head.u + 63, work->domes[i][j].head.v + k * 2,
                                work->domes[i][j].head.u, work->domes[i][j].head.v + (k + 1) * 2,
                                work->domes[i][j].head.u + 63, work->domes[i][j].head.v + (k + 1) * 2);
                            setRGB0(&work->shaded, work->domes[i][j].head.color[0] / (9 - k),
                                work->domes[i][j].head.color[1] / (9 - k), work->domes[i][j].head.color[2] / (9 - k));
                            setRGB1(&work->shaded, work->domes[i][j].head.color[0] / (9 - k),
                                work->domes[i][j].head.color[1] / (9 - k), work->domes[i][j].head.color[2] / (9 - k));
                            if (k != 7) {
                                setRGB2(&work->shaded, work->domes[i][j].head.color[0] / (8 - k),
                                    work->domes[i][j].head.color[1] / (8 - k), work->domes[i][j].head.color[2] / (8 - k));
                                setRGB3(&work->shaded, work->domes[i][j].head.color[0] / (8 - k),
                                    work->domes[i][j].head.color[1] / (8 - k), work->domes[i][j].head.color[2] / (8 - k));
                            } else {
                                setRGB2(&work->shaded, 0, 0, 0);
                                setRGB3(&work->shaded, 0, 0, 0);
                            }
                            depth = RotAverage4(&work->spun[l][k], &work->spun[l + 1][k],
                                &work->spun[l][k + 1], &work->spun[l + 1][k + 1],
                                (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                                (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    if (work->flags[0] < work->burst_count && work->flags[0] * 20 < work->head.elapsed
                        && work->domes[i][j].head.elapsed < work->domes[i][j].head.duration * 4 / 5) {
                        work->bursts[work->flags[0]][j].head.active = 1;
                        work->bursts[work->flags[0]][j].xform.position = work->domes[i][j].xform.position;
                        work->flags[0]++;
                    }
                    work->domes[i][j].head.u += 0x40;
                    if (work->domes[i][j].head.u == 0xC0) {
                        work->domes[i][j].head.u = 0;
                    }
                    work->domes[i][j].head.elapsed += step;
                    if (work->domes[i][j].head.duration < work->domes[i][j].head.elapsed) {
                        work->domes[i][j].head.active = 0;
                        if (work->flags[4] == 0) {
                            work->flags[4] = 1;
                        }
                    }
                    if (work->flags[4] == 0) {
                        result = 4;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->burst_count; i++) {
                if (work->bursts[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->bursts[i][j].xform.scale);
                    RotMatrixY(work->bursts[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->bursts[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->bursts[i][j].xform.position.vz;
                    unit.super = work->bursts[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->bursts[i][j].xform.rotation.vy -= step * 1500;
                    work->bursts[i][j].xform.scale.vx += 0x5000 / work->bursts[i][j].head.duration * step;
                    work->bursts[i][j].xform.scale.vz += 0x5000 / work->bursts[i][j].head.duration * step;
                    if (work->bursts[i][j].head.duration / 2 < work->bursts[i][j].head.elapsed) {
                        /* Reading each colour before its fade step keeps the
                         * burst address ahead of the table lookup. */
                        color = work->bursts[i][j].head.color[0];
                        fade = gModelVariant628_aConfigs[work->head.command].red / (work->bursts[i][j].head.duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].head.color[0] -= fade;
                        } else {
                            work->bursts[i][j].head.color[0] = 0;
                        }
                        color = work->bursts[i][j].head.color[1];
                        fade = gModelVariant628_aConfigs[work->head.command].green / (work->bursts[i][j].head.duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].head.color[1] -= fade;
                        } else {
                            work->bursts[i][j].head.color[1] = 0;
                        }
                        color = work->bursts[i][j].head.color[2];
                        fade = gModelVariant628_aConfigs[work->head.command].blue / (work->bursts[i][j].head.duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].head.color[2] -= fade;
                        } else {
                            work->bursts[i][j].head.color[2] = 0;
                        }
                    }
                    for (l = 0; l < 16; l++) {
                        for (k = 0; k < 2; k++) {
                            mid[k].vx = (work->ring[l + k].inner.vx + work->ring[l + k].outer.vx) / 2;
                            mid[k].vy = (work->ring[l + k].inner.vy + work->ring[l + k].outer.vy) / 2;
                            mid[k].vz = (work->ring[l + k].inner.vz + work->ring[l + k].outer.vz) / 2;
                        }
                        setPolyGT4(&work->shaded);
                        setSemiTrans(&work->shaded, 1);
                        work->shaded.tpage = work->tpage[2];
                        work->shaded.clut = work->clut[2];
                        setUV4(&work->shaded, work->bursts[i][j].head.u + 0x30, work->bursts[i][j].head.v + 0x10,
                            work->bursts[i][j].head.u + 0x10, work->bursts[i][j].head.v + 0x10,
                            work->bursts[i][j].head.u + 0x30, work->bursts[i][j].head.v + 8,
                            work->bursts[i][j].head.u + 0x10, work->bursts[i][j].head.v + 8);
                        setRGB2(&work->shaded, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1],
                            work->bursts[i][j].head.color[2]);
                        setRGB3(&work->shaded, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1],
                            work->bursts[i][j].head.color[2]);
                        setRGB0(&work->shaded, 0, 0, 0);
                        setRGB1(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&work->ring[l].inner, &work->ring[l + 1].inner, &mid[0], &mid[1],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                        setPolyGT4(&work->shaded);
                        setSemiTrans(&work->shaded, 1);
                        work->shaded.tpage = work->tpage[2];
                        work->shaded.clut = work->clut[2];
                        setUV4(&work->shaded, work->bursts[i][j].head.u + 0x30, work->bursts[i][j].head.v + 8,
                            work->bursts[i][j].head.u + 0x10, work->bursts[i][j].head.v + 8,
                            work->bursts[i][j].head.u + 0x30, work->bursts[i][j].head.v,
                            work->bursts[i][j].head.u + 0x10, work->bursts[i][j].head.v);
                        setRGB0(&work->shaded, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1],
                            work->bursts[i][j].head.color[2]);
                        setRGB1(&work->shaded, work->bursts[i][j].head.color[0], work->bursts[i][j].head.color[1],
                            work->bursts[i][j].head.color[2]);
                        setRGB2(&work->shaded, 0, 0, 0);
                        setRGB3(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&mid[0], &mid[1], &work->ring[l].outer, &work->ring[l + 1].outer,
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                    work->bursts[i][j].head.u += 0x40;
                    if (work->bursts[i][j].head.u == 0xC0) {
                        work->bursts[i][j].head.u = 0;
                    }
                    work->bursts[i][j].head.elapsed += step;
                    if (work->bursts[i][j].head.duration < work->bursts[i][j].head.elapsed) {
                        work->bursts[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        if (work->flags[4] == 1) {
            result = 1;
            work->flags[4] = 2;
        }
        work->head.elapsed += step;
        if (inactive == work->count * (1 + work->spark_count + work->dome_count + work->burst_count)) {
            result = 2;
        }
    }
    return result;
}

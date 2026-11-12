#include "../../types.h"
#include "variant630_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 630's entry: three volleys of glowing orbs leave a model joint
 * and arc out to points round a circle. Each orb that lands leaves a
 * growing, fading sprite edged with a ring of shaded bands, while ten
 * columns of sparks drift out from the joint. The setup also prints the work-area size. */

/* The work-size format string; retail's padding byte after the terminator is 0xAF. */
#ifndef VERSION_FRENCH
#ifndef MODEL630_STRING_PAD
#define MODEL630_STRING_PAD "\xAF"
#endif
static const char gModelVariant630_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL630_STRING_PAD;
#endif

/* Fades channel C of ring i towards black over the second half of its
 * life. The step is spelled out twice rather than kept in a temporary,
 * which keeps retail's register choice. */
#define MODEL630_FADE(c)                                                                         \
    if (gModelVariant630_aConfigs[work->head.command].color[c] / (work->rings[i].head.duration / 2 - 1) * step            \
        < work->rings[i].head.color[c]) {                                                             \
        work->rings[i].head.color[c] -= gModelVariant630_aConfigs[work->head.command].color[c]                            \
            / (work->rings[i].head.duration / 2 - 1) * step;                                          \
    } else {                                                                                     \
        work->rings[i].head.color[c] = 0;                                                             \
    }

s32 ModelVariant630_HandleRequest(u8 *context, s32 command)
{
    Model630State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR rot, vec;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, angle;
    u32 tex;
    long depth;

    work = (Model630State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model630State));
#ifndef VERSION_FRENCH
        printf(gModelVariant630_szWorkSizeFormat, sizeof(Model630State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant630_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 0, &gModelVariant630_aImages[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->count = 10;
            work->rows = 3;
            break;
        }
        work->radius = gModelVariant630_aConfigs[command].radius;
        work->height = gModelVariant630_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->orbs[i].head.duration = 16;
            work->orbs[i].head.active = 0;
            work->orbs[i].head.elapsed = 0;
            work->orbs[i].head.mode = 1;
            work->orbs[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->orbs[i].head.color[0] = gModelVariant630_aConfigs[command].color[0];
            work->orbs[i].head.color[1] = gModelVariant630_aConfigs[command].color[1];
            work->orbs[i].head.color[2] = gModelVariant630_aConfigs[command].color[2];
            work->orbs[i].head.u = 0;
            work->orbs[i].head.v = 32;
            setVector(&work->orbs[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->orbs[i].target,
                work->radius * rsin(i * 56 + (i << 12) / work->count) / 4096 * (work->head.slot * 2 - 1) * (i % 2 * 2 - 1),
                work->radius * rsin(i * 113 + (i << 12) / work->count) / 4096 * (work->head.slot * 2 - 1),
                work->radius * rcos(i * 170 + (i << 12) / work->count) / 4096 * (work->head.slot * 2 - 1));
            switch (command) {
            case 0:
                setVector(&work->orbs[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->orbs[i].xform.position, 0, 0, 0);
                break;
            }
            work->rings[i] = work->orbs[i];
            setVector(&work->rings[i].xform.scale, 0, 0, 0);
            work->rings[i].head.duration = 32;
        }
        for (i = 0; i < 10; i++) {
            work->sparks[i][0].head.duration = 16;
            work->sparks[i][0].head.active = 0;
            work->sparks[i][0].head.elapsed = 0;
            work->sparks[i][0].head.mode = 1;
            work->sparks[i][0].xform.coord = Model_GetCameraCoordinateUnit();
            work->sparks[i][0].head.color[0] = 128;
            work->sparks[i][0].head.color[1] = 128;
            work->sparks[i][0].head.color[2] = 128;
            work->sparks[i][0].head.u = 0;
            work->sparks[i][0].head.v = 0;
            setVector(&work->sparks[i][0].xform.scale, 4096, 4096, 4096);
            angle = (i << 12) / 10;
            setVector(&vec, 20 * rsin(angle) / 4096, 20 * rcos(angle) / 4096, 0);
            switch (command) {
            case 0:
                setVector(&work->sparks[i][0].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->sparks[i][0].xform.position, 0, 0, 0);
                setVector(&rot, -((work->head.slot * 2 - 1) * 455), 0, 0);
                RotMatrix(&rot, &m);
                ApplyMatrix(&m, &vec, &work->sparks[i][0].target);
                break;
            }
            for (k = 0; k < work->rows; k++) {
                work->sparks[i][k] = work->sparks[i][0];
            }
        }
        work->delay = gModelVariant630_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->square[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->square[3], work->radius / 2, work->radius / 2, 0);
        for (i = 0; i < 17; i++) {
            setVector(&work->rim[i][0], work->radius / 2 * rsin(i << 8) / 4096,
                work->radius / 2 * rcos(i << 8) / 4096, 0);
            setVector(&work->rim[i][1], work->radius * rsin(i << 8) / 4096,
                work->radius * rcos(i << 8) / 4096, 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->head.command) {
    case 0:
        if (work->flags[0] < 3 && work->head.elapsed >= work->flags[0] * 8) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 12);
            GsGetLwUnit(&unit, &ls);
            /* "+ -1" keeps retail's negation of the side factor; "- 1"
             * folds into "1 - slot * 2". */
            for (i = work->flags[0] * 3; i < (work->flags[0] + 1) * 3; i++) {
                work->orbs[i].head.active = 1;
                work->orbs[i].xform.position.vx += ls.t[0] * -(work->head.slot * 2 + -1);
                work->orbs[i].xform.position.vy += ls.t[1];
                work->orbs[i].xform.position.vz += ls.t[2] * -(work->head.slot * 2 + -1);
            }
            for (i = 0; i < 10; i++) {
                work->sparks[i][work->flags[0]].head.active = 1;
                work->sparks[i][work->flags[0]].xform.position.vx += ls.t[0];
                work->sparks[i][work->flags[0]].xform.position.vy += ls.t[1];
                work->sparks[i][work->flags[0]].xform.position.vz += ls.t[2];
            }
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->orbs[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->orbs[i].xform.scale);
            RotMatrixZ(work->orbs[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->orbs[i].xform.rotation.vx - (view.pitch - 0x1000) * (work->head.slot * 2 - 1), &unit.matrix);
            /* The 0xC06 yaw offset goes through a variable so that GCC adds
             * it before subtracting the view yaw. */
            angle = 0xC06;
            RotMatrixY(work->orbs[i].xform.rotation.vy + angle - view.yaw, &unit.matrix);
            /* The orb flies from its joint towards its target, offset by 350 in
             * y and 450 in z; adding each constant before the target keeps
             * retail's operand order. */
            unit.matrix.t[0] = ((work->orbs[i].xform.position.vx + work->orbs[i].target.vx) * work->orbs[i].head.elapsed
                    / work->orbs[i].head.duration - work->orbs[i].xform.position.vx) * (work->head.slot * 2 - 1);
            unit.matrix.t[1] = work->orbs[i].xform.position.vy
                - (work->orbs[i].xform.position.vy + 350 + work->orbs[i].target.vy) * work->orbs[i].head.elapsed
                    / work->orbs[i].head.duration;
            unit.matrix.t[2] = ((work->orbs[i].xform.position.vz + 450 + work->orbs[i].target.vz) * work->orbs[i].head.elapsed
                    / work->orbs[i].head.duration - work->orbs[i].xform.position.vz) * (work->head.slot * 2 - 1);
            unit.super = work->orbs[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            setPolyFT4(&work->quad);
            setSemiTrans(&work->quad, 1);
            work->quad.tpage = work->tpage[0];
            work->quad.clut = work->clut[0];
            setUV4(&work->quad, work->orbs[i].head.u, work->orbs[i].head.v,
                work->orbs[i].head.u + 31, work->orbs[i].head.v,
                work->orbs[i].head.u, work->orbs[i].head.v + 31,
                work->orbs[i].head.u + 31, work->orbs[i].head.v + 31);
            setRGB0(&work->quad, work->orbs[i].head.color[0], work->orbs[i].head.color[1], work->orbs[i].head.color[2]);
            depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                (long *)&work->quad.x0, (long *)&work->quad.x1,
                (long *)&work->quad.x2, (long *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            work->orbs[i].head.u += 32;
            if (work->orbs[i].head.u == 96) {
                work->orbs[i].head.u = 0;
            }
            work->orbs[i].head.elapsed += step;
            if (work->orbs[i].head.duration < work->orbs[i].head.elapsed) {
                work->orbs[i].head.active = 0;
                work->rings[i].head.active = 1;
                work->rings[i].xform.position.vx = unit.matrix.t[0];
                work->rings[i].xform.position.vy = unit.matrix.t[1];
                work->rings[i].xform.position.vz = unit.matrix.t[2];
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
            RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].xform.rotation.vx - (view.pitch - 0x1000) * (work->head.slot * 2 - 1), &unit.matrix);
            angle = 0xC06;
            RotMatrixY(work->rings[i].xform.rotation.vy + angle - view.yaw, &unit.matrix);
            unit.matrix.t[0] = work->rings[i].xform.position.vx;
            unit.matrix.t[1] = work->rings[i].xform.position.vy;
            unit.matrix.t[2] = work->rings[i].xform.position.vz;
            unit.super = work->rings[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->rings[i].xform.scale.vx += 0x5000 / work->rings[i].head.duration * step;
            work->rings[i].xform.scale.vy += 0x5000 / work->rings[i].head.duration * step;
            work->rings[i].xform.scale.vz += 0x5000 / work->rings[i].head.duration * step;
            if (work->rings[i].head.elapsed > work->rings[i].head.duration / 2) {
                MODEL630_FADE(0);
                MODEL630_FADE(1);
                MODEL630_FADE(2);
            }
            setPolyFT4(&work->quad);
            setSemiTrans(&work->quad, 1);
            work->quad.tpage = work->tpage[0];
            work->quad.clut = work->clut[0];
            setUV4(&work->quad, work->rings[i].head.u, work->rings[i].head.v,
                work->rings[i].head.u + 31, work->rings[i].head.v,
                work->rings[i].head.u, work->rings[i].head.v + 31,
                work->rings[i].head.u + 31, work->rings[i].head.v + 31);
            setRGB0(&work->quad, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
            depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                (long *)&work->quad.x0, (long *)&work->quad.x1,
                (long *)&work->quad.x2, (long *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            /* The band loop reuses k rather than a separate counter; the extra
             * uses keep retail's register choice for the spark loops. */
            for (k = 0; k < 16; k++) {
                setPolyG4(&work->band);
                setRGB0(&work->band, 0, 0, 0);
                setRGB1(&work->band, 0, 0, 0);
                setRGB2(&work->band, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                setRGB3(&work->band, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                depth = RotAverage4(&work->rim[k][0], &work->rim[k + 1][0], &work->rim[k][1], &work->rim[k + 1][1],
                    (long *)&work->band.x0, (long *)&work->band.x1,
                    (long *)&work->band.x2, (long *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->band, Graphics_GetDefaultOrderingTable(), (u16)depth, work->rings[i].head.mode);
                }
            }
            work->rings[i].head.u += 32;
            if (work->rings[i].head.u == 96) {
                work->rings[i].head.u = 0;
            }
            work->rings[i].head.elapsed += step;
            if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                work->rings[i].head.active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < 10; i++) {
        for (k = 0; k < work->rows; k++) {
            if (work->sparks[i][k].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->sparks[i][k].xform.scale);
                RotMatrixZ(work->sparks[i][k].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i][k].xform.rotation.vx - (view.pitch - 0x1000) * (work->head.slot * 2 - 1), &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->sparks[i][k].xform.rotation.vy + angle - view.yaw, &unit.matrix);
                unit.matrix.t[0] = work->sparks[i][k].xform.position.vx;
                unit.matrix.t[1] = work->sparks[i][k].xform.position.vy;
                unit.matrix.t[2] = work->sparks[i][k].xform.position.vz;
                unit.super = work->sparks[i][k].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->sparks[i][k].xform.position.vx += work->sparks[i][k].target.vx * step / 2;
                work->sparks[i][k].xform.position.vy += work->sparks[i][k].target.vy * step / 2;
                work->sparks[i][k].xform.position.vz += work->sparks[i][k].target.vz * step / 2;
                setPolyFT4(&work->quad);
                setSemiTrans(&work->quad, 1);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setUV4(&work->quad, work->sparks[i][k].head.u, work->sparks[i][k].head.v,
                    work->sparks[i][k].head.u + 31, work->sparks[i][k].head.v,
                    work->sparks[i][k].head.u, work->sparks[i][k].head.v + 31,
                    work->sparks[i][k].head.u + 31, work->sparks[i][k].head.v + 31);
                setRGB0(&work->quad, work->sparks[i][k].head.color[0], work->sparks[i][k].head.color[1],
                    work->sparks[i][k].head.color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (long *)&work->quad.x0, (long *)&work->quad.x1,
                    (long *)&work->quad.x2, (long *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->sparks[i][k].head.u += 32;
                if (work->sparks[i][k].head.u == 224) {
                    work->sparks[i][k].head.u = 0;
                }
                work->sparks[i][k].head.elapsed += step;
                if (work->sparks[i][k].head.duration < work->sparks[i][k].head.elapsed) {
                    work->sparks[i][k].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == (work->rows * 5 + work->count) * 2) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

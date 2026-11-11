#include "../../types.h"
#include "variant675us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 675's entry: a pulsing ring of shaded strips spins over a fan of
 * triangles while bolts of light leap from the model out to a circle of
 * targets, leaving fading line trails; each landed bolt raises a curtain of
 * sparks between its path and the ground. The setup also prints the
 * work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x8F in slot zero. */
#ifndef MODEL675_STRING_PAD
#define MODEL675_STRING_PAD "\x8F"
#endif
#ifndef VERSION_FRENCH
static const char gModelVariant675Us_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL675_STRING_PAD;
#endif

s32 ModelVariant675Us_HandleRequest(u8 *context, s32 command)
{
    Model675UsState *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    SVECTOR tip, mid;
    SVECTOR mids[2];
    u8 u[3], v[3];
    long interpolation, flag, dummy;
    s32 result, i, step, inactive;
    s32 j, k, m, n, d;
    u32 tex;
    long depth;
    s32 frame;
    s32 red, green, blue;

    work = (Model675UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model675UsState));
#ifndef VERSION_FRENCH
        printf(gModelVariant675Us_szWorkSizeFormat, sizeof(Model675UsState));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        Model_CopySlotPosition(work->head.slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant675Us_aImages[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant675Us_aImages[1]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant675Us_aImages[4]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant675Us_aImages[0]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant675Us_aImages[3]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->ring_count = 1;
            work->bolt_count = 16;
            work->trail_count = 8;
            break;
        }
        work->place.radius = gModelVariant675Us_aConfigs[command].radius;
        work->place.height = gModelVariant675Us_aConfigs[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            switch (command) {
            case 0:
                work->rings[i].head.duration = 0x258;
                break;
            case 1:
                work->rings[i].head.duration = 0x140;
                break;
            case 2:
                work->rings[i].head.duration = 0x182;
                break;
            }
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant675Us_aConfigs[command].red;
            work->rings[i].head.color[1] = gModelVariant675Us_aConfigs[command].green;
            work->rings[i].head.color[2] = gModelVariant675Us_aConfigs[command].blue;
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 0;
            setVector(&work->rings[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
            work->rings[i].ticks = 0;
        }
        for (i = 0; i < work->bolt_count; i++) {
            work->bolts[i].head.active = 0;
            work->bolts[i].head.elapsed = 0;
            work->bolts[i].head.duration = 0x20;
            work->bolts[i].head.mode = 1;
            work->bolts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bolts[i].head.color[0] = gModelVariant675Us_aConfigs[command].bolt_red;
            work->bolts[i].head.color[1] = gModelVariant675Us_aConfigs[command].bolt_green;
            work->bolts[i].head.color[2] = gModelVariant675Us_aConfigs[command].bolt_blue;
            setVector(&work->bolts[i].xform.scale, 0x1000, 0x1000, 0x1000);
            d = i % 2 * 2 - 1;
            n = work->place.height / 3;
            setVector(&work->bolts[i].growth,
                d * n * rsin((i << 12) / work->bolt_count) / 4096 + work->origin.vx, 0,
                n * rcos((i << 12) / work->bolt_count) / 4096 + work->origin.vz * 2);
            setVector(&work->bolts[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->bolts[i].xform.position, 0, 0, 0);
            work->bolts[i].ticks = 0;
        }
        for (k = 0; k < work->bolt_count; k++) {
            for (i = 0; i < work->trail_count; i++) {
                work->trails[i][k].head.active = 0;
                work->trails[i][k].head.elapsed = 0;
                work->trails[i][k].head.duration = 0x10;
                work->trails[i][k].head.mode = 1;
                work->trails[i][k].xform.coord = Model_GetCameraCoordinateUnit();
                work->trails[i][k].head.color[0] = gModelVariant675Us_aConfigs[command].bolt_red;
                work->trails[i][k].head.color[1] = gModelVariant675Us_aConfigs[command].bolt_green;
                work->trails[i][k].head.color[2] = gModelVariant675Us_aConfigs[command].bolt_blue;
                setVector(&work->trails[i][k].xform.scale, 0x1000, 0x1000, 0x1000);
                setVector(&work->trails[i][k].growth, 0, 0, 0);
                setVector(&work->trails[i][k].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->trails[i][k].xform.position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            work->sparks[i].head.active = 0;
            work->sparks[i].head.elapsed = 0;
            work->sparks[i].head.duration = 0x100;
            work->sparks[i].head.mode = 1;
            work->sparks[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].head.color[0] = gModelVariant675Us_aConfigs[command].bolt_red;
            work->sparks[i].head.color[1] = gModelVariant675Us_aConfigs[command].bolt_green;
            work->sparks[i].head.color[2] = gModelVariant675Us_aConfigs[command].bolt_blue;
            work->sparks[i].head.u = 0;
            work->sparks[i].head.v = 0x80;
            setVector(&work->sparks[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i].growth, 0, 0, 0);
            setVector(&work->sparks[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->sparks[i].xform.position, 0, 0, 0);
        }
        work->place.delay = gModelVariant675Us_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->place.radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->place.radius;
        d = work->place.radius >> 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_upper[i], n * rsin(i << 8) / 4096, d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_lower[i], n * rsin(i << 8) / 4096, -d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_middle[i], (work->ring_upper[i].vx + work->ring_lower[i].vx) / 2,
                (work->ring_upper[i].vy + work->ring_lower[i].vy) / 2,
                (work->ring_upper[i].vz + work->ring_lower[i].vz) / 2);
        }
        n = work->place.radius * 2;
        for (i = 0; i < 17; i++) {
            setVector(&work->fan[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
        }
        setVector(&work->center, 0, 0, 0);
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                work->rings[0].head.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 8);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 5 + 0x168 < work->head.elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].head.active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 8);
                GsGetLwUnit(&unit, &ls);
                setVector(&work->bolts[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->head.elapsed == 0) {
                work->rings[0].head.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 8);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 10 + 0x50 < work->head.elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].head.active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 8);
                GsGetLwUnit(&unit, &ls);
                setVector(&work->bolts[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->head.elapsed == 0) {
                work->rings[0].head.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 1);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 10 + 0x64 < work->head.elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].head.active = 1;
                work->flags[0]++;
            }
            for (i = 0; i < work->bolt_count; i++) {
                setVector(&work->bolts[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->rings[i].head.elapsed < work->rings[i].head.duration / 2) {
                    n = (work->rings[i].head.elapsed << 13) / work->rings[i].head.duration;
                } else {
                    n = 0x1000;
                }
                if ((work->rings[i].ticks & 1) == 0) {
                    n /= 2;
                }
                setVector(&work->rings[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].xform.position.vx;
                unit.matrix.t[1] = work->rings[i].xform.position.vy;
                unit.matrix.t[2] = work->rings[i].xform.position.vz;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].head.duration * 9 / 10 < work->rings[i].head.elapsed) {
                    n = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 10;
                    d = work->rings[i].head.duration;
                    work->rings[i].head.color[0] = gModelVariant675Us_aConfigs[work->head.command].red * n / d;
                    work->rings[i].head.color[1] = gModelVariant675Us_aConfigs[work->head.command].green * n / d;
                    work->rings[i].head.color[2] = gModelVariant675Us_aConfigs[work->head.command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->rings[i].head.u, work->rings[i].head.v, work->rings[i].head.u + 63,
                    work->rings[i].head.v, work->rings[i].head.u, work->rings[i].head.v + 63,
                    work->rings[i].head.u + 63, work->rings[i].head.v + 63);
                setRGB0(&work->flat, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (long *)&work->flat.x0, (long *)&work->flat.x1,
                    (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                for (k = 0; k < 8; k++) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    if (work->rings[i].head.elapsed < work->rings[i].head.duration / 2) {
                        n = (work->rings[i].head.elapsed << 13) / work->rings[i].head.duration;
                    } else {
                        n = 0x1000;
                    }
                    j = k + 1;
                    d = n * rsin(j << 7) / 4096;
                    n = n * rcos(k << 7) / 4096;
                    setVector(&work->rings[i].xform.scale, n, d, n);
                    ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                    n = k % 2 * 2 - 1;
                    d = k << 9;
                    /* The shift spellings stop GCC reassociating the (k + 1) multipliers. */
                    RotMatrixY(work->rings[i].head.elapsed * ((j << 2) + j) * n + d, &unit.matrix);
                    RotMatrixZ(work->rings[i].head.elapsed * (((j << 1) + j) << 1) * n + d, &unit.matrix);
                    RotMatrixX(work->rings[i].head.elapsed * ((j << 3) - j) * n + d, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i].xform.position.vx;
                    unit.matrix.t[1] = work->rings[i].xform.position.vy;
                    unit.matrix.t[2] = work->rings[i].xform.position.vz;
                    unit.super = work->rings[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (m = 0; m < 16; m++) {
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->tpage[1];
                        work->shaded.clut = work->clut[1];
                        setSemiTrans(&work->shaded, 1);
                        u[0] = work->rings[i].head.u;
                        v[0] = work->rings[i].head.v + 128;
                        u[1] = work->rings[i].head.u + 63;
                        v[1] = work->rings[i].head.v + 191;
                        setUV4(&work->shaded, u[0], v[0], u[1], v[0], u[0], v[1], u[1], v[1]);
                        setRGB0(&work->shaded, 0, 0, 0);
                        setRGB1(&work->shaded, 0, 0, 0);
                        setRGB2(&work->shaded, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                        setRGB3(&work->shaded, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                        depth = RotAverage4(&work->ring_upper[m], &work->ring_upper[m + 1],
                            &work->ring_middle[m], &work->ring_middle[m + 1],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->tpage[1];
                        work->shaded.clut = work->clut[1];
                        setSemiTrans(&work->shaded, 1);
                        setUV4(&work->shaded, u[0], v[0], u[1], v[0], u[0], v[1], u[1], v[1]);
                        setRGB2(&work->shaded, 0, 0, 0);
                        setRGB3(&work->shaded, 0, 0, 0);
                        setRGB0(&work->shaded, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                        setRGB1(&work->shaded, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                        depth = RotAverage4(&work->ring_middle[m], &work->ring_middle[m + 1],
                            &work->ring_lower[m], &work->ring_lower[m + 1],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->rings[i].head.elapsed < work->rings[i].head.duration / 2) {
                    n = (work->rings[i].head.elapsed << 13) / work->rings[i].head.duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->rings[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].xform.position.vx;
                unit.matrix.t[1] = work->rings[i].xform.position.vy;
                unit.matrix.t[2] = work->rings[i].xform.position.vz;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                u[1] = work->rings[i].head.u + 64;
                frame = work->rings[i].head.elapsed % 64 * 63 / 64;
                v[1] = frame - 128;
                u[2] = work->rings[i].head.u + 127;
                v[2] = v[1];
                u[0] = work->rings[i].head.u + 96;
                v[0] = frame - 64;
                for (k = 0; k < 16; k++) {
                    setPolyGT3(&work->fan_tri);
                    work->fan_tri.tpage = work->tpage[2];
                    work->fan_tri.clut = work->clut[2];
                    setSemiTrans(&work->fan_tri, 1);
                    setUV3(&work->fan_tri, u[0], v[0], u[1], v[1], u[2], v[2]);
                    setRGB0(&work->fan_tri, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    setRGB1(&work->fan_tri, 0, 0, 0);
                    setRGB2(&work->fan_tri, 0, 0, 0);
                    depth = RotAverage3(&work->center, &work->fan[k], &work->fan[k + 1],
                        (long *)&work->fan_tri.x0, (long *)&work->fan_tri.x1,
                        (long *)&work->fan_tri.x2, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan_tri, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->rings[i].head.elapsed += step;
                work->rings[i].ticks++;
                if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                    work->rings[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            if (work->bolts[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->bolts[i].xform.scale);
                unit.matrix.t[0] = work->bolts[i].xform.position.vx;
                unit.matrix.t[1] = work->bolts[i].xform.position.vy;
                unit.matrix.t[2] = work->bolts[i].xform.position.vz;
                unit.super = work->bolts[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->line.attribute = 0x50000000;
                work->line.r = work->bolts[i].head.color[0];
                work->line.g = work->bolts[i].head.color[1];
                work->line.b = work->bolts[i].head.color[2];
                d = i % 2 * 2 - 1;
                n = work->place.radius;
                tip.vx = (work->bolts[i].growth.vx - work->bolts[i].xform.position.vx) * work->bolts[i].head.elapsed
                    / work->bolts[i].head.duration
                    + n * rsin(d * (work->bolts[i].head.elapsed << 12) / work->bolts[i].head.duration
                        + (i << 12) / work->bolt_count) / 4096;
                tip.vy = work->bolts[i].xform.position.vy * -1;
                tip.vz = (work->bolts[i].growth.vz - work->bolts[i].xform.position.vz) * work->bolts[i].head.elapsed
                    / work->bolts[i].head.duration;
                mid.vx = (tip.vx + work->center.vx) / 2;
                mid.vy = (tip.vy + work->center.vy) / 2;
                mid.vz = (tip.vz + work->center.vz) / 2;
                RotTransPers(&work->center, (long *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&tip, (long *)&work->line.x1, &interpolation, &flag);
                depth = RotTransPers(&mid, &dummy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->bolts[i].head.elapsed += step;
                if (work->bolts[i].head.duration < work->bolts[i].head.elapsed) {
                    work->bolts[i].head.active = 0;
                    work->sparks[i].head.active = 1;
                    work->sparks[i].xform.position = work->bolts[i].xform.position;
                    work->sparks[i].xform.position.vy = 0;
                    k = 0;
                    do {
                        n = work->place.radius;
                        setVector(&work->curtain_upper[k][i],
                            (work->bolts[i].growth.vx - work->bolts[i].xform.position.vx) * k / 8
                                + n * rsin(d * (k << 12) / 8 + (i << 12) / work->bolt_count) / 4096,
                            0, (work->bolts[i].growth.vz - work->bolts[i].xform.position.vz) * k / 8);
                        work->curtain_lower[k][i] = work->curtain_upper[k][i];
                        /* This sum is never used and GCC deletes it after loop optimisation,
                         * but its size keeps the bolt address from being hoisted out of
                         * this loop. */
                        j = work->sparks[i].growth.vy * k * d;
                        j = work->sparks[i].xform.position.vy * k * d + j;
                        j = work->sparks[i].xform.scale.vx * k * d + j;
                        j = work->sparks[i].xform.scale.vy * k * d + j;
                        j = work->sparks[i].xform.scale.vz * k * d + j;
                        j = work->sparks[i].xform.rotation.vx * k * d + j;
                        j = work->sparks[i].xform.rotation.vy * k * d + j;
                        j = work->sparks[i].xform.rotation.vz * k * d + j;
                        j = work->sparks[i].growth.vx * k * d + j;
                        j = work->sparks[i].xform.position.vx * k * d + j;
                        {
                            s32 h;

                            n = rcos(k << 7);
                            h = work->place.height;
                            work->curtain_lower[k][i].vy = -h + h * n / 4096;
                        }
                    } while (++k < 9);
                }
                work->trails[work->bolts[i].ticks][i].head.active = 1;
                work->trails[work->bolts[i].ticks][i].head.elapsed = 0;
                work->trails[work->bolts[i].ticks][i].xform.position = work->bolts[i].xform.position;
                setVector(&work->trails[work->bolts[i].ticks][i].growth, tip.vx, tip.vy, tip.vz);
                work->trails[work->bolts[i].ticks][i].head.color[0] = work->bolts[i].head.color[0];
                work->trails[work->bolts[i].ticks][i].head.color[1] = work->bolts[i].head.color[1];
                work->trails[work->bolts[i].ticks][i].head.color[2] = work->bolts[i].head.color[2];
                work->bolts[i].ticks++;
                if (work->bolts[i].ticks >= work->trail_count) {
                    work->bolts[i].ticks = 0;
                }
            } else {
                inactive++;
            }
        }
        for (k = 0; k < work->bolt_count; k++) {
            for (i = 0; i < work->trail_count; i++) {
                if (work->trails[i][k].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->trails[i][k].xform.scale);
                    unit.matrix.t[0] = work->trails[i][k].xform.position.vx;
                    unit.matrix.t[1] = work->trails[i][k].xform.position.vy;
                    unit.matrix.t[2] = work->trails[i][k].xform.position.vz;
                    unit.super = work->trails[i][k].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    n = work->trails[i][k].head.duration - work->trails[i][k].head.elapsed;
                    d = work->trails[i][k].head.duration;
                    red = work->trails[i][k].head.color[0] * n / d;
                    green = work->trails[i][k].head.color[1] * n / d;
                    blue = work->trails[i][k].head.color[2] * n / d;
                    work->line.attribute = 0x50000000;
                    work->line.r = red;
                    work->line.g = green;
                    work->line.b = blue;
                    tip.vx = work->trails[i][k].growth.vx;
                    tip.vy = work->trails[i][k].growth.vy;
                    tip.vz = work->trails[i][k].growth.vz;
                    mid.vx = (tip.vx + work->center.vx) / 2;
                    mid.vy = (tip.vy + work->center.vy) / 2;
                    mid.vz = (tip.vz + work->center.vz) / 2;
                    RotTransPers(&work->center, (long *)&work->line.x0, &interpolation, &flag);
                    RotTransPers(&tip, (long *)&work->line.x1, &interpolation, &flag);
                    depth = RotTransPers(&mid, &dummy, &interpolation, &flag);
                    if (flag >= 0 && work->trails[i][k].head.elapsed != 0) {
                        GsSortLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    work->trails[i][k].head.elapsed += step;
                    if (work->trails[i][k].head.duration < work->trails[i][k].head.elapsed) {
                        work->trails[i][k].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            if (work->sparks[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                if (work->sparks[i].head.elapsed < work->sparks[i].head.duration / 16) {
                    n = (work->sparks[i].head.elapsed << 16) / work->sparks[i].head.duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->sparks[i].xform.scale, 0x1000, n, 0x1000);
                ScaleMatrix(&unit.matrix, &work->sparks[i].xform.scale);
                unit.matrix.t[0] = work->sparks[i].xform.position.vx;
                unit.matrix.t[1] = work->sparks[i].xform.position.vy;
                unit.matrix.t[2] = work->sparks[i].xform.position.vz;
                unit.super = work->sparks[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i].head.duration * 9 / 10 < work->sparks[i].head.elapsed) {
                    n = (work->sparks[i].head.duration - work->sparks[i].head.elapsed) * 10;
                    d = work->sparks[i].head.duration;
                    work->sparks[i].head.color[0] = gModelVariant675Us_aConfigs[work->head.command].bolt_red * n / d;
                    work->sparks[i].head.color[1] = gModelVariant675Us_aConfigs[work->head.command].bolt_green * n / d;
                    work->sparks[i].head.color[2] = gModelVariant675Us_aConfigs[work->head.command].bolt_blue * n / d;
                }
                work->sparks[i].head.v = (work->sparks[i].head.elapsed + i * 100) % 32 * 2 - 128;
                for (k = 0; k < 8; k++) {
                    for (m = 0; m < 2; m++) {
                        mids[m] = work->curtain_upper[k + m][i];
                        mids[m].vy = (work->curtain_upper[k + m][i].vy + work->curtain_lower[k + m][i].vy) / 2;
                    }
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[1];
                    work->shaded.clut = work->clut[1];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, work->sparks[i].head.u, work->sparks[i].head.v + 63,
                        work->sparks[i].head.u + 63, work->sparks[i].head.v + 63, work->sparks[i].head.u,
                        work->sparks[i].head.v, work->sparks[i].head.u + 63, work->sparks[i].head.v);
                    setRGB0(&work->shaded, 0, 0, 0);
                    setRGB1(&work->shaded, 0, 0, 0);
                    setRGB2(&work->shaded, work->sparks[i].head.color[0], work->sparks[i].head.color[1], work->sparks[i].head.color[2]);
                    if (k == 7) {
                        setRGB3(&work->shaded, 0, 0, 0);
                    } else {
                        setRGB3(&work->shaded, work->sparks[i].head.color[0], work->sparks[i].head.color[1], work->sparks[i].head.color[2]);
                    }
                    depth = RotAverage4(&work->curtain_upper[k][i], &work->curtain_upper[k + 1][i],
                        &mids[0], &mids[1],
                        (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                        (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[1];
                    work->shaded.clut = work->clut[1];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, work->sparks[i].head.u, work->sparks[i].head.v + 63,
                        work->sparks[i].head.u + 63, work->sparks[i].head.v + 63, work->sparks[i].head.u,
                        work->sparks[i].head.v, work->sparks[i].head.u + 63, work->sparks[i].head.v);
                    setRGB2(&work->shaded, 0, 0, 0);
                    setRGB3(&work->shaded, 0, 0, 0);
                    setRGB0(&work->shaded, work->sparks[i].head.color[0], work->sparks[i].head.color[1], work->sparks[i].head.color[2]);
                    if (k == 7) {
                        setRGB1(&work->shaded, 0, 0, 0);
                    } else {
                        setRGB1(&work->shaded, work->sparks[i].head.color[0], work->sparks[i].head.color[1], work->sparks[i].head.color[2]);
                    }
                    depth = RotAverage4(&mids[0], &mids[1],
                        &work->curtain_lower[k][i], &work->curtain_lower[k + 1][i],
                        (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                        (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->sparks[i].head.elapsed += step;
                if (work->flags[4] == 0) {
                    result = 4;
                }
                if (work->sparks[i].head.duration < work->sparks[i].head.elapsed) {
                    work->sparks[i].head.active = 0;
                    if (work->flags[4] == 0 && i == work->bolt_count - 1) {
                        result = 1;
                        work->flags[4] = result;
                    }
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->ring_count + work->bolt_count * (work->trail_count + 2)) {
            result = 2;
        }
    }
    return result;
}

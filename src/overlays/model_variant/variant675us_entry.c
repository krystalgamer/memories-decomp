#include "../../types.h"
#include "variant675us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

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
static const char D_8013DAE0[24] = "E_DATA size = %x byte\n\0" MODEL675_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model675UsState *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model675UsViewSnapshot view;
    SVECTOR tip, mid;
    SVECTOR mids[2];
    u8 u[3], v[3];
    PSXLONG interpolation, flag, dummy;
    s32 result, i, step, inactive;
    s32 j, k, m, n, d;
    u32 tex;
    PSXLONG depth;
    s32 frame;
    s32 red, green, blue;

    work = (Model675UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model675UsState));
#ifndef VERSION_FRENCH
        printf(D_8013DAE0, sizeof(Model675UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAF8[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAF8[1]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAF8[4]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAF8[0]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAF8[3]);
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
        work->radius = D_8013DB84[command].radius;
        work->height = D_8013DB84[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            switch (command) {
            case 0:
                work->rings[i].duration = 0x258;
                break;
            case 1:
                work->rings[i].duration = 0x140;
                break;
            case 2:
                work->rings[i].duration = 0x182;
                break;
            }
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].red = D_8013DB84[command].red;
            work->rings[i].green = D_8013DB84[command].green;
            work->rings[i].blue = D_8013DB84[command].blue;
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
            work->rings[i].ticks = 0;
        }
        for (i = 0; i < work->bolt_count; i++) {
            work->bolts[i].active = 0;
            work->bolts[i].elapsed = 0;
            work->bolts[i].duration = 0x20;
            work->bolts[i].mode = 1;
            work->bolts[i].coord = Model_GetCameraCoordinateUnit();
            work->bolts[i].red = D_8013DB84[command].bolt_red;
            work->bolts[i].green = D_8013DB84[command].bolt_green;
            work->bolts[i].blue = D_8013DB84[command].bolt_blue;
            setVector(&work->bolts[i].scale, 0x1000, 0x1000, 0x1000);
            d = i % 2 * 2 - 1;
            n = work->height / 3;
            setVector(&work->bolts[i].growth,
                d * n * rsin((i << 12) / work->bolt_count) / 4096 + work->origin.vx, 0,
                n * rcos((i << 12) / work->bolt_count) / 4096 + work->origin.vz * 2);
            setVector(&work->bolts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bolts[i].position, 0, 0, 0);
            work->bolts[i].ticks = 0;
        }
        for (k = 0; k < work->bolt_count; k++) {
            for (i = 0; i < work->trail_count; i++) {
                work->trails[i][k].active = 0;
                work->trails[i][k].elapsed = 0;
                work->trails[i][k].duration = 0x10;
                work->trails[i][k].mode = 1;
                work->trails[i][k].coord = Model_GetCameraCoordinateUnit();
                work->trails[i][k].red = D_8013DB84[command].bolt_red;
                work->trails[i][k].green = D_8013DB84[command].bolt_green;
                work->trails[i][k].blue = D_8013DB84[command].bolt_blue;
                setVector(&work->trails[i][k].scale, 0x1000, 0x1000, 0x1000);
                setVector(&work->trails[i][k].growth, 0, 0, 0);
                setVector(&work->trails[i][k].rotation, 0, work->slot * 2048, 0);
                setVector(&work->trails[i][k].position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].duration = 0x100;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].red = D_8013DB84[command].bolt_red;
            work->sparks[i].green = D_8013DB84[command].bolt_green;
            work->sparks[i].blue = D_8013DB84[command].bolt_blue;
            work->sparks[i].u = 0;
            work->sparks[i].v = 0x80;
            setVector(&work->sparks[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i].growth, 0, 0, 0);
            setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sparks[i].position, 0, 0, 0);
        }
        work->delay = D_8013DB84[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->radius;
        d = work->radius >> 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_upper[i], n * rsin(i << 8) / 4096, d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_lower[i], n * rsin(i << 8) / 4096, -d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_middle[i], (work->ring_upper[i].vx + work->ring_lower[i].vx) / 2,
                (work->ring_upper[i].vy + work->ring_lower[i].vy) / 2,
                (work->ring_upper[i].vz + work->ring_lower[i].vz) / 2);
        }
        n = work->radius * 2;
        for (i = 0; i < 17; i++) {
            setVector(&work->fan[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
        }
        setVector(&work->center, 0, 0, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                work->rings[0].active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 5 + 0x168 < work->elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                setVector(&work->bolts[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->elapsed == 0) {
                work->rings[0].active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 10 + 0x50 < work->elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                setVector(&work->bolts[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed == 0) {
                work->rings[0].active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].position, ls.t[0], ls.t[1], ls.t[2]);
            if (work->flags[0] * 10 + 0x64 < work->elapsed && work->flags[0] < work->bolt_count) {
                work->bolts[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            for (i = 0; i < work->bolt_count; i++) {
                setVector(&work->bolts[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            break;
        }
        PushMatrix();
        view = *(Model675UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->rings[i].elapsed < work->rings[i].duration / 2) {
                    n = (work->rings[i].elapsed << 13) / work->rings[i].duration;
                } else {
                    n = 0x1000;
                }
                if ((work->rings[i].ticks & 1) == 0) {
                    n /= 2;
                }
                setVector(&work->rings[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].duration * 9 / 10 < work->rings[i].elapsed) {
                    n = (work->rings[i].duration - work->rings[i].elapsed) * 10;
                    d = work->rings[i].duration;
                    work->rings[i].red = D_8013DB84[work->command].red * n / d;
                    work->rings[i].green = D_8013DB84[work->command].green * n / d;
                    work->rings[i].blue = D_8013DB84[work->command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->rings[i].u, work->rings[i].v, work->rings[i].u + 63,
                    work->rings[i].v, work->rings[i].u, work->rings[i].v + 63,
                    work->rings[i].u + 63, work->rings[i].v + 63);
                setRGB0(&work->flat, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                for (k = 0; k < 8; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    if (work->rings[i].elapsed < work->rings[i].duration / 2) {
                        n = (work->rings[i].elapsed << 13) / work->rings[i].duration;
                    } else {
                        n = 0x1000;
                    }
                    j = k + 1;
                    d = n * rsin(j << 7) / 4096;
                    n = n * rcos(k << 7) / 4096;
                    setVector(&work->rings[i].scale, n, d, n);
                    ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                    n = k % 2 * 2 - 1;
                    d = k << 9;
                    /* The shift spellings stop GCC reassociating the (k + 1) multipliers. */
                    RotMatrixY(work->rings[i].elapsed * ((j << 2) + j) * n + d, &unit.matrix);
                    RotMatrixZ(work->rings[i].elapsed * (((j << 1) + j) << 1) * n + d, &unit.matrix);
                    RotMatrixX(work->rings[i].elapsed * ((j << 3) - j) * n + d, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i].position.vx;
                    unit.matrix.t[1] = work->rings[i].position.vy;
                    unit.matrix.t[2] = work->rings[i].position.vz;
                    unit.super = work->rings[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (m = 0; m < 16; m++) {
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->tpage[1];
                        work->shaded.clut = work->clut[1];
                        setSemiTrans(&work->shaded, 1);
                        u[0] = work->rings[i].u;
                        v[0] = work->rings[i].v + 128;
                        u[1] = work->rings[i].u + 63;
                        v[1] = work->rings[i].v + 191;
                        setUV4(&work->shaded, u[0], v[0], u[1], v[0], u[0], v[1], u[1], v[1]);
                        setRGB0(&work->shaded, 0, 0, 0);
                        setRGB1(&work->shaded, 0, 0, 0);
                        setRGB2(&work->shaded, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                        setRGB3(&work->shaded, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                        depth = RotAverage4(&work->ring_upper[m], &work->ring_upper[m + 1],
                            &work->ring_middle[m], &work->ring_middle[m + 1],
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                        }
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->tpage[1];
                        work->shaded.clut = work->clut[1];
                        setSemiTrans(&work->shaded, 1);
                        setUV4(&work->shaded, u[0], v[0], u[1], v[0], u[0], v[1], u[1], v[1]);
                        setRGB2(&work->shaded, 0, 0, 0);
                        setRGB3(&work->shaded, 0, 0, 0);
                        setRGB0(&work->shaded, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                        setRGB1(&work->shaded, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                        depth = RotAverage4(&work->ring_middle[m], &work->ring_middle[m + 1],
                            &work->ring_lower[m], &work->ring_lower[m + 1],
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                        }
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->rings[i].elapsed < work->rings[i].duration / 2) {
                    n = (work->rings[i].elapsed << 13) / work->rings[i].duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->rings[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                u[1] = work->rings[i].u + 64;
                frame = work->rings[i].elapsed % 64 * 63 / 64;
                v[1] = frame - 128;
                u[2] = work->rings[i].u + 127;
                v[2] = v[1];
                u[0] = work->rings[i].u + 96;
                v[0] = frame - 64;
                for (k = 0; k < 16; k++) {
                    setPolyGT3(&work->fan_tri);
                    work->fan_tri.tpage = work->tpage[2];
                    work->fan_tri.clut = work->clut[2];
                    setSemiTrans(&work->fan_tri, 1);
                    setUV3(&work->fan_tri, u[0], v[0], u[1], v[1], u[2], v[2]);
                    setRGB0(&work->fan_tri, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB1(&work->fan_tri, 0, 0, 0);
                    setRGB2(&work->fan_tri, 0, 0, 0);
                    depth = RotAverage3(&work->center, &work->fan[k], &work->fan[k + 1],
                        (PSXLONG *)&work->fan_tri.x0, (PSXLONG *)&work->fan_tri.x1,
                        (PSXLONG *)&work->fan_tri.x2, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan_tri, func_80058F10(), (u16)depth);
                    }
                }
                work->rings[i].elapsed += step;
                work->rings[i].ticks++;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            if (work->bolts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->bolts[i].scale);
                unit.matrix.t[0] = work->bolts[i].position.vx;
                unit.matrix.t[1] = work->bolts[i].position.vy;
                unit.matrix.t[2] = work->bolts[i].position.vz;
                unit.super = work->bolts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->line.attribute = 0x50000000;
                work->line.r = work->bolts[i].red;
                work->line.g = work->bolts[i].green;
                work->line.b = work->bolts[i].blue;
                d = i % 2 * 2 - 1;
                n = work->radius;
                tip.vx = (work->bolts[i].growth.vx - work->bolts[i].position.vx) * work->bolts[i].elapsed
                    / work->bolts[i].duration
                    + n * rsin(d * (work->bolts[i].elapsed << 12) / work->bolts[i].duration
                        + (i << 12) / work->bolt_count) / 4096;
                tip.vy = work->bolts[i].position.vy * -1;
                tip.vz = (work->bolts[i].growth.vz - work->bolts[i].position.vz) * work->bolts[i].elapsed
                    / work->bolts[i].duration;
                mid.vx = (tip.vx + work->center.vx) / 2;
                mid.vy = (tip.vy + work->center.vy) / 2;
                mid.vz = (tip.vz + work->center.vz) / 2;
                RotTransPers(&work->center, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&tip, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                depth = RotTransPers(&mid, &dummy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortLine(&work->line, func_80058F10(), (u16)depth);
                }
                work->bolts[i].elapsed += step;
                if (work->bolts[i].duration < work->bolts[i].elapsed) {
                    work->bolts[i].active = 0;
                    work->sparks[i].active = 1;
                    work->sparks[i].position = work->bolts[i].position;
                    work->sparks[i].position.vy = 0;
                    k = 0;
                    do {
                        n = work->radius;
                        setVector(&work->curtain_upper[k][i],
                            (work->bolts[i].growth.vx - work->bolts[i].position.vx) * k / 8
                                + n * rsin(d * (k << 12) / 8 + (i << 12) / work->bolt_count) / 4096,
                            0, (work->bolts[i].growth.vz - work->bolts[i].position.vz) * k / 8);
                        work->curtain_lower[k][i] = work->curtain_upper[k][i];
                        /* This sum is never used and GCC deletes it after loop optimisation,
                         * but its size keeps the bolt address from being hoisted out of
                         * this loop. */
                        j = work->sparks[i].growth.vy * k * d;
                        j = work->sparks[i].position.vy * k * d + j;
                        j = work->sparks[i].scale.vx * k * d + j;
                        j = work->sparks[i].scale.vy * k * d + j;
                        j = work->sparks[i].scale.vz * k * d + j;
                        j = work->sparks[i].rotation.vx * k * d + j;
                        j = work->sparks[i].rotation.vy * k * d + j;
                        j = work->sparks[i].rotation.vz * k * d + j;
                        j = work->sparks[i].growth.vx * k * d + j;
                        j = work->sparks[i].position.vx * k * d + j;
                        {
                            s32 h;

                            n = rcos(k << 7);
                            h = work->height;
                            work->curtain_lower[k][i].vy = -h + h * n / 4096;
                        }
                    } while (++k < 9);
                }
                work->trails[work->bolts[i].ticks][i].active = 1;
                work->trails[work->bolts[i].ticks][i].elapsed = 0;
                work->trails[work->bolts[i].ticks][i].position = work->bolts[i].position;
                setVector(&work->trails[work->bolts[i].ticks][i].growth, tip.vx, tip.vy, tip.vz);
                work->trails[work->bolts[i].ticks][i].red = work->bolts[i].red;
                work->trails[work->bolts[i].ticks][i].green = work->bolts[i].green;
                work->trails[work->bolts[i].ticks][i].blue = work->bolts[i].blue;
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
                if (work->trails[i][k].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->trails[i][k].scale);
                    unit.matrix.t[0] = work->trails[i][k].position.vx;
                    unit.matrix.t[1] = work->trails[i][k].position.vy;
                    unit.matrix.t[2] = work->trails[i][k].position.vz;
                    unit.super = work->trails[i][k].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    n = work->trails[i][k].duration - work->trails[i][k].elapsed;
                    d = work->trails[i][k].duration;
                    red = work->trails[i][k].red * n / d;
                    green = work->trails[i][k].green * n / d;
                    blue = work->trails[i][k].blue * n / d;
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
                    RotTransPers(&work->center, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                    RotTransPers(&tip, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                    depth = RotTransPers(&mid, &dummy, &interpolation, &flag);
                    if (flag >= 0 && work->trails[i][k].elapsed != 0) {
                        GsSortLine(&work->line, func_80058F10(), (u16)depth);
                    }
                    work->trails[i][k].elapsed += step;
                    if (work->trails[i][k].duration < work->trails[i][k].elapsed) {
                        work->trails[i][k].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->bolt_count; i++) {
            if (work->sparks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->sparks[i].elapsed < work->sparks[i].duration / 16) {
                    n = (work->sparks[i].elapsed << 16) / work->sparks[i].duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->sparks[i].scale, 0x1000, n, 0x1000);
                ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                unit.matrix.t[0] = work->sparks[i].position.vx;
                unit.matrix.t[1] = work->sparks[i].position.vy;
                unit.matrix.t[2] = work->sparks[i].position.vz;
                unit.super = work->sparks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i].duration * 9 / 10 < work->sparks[i].elapsed) {
                    n = (work->sparks[i].duration - work->sparks[i].elapsed) * 10;
                    d = work->sparks[i].duration;
                    work->sparks[i].red = D_8013DB84[work->command].bolt_red * n / d;
                    work->sparks[i].green = D_8013DB84[work->command].bolt_green * n / d;
                    work->sparks[i].blue = D_8013DB84[work->command].bolt_blue * n / d;
                }
                work->sparks[i].v = (work->sparks[i].elapsed + i * 100) % 32 * 2 - 128;
                for (k = 0; k < 8; k++) {
                    for (m = 0; m < 2; m++) {
                        mids[m] = work->curtain_upper[k + m][i];
                        mids[m].vy = (work->curtain_upper[k + m][i].vy + work->curtain_lower[k + m][i].vy) / 2;
                    }
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[1];
                    work->shaded.clut = work->clut[1];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, work->sparks[i].u, work->sparks[i].v + 63,
                        work->sparks[i].u + 63, work->sparks[i].v + 63, work->sparks[i].u,
                        work->sparks[i].v, work->sparks[i].u + 63, work->sparks[i].v);
                    setRGB0(&work->shaded, 0, 0, 0);
                    setRGB1(&work->shaded, 0, 0, 0);
                    setRGB2(&work->shaded, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    if (k == 7) {
                        setRGB3(&work->shaded, 0, 0, 0);
                    } else {
                        setRGB3(&work->shaded, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    }
                    depth = RotAverage4(&work->curtain_upper[k][i], &work->curtain_upper[k + 1][i],
                        &mids[0], &mids[1],
                        (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                        (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                    }
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[1];
                    work->shaded.clut = work->clut[1];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, work->sparks[i].u, work->sparks[i].v + 63,
                        work->sparks[i].u + 63, work->sparks[i].v + 63, work->sparks[i].u,
                        work->sparks[i].v, work->sparks[i].u + 63, work->sparks[i].v);
                    setRGB2(&work->shaded, 0, 0, 0);
                    setRGB3(&work->shaded, 0, 0, 0);
                    setRGB0(&work->shaded, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    if (k == 7) {
                        setRGB1(&work->shaded, 0, 0, 0);
                    } else {
                        setRGB1(&work->shaded, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    }
                    depth = RotAverage4(&mids[0], &mids[1],
                        &work->curtain_lower[k][i], &work->curtain_lower[k + 1][i],
                        (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                        (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                    }
                }
                work->sparks[i].elapsed += step;
                if (work->flags[4] == 0) {
                    result = 4;
                }
                if (work->sparks[i].duration < work->sparks[i].elapsed) {
                    work->sparks[i].active = 0;
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
        work->elapsed += step;
        if (inactive == work->ring_count + work->bolt_count * (work->trail_count + 2)) {
            result = 2;
        }
    }
    return result;
}

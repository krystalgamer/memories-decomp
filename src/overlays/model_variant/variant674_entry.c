#include "../../types.h"
#include "variant674_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 674's entry: a pulsing ring of shaded strips spins up around a
 * model point and hands over to a growing dome of sprites wrapped in a
 * spinning cone of strips. The setup also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x8F in slot zero and 0xAC in slot one. */
#ifndef MODEL674_STRING_PAD
#define MODEL674_STRING_PAD "\x8F"
#endif
#ifndef VERSION_FRENCH
static const char gModelVariant674_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL674_STRING_PAD;
#endif

s32 ModelVariant674_HandleRequest(u8 *context, s32 command)
{
    Model674State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    /* Arrays, so the UV bytes live in the frame like the original's. */
    u8 u[2], v[2];
    long interpolation, flag;
    s32 result, i, step, inactive;
    s32 j, k, m, n, d, nn;
    u32 tex;
    long depth;

    work = (Model674State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model674State));
#ifndef VERSION_FRENCH
        printf(gModelVariant674_szWorkSizeFormat, sizeof(Model674State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant674_aImages[1]);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant674_aImages[0]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant674_aImages[2]);
        work->textures.tpage[1] = tex >> 16;
        work->textures.clut[1] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant674_aImages[3]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant674_aImages[4]);
        work->textures.tpage[2] = tex >> 16;
        work->textures.clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->count = 1;
            break;
        }
        work->place.radius = gModelVariant674_aConfigs[command].radius;
        work->place.height = gModelVariant674_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            if (command == 2) {
                work->rings[i].head.duration = 0x158;
            } else {
                work->rings[i].head.duration = 0x76;
            }
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant674_aConfigs[command].red;
            work->rings[i].head.color[1] = gModelVariant674_aConfigs[command].green;
            work->rings[i].head.color[2] = gModelVariant674_aConfigs[command].blue;
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 0;
            setVector(&work->rings[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->domes[i].head.active = 0;
            work->domes[i].head.elapsed = 0;
            switch (command) {
            case 0:
            case 2:
                work->domes[i].head.duration = 0x80;
                break;
            case 1:
                work->domes[i].head.duration = 0xC4;
                break;
            }
            work->domes[i].head.mode = 1;
            work->domes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i].head.color[0] = gModelVariant674_aConfigs[command].dome_red;
            work->domes[i].head.color[1] = gModelVariant674_aConfigs[command].dome_green;
            work->domes[i].head.color[2] = gModelVariant674_aConfigs[command].dome_blue;
            work->domes[i].head.u = 0;
            work->domes[i].head.v = 0x80;
            setVector(&work->domes[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->domes[i].growth, 0, 0, 0);
            setVector(&work->domes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->domes[i].xform.position, 0, 0, 0);
        }
        work->place.delay = gModelVariant674_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->place.radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->place.radius;
        /* Shifting the field (not n) keeps the srl; n and d are reused
         * throughout to pin their registers. */
        d = work->place.radius >> 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_upper[i], n * rsin(i << 8) / 4096, d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_lower[i], n * rsin(i << 8) / 4096, -d, n * rcos(i << 8) / 4096);
            setVector(&work->ring_middle[i], (work->ring_upper[i].vx + work->ring_lower[i].vx) / 2,
                (work->ring_upper[i].vy + work->ring_lower[i].vy) / 2,
                (work->ring_upper[i].vz + work->ring_lower[i].vz) / 2);
        }
        n = work->place.radius;
        nn = n * n;
        for (i = 0; i < 9; i++) {
            for (k = 0; k < 17; k++) {
                setVector(&work->dome[k][i],
                    SquareRoot0(nn - n * rcos(i << 8) / 4096 * (n * rcos(i << 8) / 4096)) * rsin(k << 8) / 4096,
                    n * rcos(i << 8) / 4096,
                    SquareRoot0(nn - n * rcos(i << 8) / 4096 * (n * rcos(i << 8) / 4096)) * rcos(k << 8) / 4096);
            }
        }
        n = work->place.radius / 2;
        d = work->place.radius * 2;
        for (i = 0; i < 9; i++) {
            setVector(&work->cone_inner[i], n * rsin(i << 9) / 4096, n * rcos(i << 9) / 4096, 0);
            setVector(&work->cone_outer[i], d * rsin(i << 9) / 4096, d * rcos(i << 9) / 4096, 0);
            setVector(&work->cone_middle[i], (work->cone_inner[i].vx + work->cone_outer[i].vx) / 2,
                (work->cone_inner[i].vy + work->cone_outer[i].vy) / 2,
                (work->cone_inner[i].vz + work->cone_outer[i].vz) / 2);
        }
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                work->rings[0].head.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0x10);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            break;
        case 1:
            if (work->head.elapsed == 0) {
                work->domes[0].head.active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 3);
                GsGetLwUnit(&unit, &ls);
                setVector(&work->domes[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            break;
        case 2:
            if (work->head.elapsed == 0) {
                work->rings[0].head.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0xE);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->rings[0].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
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
                setPolyFT4(&work->flat);
                work->flat.tpage = work->textures.tpage[0];
                work->flat.clut = work->textures.clut[0];
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
                        work->shaded.tpage = work->textures.tpage[1];
                        work->shaded.clut = work->textures.clut[1];
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
                        work->shaded.tpage = work->textures.tpage[1];
                        work->shaded.clut = work->textures.clut[1];
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
                work->rings[i].head.elapsed += step;
                work->rings[i].ticks++;
                if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                    work->rings[i].head.active = 0;
                    work->domes[i].head.active = 1;
                    setVector(&work->domes[i].xform.position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->domes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                n = (work->domes[i].head.elapsed << 13) / work->domes[i].head.duration + 0x800;
                setVector(&work->domes[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                RotMatrixZ(work->domes[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->domes[i].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].xform.position.vx;
                unit.matrix.t[1] = work->domes[i].xform.position.vy;
                unit.matrix.t[2] = work->domes[i].xform.position.vz;
                unit.super = work->domes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    n = (work->domes[i].head.duration - work->domes[i].head.elapsed) * 2;
                    d = work->domes[i].head.duration;
                    work->domes[i].head.color[0] = gModelVariant674_aConfigs[work->head.command].dome_red * n / d;
                    work->domes[i].head.color[1] = gModelVariant674_aConfigs[work->head.command].dome_green * n / d;
                    work->domes[i].head.color[2] = gModelVariant674_aConfigs[work->head.command].dome_blue * n / d;
                } else {
                    work->domes[i].head.color[0] = gModelVariant674_aConfigs[work->head.command].dome_red;
                    work->domes[i].head.color[1] = gModelVariant674_aConfigs[work->head.command].dome_green;
                    work->domes[i].head.color[2] = gModelVariant674_aConfigs[work->head.command].dome_blue;
                }
                work->domes[i].head.v = work->domes[i].head.elapsed % 16 * 63 / 16 + 128;
                for (k = 0; k < 8; k++) {
                    for (m = 0; m < 16; m++) {
                        setPolyFT4(&work->flat);
                        work->flat.tpage = work->textures.tpage[2];
                        work->flat.clut = work->textures.clut[2];
                        setSemiTrans(&work->flat, 1);
                        u[0] = work->domes[i].head.u + m % 2 * 63 / 2 + 64;
                        u[1] = work->domes[i].head.u + (m % 2 + 1) * 63 / 2 + 64;
                        setUV4(&work->flat, u[0], work->domes[i].head.v, u[1], work->domes[i].head.v,
                            u[0], work->domes[i].head.v + 64, u[1], work->domes[i].head.v + 64);
                        setRGB0(&work->flat, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        depth = RotAverage4(&work->dome[m][k], &work->dome[m + 1][k],
                            &work->dome[m][k + 1], &work->dome[m + 1][k + 1],
                            (long *)&work->flat.x0, (long *)&work->flat.x1,
                            (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    n = (work->domes[i].head.duration - work->domes[i].head.elapsed) * 2;
                    d = work->domes[i].head.duration;
                    work->domes[i].head.color[0] = gModelVariant674_aConfigs[work->head.command].red * n / d;
                    work->domes[i].head.color[1] = gModelVariant674_aConfigs[work->head.command].green * n / d;
                    work->domes[i].head.color[2] = gModelVariant674_aConfigs[work->head.command].blue * n / d;
                } else {
                    work->domes[i].head.color[0] = gModelVariant674_aConfigs[work->head.command].red;
                    work->domes[i].head.color[1] = gModelVariant674_aConfigs[work->head.command].green;
                    work->domes[i].head.color[2] = gModelVariant674_aConfigs[work->head.command].blue;
                }
                for (k = 0; k < 8; k++) {
                    work->domes[i].head.v = (work->domes[i].head.elapsed + k * 10) % 32 * 63 / 32 + 128;
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                    n = k % 2 * 2 - 1;
                    d = k << 9;
                    RotMatrixZ(n * work->domes[i].head.elapsed * 5 + d, &unit.matrix);
                    RotMatrixX(n * work->domes[i].head.elapsed * 6 + d, &unit.matrix);
                    RotMatrixY(n * work->domes[i].head.elapsed * 7 + d, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i].xform.position.vx;
                    unit.matrix.t[1] = work->domes[i].xform.position.vy;
                    unit.matrix.t[2] = work->domes[i].xform.position.vz;
                    unit.super = work->domes[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (m = 0; m < 8; m++) {
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->textures.tpage[1];
                        work->shaded.clut = work->textures.clut[1];
                        setSemiTrans(&work->shaded, 1);
                        setUV4(&work->shaded, work->domes[i].head.u, work->domes[i].head.v + 63,
                            work->domes[i].head.u + 63, work->domes[i].head.v + 63, work->domes[i].head.u,
                            work->domes[i].head.v, work->domes[i].head.u + 63, work->domes[i].head.v);
                        setRGB2(&work->shaded, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB3(&work->shaded, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB0(&work->shaded, 0, 0, 0);
                        setRGB1(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&work->cone_inner[m], &work->cone_inner[m + 1],
                            &work->cone_middle[m], &work->cone_middle[m + 1],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->textures.tpage[1];
                        work->shaded.clut = work->textures.clut[1];
                        setSemiTrans(&work->shaded, 1);
                        setUV4(&work->shaded, work->domes[i].head.u, work->domes[i].head.v + 63,
                            work->domes[i].head.u + 63, work->domes[i].head.v + 63, work->domes[i].head.u,
                            work->domes[i].head.v, work->domes[i].head.u + 63, work->domes[i].head.v);
                        setRGB0(&work->shaded, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB1(&work->shaded, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB2(&work->shaded, 0, 0, 0);
                        setRGB3(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&work->cone_middle[m], &work->cone_middle[m + 1],
                            &work->cone_outer[m], &work->cone_outer[m + 1],
                            (long *)&work->shaded.x0, (long *)&work->shaded.x1,
                            (long *)&work->shaded.x2, (long *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                work->domes[i].head.elapsed += step;
                if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                    work->domes[i].head.active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->count * 2) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

#include "../../types.h"
#include "variant683_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 683's entry: a stream of blinking orbs spirals in towards the other
 * model's position, then a pale screen flash and a burst that flies back
 * to the active model: a striped dome inside a shaded ring, a flash square
 * and 32 rays fanning out. The setup also prints the work-area size. */

s32 ModelVariant683_HandleRequest(u8 *context, s32 command)
{
    Model683State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model683ViewSnapshot view;
    /* Only two bytes are used; the size keeps the original frame layout. */
    u8 uv[8];
    VECTOR target;
    VECTOR current;
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 n, d, s;
    u32 tex;
    PSXLONG depth;

    work = (Model683State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model683State));
#ifndef VERSION_FRENCH
        printf(gModelVariant683_D2514, sizeof(Model683State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[3]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[2]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[5]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[0]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[4]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant683_D252C[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        if (command == 0) {
            work->count = 96;
            work->burst_count = 1;
        }
        work->radius = gModelVariant683_D25D4[command].radius;
        work->height = gModelVariant683_D25D4[command].height;
        for (i = 0; i < work->count; i++) {
            work->orbs[i].duration = 96;
            work->orbs[i].active = 0;
            work->orbs[i].elapsed = 0;
            work->orbs[i].mode = 1;
            work->orbs[i].coord = Model_GetCameraCoordinateUnit();
            work->orbs[i].red = gModelVariant683_D25D4[command].orb[0];
            work->orbs[i].green = gModelVariant683_D25D4[command].orb[1];
            work->orbs[i].blue = gModelVariant683_D25D4[command].orb[2];
            work->orbs[i].u = 0;
            work->orbs[i].v = 0;
            setVector(&work->orbs[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->orbs[i].spin, 0, 0, 0);
            setVector(&work->orbs[i].rotation, 0, work->slot * 2048, 0);
            d = i % 2 * 2 - 1;
            n = work->radius * 4;
            setVector(&work->orbs[i].position, n * rsin((i % 10 << 12) / 10 + i * 50) / 4096 + work->origin.vx,
                d * n * rsin((i % 16 << 8) + i * 50) / 4096 + work->origin.vy,
                d * n * rcos((i % 8 << 9) + i * 50) / 4096 + work->origin.vz);
        }
        work->fade.duration = 64;
        work->fade.active = 0;
        work->fade.elapsed = 0;
        work->fade.mode = 1;
        work->fade.coord = Model_GetCameraCoordinateUnit();
        work->fade.red = gModelVariant683_D25D4[command].fade[0];
        work->fade.green = gModelVariant683_D25D4[command].fade[1];
        work->fade.blue = gModelVariant683_D25D4[command].fade[2];
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 128;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].red = gModelVariant683_D25D4[command].burst[0];
            work->bursts[i].green = gModelVariant683_D25D4[command].burst[1];
            work->bursts[i].blue = gModelVariant683_D25D4[command].burst[2];
            work->bursts[i].u = 0;
            work->bursts[i].v = 128;
            setVector(&work->bursts[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->bursts[i].spin, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i].position, 0, 0, 0);
        }
        setVector(&offset, work->radius * 3, 0, 0);
        for (i = 0; i < 32; i++) {
            PushMatrix();
            n = i % 2 * 2 - 1;
            m = D_800FE168;
            RotMatrixZ(n * (i << 11) / 32, &m);
            /* Spelled out so the quotient stays in a scratch register. */
            d = i - i / 16 * 16;
            RotMatrixY(n * (d << 12) / 16, &m);
            RotMatrixX(d << 8, &m);
            ApplyMatrixLV(&m, &offset, &work->offsets[i]);
            PopMatrix();
        }
        n = work->radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->radius;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->grid[j][i],
                    SquareRoot0(n * n - n * rcos(i << 8) / 4096 * (n * rcos(i << 8) / 4096)) * rsin(j << 8) / 4096,
                    n * rcos(i << 8) / 4096,
                    SquareRoot0(n * n - n * rcos(i << 8) / 4096 * (n * rcos(i << 8) / 4096)) * rcos(j << 8) / 4096);
            }
        }
        d = work->radius * 2;
        n = work->radius / 10;
        for (i = 0; i < 17; i++) {
            setVector(&work->inner[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
            setVector(&work->outer[i], d * rsin(i << 8) / 4096, d * rcos(i << 8) / 4096, 0);
            setVector(&work->middle[i], (work->inner[i].vx + work->outer[i].vx) / 2,
                (work->inner[i].vy + work->outer[i].vy) / 2, (work->inner[i].vz + work->outer[i].vz) / 2);
        }
        n = work->radius;
        setVector(&work->burst[0], -n, -n, 0);
        setVector(&work->burst[1], n, -n, 0);
        setVector(&work->burst[2], -n, n, 0);
        setVector(&work->burst[3], n, n, 0);
        n = work->radius / 2;
        setVector(&work->ray[0], -n, -n, 0);
        setVector(&work->ray[1], n, -n, 0);
        setVector(&work->ray[2], -n, n, 0);
        setVector(&work->ray[3], n, n, 0);
        work->delay = gModelVariant683_D25D4[command].delay;
        for (i = 0; i < 6; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        if (work->command == 0) {
            if (work->elapsed < 210 && work->flags[0] < work->count) {
                work->orbs[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            if (work->elapsed >= 307 && work->flags[1] == 0) {
                result = work->flags[1] = work->bursts[0].active = work->fade.active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 12);
            GsGetLwUnit(&unit, &ls);
            setVector(&target, ls.t[0], ls.t[1], ls.t[2]);
        } else {
            setVector(&target, 0, 0, 0);
        }
        PushMatrix();
        view = *(Model683ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->orbs[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->orbs[i].duration / 2 < work->orbs[i].elapsed) {
                    n = ((work->orbs[i].duration - work->orbs[i].elapsed) << 13) / work->orbs[i].duration;
                } else {
                    n = 0x1000;
                }
                if (work->orbs[i].spin.vx % 2 == 0) {
                    n /= 2;
                }
                setVector(&work->orbs[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->orbs[i].scale);
                RotMatrixZ(work->orbs[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->orbs[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->orbs[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                s = i % 2 * 2 - 1;
                d = (i % 4 / 2 + 1) * rsin((work->orbs[i].elapsed << 11) / work->orbs[i].duration);
                n = s * (work->radius * ((s32)((u32)rsin((work->orbs[i].elapsed << 11) / work->orbs[i].duration) << 1)) / 4096) * d / 4096;
                d = s * (work->radius * ((s32)((u32)rcos((work->orbs[i].elapsed << 11) / work->orbs[i].duration) << 1)) / 4096) * d / 4096;
                unit.matrix.t[0] = n + work->orbs[i].position.vx
                    + (work->origin.vx - work->orbs[i].position.vx) * work->orbs[i].elapsed / work->orbs[i].duration;
                unit.matrix.t[1] = work->orbs[i].position.vy
                    + (work->origin.vy - work->orbs[i].position.vy) * work->orbs[i].elapsed / work->orbs[i].duration;
                unit.matrix.t[2] = d + work->orbs[i].position.vz
                    + (work->origin.vz - work->orbs[i].position.vz) * work->orbs[i].elapsed / work->orbs[i].duration;
                unit.super = work->orbs[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->orbs[i].elapsed < work->orbs[i].duration / 2) {
                    n = work->orbs[i].elapsed * 2;
                    d = work->orbs[i].duration;
                    work->orbs[i].red = gModelVariant683_D25D4[work->command].orb[0] * n / d;
                    work->orbs[i].green = gModelVariant683_D25D4[work->command].orb[1] * n / d;
                    work->orbs[i].blue = gModelVariant683_D25D4[work->command].orb[2] * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->orbs[i].u, work->orbs[i].v, work->orbs[i].u + 63, work->orbs[i].v,
                    work->orbs[i].u, work->orbs[i].v + 63, work->orbs[i].u + 63, work->orbs[i].v + 63);
                setRGB0(&work->flat, work->orbs[i].red, work->orbs[i].green, work->orbs[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                work->orbs[i].spin.vx++;
                work->orbs[i].elapsed += step;
                if (work->orbs[i].duration < work->orbs[i].elapsed) {
                    work->orbs[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        if (work->fade.active == 1) {
            if (work->fade.elapsed < work->fade.duration) {
                n = work->fade.duration - work->fade.elapsed;
                d = work->fade.duration;
                work->fade.red = gModelVariant683_D25D4[work->command].fade[0] * n / d;
                work->fade.green = gModelVariant683_D25D4[work->command].fade[1] * n / d;
                work->fade.blue = gModelVariant683_D25D4[work->command].fade[2] * n / d;
            }
            setPolyF4(&work->fill);
            setRGB0(&work->fill, work->fade.red, work->fade.green, work->fade.blue);
            setXY4(&work->fill, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
                GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
            Graphics_SubmitDrawModePacket((u32 *)&work->fill, func_80058F10(), 0, work->fade.mode);
            work->fade.elapsed += step;
            if (work->fade.duration < work->fade.elapsed) {
                work->fade.active = 0;
            }
        } else {
            inactive++;
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->bursts[i].elapsed > work->bursts[i].duration * 3 / 4) {
                    n = ((work->bursts[i].duration - work->bursts[i].elapsed) << 14) / work->bursts[i].duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->bursts[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].elapsed * 5, &unit.matrix);
                RotMatrixX(work->bursts[i].elapsed * 6, &unit.matrix);
                RotMatrixY(work->bursts[i].elapsed * 7, &unit.matrix);
                current.vx = work->origin.vx
                    + (target.vx - work->origin.vx) * work->bursts[i].elapsed / work->bursts[i].duration;
                current.vy = work->origin.vy
                    + (target.vy - work->origin.vy) * work->bursts[i].elapsed / work->bursts[i].duration;
                current.vz = work->origin.vz
                    + (target.vz - work->origin.vz) * work->bursts[i].elapsed / work->bursts[i].duration;
                unit.matrix.t[0] = current.vx;
                unit.matrix.t[1] = current.vy;
                unit.matrix.t[2] = current.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i].elapsed < work->bursts[i].duration / 2) {
                    n = work->bursts[i].elapsed * 2;
                    d = work->bursts[i].duration;
                    work->bursts[i].red = gModelVariant683_D25D4[work->command].burst[0] * n / d;
                    work->bursts[i].green = gModelVariant683_D25D4[work->command].burst[1] * n / d;
                    work->bursts[i].blue = gModelVariant683_D25D4[work->command].burst[2] * n / d;
                } else {
                    work->bursts[i].red = gModelVariant683_D25D4[work->command].burst[0];
                    work->bursts[i].green = gModelVariant683_D25D4[work->command].burst[1];
                    work->bursts[i].blue = gModelVariant683_D25D4[work->command].burst[2];
                }
                work->bursts[i].v = (work->bursts[i].elapsed % 16) * 63 / 16 - 128;
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 16; k++) {
                        setPolyFT4(&work->flat);
                        work->flat.tpage = work->tpage[1];
                        work->flat.clut = work->clut[1];
                        setSemiTrans(&work->flat, 1);
                        uv[0] = k % 2 * 63 / 2;
                        uv[1] = (k % 2 + 1) * 63 / 2;
                        setUV4(&work->flat, uv[0], work->bursts[i].v, uv[1], work->bursts[i].v,
                            uv[0], work->bursts[i].v + 64, uv[1], work->bursts[i].v + 64);
                        setRGB0(&work->flat, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                        depth = RotAverage4(&work->grid[k][j], &work->grid[k + 1][j],
                            &work->grid[k][j + 1], &work->grid[k + 1][j + 1],
                            (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                            (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->bursts[i].v = (work->bursts[i].elapsed % 32) * 63 / 32 - 128;
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = current.vx;
                unit.matrix.t[1] = current.vy;
                unit.matrix.t[2] = current.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                uv[0] = 64;
                uv[1] = 127;
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[2];
                    work->shaded.clut = work->clut[2];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, uv[0], work->bursts[i].v + 63, uv[1], work->bursts[i].v + 63,
                        uv[0], work->bursts[i].v, uv[1], work->bursts[i].v);
                    setRGB2(&work->shaded, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                    setRGB3(&work->shaded, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                    setRGB0(&work->shaded, 0, 0, 0);
                    setRGB1(&work->shaded, 0, 0, 0);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1], &work->middle[j], &work->middle[j + 1],
                        (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                        (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                    }
                    setPolyGT4(&work->shaded);
                    work->shaded.tpage = work->tpage[2];
                    work->shaded.clut = work->clut[2];
                    setSemiTrans(&work->shaded, 1);
                    setUV4(&work->shaded, uv[0], work->bursts[i].v + 63, uv[1], work->bursts[i].v + 63,
                        uv[0], work->bursts[i].v, uv[1], work->bursts[i].v);
                    setRGB0(&work->shaded, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                    setRGB1(&work->shaded, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                    setRGB2(&work->shaded, 0, 0, 0);
                    setRGB3(&work->shaded, 0, 0, 0);
                    depth = RotAverage4(&work->middle[j], &work->middle[j + 1], &work->outer[j], &work->outer[j + 1],
                        (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                        (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->bursts[i].elapsed > work->bursts[i].duration * 3 / 4) {
                    n = ((work->bursts[i].duration - work->bursts[i].elapsed) << 14) / work->bursts[i].duration;
                } else {
                    n = 0x1000;
                }
                if (work->bursts[i].spin.vx % 2 == 0) {
                    n /= 2;
                }
                setVector(&work->bursts[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = current.vx;
                unit.matrix.t[1] = current.vy;
                unit.matrix.t[2] = current.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->flat);
                setUV4(&work->flat, 0, 0, 63, 0, 0, 63, 63, 63);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setRGB0(&work->flat, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                depth = RotAverage4(&work->burst[0], &work->burst[1], &work->burst[2], &work->burst[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                n = (work->bursts[i].elapsed << 12) / work->bursts[i].duration + 0x1000;
                setVector(&work->bursts[i].scale, n, n, n);
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    n = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    d = work->bursts[i].duration;
                    work->bursts[i].red = gModelVariant683_D25D4[work->command].burst[0] * n / d;
                    work->bursts[i].green = gModelVariant683_D25D4[work->command].burst[1] * n / d;
                    work->bursts[i].blue = gModelVariant683_D25D4[work->command].burst[2] * n / d;
                } else {
                    work->bursts[i].red = gModelVariant683_D25D4[work->command].burst[0];
                    work->bursts[i].green = gModelVariant683_D25D4[work->command].burst[1];
                    work->bursts[i].blue = gModelVariant683_D25D4[work->command].burst[2];
                }
                for (j = 0; j < 32; j++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                    RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                    RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->origin.vx
                        + work->offsets[j].vx * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[1] = work->origin.vy
                        + work->offsets[j].vy * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[2] = work->origin.vz
                        + work->offsets[j].vz * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[3];
                    work->flat.clut = work->clut[3];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->bursts[i].u, 64, work->bursts[i].u + 31, 64,
                        work->bursts[i].u, 95, work->bursts[i].u + 31, 95);
                    setRGB0(&work->flat, work->bursts[i].red, work->bursts[i].green, work->bursts[i].blue);
                    depth = RotAverage4(&work->ray[0], &work->ray[1], &work->ray[2], &work->ray[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                }
                work->bursts[i].spin.vx++;
                if (work->bursts[i].u < 0xE0) {
                    work->bursts[i].u += 32;
                }
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (work->flags[1] == 1 && inactive == work->count + 1 + work->burst_count) {
            result = 2;
        }
    }
    return result;
}

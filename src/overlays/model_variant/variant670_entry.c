#include "../../types.h"
#include "variant670_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 670's entry: textured shells grow from a model point one after
 * another, each a sprite with a meshed dome on an eight-point base, and each
 * hands a spinning ring band with a square flash to the same spot. The
 * setup also prints the work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL670_STRING_PAD
#define MODEL670_STRING_PAD "\xA6"
#endif
static const char D_8013CD68[24] = "E_DATA size = %x byte\n\0" MODEL670_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model670State *work;
    MATRIX m;
    GsCOORDUNIT unit;
    Model670ViewSnapshot view;
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, n, h;
    u32 tex;
    PSXLONG depth;

    work = (Model670State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model670State));
#ifndef VERSION_FRENCH
        printf(D_8013CD68, sizeof(Model670State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)work->position);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CD80[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CD80[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CD80[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 32;
            break;
        case 1:
        case 2:
            work->count = 16;
            break;
        }
        work->radius = D_8013CDD4[command].radius;
        work->height = D_8013CDD4[command].height;
        for (i = 0; i < work->count; i++) {
            work->shells[i].duration = 0x30;
            work->shells[i].active = 0;
            work->shells[i].elapsed = 0;
            work->shells[i].mode = 1;
            work->shells[i].coord = Model_GetCameraCoordinateUnit();
            work->shells[i].red = D_8013CDD4[command].red;
            work->shells[i].green = D_8013CDD4[command].green;
            work->shells[i].blue = D_8013CDD4[command].blue;
            work->shells[i].u = 0;
            work->shells[i].v = 0;
            setVector(&work->shells[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->shells[i].growth, 0, 0, 0);
            setVector(&work->shells[i].rotation, 0, work->slot * 2048,
                (i % 2 * 2 - 1) * (i << 12) / work->count);
            setVector(&work->shells[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].duration = 0x40;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 2;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].red = D_8013CDD4[command].ring_red;
            work->rings[i].green = D_8013CDD4[command].ring_green;
            work->rings[i].blue = D_8013CDD4[command].ring_blue;
            work->rings[i].u = 0;
            work->rings[i].v = 0x80;
            setVector(&work->rings[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            /* n and h are shared scratch variables throughout; reusing them
             * here keeps the original register assignment. */
            h = work->radius;
            n = i % 2 * 2 - 1;
            setVector(&work->rings[i].position,
                work->position[0] + n * h * rsin((i << 12) / work->count) / 4096,
                work->position[1] + n * -h * rcos((i << 12) / work->count) / 4096,
                work->position[2] + n * h * rcos((i << 12) / work->count) / 4096);
        }
        work->delay = D_8013CDD4[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->radius / 3;
        for (i = 0; i < 8; i++) {
            setVector(&work->base[0][i], n * rsin(i << 9) / 4096, n * rcos(i << 9) / 4096, 0);
            setVector(&work->base[1][i], n * rsin((i << 9) + 0x800) / 4096,
                n * rcos((i << 9) + 0x800) / 4096, 0);
        }
        for (i = 0; i < 4; i++) {
            n = work->radius * rsin(i << 8) / 4096 + work->radius;
            h = work->height * -rcos(i * 819 / 4) / 4096;
            for (j = 0; j < 8; j++) {
                /* (i + 8) * 0x80 becomes the inner loop's start angle, which the
                 * outer loop then steps by 0x80 in a stack slot. */
                k = j * 0x200 + (i + 8) * 0x80;
                setVector(&work->dome[j][i], n * rsin(k) / 4096, n * rcos(k) / 4096, h);
            }
        }
        n = work->radius;
        h = n * 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->inner[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
            setVector(&work->outer[i], h * rsin(i << 8) / 4096, h * rcos(i << 8) / 4096, 0);
        }
        n = work->radius * 2;
        setVector(&work->corners[0], -n, -n, 0);
        setVector(&work->corners[1], n, -n, 0);
        setVector(&work->corners[2], -n, n, 0);
        setVector(&work->corners[3], n, n, 0);
        /* A divisor variable keeps the real div instructions. */
        n = 5;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 2; k++) {
                    setVector(&work->mesh[k][j][i],
                        work->base[k][j].vx + (work->dome[j][i].vx - work->base[k][j].vx) / n,
                        work->base[k][j].vy + (work->dome[j][i].vy - work->base[k][j].vy) / n,
                        work->base[k][j].vz + (work->dome[j][i].vz - work->base[k][j].vz) / n);
                }
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed < 0x13C && work->elapsed >= work->flags[0] * 30
                && work->flags[0] < work->count) {
                work->shells[work->flags[0]].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 2);
                GsGetLwUnit(&unit, &m);
                setVector(&work->shells[work->flags[0]].position, m.t[0], m.t[1], m.t[2]);
                work->rings[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->elapsed < 0x64 && work->elapsed >= work->flags[0] * 30
                && work->flags[0] < work->count) {
                work->shells[work->flags[0]].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0xD);
                GsGetLwUnit(&unit, &m);
                setVector(&work->shells[work->flags[0]].position, m.t[0], m.t[1], m.t[2]);
                work->rings[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed < 0x54 && work->elapsed >= work->flags[0] * 20
                && work->flags[0] < work->count) {
                work->shells[work->flags[0]].active = 1;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0x1E);
                GsGetLwUnit(&unit, &m);
                setVector(&offset, 0, -0x82, 0);
                PushMatrix();
                ApplyMatrixLV(&m, &offset, &work->shells[work->flags[0]].position);
                PopMatrix();
                /* The macro's member addressing matches the original stores. */
                applyVector(&work->shells[work->flags[0]].position, m.t[0], m.t[1], m.t[2], +=);
                work->rings[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model670ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->shells[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->shells[i].elapsed * 3 << 12) / work->shells[i].duration + 0x800;
                setVector(&work->shells[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->shells[i].scale);
                RotMatrixZ(0, &unit.matrix);
                RotMatrixX(work->shells[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->shells[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->shells[i].position.vx;
                unit.matrix.t[1] = work->shells[i].position.vy;
                unit.matrix.t[2] = work->shells[i].position.vz;
                unit.super = work->shells[i].coord;
                GsGetLsUnit(&unit, &m);
                GsSetLsMatrix(&m);
                if (work->shells[i].duration / 3 < work->shells[i].elapsed) {
                    n = (work->shells[i].duration - work->shells[i].elapsed) * 3;
                    h = work->shells[i].duration * 2;
                    work->shells[i].red = D_8013CDD4[work->command].red * n / h;
                    work->shells[i].green = D_8013CDD4[work->command].green * n / h;
                    work->shells[i].blue = D_8013CDD4[work->command].blue * n / h;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->shells[i].u, work->shells[i].v, work->shells[i].u + 63,
                    work->shells[i].v, work->shells[i].u, work->shells[i].v + 63,
                    work->shells[i].u + 63, work->shells[i].v + 63);
                setRGB0(&work->sprite, work->shells[i].red, work->shells[i].green, work->shells[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->shells[i].scale);
                RotMatrixZ(work->shells[i].rotation.vz, &unit.matrix);
                RotMatrixY(work->shells[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->shells[i].position.vx;
                unit.matrix.t[1] = work->shells[i].position.vy;
                unit.matrix.t[2] = work->shells[i].position.vz;
                unit.super = work->shells[i].coord;
                GsGetLsUnit(&unit, &m);
                GsSetLsMatrix(&m);
                for (j = 0; j < 4; j++) {
                    for (k = 0; k < 8; k++) {
                        setPolyGT3(&work->fan);
                        work->fan.tpage = work->tpage[0];
                        work->fan.clut = work->clut[0];
                        setSemiTrans(&work->fan, 1);
                        setUV3(&work->fan, work->shells[i].u + 64, work->shells[i].v,
                            work->shells[i].u + 127, work->shells[i].v, work->shells[i].u + 96,
                            work->shells[i].v + 63);
                        setRGB2(&work->fan, 0, 0, 0);
                        setRGB0(&work->fan, work->shells[i].red, work->shells[i].green,
                            work->shells[i].blue);
                        setRGB1(&work->fan, work->shells[i].red, work->shells[i].green,
                            work->shells[i].blue);
                        depth = RotAverage3(&work->mesh[0][k][j], &work->mesh[1][k][j], &work->dome[k][j],
                            (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                        }
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[0];
                        work->strip.clut = work->clut[0];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->shells[i].u + 64, work->shells[i].v,
                            work->shells[i].u + 127, work->shells[i].v, work->shells[i].u + 64,
                            work->shells[i].v + 63, work->shells[i].u + 127, work->shells[i].v + 63);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->shells[i].red, work->shells[i].green,
                            work->shells[i].blue);
                        setRGB3(&work->strip, work->shells[i].red, work->shells[i].green,
                            work->shells[i].blue);
                        depth = RotAverage4(&work->base[0][k], &work->base[1][k], &work->mesh[0][k][j],
                            &work->mesh[1][k][j], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->shells[i].elapsed += step;
                if (work->shells[i].duration < work->shells[i].elapsed) {
                    work->shells[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->rings[i].elapsed * 3 << 12) / work->rings[i].duration + 0x800;
                setVector(&work->rings[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &m);
                GsSetLsMatrix(&m);
                if (work->rings[i].duration / 3 < work->rings[i].elapsed) {
                    n = (work->rings[i].duration - work->rings[i].elapsed) * 3;
                    h = work->rings[i].duration * 2;
                    work->rings[i].red = D_8013CDD4[work->command].ring_red * n / h;
                    work->rings[i].green = D_8013CDD4[work->command].ring_green * n / h;
                    work->rings[i].blue = D_8013CDD4[work->command].ring_blue * n / h;
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->rings[i].u, work->rings[i].v, work->rings[i].u + 31,
                        work->rings[i].v, work->rings[i].u, work->rings[i].v + 127,
                        work->rings[i].u + 31, work->rings[i].v + 127);
                    setRGB2(&work->strip, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB3(&work->strip, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1], &work->outer[j],
                        &work->outer[j + 1], (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].elapsed * 10, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &m);
                GsSetLsMatrix(&m);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[2];
                work->sprite.clut = work->clut[2];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->rings[i].u + 32, work->rings[i].v, work->rings[i].u + 159,
                    work->rings[i].v, work->rings[i].u + 32, work->rings[i].v + 127,
                    work->rings[i].u + 159, work->rings[i].v + 127);
                setRGB0(&work->sprite, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2],
                    &work->corners[3], (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        PopMatrix();
        switch (work->command) {
        case 0:
            if (work->elapsed >= 0x97 && work->flags[4] == 0) {
                result = 3;
                work->flags[4] = 1;
            }
            break;
        case 1:
            if (work->elapsed >= 0x33 && work->flags[4] == 0) {
                result = 3;
                work->flags[4] = 1;
            }
            break;
        case 2:
            if (work->elapsed >= 0x29 && work->flags[4] == 0) {
                result = 3;
                work->flags[4] = 1;
            }
            break;
        }
        work->elapsed += step;
        if (inactive == work->count * 2) {
            result = 2;
        }
    }
    return result;
}

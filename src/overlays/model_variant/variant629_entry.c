#include "../../types.h"
#include "variant629_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Model629State *work = (Model629State *)context;
    s32 i;
    s32 j;
    s32 result;
    s32 step;
    s32 k, row, inactive, depth;
    u32 page;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    SVECTOR v;
    SVECTOR d[2];
    Model629ViewSnapshot view;
    PSXLONG interp, flag;

    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model629State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model629State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->frame = 0;
        page = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC80[1]);
        work->tpage0 = page >> 16;
        work->clut0 = page;
        page = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DC80[0]);
        work->tpage1 = page >> 16;
        work->clut1 = page;
        switch (command) {
        case 0:
            work->count_a = 1;
            work->count_b = 8;
            break;
        case 1:
            work->count_a = 4;
            work->count_b = 8;
            break;
        case 2:
        case 4:
            work->count_a = 10;
            work->count_b = 8;
            break;
        case 3:
            work->count_a = 6;
            work->count_b = 8;
            break;
        }
        work->width = D_8013DCB8[command].width;
        work->height = D_8013DCB8[command].height;
        work->radius = D_8013DCB8[command].radius;
        for (i = 0; i < work->count_a; i++) {
            work->a[i].f10 = 0x40;
            work->a[i].f00 = 0;
            work->a[i].f0E = 0;
            work->a[i].f08 = 1;
            work->a[i].coord = Model_GetCameraCoordinateUnit();
            work->a[i].r = D_8013DCB8[command].color_a[0];
            work->a[i].g = D_8013DCB8[command].color_a[1];
            work->a[i].b = D_8013DCB8[command].color_a[2];
            work->a[i].u = 0;
            work->a[i].v = 0;
            work->a[i].f0C = 0;
            setVector(&work->a[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->a[i].v40, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->a[i].rot, 0, work->slot * 0x800, 0);
                setVector(&work->a[i].pos, 0, 0, -((work->slot * 2 - 1) * 200));
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                setVector(&work->a[i].rot, 0, work->slot * 0x800, 0x1000 / (work->count_a * 2) + (i << 12) / work->count_a);
                setVector(&work->a[i].pos,
                    work->radius * rsin(0x1000 / (work->count_a * 2) + (i << 12) / work->count_a) / 4096
                        * (work->slot * 2 - 1),
                    work->radius * rcos(0x1000 / (work->count_a * 2) + (i << 12) / work->count_a) / 4096,
                    -((work->slot * 2 - 1) * 200));
                break;
            }
            work->a[i].pos.vy -= 350;
            work->a[i].pos.vz += (work->slot * 2 - 1) * 450;
        }
        for (i = 0; i < work->count_b; i++) {
            work->b[i].f10 = 0x230;
            work->b[i].f00 = 0;
            work->b[i].f0E = 0;
            work->b[i].f08 = 1;
            work->b[i].coord = Model_GetCameraCoordinateUnit();
            work->b[i].r = D_8013DCB8[command].color_b[0];
            work->b[i].g = D_8013DCB8[command].color_b[1];
            work->b[i].b = D_8013DCB8[command].color_b[2];
            setVector(&work->b[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->b[i].v40, 0, 0, 0);
            setVector(&work->b[i].pos, 0, 0, 0);
            setVector(&work->b[i].rot, 0, 0, 0);
        }
        for (i = 0; i < 32; i++) {
            work->c[i].f10 = 0x40;
            work->c[i].f00 = 0;
            work->c[i].f0E = 0;
            work->c[i].f08 = 1;
            work->c[i].coord = Model_GetCameraCoordinateUnit();
            work->c[i].r = D_8013DCB8[command].color_a[0];
            work->c[i].g = D_8013DCB8[command].color_a[1];
            work->c[i].b = D_8013DCB8[command].color_a[2];
            work->c[i].v = 0x10;
            work->c[i].u = (i % 7) * 32;
            setVector(&work->c[i].scale, 0, 0, 0);
            setVector(&work->c[i].v40, 0, -10, 0);
            setVector(&work->c[i].pos,
                work->width * (i % 3 + 1) * rsin(i << 7) / 4096 * ((i % 2) * 2 - 1) / 2,
                work->width * (21 - i) / 10,
                work->width * (i % 3 + 1) * rcos(i << 7) / 4096 * ((i % 2) * 2 - 1) / 2);
            setVector(&work->c[i].rot, 0, work->slot * 0x800, 0);
            work->c[i].pos.vy -= 350;
            work->c[i].pos.vz += (work->slot * 2 - 1) * 450;
            work->c[i].v50 = work->c[i].pos;
        }
        work->duration = D_8013DCB8[command].duration;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->quad[0], -work->width / 2, -work->width / 2, 0);
        setVector(&work->quad[1], work->width / 2, -work->width / 2, 0);
        setVector(&work->quad[2], -work->width / 2, work->width / 2, 0);
        setVector(&work->quad[3], work->width / 2, work->width / 2, 0);
        for (i = 0; i < 10; i++) {
            s32 angle = (i << 11) / 9;
            setVector(&work->petals[i].p[0][0], 0, work->radius * -rsin(angle + 0x400) / 4096, work->radius * -rsin(angle) / 4096);
            work->petals[i].p[0][1] = work->petals[i].p[0][0];
        }
        for (i = 1; i < 2; i++) {
            v.vx = 0;
            v.vy = (i * 0x3C000) / 360;
            v.vz = 0;
            RotMatrix(&v, &m);
            for (j = 0; j < 10; j++) {
                ApplyMatrixSV(&m, &work->petals[j].p[0][0], &work->petals[j].p[i][0]);
            }
            v.vx = 0;
            v.vy = (i * -0x3C000) / 360;
            v.vz = 0;
            RotMatrix(&v, &m);
            for (j = 0; j < 10; j++) {
                ApplyMatrixSV(&m, &work->petals[j].p[0][0], &work->petals[j].p[i][1]);
            }
        }
    }
    switch (work->command) {
    case 0:
    case 1:
        for (i = work->count_b; i >= 0; i--) {
            if (i > 0) {
                work->mats[i] = work->mats[i - 1];
                if (i == 1) {
                    switch (work->command) {
                    case 0:
                    case 1:
                        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 9);
                        break;
                    }
                    GsGetLwUnit(&unit, &ls);
                    work->mats[i - 1] = ls;
                }
            }
            switch (work->command) {
            case 0:
            case 1:
                v.vx = 0;
                v.vy = work->height / 2 + -work->height / 2 * rsin((i << 10) / work->count_b) / 4096;
                v.vz = 0;
                break;
            }
            ApplyMatrixSV(&work->mats[i], &v, &work->pairs[i].sv[0]);
            work->pairs[i].sv[0].vx += work->mats[i].t[0];
            work->pairs[i].sv[0].vy += work->mats[i].t[1];
            work->pairs[i].sv[0].vz += work->mats[i].t[2];
            switch (work->command) {
            case 0:
            case 1:
                v.vx = 0;
                v.vy = -(work->height * 2) + work->height / 2 * rsin((i << 10) / work->count_b) / 4096;
                v.vz = 0;
                break;
            }
            ApplyMatrixSV(&work->mats[i], &v, &work->pairs[i].sv[1]);
            work->pairs[i].sv[1].vx += work->mats[i].t[0];
            work->pairs[i].sv[1].vy += work->mats[i].t[1];
            work->pairs[i].sv[1].vz += work->mats[i].t[2];
        }
        break;
    case 2:
    case 3:
    case 4:
        for (i = work->count_b; i >= 0; i--) {
            if (i > 0) {
                for (j = 0; j < 2; j++) {
                    work->pairs[i].sv[j] = work->pairs[i - 1].sv[j];
                    if (i == 1) {
                        switch (work->command) {
                        case 2:
                            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, j * 10 + 12);
                            break;
                        case 3:
                        case 4:
                            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, j + 13);
                            break;
                        }
                        GsGetLwUnit(&unit, &ls);
                        setVector(&work->pairs[i - 1].sv[j], ls.t[0], ls.t[1], ls.t[2]);
                    }
                }
            }
            d[0].vx = (work->pairs[i].sv[1].vx - work->pairs[i].sv[0].vx) / 2;
            d[0].vy = (work->pairs[i].sv[1].vy - work->pairs[i].sv[0].vy) / 2;
            d[0].vz = (work->pairs[i].sv[1].vz - work->pairs[i].sv[0].vz) / 2;
            d[1].vx = (work->pairs[i].sv[0].vx - work->pairs[i].sv[1].vx) / 2;
            d[1].vy = (work->pairs[i].sv[0].vy - work->pairs[i].sv[1].vy) / 2;
            d[1].vz = (work->pairs[i].sv[0].vz - work->pairs[i].sv[1].vz) / 2;
            for (j = 0; j < 2; j++) {
                work->pairs[i].sv[j].vx += d[j].vx * rsin((i << 9) / work->count_b) / 4096;
                work->pairs[i].sv[j].vy += d[j].vy * rsin((i << 9) / work->count_b) / 4096;
                work->pairs[i].sv[j].vz += d[j].vz * rsin((i << 9) / work->count_b) / 4096;
            }
        }
        break;
    }
    if (work->duration > 0) {
        work->duration -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->frame > 40 && !work->flags[1]) {
                for (i = 0; i < work->count_a; i++) {
                    work->a[i].f00 = 1;
                }
                work->flags[1] = 1;
            }
            if (work->frame > 48 && !work->flags[2]) {
                for (i = 0; i < 32; i++) {
                    work->c[i].f00 = 1;
                }
                work->flags[2] = 1;
            }
            if (work->flags[0] < work->count_b) {
                work->b[work->flags[0]].f00 = 1;
                work->flags[0]++;
            }
            if (work->frame > 60) {
                for (i = 0; i < work->count_b; i++) {
                    work->b[i].f00 = 0;
                }
            }
            break;
        case 1:
            if (work->frame > 72 && !work->flags[1]) {
                for (i = 0; i < work->count_a; i++) {
                    work->a[i].f00 = 1;
                }
                for (i = 0; i < 32; i++) {
                    work->c[i].f00 = 1;
                }
                work->flags[1] = 1;
            }
            if (work->flags[0] < work->count_b) {
                work->b[work->flags[0]].f00 = 1;
                work->flags[0]++;
            }
            if (work->frame > 120) {
                for (i = 0; i < work->count_b; i++) {
                    work->b[i].f00 = 0;
                }
            }
            break;
        case 2:
            if (work->frame > 8 && !work->flags[1]) {
                for (i = 0; i < work->count_a; i++) {
                    work->a[i].f00 = 1;
                }
                for (i = 0; i < 32; i++) {
                    work->c[i].f00 = 1;
                }
                work->flags[1] = 1;
            }
            if (work->flags[0] < work->count_b) {
                work->b[work->flags[0]].f00 = 1;
                work->flags[0]++;
            }
            if (work->frame > 40) {
                for (i = 0; i < work->count_b; i++) {
                    work->b[i].f00 = 0;
                }
            }
            break;
        case 3:
            if (work->frame > 20 && !work->flags[1]) {
                for (i = 0; i < work->count_a; i++) {
                    work->a[i].f00 = 1;
                }
                for (i = 0; i < 32; i++) {
                    work->c[i].f00 = 1;
                }
                work->flags[1] = 1;
            }
            if (work->flags[0] < work->count_b) {
                work->b[work->flags[0]].f00 = 1;
                work->flags[0]++;
            }
            if (work->frame > 60) {
                for (i = 0; i < work->count_b; i++) {
                    work->b[i].f00 = 0;
                }
            }
            break;
        case 4:
            if (work->frame > 12 && !work->flags[1]) {
                for (i = 0; i < work->count_a; i += 2) {
                    work->a[i].f00 = 1;
                }
                for (i = 0; i < 32; i++) {
                    work->c[i].f00 = 1;
                    work->c[i].f10 = 0x50;
                }
                work->flags[1] = 1;
            }
            if (work->frame > 28 && !work->flags[2]) {
                for (i = 1; i < work->count_a; i += 2) {
                    work->a[i].f00 = 1;
                }
                work->flags[2] = 1;
            }
            if (work->flags[0] < work->count_b) {
                work->b[work->flags[0]].f00 = 1;
                work->flags[0]++;
            }
            if (work->frame > 60) {
                for (i = 0; i < work->count_b; i++) {
                    work->b[i].f00 = 0;
                }
            }
            break;
        }
        PushMatrix();
        view = *(Model629ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count_a; i++) {
            if (work->a[i].f00 == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->a[i].scale);
                RotMatrixZ(work->a[i].rot.vz, &unit.matrix);
                RotMatrixY(work->a[i].rot.vy, &unit.matrix);
                unit.matrix.t[0] = work->a[i].pos.vx;
                unit.matrix.t[1] = work->a[i].pos.vy;
                unit.matrix.t[2] = work->a[i].pos.vz;
                unit.super = work->a[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->a[i].scale.vy += 0x3000 / work->a[i].f10 * step;
                work->a[i].scale.vz += 0x3000 / work->a[i].f10 * step;
                if (work->a[i].f10 / 2 < work->a[i].f0E) {
                    if (D_8013DCB8[work->command].color_a[0] / (work->a[i].f10 / 2 - 1) * step < work->a[i].r) {
                        work->a[i].r -= D_8013DCB8[work->command].color_a[0] / (work->a[i].f10 / 2 - 1) * step;
                    } else {
                        work->a[i].r = 0;
                    }
                    if (D_8013DCB8[work->command].color_a[1] / (work->a[i].f10 / 2 - 1) * step < work->a[i].g) {
                        work->a[i].g -= D_8013DCB8[work->command].color_a[1] / (work->a[i].f10 / 2 - 1) * step;
                    } else {
                        work->a[i].g = 0;
                    }
                    if (D_8013DCB8[work->command].color_a[2] / (work->a[i].f10 / 2 - 1) * step < work->a[i].b) {
                        work->a[i].b -= D_8013DCB8[work->command].color_a[2] / (work->a[i].f10 / 2 - 1) * step;
                    } else {
                        work->a[i].b = 0;
                    }
                }
                for (j = 0; j < work->a[i].f0C; j++) {
                    for (k = 0; k < 1; k++) {
                        for (row = 0; row < 2; row++) {
                            setPolyGT4(&work->poly);
                            setSemiTrans(&work->poly, 1);
                            work->poly.tpage = work->tpage0;
                            work->poly.clut = work->clut0;
                            setUV4(&work->poly, j * 127 / 9, 1, (j + 1) * 127 / 9, 1,
                                j * 127 / 9, 15, (j + 1) * 127 / 9, 15);
                            setRGB0(&work->poly,
                                work->a[i].r * (4 - __builtin_abs(4 - j)) / 9 * ((1 - k) << 1),
                                work->a[i].g * (4 - __builtin_abs(4 - j)) / 9 * ((1 - k) << 1),
                                work->a[i].b * (4 - __builtin_abs(4 - j)) / 9 * ((1 - k) << 1));
                            if (j != work->a[i].f0C - 1) {
                                setRGB1(&work->poly,
                                    work->a[i].r * (4 - __builtin_abs(3 - j)) / 9 * ((1 - k) << 1),
                                    work->a[i].g * (4 - __builtin_abs(3 - j)) / 9 * ((1 - k) << 1),
                                    work->a[i].b * (4 - __builtin_abs(3 - j)) / 9 * ((1 - k) << 1));
                                setRGB3(&work->poly,
                                    work->a[i].r * (4 - __builtin_abs(3 - j)) / 9 * ((1 - (k + 1)) << 1),
                                    work->a[i].g * (4 - __builtin_abs(3 - j)) / 9 * ((1 - (k + 1)) << 1),
                                    work->a[i].b * (4 - __builtin_abs(3 - j)) / 9 * ((1 - (k + 1)) << 1));
                            } else {
                                setRGB1(&work->poly, 0, 0, 0);
                                setRGB3(&work->poly, 0, 0, 0);
                            }
                            setRGB2(&work->poly,
                                work->a[i].r * (4 - __builtin_abs(4 - j)) / 9 * ((1 - (k + 1)) << 1),
                                work->a[i].g * (4 - __builtin_abs(4 - j)) / 9 * ((1 - (k + 1)) << 1),
                                work->a[i].b * (4 - __builtin_abs(4 - j)) / 9 * ((1 - (k + 1)) << 1));
                            depth = RotAverage4(&work->petals[j].p[k][row], &work->petals[j + 1].p[k][row],
                                &work->petals[j].p[k + 1][row], &work->petals[j + 1].p[k + 1][row],
                                (PSXLONG *)&work->poly.x0, (PSXLONG *)&work->poly.x1,
                                (PSXLONG *)&work->poly.x2, (PSXLONG *)&work->poly.x3, &interp, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->poly, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                }
                if (work->a[i].f0C < 9) {
                    work->a[i].f0C++;
                }
                work->a[i].f0E += step;
                if (work->a[i].f10 < work->a[i].f0E) {
                    work->a[i].f00 = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count_b; i++) {
            if (work->b[i].f00 == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->b[i].scale);
                unit.matrix.t[0] = work->b[i].pos.vx;
                unit.matrix.t[1] = work->b[i].pos.vy;
                unit.matrix.t[2] = work->b[i].pos.vz;
                unit.super = work->b[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                d[0].vx = (work->pairs[i].sv[0].vx + work->pairs[i].sv[1].vx) / 2;
                d[0].vy = (work->pairs[i].sv[0].vy + work->pairs[i].sv[1].vy) / 2;
                d[0].vz = (work->pairs[i].sv[0].vz + work->pairs[i].sv[1].vz) / 2;
                d[1].vx = (work->pairs[i + 1].sv[0].vx + work->pairs[i + 1].sv[1].vx) / 2;
                d[1].vy = (work->pairs[i + 1].sv[0].vy + work->pairs[i + 1].sv[1].vy) / 2;
                d[1].vz = (work->pairs[i + 1].sv[0].vz + work->pairs[i + 1].sv[1].vz) / 2;
                setPolyG4(&work->g4);
                setRGB0(&work->g4, 0, 0, 0);
                setRGB2(&work->g4, 0, 0, 0);
                setRGB1(&work->g4,
                    work->b[i].r * (work->count_b - i) / work->count_b,
                    work->b[i].g * (work->count_b - i) / work->count_b,
                    work->b[i].b * (work->count_b - i) / work->count_b);
                if (work->flags[0] - 1 == i) {
                    setRGB3(&work->g4, 0, 0, 0);
                } else {
                    setRGB3(&work->g4,
                        work->b[i].r * (work->count_b - i - 1) / work->count_b,
                        work->b[i].g * (work->count_b - i - 1) / work->count_b,
                        work->b[i].b * (work->count_b - i - 1) / work->count_b);
                }
                depth = RotAverage4(&work->pairs[i].sv[0], &d[0], &work->pairs[i + 1].sv[0], &d[1],
                    (PSXLONG *)&work->g4.x0, (PSXLONG *)&work->g4.x1,
                    (PSXLONG *)&work->g4.x2, (PSXLONG *)&work->g4.x3, &interp, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->g4, func_80058F10(), (u16)depth, work->b[i].f08);
                }
                setPolyG4(&work->g4);
                setRGB1(&work->g4, 0, 0, 0);
                setRGB3(&work->g4, 0, 0, 0);
                setRGB0(&work->g4,
                    work->b[i].r * (work->count_b - i) / work->count_b,
                    work->b[i].g * (work->count_b - i) / work->count_b,
                    work->b[i].b * (work->count_b - i) / work->count_b);
                if (work->flags[0] - 1 == i) {
                    setRGB2(&work->g4, 0, 0, 0);
                } else {
                    setRGB2(&work->g4,
                        work->b[i].r * (work->count_b - i - 1) / work->count_b,
                        work->b[i].g * (work->count_b - i - 1) / work->count_b,
                        work->b[i].b * (work->count_b - i - 1) / work->count_b);
                }
                depth = RotAverage4(&d[0], &work->pairs[i].sv[1], &d[1], &work->pairs[i + 1].sv[1],
                    (PSXLONG *)&work->g4.x0, (PSXLONG *)&work->g4.x1,
                    (PSXLONG *)&work->g4.x2, (PSXLONG *)&work->g4.x3, &interp, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->g4, func_80058F10(), (u16)depth, work->b[i].f08);
                }
                work->b[i].f0E += step;
                if (work->b[i].f10 < work->b[i].f0E) {
                    work->b[i].f00 = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 32; i++) {
            if (work->c[i].f00 == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->c[i].scale);
                RotMatrixX(work->c[i].rot.vx - (view.pitch - 0x1000) * (work->slot * 2 - 1), &unit.matrix);
                RotMatrixY(work->c[i].rot.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->c[i].pos.vx;
                unit.matrix.t[1] = work->c[i].pos.vy;
                unit.matrix.t[2] = work->c[i].pos.vz;
                unit.super = work->c[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->c[i].pos.vy += work->c[i].v40.vy * step;
                if (work->c[i].scale.vx < 0x1000) {
                    work->c[i].scale.vx += step * 819 / 2;
                    work->c[i].scale.vy += step * 819 / 2;
                    work->c[i].scale.vz += step * 819 / 2;
                } else {
                    work->c[i].scale.vx = 0x1000;
                    work->c[i].scale.vy = 0x1000;
                    work->c[i].scale.vz = 0x1000;
                }
                if (work->c[i].f10 / 2 < work->c[i].f0E) {
                    if (D_8013DCB8[work->command].color_a[0] / (work->c[i].f10 / 2 - 1) * step < work->c[i].r) {
                        work->c[i].r -= D_8013DCB8[work->command].color_a[0] / (work->c[i].f10 / 2 - 1) * step;
                    } else {
                        work->c[i].r = 0;
                    }
                    if (D_8013DCB8[work->command].color_a[1] / (work->c[i].f10 / 2 - 1) * step < work->c[i].g) {
                        work->c[i].g -= D_8013DCB8[work->command].color_a[1] / (work->c[i].f10 / 2 - 1) * step;
                    } else {
                        work->c[i].g = 0;
                    }
                    if (D_8013DCB8[work->command].color_a[2] / (work->c[i].f10 / 2 - 1) * step < work->c[i].b) {
                        work->c[i].b -= D_8013DCB8[work->command].color_a[2] / (work->c[i].f10 / 2 - 1) * step;
                    } else {
                        work->c[i].b = 0;
                    }
                }
                setPolyFT4(&work->ft4);
                setSemiTrans(&work->ft4, 1);
                work->ft4.tpage = work->tpage1;
                work->ft4.clut = work->clut1;
                setUV4(&work->ft4, work->c[i].u, work->c[i].v, work->c[i].u + 31, work->c[i].v,
                    work->c[i].u, work->c[i].v + 31, work->c[i].u + 31, work->c[i].v + 31);
                setRGB0(&work->ft4, work->c[i].r, work->c[i].g, work->c[i].b);
                depth = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3],
                    (PSXLONG *)&work->ft4.x0, (PSXLONG *)&work->ft4.x1,
                    (PSXLONG *)&work->ft4.x2, (PSXLONG *)&work->ft4.x3, &interp, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->ft4, func_80058F10(), (u16)depth);
                }
                work->c[i].u += 32;
                if (work->c[i].u == 0xE0) {
                    work->c[i].u = 0;
                    work->c[i].pos = work->c[i].v50;
                }
                work->c[i].f0E += step;
                if (work->c[i].f10 < work->c[i].f0E) {
                    work->c[i].f00 = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->frame += step;
        if (inactive == work->count_a + work->count_b + 32) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

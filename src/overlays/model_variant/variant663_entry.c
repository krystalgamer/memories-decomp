#include "../../types.h"
#include "variant663_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 663's entry: strip columns that spawn at a tracked model point and
 * fan out as sixteen Gouraud strips, plus up to four bursts of 32 rays with
 * Gouraud triangles and a spinning sixteen-strip fan, released on a
 * per-command timetable. The setup also prints the work-area size. */

#define SPAWN_COLUMN(point)                                                     \
    work->columns[work->flags[0]].active = 1;                                   \
    work->columns[work->flags[0]].elapsed = 0;                                  \
    work->columns[work->flags[0]].duration = 32;                                \
    work->columns[work->flags[0]].mode = 1;                                     \
    work->columns[work->flags[0]].coord = Model_GetCameraCoordinateUnit();      \
    work->columns[work->flags[0]].color[0] = D_8013E38C[work->command].color[0]; \
    work->columns[work->flags[0]].color[1] = D_8013E38C[work->command].color[1]; \
    work->columns[work->flags[0]].color[2] = D_8013E38C[work->command].color[2]; \
    work->columns[work->flags[0]].u = 0;                                        \
    work->columns[work->flags[0]].v = 0x80;                                     \
    value = 0x800;                                                              \
    setVector(&work->columns[work->flags[0]].scale, value, value, value);       \
    setVector(&work->columns[work->flags[0]].unknown_3C, 0, 0, 0);              \
    setVector(&work->columns[work->flags[0]].rotation, 0, 0, 0);                \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, point);           \
    GsGetLwUnit(&unit, &ls);                                                    \
    p0.vx = ls.t[0];                                                            \
    p0.vy = ls.t[1];                                                            \
    p0.vz = ls.t[2];                                                            \
    work->matrices[work->flags[0]] = ls

#define NEXT_COLUMN()                                                           \
    work->flags[0]++;                                                           \
    if (work->flags[0] >= work->column_count) {                                 \
        work->flags[0] = 0;                                                     \
    }

#define SPAWN_COLUMN_AT(point)                                                  \
    SPAWN_COLUMN(point);                                                        \
    setVector(&work->columns[work->flags[0]].position, p0.vx, p0.vy, p0.vz);    \
    NEXT_COLUMN()

#define RELEASE_BURST(index, threshold, code)                                   \
    if (work->elapsed >= threshold) {                                           \
        work->bursts[index].active = 1;                                         \
        result = code;                                                          \
        work->flags[1]++;                                                       \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model663State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model663ViewSnapshot view;
    SVECTOR p0, p1;
    SVECTOR pts[4][2];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    /* VALUE is shared by the 0x800 scale, the fades and both strip weights;
     * N is the fade divisor and the variable 8 the strip colours divide by. */
    s32 i, j, k, n, value;
    /* The strip colours are u16 and split into two sets; together with the
     * separate D0/D1 operands of the two ABS weights this keeps the strip
     * loop large enough that its column address is not hoisted. */
    u16 red, green, blue;
    s32 d0, d1, angle;
    u16 red1, green1, blue1;
    s32 r, g, b;
    u32 tex;
    PSXLONG depth;

    work = (Model663State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model663State));
        printf("E_DATA size = %x byte\n", sizeof(Model663State));
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E354[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E354[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 3:
        case 4:
            work->column_count = 0;
            work->burst_count = 2;
            break;
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
            work->column_count = 0;
            work->burst_count = 1;
            break;
        case 8:
            work->column_count = 0;
            work->burst_count = 3;
            break;
        case 9:
        case 13:
        case 14:
            work->column_count = 64;
            work->burst_count = 1;
            break;
        case 10:
        case 11:
            work->column_count = 64;
            work->burst_count = 4;
            break;
        case 12:
            work->column_count = 0;
            work->burst_count = 4;
            break;
        }
        work->radius = D_8013E38C[command].radius;
        work->height = D_8013E38C[command].height;
        for (i = 0; i < work->column_count; i++) {
            work->columns[i].active = 0;
            work->columns[i].elapsed = 0;
            work->columns[i].duration = 32;
            work->columns[i].mode = 1;
            work->columns[i].coord = Model_GetCameraCoordinateUnit();
            work->columns[i].color[0] = D_8013E38C[command].color[0];
            work->columns[i].color[1] = D_8013E38C[command].color[1];
            work->columns[i].color[2] = D_8013E38C[command].color[2];
            work->columns[i].u = 0;
            work->columns[i].v = 0x80;
            value = 0x800;
            setVector(&work->columns[i].scale, value, value, value);
            setVector(&work->columns[i].unknown_3C, 0, 0, 0);
            setVector(&work->columns[i].rotation, 0, 0, 0);
            setVector(&work->columns[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 48;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013E38C[command].color[3];
            work->bursts[i].color[1] = D_8013E38C[command].color[4];
            work->bursts[i].color[2] = D_8013E38C[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 0x80;
            value = 0x800;
            setVector(&work->bursts[i].scale, value, value, value);
            setVector(&work->bursts[i].unknown_3C, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->bursts[i].position, (i * 2 - 1) * -(work->direction * 50), 0, 0);
                break;
            case 6:
                setVector(&work->bursts[i].position, 0, 50, 0);
                break;
            default:
                setVector(&work->bursts[i].position, 0, 0, 0);
                break;
            }
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013E38C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        switch (command) {
        case 9:
        case 10:
        case 11:
            for (i = 0; i < 17; i++) {
                setVector(&work->spokes[i][0], 0, work->radius * rcos(i << 7) / 4096,
                    work->radius * rsin(i << 7) / 4096);
                setVector(&work->spokes[i][1], 0, work->radius * rcos(i << 7) / 4096,
                    work->height + work->radius * rsin(i << 7) / 4096);
            }
            break;
        case 13:
            for (i = 0; i < 17; i++) {
                setVector(&work->spokes[i][0], work->radius * -rcos(i << 7) / 4096,
                    work->radius * -rsin(i << 7) / 4096, 0);
                /* Negated as "* -1" so the sum is formed in the sine's register. */
                setVector(&work->spokes[i][1], work->radius * -rcos(i << 7) / 4096,
                    (work->radius * rsin(i << 7) / 4096 + work->height) * -1, 0);
            }
            break;
        case 14:
            for (i = 0; i < 17; i++) {
                setVector(&work->spokes[i][0], work->radius * -rcos(i << 7) / 4096, 0,
                    work->radius * -rsin(i << 7) / 4096);
                setVector(&work->spokes[i][1], work->radius * -rcos(i << 7) / 4096, 0,
                    work->height + work->radius * -rsin(i << 7) / 4096);
            }
            break;
        }
        for (i = 0; i < 33; i++) {
            setVector(&work->rays[i][0], work->radius * rcos(i << 7) / 4096,
                work->radius * rsin(i << 7) / 4096, 0);
            setVector(&work->rays[i][1], (work->radius * 2 + work->radius * (i % 2)) * rcos(i << 7) / 4096,
                (work->radius * 2 + work->radius * (i % 2)) * rsin(i << 7) / 4096, 0);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->fan[i][0], (work->radius >> 1) * rsin(i << 8) / 4096,
                (work->radius >> 1) * rcos(i << 8) / 4096, 0);
            setVector(&work->fan[i][1], (work->radius * 3 >> 1) * rsin(i << 8) / 4096,
                (work->radius * 3 >> 1) * rcos(i << 8) / 4096, 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed == 0) {
                    work->bursts[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                RELEASE_BURST(1, 15, 1);
                break;
            }
            break;
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
            if (work->elapsed == 0) {
                work->bursts[0].active = 1;
                result = 1;
            }
            break;
        case 3:
        case 4:
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed == 0) {
                    work->bursts[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                RELEASE_BURST(1, 13, 1);
                break;
            }
            break;
        case 8:
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed == 0) {
                    work->bursts[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                RELEASE_BURST(1, 13, 4);
                break;
            case 2:
                RELEASE_BURST(2, 29, 1);
                break;
            }
            break;
        case 9:
            if (work->elapsed < 24) {
                SPAWN_COLUMN(31);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 32);
                GsGetLwUnit(&unit, &ls);
                p1.vx = ls.t[0];
                p1.vy = ls.t[1];
                p1.vz = ls.t[2];
                setVector(&work->columns[work->flags[0]].position, (p0.vx + p1.vx) / 2,
                    (p0.vy + p1.vy) / 2, (p0.vz + p1.vz) / 2);
                NEXT_COLUMN();
            }
            if (work->elapsed >= 19 && work->flags[1] == 0) {
                work->bursts[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 10:
            if (work->elapsed < 106) {
                SPAWN_COLUMN_AT(23);
            }
            switch (work->flags[1]) {
            case 0:
                RELEASE_BURST(0, 17, 4);
                break;
            case 1:
                RELEASE_BURST(1, 45, 4);
                break;
            case 2:
                RELEASE_BURST(2, 73, 4);
                break;
            case 3:
                RELEASE_BURST(3, 101, 1);
                break;
            }
            break;
        case 11:
            if (work->elapsed < 110) {
                SPAWN_COLUMN_AT(5);
            }
            switch (work->flags[1]) {
            case 0:
                RELEASE_BURST(0, 25, 4);
                break;
            case 1:
                RELEASE_BURST(1, 41, 4);
                break;
            case 2:
                RELEASE_BURST(2, 57, 4);
                break;
            case 3:
                RELEASE_BURST(3, 73, 1);
                break;
            }
            break;
        case 12:
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed == 0) {
                    work->bursts[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                RELEASE_BURST(1, 21, 4);
                break;
            case 2:
                RELEASE_BURST(2, 47, 4);
                break;
            case 3:
                RELEASE_BURST(3, 63, 1);
                break;
            }
            break;
        case 13:
            if (work->elapsed < 40) {
                SPAWN_COLUMN_AT(19);
            }
            if (work->elapsed >= 21 && work->flags[1] == 0) {
                work->bursts[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 14:
            if (work->elapsed < 134) {
                SPAWN_COLUMN_AT(1);
            }
            if (work->elapsed >= 99 && work->flags[1] == 0) {
                work->bursts[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model663ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->column_count; i++) {
            if (work->columns[i].active == 1) {
                unit.flg = 0;
                unit.matrix = work->matrices[i];
                value = (work->columns[i].elapsed << 13) / work->columns[i].duration + 0x800;
                setVector(&work->columns[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->columns[i].scale);
                unit.matrix.t[0] = work->columns[i].position.vx;
                unit.matrix.t[1] = work->columns[i].position.vy;
                unit.matrix.t[2] = work->columns[i].position.vz;
                unit.super = work->columns[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->columns[i].duration / 2 < work->columns[i].elapsed) {
                    value = (work->columns[i].duration - work->columns[i].elapsed) * 2;
                    n = work->columns[i].duration;
                    work->columns[i].color[0] = D_8013E38C[work->command].color[0] * value / n;
                    work->columns[i].color[1] = D_8013E38C[work->command].color[1] * value / n;
                    work->columns[i].color[2] = D_8013E38C[work->command].color[2] * value / n;
                }
                /* Each strip fades toward its ends: 8 - |8 - k| on the inner
                 * corners and 8 - |7 - k| on the outer ones, in eighths. */
                for (k = 0, n = 8; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->columns[i].u, work->columns[i].v,
                        work->columns[i].u + 31, work->columns[i].v,
                        work->columns[i].u, work->columns[i].v + 127,
                        work->columns[i].u + 31, work->columns[i].v + 127);
                    d0 = 8 - k;
                    value = 8 - ABS(d0);
                    red = work->columns[i].color[0] * value / n;
                    green = work->columns[i].color[1] * value / n;
                    blue = work->columns[i].color[2] * value / n;
                    d1 = 7 - k;
                    value = n - ABS(d1);
                    setRGB0(&work->strip, red, green, blue);
                    setRGB2(&work->strip, red, green, blue);
                    red1 = work->columns[i].color[0] * value / n;
                    green1 = work->columns[i].color[1] * value / n;
                    blue1 = work->columns[i].color[2] * value / n;
                    setRGB1(&work->strip, red1, green1, blue1);
                    setRGB3(&work->strip, red1, green1, blue1);
                    depth = RotAverage4(&work->spokes[k][0], &work->spokes[k + 1][0],
                        &work->spokes[k][1], &work->spokes[k + 1][1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->columns[i].elapsed += step;
                if (work->columns[i].duration < work->columns[i].elapsed) {
                    work->columns[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->bursts[i].elapsed << 13) / work->bursts[i].duration + 0x800;
                setVector(&work->bursts[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    n = work->bursts[i].duration;
                    work->bursts[i].color[0] = D_8013E38C[work->command].color[3] * value / n;
                    work->bursts[i].color[1] = D_8013E38C[work->command].color[4] * value / n;
                    work->bursts[i].color[2] = D_8013E38C[work->command].color[5] * value / n;
                }
                /* P0 becomes the burst centre shared by every ray triangle. */
                setVector(&p0, 0, 0, 0);
                for (k = 0; k < 32; k++) {
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v,
                        work->bursts[i].u + 31, work->bursts[i].v,
                        work->bursts[i].u, work->bursts[i].v + 127,
                        work->bursts[i].u + 31, work->bursts[i].v + 127);
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->sprite, r, g, b);
                    depth = RotAverage4(&work->rays[k][0], &work->rays[k + 1][0],
                        &work->rays[k][1], &work->rays[k + 1][1],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    setPolyG3(&work->tri);
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->tri, r, g, b);
                    setRGB1(&work->tri, 0, 0, 0);
                    setRGB2(&work->tri, 0, 0, 0);
                    depth = RotAverage3(&p0, &work->rays[k][0], &work->rays[k + 1][0],
                        (PSXLONG *)&work->tri.x0, (PSXLONG *)&work->tri.x1,
                        (PSXLONG *)&work->tri.x2, &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->tri, func_80058F10(), (u16)depth,
                            work->bursts[i].mode);
                    }
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    if (k % 2 == 0) {
                        setUV4(&work->strip, work->bursts[i].u, work->bursts[i].v + 128,
                            work->bursts[i].u + 63, work->bursts[i].v + 128,
                            work->bursts[i].u, work->bursts[i].v + 255,
                            work->bursts[i].u + 63, work->bursts[i].v + 255);
                    } else {
                        setUV4(&work->strip, work->bursts[i].u + 63, work->bursts[i].v + 128,
                            work->bursts[i].u, work->bursts[i].v + 128,
                            work->bursts[i].u + 63, work->bursts[i].v + 255,
                            work->bursts[i].u, work->bursts[i].v + 255);
                    }
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    PushMatrix();
                    m = D_800FE168;
                    /* Alternate strips spin the other way, faster outward. */
                    angle = ((k % 2) * 2 - 1) * 55;
                    RotMatrixZ(work->bursts[i].elapsed * (k * 5 + 1) * angle, &m);
                    pts[0][0] = work->fan[k][0];
                    pts[1][0] = work->fan[k + 1][0];
                    pts[2][0] = work->fan[k][1];
                    pts[3][0] = work->fan[k + 1][1];
                    for (j = 0; j < 4; j++) {
                        ApplyMatrixSV(&m, &pts[j][0], &pts[j][1]);
                    }
                    PopMatrix();
                    depth = RotAverage4(&pts[0][1], &pts[1][1], &pts[2][1], &pts[3][1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
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
        if (inactive == work->column_count + work->burst_count) {
            result = 2;
        }
    }
    return result;
}

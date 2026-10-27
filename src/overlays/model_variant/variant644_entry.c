#include "../../types.h"
#include "variant644_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 644's entry: one or two glowing cages (a Gouraud band and fan)
 * and spinning line stars follow a model joint, while up to 32 line-cage
 * bursts spawn on a timer around the model, grow and fade. The setup also
 * prints the work-area size. */

/* The work-size format string; retail's padding byte after the terminator is 0x3C. */
#ifndef VERSION_FRENCH
static const char D_8013D200[24] = "E_DATA size = %x byte\n\0<";
#endif

/* Moves every glow and star to the joint's world position. */
#define MODEL644_PLACE_ALL(joint)                                                    \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, joint);                \
    GsGetLwUnit(&unit, &ls);                                                         \
    for (i = 0; i < work->count[0]; i++) {                                           \
        work->glows[i].active = 1;                                                   \
        setVector(&work->glows[i].position, ls.t[0], ls.t[1], ls.t[2]);              \
    }                                                                                \
    for (i = 0; i < work->count[1]; i++) {                                           \
        work->stars[i].active = 1;                                                   \
        setVector(&work->stars[i].position, ls.t[0], ls.t[1], ls.t[2]);              \
    }

/* Advances the spawn timer, wrapping after six frames. */
#define MODEL644_TICK()                                                              \
    work->flags[0] += step;                                                          \
    if (work->flags[0] >= 6) {                                                       \
        work->flags[0] = 0;                                                          \
    }

/* Starts the burst under the cursor, then advances the cursor. */
#define MODEL644_SPAWN_BURST(limit)                                                  \
    if (work->elapsed < limit) {                                                     \
        work->bursts[work->flags[1]].active = 1;                                     \
        work->bursts[work->flags[1]].elapsed = 0;                                    \
        setVector(&work->bursts[work->flags[1]].scale, 0, 0, 0);                     \
        work->bursts[work->flags[1]].color[0] = D_8013D234[work->command].color[0];  \
        work->bursts[work->flags[1]].color[1] = D_8013D234[work->command].color[1];  \
        work->bursts[work->flags[1]].color[2] = D_8013D234[work->command].color[2];  \
        work->flags[1]++;                                                            \
        if (work->flags[1] >= work->count[2]) {                                      \
            work->flags[1] = 0;                                                      \
        }                                                                            \
    }

/* The cage ring's horizontal radius on a sphere of radius * scale at the
 * height of ring i. Assigning RISE and RISE2 inside the products keeps
 * radius * (cos * scale) from being reassociated into (radius * scale) * cos;
 * two variables keep retail's temporaries apart. */
#define MODEL644_CAGE_REACH(scale, square)                                           \
    SquareRoot0(work->radius * square * work->radius                                 \
        - work->radius * (rise = rcos(i << 8) * scale) / 4096                        \
            * (work->radius * (rise2 = rcos(i << 8) * scale) / 4096))

s32 func_8013B004(u8 *context, s32 command)
{
    Model644State *work;
    MATRIX ls, world;
    GsCOORDUNIT unit;
    Model644ViewSnapshot view;
    SVECTOR middle;
    PSXLONG interpolation, flag, sxy;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, divisor, spread, rise, rise2;
    u8 red, green, blue;
    u32 tex;
    PSXLONG depth;

    work = (Model644State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model644State));
#ifndef VERSION_FRENCH
        printf(D_8013D200, sizeof(Model644State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D218);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->count[0] = 1;
            work->count[1] = 1;
            work->count[2] = 32;
            break;
        case 3:
            work->count[0] = 2;
            work->count[1] = 2;
            work->count[2] = 32;
            break;
        }
        work->radius = D_8013D234[command].radius;
        work->height = D_8013D234[command].height;
        for (i = 0; i < work->count[0]; i++) {
            work->glows[i].active = 0;
            work->glows[i].elapsed = 0;
            work->glows[i].duration = 0;
            work->glows[i].mode = 1;
            work->glows[i].coord = Model_GetCameraCoordinateUnit();
            work->glows[i].color[0] = D_8013D234[command].color[0];
            work->glows[i].color[1] = D_8013D234[command].color[1];
            work->glows[i].color[2] = D_8013D234[command].color[2];
            work->glows[i].u = 0;
            work->glows[i].v = 0;
            setVector(&work->glows[i].scale, 4096, 4096, 4096);
            setVector(&work->glows[i].unknown_40, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->glows[i].rotation, 0, work->slot * 2048, 0);
                break;
            case 1:
            case 2:
            case 3:
                setVector(&work->glows[i].rotation, 0, 0, 0);
                break;
            }
            setVector(&work->glows[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count[1]; i++) {
            work->stars[i].active = 0;
            work->stars[i].elapsed = 0;
            work->stars[i].duration = 0;
            work->stars[i].mode = 1;
            work->stars[i].coord = Model_GetCameraCoordinateUnit();
            work->stars[i].color[0] = D_8013D234[command].color[0];
            work->stars[i].color[1] = D_8013D234[command].color[1];
            work->stars[i].color[2] = D_8013D234[command].color[2];
            setVector(&work->stars[i].scale, 4096, 4096, 4096);
            setVector(&work->stars[i].unknown_40, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->stars[i].rotation, 0, work->slot * 2048, 0);
                break;
            case 1:
            case 2:
            case 3:
                setVector(&work->stars[i].rotation, 0, 0, 0);
                break;
            }
            setVector(&work->stars[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count[2]; i++) {
            work->bursts[i].duration = 32;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013D234[command].color[0];
            work->bursts[i].color[1] = D_8013D234[command].color[1];
            work->bursts[i].color[2] = D_8013D234[command].color[2];
            setVector(&work->bursts[i].scale, 0, 0, 0);
            setVector(&work->bursts[i].unknown_40, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0,
                work->slot * 2048 + (i << 12) / work->count[2], 512);
            /* The "<< 2" factors (and "<< 1" and "value << 2" below) keep
             * GCC from moving the constant onto the radius. */
            setVector(&work->bursts[i].position,
                ((i % 7) % 2 * 2 - 1) * (work->radius * ((i % 3 + 1) << 2) * rsin((i << 12) / work->count[2]) / 4096),
                ((i % 3) % 2 * 2 - 1) * -(work->radius * ((i % 4 + 1) << 2) * rsin((i << 12) / work->count[2]) / 4096),
                (i % 2 * 2 - 1) * (work->radius * ((i % 5 + 1) << 2) * rsin((i << 12) / work->count[2]) / 4096));
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013D234[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (j = 0; j < 4; j++) {
            value = rsin((j + 1) << 8);
            value = value * value / 4096;
            for (i = 0; i < 9; i++) {
                setVector(&work->band[i][j], work->radius * value / 4096 * rsin(i << 9) / 4096,
                    work->radius * value / 4096 * rcos(i << 9) / 4096, 0);
                setVector(&work->band[i][j + 4], work->radius * (value << 2) / 4096 * rsin(i << 9) / 4096,
                    work->radius * (value << 2) / 4096 * rcos(i << 9) / 4096,
                    work->height * -rcos(j << 8) / 4096);
            }
        }
        for (i = 0; i < 9; i++) {
            setVector(&work->rim[i], work->radius * (rsin(i << 9) << 1) / 4096,
                work->radius * (rcos(i << 9) << 1) / 4096, 0);
        }
        for (i = 0; i < 8; i++) {
            /* Assigning SPREAD inside the product keeps i * (sign * 10)
             * from being reassociated into a strength-reduced i * 10. */
            setVector(&work->spokes[i].from, i * (spread = (i % 2 * 2 - 1) * 10) * rsin(i << 9) / 4096,
                i * spread * rcos(i << 9) / 4096, 0);
            setVector(&work->spokes[i].to, i * spread * rsin(i << 9) / 4096,
                i * spread * rcos(i << 9) / 4096, -(work->height * 3));
        }
        for (i = 0; i < 9; i++) {
            /* j * 512 rather than j << 9 keeps the angle out of loop
             * strength reduction, as in retail. */
            for (j = 0; j < 8; j++) {
                setVector(&work->cage[j][i].from, MODEL644_CAGE_REACH(6, 36) * rsin(j * 512) / 4096,
                    work->radius * -(rcos(i << 8) * 6) / 4096,
                    MODEL644_CAGE_REACH(6, 36) * rcos(j * 512) / 4096);
                setVector(&work->cage[j][i].to, MODEL644_CAGE_REACH(10, 100) * rsin(j * 512) / 4096,
                    work->radius * -(rcos(i << 8) * 10) / 4096,
                    MODEL644_CAGE_REACH(10, 100) * rcos(j * 512) / 4096);
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed < 250 && work->flags[0] == 0) {
            MODEL644_PLACE_ALL(17);
        }
        MODEL644_TICK();
        MODEL644_SPAWN_BURST(250);
        break;
    case 1:
        if (work->elapsed < 180 && work->flags[0] == 0) {
            MODEL644_PLACE_ALL(9);
        }
        MODEL644_TICK();
        MODEL644_SPAWN_BURST(180);
        break;
    case 2:
        if (work->elapsed < 250 && work->flags[0] == 0) {
            MODEL644_PLACE_ALL(3);
        }
        MODEL644_TICK();
        MODEL644_SPAWN_BURST(250);
        break;
    case 3:
        if (work->elapsed < 100 && work->flags[0] == 0) {
            for (i = 0; i < work->count[0]; i++) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, (i + 1) * 6);
                GsGetLwUnit(&unit, &ls);
                work->glows[i].active = 1;
                setVector(&work->glows[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->count[1]; i++) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, (i + 1) * 6);
                GsGetLwUnit(&unit, &ls);
                work->stars[i].active = 1;
                setVector(&work->stars[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        MODEL644_TICK();
        MODEL644_SPAWN_BURST(100);
        break;
    }
    PushMatrix();
    view = *(Model644ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    switch (work->command) {
    case 0:
        world = D_800FE168;
        break;
    case 1:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 9);
        GsGetLwUnit(&unit, &world);
        break;
    case 2:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
        GsGetLwUnit(&unit, &world);
        break;
    case 3:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 4);
        GsGetLwUnit(&unit, &world);
        break;
    }
    for (i = 0; i < work->count[0]; i++) {
        if (work->glows[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->glows[i].scale);
            RotMatrixY(work->glows[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->glows[i].position.vx;
            unit.matrix.t[1] = work->glows[i].position.vy;
            unit.matrix.t[2] = work->glows[i].position.vz;
            unit.super = work->glows[i].coord;
            PushMatrix();
            MulMatrix(&unit.matrix, &world);
            PopMatrix();
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            for (k = 0; k < 4; k++) {
                /* Inside the loop, the constant divisor is set after the
                 * counter; VALUE holds k + 1 so it lands in retail's s0. */
                divisor = 4;
                value = k + 1;
                red = D_8013D234[work->command].color[0] * value / divisor;
                green = D_8013D234[work->command].color[1] * value / divisor;
                blue = D_8013D234[work->command].color[2] * value / divisor;
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->glows[i].u, work->glows[i].v,
                        work->glows[i].u + 31, work->glows[i].v,
                        work->glows[i].u, work->glows[i].v + 127,
                        work->glows[i].u + 31, work->glows[i].v + 127);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, red, green, blue);
                    setRGB3(&work->strip, red, green, blue);
                    depth = RotAverage4(&work->band[j][k], &work->band[j + 1][k],
                        &work->band[j][k + 4], &work->band[j + 1][k + 4],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            setVector(&middle, 0, 0, -(work->height * 3) / 2);
            for (j = 0; j < 8; j++) {
                setPolyGT3(&work->fan);
                work->fan.tpage = work->tpage[0];
                work->fan.clut = work->clut[0];
                setSemiTrans(&work->fan, 1);
                setUV3(&work->fan, work->glows[i].u, work->glows[i].v,
                    work->glows[i].u + 31, work->glows[i].v,
                    work->glows[i].u + 15, work->glows[i].v + 127);
                setRGB0(&work->fan, 0, 0, 0);
                setRGB1(&work->fan, 0, 0, 0);
                setRGB2(&work->fan, red, green, blue);
                depth = RotAverage3(&work->rim[j], &work->rim[j + 1], &middle,
                    (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                }
            }
            work->glows[i].elapsed += step;
            if (work->glows[i].duration < work->glows[i].elapsed) {
                work->glows[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count[1]; i++) {
        if (work->stars[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->stars[i].scale);
            switch (work->command) {
            case 0:
            case 2:
            case 3:
                RotMatrixZ(work->stars[i].rotation.vz, &unit.matrix);
                break;
            }
            RotMatrixY(work->stars[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->stars[i].position.vx;
            unit.matrix.t[1] = work->stars[i].position.vy;
            unit.matrix.t[2] = work->stars[i].position.vz;
            unit.super = work->stars[i].coord;
            PushMatrix();
            MulMatrix2(&world, &unit.matrix);
            PopMatrix();
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->stars[i].rotation.vz += step * 375;
            for (j = 0; j < 8; j++) {
                work->line.attribute = 0x50000000;
                work->line.r0 = 0;
                work->line.g0 = 0;
                work->line.b0 = 0;
                work->line.r1 = work->stars[i].color[0];
                work->line.g1 = work->stars[i].color[1];
                work->line.b1 = work->stars[i].color[2];
                RotTransPers(&work->spokes[j].from, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&work->spokes[j].to, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                setVector(&middle, (work->spokes[j].from.vx + work->spokes[j].to.vx) / 2,
                    (work->spokes[j].from.vy + work->spokes[j].to.vy) / 2,
                    (work->spokes[j].from.vz + work->spokes[j].to.vz) / 2);
                depth = RotTransPers(&middle, &sxy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                }
            }
            work->stars[i].elapsed += step;
            if (work->stars[i].duration < work->stars[i].elapsed) {
                work->stars[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count[2]; i++) {
        if (work->bursts[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
            RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
            RotMatrixY(work->bursts[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->bursts[i].position.vx;
            unit.matrix.t[1] = work->bursts[i].position.vy;
            unit.matrix.t[2] = work->bursts[i].position.vz;
            unit.super = work->bursts[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                /* The divisor is reloaded into DIVISOR, as in 659. */
                divisor = work->bursts[i].duration;
                work->bursts[i].color[0] = D_8013D234[work->command].color[0] * value / divisor;
                work->bursts[i].color[1] = D_8013D234[work->command].color[1] * value / divisor;
                work->bursts[i].color[2] = D_8013D234[work->command].color[2] * value / divisor;
            }
            value = work->bursts[i].elapsed * 3 * 8192 / work->bursts[i].duration;
            setVector(&work->bursts[i].scale, value, value, value);
            for (j = 1; j < 8; j++) {
                for (k = 0; k < 8; k++) {
                    work->line.attribute = 0x50000000;
                    work->line.r0 = 0;
                    work->line.g0 = 0;
                    work->line.b0 = 0;
                    work->line.r1 = work->bursts[i].color[0];
                    work->line.g1 = work->bursts[i].color[1];
                    work->line.b1 = work->bursts[i].color[2];
                    RotTransPers(&work->cage[k][j].from, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                    depth = RotTransPers(&work->cage[k][j].to, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->bursts[i].elapsed += step;
            if (work->bursts[i].duration < work->bursts[i].elapsed) {
                work->bursts[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->count[0] + work->count[1] + work->count[2]) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

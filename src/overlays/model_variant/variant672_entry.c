#include "../../types.h"
#include "variant672_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 672's entry: up to 64 Gouraud streaks shoot from one or two model
 * joints toward a ring around the other side, each releasing a spinning,
 * fading sprite with a ribbon fan, while three-sprite bursts grow at the
 * streak origins. The setup also prints the work-area size. */

/* The work-size format string. The padding byte after its terminator is 'x'
 * in the slot-zero images and 0 in the slot-one copies. */
#ifndef MODEL672_STRING_PAD
#define MODEL672_STRING_PAD "x"
#endif
#ifndef VERSION_FRENCH
static const char D_8013CB00[24] = "E_DATA size = %x byte\n\0" MODEL672_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model672State *work;
    MATRIX ls, m0, m1;
    GsCOORDUNIT unit;
    Model672ViewSnapshot view;
    SVECTOR point, mid;
    PSXLONG interpolation, flag, sxy;
    s32 result, step, i, j, inactive;
    s32 value, amount, angle, phase, offset;
    u32 tex;
    PSXLONG depth;

    work = (Model672State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model672State));
#ifndef VERSION_FRENCH
        printf(D_8013CB00, sizeof(Model672State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->target);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->count = 64;
            break;
        }
        work->radius = D_8013CB6C[command].radius;
        work->height = D_8013CB6C[command].height;
        for (i = 0; i < work->count; i++) {
            work->streaks[i].duration = 32;
            work->streaks[i].active = 0;
            work->streaks[i].elapsed = 0;
            work->streaks[i].mode = 1;
            work->streaks[i].coord = Model_GetCameraCoordinateUnit();
            work->streaks[i].color[0] = D_8013CB6C[command].color[0];
            work->streaks[i].color[1] = D_8013CB6C[command].color[1];
            work->streaks[i].color[2] = D_8013CB6C[command].color[2];
            setVector(&work->streaks[i].scale, 4096, 4096, 4096);
            value = work->radius * rsin((i << 12) / work->count) / 4096;
            /* The sign goes in amount; the sprite loop below reuses these
             * temporaries in other roles, matching retail's registers. */
            amount = (i % 2) * 2 - 1;
            angle = (i % 8) << 9;
            setVector(&work->streaks[i].target, work->target.vx + amount * value * rsin(angle) / 4096,
                work->target.vy + value * rcos(angle) / 4096, work->target.vz * 2);
            setVector(&work->streaks[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->streaks[i].position, 0, 0, 0);
            work->streaks[i].triggered = 0;
        }
        for (i = 0; i < work->count; i++) {
            work->sprites[i].active = 0;
            work->sprites[i].elapsed = 0;
            work->sprites[i].duration = 32;
            work->sprites[i].mode = 1;
            work->sprites[i].coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].color[0] = D_8013CB6C[command].color[3];
            work->sprites[i].color[1] = D_8013CB6C[command].color[4];
            work->sprites[i].color[2] = D_8013CB6C[command].color[5];
            work->sprites[i].u = 0;
            work->sprites[i].v = 32;
            setVector(&work->sprites[i].scale, 4096, 4096, 4096);
            setVector(&work->sprites[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sprites[i].target, 0, 0, 0);
            /* angle, amount and phase swap roles here; the sign in phase,
             * computed before the angle, keeps the multiply's slot. */
            angle = i << 12;
            value = work->radius * rsin(angle / work->count) / 4096;
            phase = (i % 2) * 2 - 1;
            amount = (i % 8) << 9;
            offset = phase * value;
            setVector(&work->sprites[i].position, work->target.vx + offset * rsin(amount) / 4096,
                work->target.vy + value * rcos(amount) / 4096,
                work->target.vz + offset * rsin(angle / work->count) / 4096);
        }
        for (i = 0; i < work->count; i++) {
            work->bursts[i].duration = 32;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013CB6C[command].color[0];
            work->bursts[i].color[1] = D_8013CB6C[command].color[1];
            work->bursts[i].color[2] = D_8013CB6C[command].color[2];
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, work->height * 2);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i].position, 0, 0, 0);
        }
        work->delay = D_8013CB6C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->center, 0, 0, 0);
        /* phase also carries the circle angle; it keeps phase in a saved register. */
        for (i = 0; i < 17; i++) {
            phase = i << 8;
            setVector(&work->circle[i], value * rsin(phase) / 4096, value * rcos(phase) / 4096, 0);
        }
        value = work->radius / 2;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        value = work->height;
        setVector(&work->burst_corners[0], -value, -value, 0);
        setVector(&work->burst_corners[1], value, -value, 0);
        setVector(&work->burst_corners[2], -value, value, 0);
        setVector(&work->burst_corners[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        /* (index + array) spellings keep retail's index + base operand order. */
        case 0:
            if (work->elapsed < 100 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 4);
                GsGetLwUnit(&unit, &m0);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
                GsGetLwUnit(&unit, &m1);
                (work->flags[0] + work->streaks)->position.vx = (m0.t[0] + m1.t[0]) / 2;
                (work->flags[0] + work->streaks)->position.vy = (m0.t[1] + m1.t[1]) / 2;
                (work->flags[0] + work->streaks)->position.vz = (m0.t[2] + m1.t[2]) / 2;
                work->streaks[work->flags[0]].active = 1;
                work->bursts[work->flags[0]].position = work->streaks[work->flags[0]].position;
                work->bursts[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->elapsed < 200 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 22);
                GsGetLwUnit(&unit, &m0);
                (work->flags[0] + work->streaks)->position.vx = m0.t[0];
                (work->flags[0] + work->streaks)->position.vy = m0.t[1];
                (work->flags[0] + work->streaks)->position.vz = m0.t[2];
                work->streaks[work->flags[0]].active = 1;
                work->bursts[work->flags[0]].position = work->streaks[work->flags[0]].position;
                work->bursts[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed < 84 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, (u8)(work->flags[0] % 5) * 3 + 9);
                GsGetLwUnit(&unit, &m0);
                (work->flags[0] + work->streaks)->position.vx = m0.t[0];
                (work->flags[0] + work->streaks)->position.vy = m0.t[1];
                (work->flags[0] + work->streaks)->position.vz = m0.t[2];
                work->streaks[work->flags[0]].active = 1;
                work->bursts[work->flags[0]].position = work->streaks[work->flags[0]].position;
                work->bursts[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model672ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->streaks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->streaks[i].scale);
                unit.matrix.t[0] = work->streaks[i].position.vx;
                unit.matrix.t[1] = work->streaks[i].position.vy;
                unit.matrix.t[2] = work->streaks[i].position.vz;
                unit.super = work->streaks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                /* The streaks compare elapsed first, so it loads before duration. */
                if (work->streaks[i].elapsed > work->streaks[i].duration * 3 / 4) {
                    value = (work->streaks[i].duration - work->streaks[i].elapsed) * 4;
                    /* amount, not a fresh variable, holds the divisor in a saved register. */
                    amount = work->streaks[i].duration;
                    work->streaks[i].color[0] = D_8013CB6C[work->command].color[0] * value / amount;
                    work->streaks[i].color[1] = D_8013CB6C[work->command].color[1] * value / amount;
                    work->streaks[i].color[2] = D_8013CB6C[work->command].color[2] * value / amount;
                }
                work->line.attribute = 0x50000000;
                work->line.r0 = 0;
                work->line.g0 = 0;
                work->line.b0 = 0;
                work->line.r1 = work->streaks[i].color[0];
                work->line.g1 = work->streaks[i].color[1];
                work->line.b1 = work->streaks[i].color[2];
                point.vx = (work->streaks[i].target.vx - work->streaks[i].position.vx) * work->streaks[i].elapsed
                    / work->streaks[i].duration;
                point.vy = (work->streaks[i].target.vy - work->streaks[i].position.vy) * work->streaks[i].elapsed
                    / work->streaks[i].duration;
                point.vz = (work->streaks[i].target.vz - work->streaks[i].position.vz) * work->streaks[i].elapsed
                    / work->streaks[i].duration;
                mid.vx = (point.vx + work->center.vx) / 2;
                mid.vy = (point.vy + work->center.vy) / 2;
                mid.vz = (point.vz + work->center.vz) / 2;
                RotTransPers(&work->center, (PSXLONG *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&point, (PSXLONG *)&work->line.x1, &interpolation, &flag);
                depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                }
                work->streaks[i].elapsed += step;
                if (work->streaks[i].triggered == 0) {
                    if (work->streaks[i].duration * 2 / 3 < work->streaks[i].elapsed) {
                        work->sprites[i].active = 1;
                        work->streaks[i].triggered = 1;
                    }
                }
                if (work->streaks[i].elapsed > work->streaks[i].duration) {
                    work->streaks[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->sprites[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = work->sprites[i].elapsed * 0x3000 / work->sprites[i].duration + 0x1000;
                setVector(&work->sprites[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->sprites[i].scale);
                RotMatrixZ(work->sprites[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->sprites[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->sprites[i].position.vx;
                unit.matrix.t[1] = work->sprites[i].position.vy;
                unit.matrix.t[2] = work->sprites[i].position.vz;
                unit.super = work->sprites[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sprites[i].duration / 2 < work->sprites[i].elapsed) {
                    value = (work->sprites[i].duration - work->sprites[i].elapsed) * 2;
                    amount = work->sprites[i].duration;
                    work->sprites[i].color[0] = D_8013CB6C[work->command].color[3] * value / amount;
                    work->sprites[i].color[1] = D_8013CB6C[work->command].color[4] * value / amount;
                    work->sprites[i].color[2] = D_8013CB6C[work->command].color[5] * value / amount;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->sprites[i].u, work->sprites[i].v,
                    work->sprites[i].u + 63, work->sprites[i].v,
                    work->sprites[i].u, work->sprites[i].v + 63,
                    work->sprites[i].u + 63, work->sprites[i].v + 63);
                setRGB0(&work->sprite, work->sprites[i].color[0], work->sprites[i].color[1], work->sprites[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT3(&work->ribbon);
                    work->ribbon.tpage = work->tpage[1];
                    work->ribbon.clut = work->clut[1];
                    setSemiTrans(&work->ribbon, 1);
                    setUV3(&work->ribbon, work->sprites[i].u + 32, work->sprites[i].v + 64,
                        work->sprites[i].u, work->sprites[i].v + 127,
                        work->sprites[i].u + 63, work->sprites[i].v + 127);
                    setRGB0(&work->ribbon, work->sprites[i].color[0], work->sprites[i].color[1], work->sprites[i].color[2]);
                    setRGB1(&work->ribbon, 0, 0, 0);
                    setRGB2(&work->ribbon, 0, 0, 0);
                    depth = RotAverage3(&work->center, &work->circle[j], &work->circle[j + 1],
                        (PSXLONG *)&work->ribbon.x0, (PSXLONG *)&work->ribbon.x1, (PSXLONG *)&work->ribbon.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->ribbon, func_80058F10(), (u16)depth);
                    }
                }
                work->sprites[i].elapsed += step;
                if (work->sprites[i].duration < work->sprites[i].elapsed) {
                    work->sprites[i].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->bursts[i].active == 1) {
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    amount = work->bursts[i].duration;
                    work->bursts[i].color[0] = D_8013CB6C[work->command].color[0] * value / amount;
                    work->bursts[i].color[1] = D_8013CB6C[work->command].color[1] * value / amount;
                    work->bursts[i].color[2] = D_8013CB6C[work->command].color[2] * value / amount;
                }
                for (j = 0; j < 3; j++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                    RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    /* The angle stays inline; a variable would claim a saved register. */
                    unit.matrix.t[0] = work->bursts[i].position.vx
                        + work->bursts[i].target.vz * rsin((j << 12) / 3 + i * 682) / 4096 * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[1] = work->bursts[i].position.vy
                        + work->bursts[i].target.vz * rcos((j << 12) / 3 + i * 682) / 4096 * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[2] = work->bursts[i].position.vz
                        + work->direction * work->bursts[i].target.vz * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v,
                        work->bursts[i].u + 31, work->bursts[i].v,
                        work->bursts[i].u, work->bursts[i].v + 31,
                        work->bursts[i].u + 31, work->bursts[i].v + 31);
                    setRGB0(&work->sprite, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    depth = RotAverage4(&work->burst_corners[0], &work->burst_corners[1], &work->burst_corners[2],
                        &work->burst_corners[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                }
                if (work->bursts[i].u < 224) {
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
        if (inactive == work->count * 3) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

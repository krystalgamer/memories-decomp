#include "../../types.h"
#include "variant645_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 645's entry: a model joint leaves a trail of growing, fading
 * Gouraud tubes, then up to four camera-facing halos open, each a band of
 * sprites with a Gouraud fan and a ring of spinning blades. The setup also
 * prints the work-area size. */

/* Arms the puff under the cursor at a model joint: resets it, reads the
 * joint's world matrix into ls and its position into here, and keeps the
 * matrix for drawing. The scale goes through value so the constant shares
 * value's register. */
#define MODEL645_ARM_PUFF(work, ls, unit, here, value, joint)                                          \
    {                                                                                            \
        (work)->puffs[(work)->flags[0]].active = 1;                                              \
        (work)->puffs[(work)->flags[0]].elapsed = 0;                                             \
        (work)->puffs[(work)->flags[0]].duration = 32;                                           \
        (work)->puffs[(work)->flags[0]].mode = 1;                                                \
        (work)->puffs[(work)->flags[0]].coord = Model_GetCameraCoordinateUnit();                 \
        (work)->puffs[(work)->flags[0]].color[0] = D_8013FF14[(work)->command].color[0];         \
        (work)->puffs[(work)->flags[0]].color[1] = D_8013FF14[(work)->command].color[1];         \
        (work)->puffs[(work)->flags[0]].color[2] = D_8013FF14[(work)->command].color[2];         \
        (work)->puffs[(work)->flags[0]].u = 0;                                                   \
        (work)->puffs[(work)->flags[0]].v = 128;                                                 \
        value = 2048;                                                                            \
        setVector(&(work)->puffs[(work)->flags[0]].scale, value, value, value);                  \
        setVector(&(work)->puffs[(work)->flags[0]].unknown_3C, 0, 0, 0);                         \
        setVector(&(work)->puffs[(work)->flags[0]].rotation, 0, 0, 0);                           \
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry((work)->slot, joint);                      \
        GsGetLwUnit(&unit, &ls);                                                                 \
        setVector(&here, ls.t[0], ls.t[1], ls.t[2]);                                             \
        (work)->joints[(work)->flags[0]] = ls;                                                   \
    }

/* Advances the puff cursor, wrapping at the puff count. The do-while
 * wrappers here and below are matching quirks: their loop notes weigh the
 * work pointer's uses, which keeps it in retail's register. */
#define MODEL645_NEXT_PUFF(work)                                                                 \
    do {                                                                                         \
        (work)->flags[0]++;                                                                      \
        if ((work)->flags[0] >= (work)->puff_count) {                                            \
            (work)->flags[0] = 0;                                                                \
        }                                                                                        \
    } while (0)

/* Places the armed puff, then advances the cursor. */
#define MODEL645_PLACE_PUFF(work, x, y, z)                                                       \
    do {                                                                                         \
        setVector(&(work)->puffs[(work)->flags[0]].position, x, y, z);                           \
        (work)->flags[0]++;                                                                      \
        if ((work)->flags[0] >= (work)->puff_count) {                                            \
            (work)->flags[0] = 0;                                                                \
        }                                                                                        \
    } while (0)

s32 func_8013B004(u8 *context, s32 command)
{
    Model645State *work;
    MATRIX ls, rot;
    GsCOORDUNIT unit;
    Model645ViewSnapshot view;
    SVECTOR here, there;
    Model645Corner corners[4];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, n;
    s32 value, divisor, offset, next_offset;
    s32 red0, green0, blue0, red1, green1, blue1;
    /* The halo colour; each tube segment edge has its own triple. */
    s32 red, green, blue;
    u32 tex;
    PSXLONG depth;

    work = (Model645State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model645State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model645State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FEDC[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FEDC[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        /* Retail keeps three identical case bodies for the one-halo commands. */
        switch (command) {
        case 0:
            work->puff_count = 64;
            work->halo_count = 1;
            break;
        case 1:
            work->puff_count = 64;
            work->halo_count = 1;
            break;
        case 8:
        case 9:
        case 12:
            work->puff_count = 64;
            work->halo_count = 1;
            break;
        case 3:
        case 4:
        case 6:
        case 7:
        case 10:
        case 11:
            work->puff_count = 0;
            work->halo_count = 4;
            break;
        case 2:
        case 5:
        case 13:
        case 14:
        case 15:
            work->puff_count = 64;
            work->halo_count = 4;
            break;
        }
        work->radius = D_8013FF14[command].radius;
        work->height = D_8013FF14[command].height;
        for (i = 0; i < work->puff_count; i++) {
            work->puffs[i].duration = 32;
            work->puffs[i].active = 0;
            work->puffs[i].elapsed = 0;
            work->puffs[i].mode = 1;
            work->puffs[i].coord = Model_GetCameraCoordinateUnit();
            work->puffs[i].color[0] = D_8013FF14[command].color[0];
            work->puffs[i].color[1] = D_8013FF14[command].color[1];
            work->puffs[i].color[2] = D_8013FF14[command].color[2];
            work->puffs[i].u = 0;
            work->puffs[i].v = 128;
            /* The scale goes through value so the loop hoists the constant into
             * value's register. */
            value = 2048;
            setVector(&work->puffs[i].scale, value, value, value);
            setVector(&work->puffs[i].unknown_3C, 0, 0, 0);
            setVector(&work->puffs[i].rotation, 0, 0, 0);
            setVector(&work->puffs[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->halo_count; i++) {
            work->halos[i].duration = 48;
            work->halos[i].active = 0;
            work->halos[i].elapsed = 0;
            work->halos[i].mode = 1;
            work->halos[i].coord = Model_GetCameraCoordinateUnit();
            work->halos[i].color[0] = D_8013FF14[command].color[3];
            work->halos[i].color[1] = D_8013FF14[command].color[4];
            work->halos[i].color[2] = D_8013FF14[command].color[5];
            work->halos[i].u = 0;
            work->halos[i].v = 128;
            value = 2048;
            setVector(&work->halos[i].scale, value, value, value);
            setVector(&work->halos[i].unknown_3C, 0, 0, 0);
            setVector(&work->halos[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 3:
            case 4:
            case 6:
            case 7:
            case 10:
            case 11:
                setVector(&work->halos[i].position,
                    work->direction * work->radius * rsin((i << 12) / work->halo_count) / 4096,
                    work->radius * rcos((i << 12) / work->halo_count) / 4096,
                    work->direction * work->radius * rsin((i << 12) / work->halo_count) / 4096);
                break;
            default:
                setVector(&work->halos[i].position, 0, 0, 0);
                break;
            }
            work->halos[i].position.vy -= 350;
            work->halos[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013FF14[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        switch (command) {
        case 0:
        case 8:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], 0, work->radius * rcos(i << 7) / 4096,
                    work->radius * rsin(i << 7) / 4096);
                setVector(&work->tube[i][1], 0, work->radius * rcos(i << 7) / 4096,
                    work->height + work->radius * rsin(i << 7) / 4096);
            }
            break;
        case 1:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], work->radius * rcos(i << 7) / 4096,
                    work->radius * rsin(i << 7) / 4096, 0);
                setVector(&work->tube[i][1], work->radius * rcos(i << 7) / 4096,
                    work->height + work->radius * rsin(i << 7) / 4096, 0);
            }
            break;
        case 2:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], work->radius * -rcos(i << 7) / 4096,
                    work->radius * -rsin(i << 7) / 4096, 0);
                setVector(&work->tube[i][1], work->radius * -rcos(i << 7) / 4096,
                    work->height + work->radius * -rsin(i << 7) / 4096, 0);
            }
            break;
        case 5:
        case 13:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], work->radius * -rcos(i << 7) / 4096, 0,
                    work->radius * -rsin(i << 7) / 4096);
                setVector(&work->tube[i][1], work->radius * -rcos(i << 7) / 4096, 0,
                    work->height + work->radius * -rsin(i << 7) / 4096);
            }
            break;
        case 9:
        case 15:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], work->radius * rcos(i << 7) / 4096, 0,
                    work->radius * rsin(i << 7) / 4096);
                setVector(&work->tube[i][1], work->radius * rcos(i << 7) / 4096, 0,
                    work->height + work->radius * rsin(i << 7) / 4096);
            }
            break;
        case 12:
        case 14:
            for (i = 0; i < 17; i++) {
                setVector(&work->tube[i][0], 0, work->radius * -rcos(i << 7) / 4096,
                    work->radius * -rsin(i << 7) / 4096);
                setVector(&work->tube[i][1], 0, work->radius * -rcos(i << 7) / 4096,
                    work->height + work->radius * -rsin(i << 7) / 4096);
            }
            break;
        }
        for (i = 0; i < 33; i++) {
            setVector(&work->band[i][0], work->radius * rcos(i << 7) / 4096,
                work->radius * rsin(i << 7) / 4096, 0);
            /* Written as a sum: retail multiplies the radius by i % 2 and adds it doubled. */
            setVector(&work->band[i][1], (work->radius * 2 + work->radius * (i % 2)) * rcos(i << 7) / 4096,
                (work->radius * 2 + work->radius * (i % 2)) * rsin(i << 7) / 4096, 0);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->blades[i][0], (work->radius >> 1) * rsin(i << 8) / 4096,
                (work->radius >> 1) * rcos(i << 8) / 4096, 0);
            setVector(&work->blades[i][1], (work->radius * 3 >> 1) * rsin(i << 8) / 4096,
                (work->radius * 3 >> 1) * rcos(i << 8) / 4096, 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed < 100) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 10);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 11);
                GsGetLwUnit(&unit, &ls);
                setVector(&there, ls.t[0], ls.t[1], ls.t[2]);
                setVector(&work->puffs[work->flags[0]].position, (here.vx + there.vx) / 2,
                    (here.vy + there.vy) / 2, (here.vz + there.vz) / 2);
                MODEL645_NEXT_PUFF(work);
            }
            /* The flag stores come before the result so they reuse its register. */
            if (work->elapsed >= 45 && work->flags[1] == 0) {
                work->halos[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 1:
            if (work->elapsed < 100) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 4);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            if (work->elapsed >= 61 && work->flags[1] == 0) {
                work->halos[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 2:
            if (work->elapsed < 142) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 5);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed >= 21) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 56) {
                    work->halos[1].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 91) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 126) {
                    work->halos[3].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        case 3:
        case 4:
        case 6:
        case 7:
        case 10:
            if (work->elapsed == 0 && work->flags[1] == 0) {
                for (i = 0; i < work->halo_count; i++) {
                    work->halos[i].active = 1;
                }
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 5:
            if (work->elapsed < 130) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 15);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed >= 51) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 61) {
                    work->halos[1].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 71) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 81) {
                    work->halos[3].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        case 8:
            if (work->elapsed < 90) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 0);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            if (work->elapsed >= 19 && work->flags[1] == 0) {
                work->halos[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 9:
            if (work->elapsed < 130) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 3);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            if (work->elapsed >= 51 && work->flags[1] == 0) {
                work->halos[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 11:
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed == 0) {
                    work->halos[3].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 11) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 21) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 31) {
                    work->halos[1].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        case 12:
            if (work->elapsed < 80) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 3);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            if (work->elapsed >= 41 && work->flags[1] == 0) {
                work->halos[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
            break;
        case 13:
            if (work->elapsed < 160) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 3);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed >= 101) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 121) {
                    work->halos[1].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 141) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 161) {
                    work->halos[3].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        case 14:
            if (work->elapsed < 110) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 1);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed >= 31) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 51) {
                    work->halos[1].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 71) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 91) {
                    work->halos[3].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        case 15:
            if (work->elapsed < 82) {
                MODEL645_ARM_PUFF(work, ls, unit, here, value, 3);
                MODEL645_PLACE_PUFF(work, here.vx, here.vy, here.vz);
            }
            switch (work->flags[1]) {
            case 0:
                if (work->elapsed >= 15) {
                    work->halos[0].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 1:
                if (work->elapsed >= 33) {
                    work->halos[1].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 2:
                if (work->elapsed >= 55) {
                    work->halos[2].active = 1;
                    result = 4;
                    work->flags[1]++;
                }
                break;
            case 3:
                if (work->elapsed >= 77) {
                    work->halos[3].active = 1;
                    result = 1;
                    work->flags[1]++;
                }
                break;
            }
            break;
        }
        PushMatrix();
        view = *(Model645ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->puff_count; i++) {
            if (work->puffs[i].active == 1) {
                unit.flg = 0;
                unit.matrix = work->joints[i];
                value = (work->puffs[i].elapsed << 13) / work->puffs[i].duration + 2048;
                setVector(&work->puffs[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->puffs[i].scale);
                unit.matrix.t[0] = work->puffs[i].position.vx;
                unit.matrix.t[1] = work->puffs[i].position.vy;
                unit.matrix.t[2] = work->puffs[i].position.vz;
                unit.super = work->puffs[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->puffs[i].duration / 2 < work->puffs[i].elapsed) {
                    value = (work->puffs[i].duration - work->puffs[i].elapsed) * 2;
                    divisor = work->puffs[i].duration;
                    work->puffs[i].color[0] = D_8013FF14[work->command].color[0] * value / divisor;
                    work->puffs[i].color[1] = D_8013FF14[work->command].color[1] * value / divisor;
                    work->puffs[i].color[2] = D_8013FF14[work->command].color[2] * value / divisor;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    /* Setting the semi-transparency after the texture fields keeps
                     * retail's reload and OR of the code byte. */
                    setSemiTrans(&work->strip, 1);
                    /* Each edge's brightness peaks at the middle segment. ABS on a
                     * temporary keeps the outer subtraction out of the condition;
                     * the divisor is set afterwards so "8 - k" keeps its own
                     * constant and the loop hoists the divisor first. */
                    offset = 8 - k;
                    value = 8 - ABS(offset);
                    divisor = 8;
                    setUV4(&work->strip, work->puffs[i].u, work->puffs[i].v,
                        work->puffs[i].u + 31, work->puffs[i].v,
                        work->puffs[i].u, work->puffs[i].v + 127,
                        work->puffs[i].u + 31, work->puffs[i].v + 127);
                    /* The colours go through temporaries: retail loads all three
                     * before the first store. */
                    red0 = work->puffs[i].color[0] * value / divisor;
                    green0 = work->puffs[i].color[1] * value / divisor;
                    blue0 = work->puffs[i].color[2] * value / divisor;
                    next_offset = 7 - k;
                    value = 8 - ABS(next_offset);
                    setRGB0(&work->strip, red0, green0, blue0);
                    setRGB2(&work->strip, red0, green0, blue0);
                    red1 = work->puffs[i].color[0] * value / divisor;
                    green1 = work->puffs[i].color[1] * value / divisor;
                    blue1 = work->puffs[i].color[2] * value / divisor;
                    setRGB1(&work->strip, red1, green1, blue1);
                    setRGB3(&work->strip, red1, green1, blue1);
                    depth = RotAverage4(&work->tube[k][0], &work->tube[k + 1][0],
                        &work->tube[k][1], &work->tube[k + 1][1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->puffs[i].elapsed += step;
                if (work->puffs[i].duration < work->puffs[i].elapsed) {
                    work->puffs[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->halo_count; i++) {
            if (work->halos[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->halos[i].elapsed << 13) / work->halos[i].duration + 2048;
                setVector(&work->halos[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->halos[i].scale);
                RotMatrixZ(work->halos[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->halos[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->halos[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->halos[i].position.vx;
                unit.matrix.t[1] = work->halos[i].position.vy;
                unit.matrix.t[2] = work->halos[i].position.vz;
                unit.super = work->halos[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->halos[i].duration / 2 < work->halos[i].elapsed) {
                    value = (work->halos[i].duration - work->halos[i].elapsed) * 2;
                    divisor = work->halos[i].duration;
                    work->halos[i].color[0] = D_8013FF14[work->command].color[3] * value / divisor;
                    work->halos[i].color[1] = D_8013FF14[work->command].color[4] * value / divisor;
                    work->halos[i].color[2] = D_8013FF14[work->command].color[5] * value / divisor;
                }
                setVector(&here, 0, 0, 0);
                for (k = 0; k < 32; k++) {
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->halos[i].u, work->halos[i].v,
                        work->halos[i].u + 31, work->halos[i].v,
                        work->halos[i].u, work->halos[i].v + 127,
                        work->halos[i].u + 31, work->halos[i].v + 127);
                    red = work->halos[i].color[0];
                    green = work->halos[i].color[1];
                    blue = work->halos[i].color[2];
                    setRGB0(&work->sprite, red, green, blue);
                    depth = RotAverage4(&work->band[k][0], &work->band[k + 1][0],
                        &work->band[k][1], &work->band[k + 1][1],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    setPolyG3(&work->fan);
                    red = work->halos[i].color[0];
                    green = work->halos[i].color[1];
                    blue = work->halos[i].color[2];
                    setRGB0(&work->fan, red, green, blue);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&here, &work->band[k][0], &work->band[k + 1][0],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->fan, func_80058F10(), (u16)depth,
                            work->halos[i].mode);
                    }
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    if (k % 2 == 0) {
                        setUV4(&work->strip, work->halos[i].u, work->halos[i].v + 128,
                            work->halos[i].u + 63, work->halos[i].v + 128,
                            work->halos[i].u, work->halos[i].v + 255,
                            work->halos[i].u + 63, work->halos[i].v + 255);
                    } else {
                        setUV4(&work->strip, work->halos[i].u + 63, work->halos[i].v + 128,
                            work->halos[i].u, work->halos[i].v + 128,
                            work->halos[i].u + 63, work->halos[i].v + 255,
                            work->halos[i].u, work->halos[i].v + 255);
                    }
                    red = work->halos[i].color[0];
                    green = work->halos[i].color[1];
                    blue = work->halos[i].color[2];
                    setRGB0(&work->strip, red, green, blue);
                    setRGB1(&work->strip, red, green, blue);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    PushMatrix();
                    rot = D_800FE168;
                    /* "* 55 * sign" folds into retail's elapsed * (5k + 1) * (sign * 55). */
                    RotMatrixZ(work->halos[i].elapsed * (k * 5 + 1) * 55 * (k % 2 * 2 - 1), &rot);
                    corners[0].in = work->blades[k][0];
                    corners[1].in = work->blades[k + 1][0];
                    corners[2].in = work->blades[k][1];
                    corners[3].in = work->blades[k + 1][1];
                    for (n = 0; n < 4; n++) {
                        ApplyMatrixSV(&rot, &corners[n].in, &corners[n].out);
                    }
                    PopMatrix();
                    depth = RotAverage4(&corners[0].out, &corners[1].out, &corners[2].out, &corners[3].out,
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->halos[i].elapsed += step;
                if (work->halos[i].duration < work->halos[i].elapsed) {
                    work->halos[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->puff_count + work->halo_count) {
            result = 2;
        }
    }
    return result;
}

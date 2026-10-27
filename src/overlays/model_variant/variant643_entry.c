#include "../../types.h"
#include "variant643_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 643's entry: up to four growing textured cone bands and a
 * Gouraud halo ring placed at a model joint, then 16 columns of eight
 * sparks that fly outward and respawn while fading. The setup also prints
 * the work-area size. */

/* Places every band and halo at the joint's world position. */
#define MODEL643_PLACE_ALL(joint)                                                    \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, joint);                \
    GsGetLwUnit(&unit, &ls);                                                         \
    for (i = 0; i < work->count[0]; i++) {                                           \
        work->bands[i].active = 1;                                                   \
        setVector(&work->bands[i].position, ls.t[0], ls.t[1], ls.t[2]);              \
    }                                                                                \
    for (i = 0; i < work->count[1]; i++) {                                           \
        work->halos[i].active = 1;                                                   \
        setVector(&work->halos[i].position, ls.t[0], ls.t[1], ls.t[2]);              \
    }

/* Starts the first spark of every column once. */
#define MODEL643_START_SPARKS()                                                      \
    for (i = 0; i < work->count[3]; i++) {                                           \
        work->sparks[0][i].active = 1;                                               \
    }                                                                                \
    work->flags[0] = 1;

s32 func_8013B004(u8 *context, s32 command)
{
    Model643State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model643ViewSnapshot view;
    SVECTOR mid[2];
    PSXLONG interpolation, flag;
    s32 result, i, j, step, inactive;
    s32 value, duration;
    u32 tex;
    PSXLONG depth;
    u8 r, g, b;

    work = (Model643State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model643State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model643State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D2B0[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D2B0[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        if (command < 4) {
            if (command >= 0) {
                work->count[0] = 4;
                work->count[1] = 1;
                work->count[2] = 8;
                work->count[3] = 16;
            }
        }
        work->radius = D_8013D2E8[command].radius;
        work->height = D_8013D2E8[command].height;
        for (i = 0; i < work->count[0]; i++) {
            work->bands[i].duration = 48;
            work->bands[i].active = 0;
            work->bands[i].elapsed = 0;
            work->bands[i].mode = 1;
            work->bands[i].coord = Model_GetCameraCoordinateUnit();
            work->bands[i].color[0] = D_8013D2E8[command].color[0];
            work->bands[i].color[1] = D_8013D2E8[command].color[1];
            work->bands[i].color[2] = D_8013D2E8[command].color[2];
            work->bands[i].u = 0;
            work->bands[i].v = 0;
            setVector(&work->bands[i].scale, 0, 0, 0);
            setVector(&work->bands[i].velocity, 0, 0, 0);
            /* Two whole setVector calls (instead of a conditional X) let the
             * compiler merge their common tail, as retail does. */
            if (command == 2) {
                setVector(&work->bands[i].rotation, -512, work->slot * 2048, 0);
            } else {
                setVector(&work->bands[i].rotation, 0, work->slot * 2048, 0);
            }
            setVector(&work->bands[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count[1]; i++) {
            work->halos[i].duration = 48;
            work->halos[i].active = 0;
            work->halos[i].elapsed = 0;
            work->halos[i].mode = 1;
            work->halos[i].coord = Model_GetCameraCoordinateUnit();
            work->halos[i].color[0] = D_8013D2E8[command].color[0];
            work->halos[i].color[1] = D_8013D2E8[command].color[1];
            work->halos[i].color[2] = D_8013D2E8[command].color[2];
            setVector(&work->halos[i].scale, 0, 0, 0);
            setVector(&work->halos[i].velocity, 0, 0, 0);
            if (command == 2) {
                setVector(&work->halos[i].rotation, -512, work->slot * 2048, 0);
            } else {
                setVector(&work->halos[i].rotation, 0, work->slot * 2048, 0);
            }
            setVector(&work->halos[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count[2]; i++) {
            for (j = 0; j < work->count[3]; j++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = 32;
                work->sparks[i][j].mode = 1;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = D_8013D2E8[command].color[3];
                work->sparks[i][j].color[1] = D_8013D2E8[command].color[4];
                work->sparks[i][j].color[2] = D_8013D2E8[command].color[5];
                work->sparks[i][j].u = 0;
                work->sparks[i][j].v = 128;
                work->sparks[i][j].next = 1;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].velocity,
                    -(rsin(j % 4 << 10) * 500 / 4096 * (j % 2 * 2 - 1) * work->direction),
                    -(rcos((j % 5 << 12) / 5) * 500 / 4096 * (j % 2 * 2 - 1)),
                    -(rcos((j % 6 << 12) / 6) * 500 / 4096 * work->direction));
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i][j].position, 0, 0, 0);
                work->sparks[i][j].position.vy = -350;
                work->sparks[i][j].position.vz = work->direction * 450;
            }
        }
        work->delay = D_8013D2E8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->cone[i][0], work->radius * rsin(i << 8) / 4096,
                work->radius * rcos(i << 8) / 4096, 0);
            /* The shifts and the negated -5 below keep radius * (sin * k) from
             * being reassociated into (radius * k) * sin. */
            setVector(&work->cone[i][1], work->radius * (rsin(i << 8) << 2) / 4096,
                work->radius * (rcos(i << 8) << 2) / 4096, -work->height);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i][0], work->radius * (rsin(i << 8) << 1) / 4096,
                work->radius * (rcos(i << 8) << 1) / 4096, 0);
            setVector(&work->ring[i][1], work->radius * -(rsin(i << 8) * -5) / 4096,
                work->radius * -(rcos(i << 8) * -5) / 4096, 0);
        }
        setVector(&work->corners[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->corners[3], work->radius / 2, work->radius / 2, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                MODEL643_PLACE_ALL(5);
            }
            if (work->elapsed > 40 && work->flags[0] == 0) {
                MODEL643_START_SPARKS();
            }
            break;
        case 1:
            if (work->elapsed == 0) {
                MODEL643_PLACE_ALL(9);
            }
            if (work->elapsed >= 8 && work->flags[0] == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 9);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->count[3]; i++) {
                    work->sparks[0][i].active = 1;
                    setVector(&work->sparks[0][i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                work->flags[0] = 1;
            }
            break;
        case 2:
            if (work->elapsed == 0) {
                MODEL643_PLACE_ALL(15);
            }
            if (work->elapsed > 40 && work->flags[0] == 0) {
                MODEL643_START_SPARKS();
            }
            break;
        case 3:
            if (work->elapsed == 0) {
                MODEL643_PLACE_ALL(6);
            }
            if (work->elapsed > 10 && work->flags[0] == 0) {
                MODEL643_START_SPARKS();
            }
            break;
        }
        PushMatrix();
        view = *(Model643ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count[0]; i++) {
            if (work->bands[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->bands[i].scale);
                RotMatrixX(work->bands[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->bands[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->bands[i].position.vx;
                unit.matrix.t[1] = work->bands[i].position.vy;
                unit.matrix.t[2] = work->bands[i].position.vz;
                unit.super = work->bands[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                value = (4096 - i * 1000) * work->bands[i].elapsed / work->bands[i].duration;
                setVector(&work->bands[i].scale, value, value, value);
                /* The band count goes through "duration" so it is read once. */
                value = i + 1;
                duration = work->count[0];
                work->bands[i].color[0] = D_8013D2E8[work->command].color[0] * value / duration;
                work->bands[i].color[1] = D_8013D2E8[work->command].color[1] * value / duration;
                work->bands[i].color[2] = D_8013D2E8[work->command].color[2] * value / duration;
                if (work->bands[i].duration / 2 < work->bands[i].elapsed) {
                    value = (work->bands[i].duration - work->bands[i].elapsed) * 2;
                    duration = work->bands[i].duration;
                    work->bands[i].color[0] = work->bands[i].color[0] * value / duration;
                    work->bands[i].color[1] = work->bands[i].color[1] * value / duration;
                    work->bands[i].color[2] = work->bands[i].color[2] * value / duration;
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->band);
                    work->band.tpage = work->tpage[0];
                    work->band.clut = work->clut[0];
                    /* Setting the semi-transparency after the texture fields keeps
                     * retail's reload and OR of the code byte. */
                    setSemiTrans(&work->band, 1);
                    setUV4(&work->band, work->bands[i].u, work->bands[i].v,
                        work->bands[i].u + 31, work->bands[i].v,
                        work->bands[i].u, work->bands[i].v + 127,
                        work->bands[i].u + 31, work->bands[i].v + 127);
                    /* Retail reads all three channels before the colour stores. */
                    r = work->bands[i].color[0];
                    g = work->bands[i].color[1];
                    b = work->bands[i].color[2];
                    setRGB0(&work->band, 0, 0, 0);
                    setRGB1(&work->band, 0, 0, 0);
                    setRGB2(&work->band, r, g, b);
                    setRGB3(&work->band, r, g, b);
                    depth = RotAverage4(&work->cone[j][0], &work->cone[j + 1][0],
                        &work->cone[j][1], &work->cone[j + 1][1],
                        (PSXLONG *)&work->band.x0, (PSXLONG *)&work->band.x1,
                        (PSXLONG *)&work->band.x2, (PSXLONG *)&work->band.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->band, func_80058F10(), (u16)depth);
                    }
                }
                work->bands[i].elapsed += step;
                if (work->bands[i].duration < work->bands[i].elapsed) {
                    work->bands[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count[1]; i++) {
            if (work->halos[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->halos[i].scale);
                RotMatrixX(work->halos[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->halos[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->halos[i].position.vx;
                unit.matrix.t[1] = work->halos[i].position.vy;
                unit.matrix.t[2] = work->halos[i].position.vz;
                unit.super = work->halos[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                value = (4096 - i * 1000) * work->halos[i].elapsed / work->halos[i].duration;
                setVector(&work->halos[i].scale, value, value, value);
                if (work->halos[i].duration / 2 < work->halos[i].elapsed) {
                    value = (work->halos[i].duration - work->halos[i].elapsed) * 2;
                    duration = work->halos[i].duration;
                    work->halos[i].color[0] = D_8013D2E8[work->command].color[0] * value / duration;
                    work->halos[i].color[1] = D_8013D2E8[work->command].color[1] * value / duration;
                    work->halos[i].color[2] = D_8013D2E8[work->command].color[2] * value / duration;
                }
                for (j = 0; j < 16; j++) {
                    setVector(&mid[0], (work->ring[j][0].vx + work->ring[j][1].vx) / 2,
                        (work->ring[j][0].vy + work->ring[j][1].vy) / 2,
                        (work->ring[j][0].vz + work->ring[j][1].vz) / 2);
                    setVector(&mid[1], (work->ring[j + 1][0].vx + work->ring[j + 1][1].vx) / 2,
                        (work->ring[j + 1][0].vy + work->ring[j + 1][1].vy) / 2,
                        (work->ring[j + 1][0].vz + work->ring[j + 1][1].vz) / 2);
                    setPolyG4(&work->halo);
                    r = work->halos[i].color[0];
                    g = work->halos[i].color[1];
                    b = work->halos[i].color[2];
                    setRGB0(&work->halo, 0, 0, 0);
                    setRGB1(&work->halo, 0, 0, 0);
                    setRGB2(&work->halo, r, g, b);
                    setRGB3(&work->halo, r, g, b);
                    depth = RotAverage4(&work->ring[j][0], &work->ring[j + 1][0], &mid[0], &mid[1],
                        (PSXLONG *)&work->halo.x0, (PSXLONG *)&work->halo.x1,
                        (PSXLONG *)&work->halo.x2, (PSXLONG *)&work->halo.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->halo, func_80058F10(), (u16)depth,
                            work->halos[i].mode);
                    }
                    setPolyG4(&work->halo);
                    r = work->halos[i].color[0];
                    g = work->halos[i].color[1];
                    b = work->halos[i].color[2];
                    setRGB2(&work->halo, 0, 0, 0);
                    setRGB3(&work->halo, 0, 0, 0);
                    setRGB0(&work->halo, r, g, b);
                    setRGB1(&work->halo, r, g, b);
                    depth = RotAverage4(&mid[0], &mid[1], &work->ring[j][1], &work->ring[j + 1][1],
                        (PSXLONG *)&work->halo.x0, (PSXLONG *)&work->halo.x1,
                        (PSXLONG *)&work->halo.x2, (PSXLONG *)&work->halo.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->halo, func_80058F10(), (u16)depth,
                            work->halos[i].mode);
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
        for (j = 0; j < work->count[3]; j++) {
            for (i = 0; i < work->count[2]; i++) {
                if (work->sparks[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                    RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                    RotMatrixY(work->sparks[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx
                        + work->sparks[i][j].velocity.vx * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy
                        + work->sparks[i][j].velocity.vy * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz
                        + work->sparks[i][j].velocity.vz * work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->sparks[i][j].duration / 2 < work->sparks[i][j].elapsed) {
                        value = (work->sparks[i][j].duration - work->sparks[i][j].elapsed) * 2;
                        duration = work->sparks[i][j].duration;
                        work->sparks[i][j].color[0] = D_8013D2E8[work->command].color[3] * value / duration;
                        work->sparks[i][j].color[1] = D_8013D2E8[work->command].color[4] * value / duration;
                        work->sparks[i][j].color[2] = D_8013D2E8[work->command].color[5] * value / duration;
                    }
                    /* Retail keeps this 1 in the shared temporary for the respawn. */
                    value = 1;
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[1];
                    work->sprite.clut = work->clut[1];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->sparks[i][j].u, work->sparks[i][j].v,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v,
                        work->sparks[i][j].u, work->sparks[i][j].v + 31,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v + 31);
                    r = work->sparks[i][j].color[0];
                    g = work->sparks[i][j].color[1];
                    b = work->sparks[i][j].color[2];
                    setRGB0(&work->sprite, r, g, b);
                    depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    work->sparks[i][j].u += 32;
                    if (work->sparks[i][j].u == 224) {
                        work->sparks[i][j].u = 0;
                    }
                    if (i == 0) {
                        work->sparks[work->sparks[0][j].next][j].active = value;
                        work->sparks[work->sparks[0][j].next][j].elapsed = 0;
                        work->sparks[work->sparks[0][j].next][j].duration = 32;
                        work->sparks[work->sparks[0][j].next][j].mode = value;
                        work->sparks[work->sparks[0][j].next][j].coord = Model_GetCameraCoordinateUnit();
                        work->sparks[work->sparks[0][j].next][j].color[0] = D_8013D2E8[work->command].color[3];
                        work->sparks[work->sparks[0][j].next][j].color[1] = D_8013D2E8[work->command].color[4];
                        work->sparks[work->sparks[0][j].next][j].color[2] = D_8013D2E8[work->command].color[5];
                        work->sparks[work->sparks[0][j].next][j].u = 0;
                        work->sparks[work->sparks[0][j].next][j].v = 128;
                        setVector(&work->sparks[work->sparks[0][j].next][j].scale, 4096, 4096, 4096);
                        setVector(&work->sparks[work->sparks[0][j].next][j].velocity, 0, 0, 0);
                        setVector(&work->sparks[work->sparks[0][j].next][j].rotation, 0, work->slot * 2048, 0);
                        setVector(&work->sparks[work->sparks[0][j].next][j].position,
                            unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                        work->sparks[0][j].next++;
                        if (work->sparks[0][j].next > work->count[2] - 1) {
                            work->sparks[0][j].next = value;
                        }
                    }
                    work->sparks[i][j].elapsed += step;
                    if (work->sparks[i][j].duration < work->sparks[i][j].elapsed) {
                        work->sparks[i][j].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->count[0] + work->count[1] + work->count[2] * work->count[3]) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

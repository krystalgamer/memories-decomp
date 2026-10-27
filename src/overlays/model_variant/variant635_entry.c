#include "../../types.h"
#include "variant635_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 635's entry: up to two copies of three staggered, spinning
 * Gouraud domes rising from a model joint, each lobbing a pulsing sprite
 * comet along a parabola across the field. Where a comet lands, two
 * spinning cones of strips grow up, and a fan of Gouraud rays flashes at
 * the joint. The setup also prints the work-area size. */

/* The work-size format string; retail's padding byte after the terminator is 0x24. */
#ifndef VERSION_FRENCH
static const char D_8013EBD0[24] = "E_DATA size = %x byte\n\0$";
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model635State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model635ViewSnapshot view;
    SVECTOR vec;
    PSXLONG interpolation, flag, sxy;
    s32 result, i, j, k, l, step, inactive;
    s32 rise, duration, elapsed, start, ramp;
    u32 tex;
    PSXLONG depth;

    work = (Model635State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model635State));
#ifndef VERSION_FRENCH
        printf(D_8013EBD0, sizeof(Model635State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EBE8[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EBE8[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->copies = 2;
            work->dome_count = 3;
            work->burst_count = 2;
            break;
        case 1:
        case 2:
            work->copies = 1;
            work->dome_count = 3;
            work->burst_count = 2;
            break;
        }
        work->radius = D_8013EC20[command].radius;
        work->height = D_8013EC20[command].height;
        for (i = 0; i < work->copies; i++) {
            work->domes[0][i].active = 0;
            work->domes[0][i].elapsed = 0;
            work->domes[0][i].duration = 32;
            work->domes[0][i].mode = 1;
            work->domes[0][i].coord = Model_GetCameraCoordinateUnit();
            work->domes[0][i].color[0] = D_8013EC20[command].color[0];
            work->domes[0][i].color[1] = D_8013EC20[command].color[1];
            work->domes[0][i].color[2] = D_8013EC20[command].color[2];
            work->domes[0][i].u = 0;
            work->domes[0][i].v = 64;
            work->domes[0][i].stage = 0;
            work->domes[0][i].launched = 0;
            setVector(&work->domes[0][i].scale, 0, 0, 0);
            PushMatrix();
            switch (command) {
            case 0:
            case 1:
                /* Each case still indexes by i: CSE substitutes the known index,
                 * leaving retail's register-based addressing. */
                switch (i) {
                case 0:
                    setVector(&vec, (work->slot * 2 - 1) * 4096 * 20 / 360, -4096 * 5 / 360, 0);
                    RotMatrix(&vec, &m);
                    setVector(&vec, 0, 0, (work->slot * 2 - 1) * 5);
                    ApplyMatrix(&m, &vec, &work->domes[0][i].velocity);
                    setVector(&work->domes[0][i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 5 / 360, 0);
                    setVector(&work->domes[0][i].position, 0, 0, 0);
                    break;
                case 1:
                    setVector(&vec, (work->slot * 2 - 1) * 4096 * 20 / 360, -4096 * 25 / 360, 0);
                    RotMatrix(&vec, &m);
                    setVector(&vec, 0, 0, (work->slot * 2 - 1) * 5);
                    ApplyMatrix(&m, &vec, &work->domes[0][i].velocity);
                    setVector(&work->domes[0][i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 25 / 360, 0);
                    setVector(&work->domes[0][i].position, 0, 0, 0);
                    break;
                case 2:
                    setVector(&vec, (work->slot * 2 - 1) * 4096 * 20 / 360, -4096 * 20 / 360, 0);
                    RotMatrix(&vec, &m);
                    setVector(&vec, 0, 0, (work->slot * 2 - 1) * 5);
                    ApplyMatrix(&m, &vec, &work->domes[0][i].velocity);
                    setVector(&work->domes[0][i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 20 / 360, 0);
                    setVector(&work->domes[0][i].position, 0, 0, 0);
                    break;
                }
                break;
            case 2:
                setVector(&vec, work->slot * 1024 - 512, 0, 0);
                RotMatrix(&vec, &m);
                setVector(&vec, 0, 0, (work->slot * 2 - 1) * 5);
                ApplyMatrix(&m, &vec, &work->domes[0][i].velocity);
                setVector(&work->domes[0][i].rotation, -512, work->slot * 2048, 0);
                setVector(&work->domes[0][i].position, 0, 0, 0);
                break;
            }
            PopMatrix();
            work->domes[0][i].position.vy -= 350;
            work->domes[0][i].position.vz -= (work->slot * 2 - 1) * 450;
            for (k = 1; k < work->dome_count; k++) {
                work->domes[k][i] = work->domes[0][i];
                work->domes[k][i].rotation.vz = k << 8;
            }
        }
        for (i = 0; i < work->copies; i++) {
            work->comets[i].active = 0;
            work->comets[i].elapsed = 0;
            work->comets[i].duration = 64;
            work->comets[i].mode = 1;
            work->comets[i].coord = Model_GetCameraCoordinateUnit();
            work->comets[i].color[0] = 0;
            work->comets[i].color[1] = 0;
            work->comets[i].color[2] = 0;
            work->comets[i].u = 0;
            work->comets[i].v = 0;
            setVector(&work->comets[i].scale, 2048, 2048, 2048);
            setVector(&work->comets[i].velocity, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
                setVector(&work->comets[i].rotation, -1024, work->slot * 2048, 0);
                setVector(&work->comets[i].position, 0, 0, 0);
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i][0].active = 0;
            work->bursts[i][0].elapsed = 0;
            work->bursts[i][0].duration = 128;
            work->bursts[i][0].mode = 1;
            work->bursts[i][0].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i][0].color[0] = D_8013EC20[command].color[3];
            work->bursts[i][0].color[1] = D_8013EC20[command].color[4];
            work->bursts[i][0].color[2] = D_8013EC20[command].color[5];
            work->bursts[i][0].u = 0;
            work->bursts[i][0].v = 0;
            work->bursts[i][0].stage = 0;
            setVector(&work->bursts[i][0].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i][0].velocity, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
            case 2:
                setVector(&work->bursts[i][0].position, 0, 0, 0);
                switch (i) {
                case 0:
                    setVector(&work->bursts[i][0].scale, 4096, 0, 4096);
                    break;
                case 1:
                    setVector(&work->bursts[i][0].scale, 6144, 0, 6144);
                    break;
                }
                break;
            }
            for (k = 0; k < work->copies; k++) {
                work->bursts[i][k] = work->bursts[i][0];
            }
        }
        for (i = 0; i < work->copies; i++) {
            work->flashes[i].active = 0;
            work->flashes[i].elapsed = 0;
            work->flashes[i].duration = 64;
            work->flashes[i].mode = 1;
            work->flashes[i].coord = Model_GetCameraCoordinateUnit();
            work->flashes[i].color[0] = D_8013EC20[command].color[6];
            work->flashes[i].color[1] = D_8013EC20[command].color[7];
            work->flashes[i].color[2] = D_8013EC20[command].color[8];
            work->flashes[i].u = 0;
            work->flashes[i].v = 0;
            work->flashes[i].stage = 0;
            work->flashes[i].launched = 0;
            setVector(&work->flashes[i].scale, 0, 0, 0);
            switch (command) {
            case 0:
            case 1:
                switch (i) {
                case 0:
                    setVector(&work->flashes[i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 5 / 360, 0);
                    setVector(&work->flashes[i].position, 0, 0, 0);
                    break;
                case 1:
                    setVector(&work->flashes[i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 25 / 360, 0);
                    setVector(&work->flashes[i].position, 0, 0, 0);
                    break;
                case 2:
                    setVector(&work->flashes[i].rotation, -4096 * 20 / 360, work->slot * 2048 - 4096 * 20 / 360, 0);
                    setVector(&work->flashes[i].position, 0, 0, 0);
                    break;
                }
                break;
            case 2:
                setVector(&work->flashes[i].rotation, -512, work->slot * 2048, 0);
                setVector(&work->flashes[i].position, 0, 0, 0);
                break;
            }
            work->flashes[i].rotation.vx -= 1024;
            work->flashes[i].position.vy -= 350;
            work->flashes[i].position.vz -= (work->slot * 2 - 1) * 450;
        }
        work->delay = D_8013EC20[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 5; i >= 0; i--) {
            for (k = 0; k < 9; k++) {
                setVector(&work->dome[k][i], work->radius * (5 - i) / 5 * rsin(k << 9) / 4096,
                    work->radius * (5 - i) / 5 * rcos(k << 9) / 4096,
                    work->height * rsin((i << 10) / 5) / 4096 - work->height);
            }
        }
        for (i = 0; i < 17; i++) {
            for (k = 0; k < 17; k++) {
                /* Each tripled cosine goes through its own block-local
                 * variable, which keeps the 3 off the radius. */
                s32 xa, xb, za, zb;

                setVector(&work->orb[k][i],
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * (xa = rcos(i << 7) * 3) / 4096 * (work->radius * (xb = rcos(i << 7) * 3) / 4096))
                        * rsin(k << 8) / 4096,
                    work->radius * 3 * rcos(i << 7) / 4096,
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * (za = rcos(i << 7) * 3) / 4096 * (work->radius * (zb = rcos(i << 7) * 3) / 4096))
                        * rcos(k << 8) / 4096);
            }
        }
        for (i = 0; i < 9; i++) {
            for (k = 0; k < 9; k++) {
                /* Shifts, not multiplications, keep i's factor off the
                 * radius and the height. */
                setVector(&work->cone[k][i], (work->radius * 4 - work->radius * (i << 1) / 8) * rsin(k << 9) / 4096,
                    -(work->height * 4) + work->height * (i << 2) / 8,
                    (work->radius * 4 - work->radius * (i << 1) / 8) * rcos(k << 9) / 4096);
            }
        }
        for (i = 0; i < 9; i++) {
            for (k = 0; k < 8; k++) {
                /* The same block-local cosines as for the orb. */
                s32 xa, xb, za, zb;

                setVector(&work->rays[k][i],
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * (xa = rcos(i << 7) * 3) / 4096 * (work->radius * (xb = rcos(i << 7) * 3) / 4096))
                        * rsin(k << 9) / 4096,
                    work->radius * 3 + work->radius * -(rcos(i << 7) * 3) / 4096,
                    SquareRoot0(work->radius * 3 * (work->radius * 3)
                        - work->radius * (za = rcos(i << 7) * 3) / 4096 * (work->radius * (zb = rcos(i << 7) * 3) / 4096))
                        * rcos(k << 9) / 4096);
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->flags[0] < work->copies) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 14);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->dome_count; i++) {
                    setVector(&work->domes[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                setVector(&work->flashes[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            if (work->flags[0] == 0) {
                if (work->elapsed == 0) {
                    work->domes[0][0].active = 1;
                    work->flashes[0].active = 1;
                    work->flags[0]++;
                }
            } else if (work->flags[0] == 1 && work->elapsed >= 21) {
                work->domes[0][1].active = 1;
                work->flashes[1].active = 1;
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->flags[0] < work->copies) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 12);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->dome_count; i++) {
                    setVector(&work->domes[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                setVector(&work->flashes[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            if (work->flags[0] == 0 && work->elapsed == 0) {
                work->domes[0][0].active = 1;
                work->flashes[0].active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->flags[0] < work->copies) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 27);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->dome_count; i++) {
                    setVector(&work->domes[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                setVector(&work->flashes[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            if (work->flags[0] == 0 && work->elapsed == 0) {
                work->domes[0][0].active = 1;
                work->flashes[0].active = 1;
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model635ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (j = 0; j < work->copies; j++) {
            for (i = 0; i < work->dome_count; i++) {
                if (work->domes[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->domes[i][j].scale);
                    RotMatrixZ(work->domes[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->domes[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->domes[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i][j].position.vx;
                    unit.matrix.t[1] = work->domes[i][j].position.vy;
                    unit.matrix.t[2] = work->domes[i][j].position.vz;
                    unit.super = work->domes[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (i == 0 && work->domes[0][j].duration / 2 < work->domes[0][j].elapsed
                        && work->domes[0][j].launched == 0) {
                        work->comets[j].active = 1;
                        work->comets[j].velocity = work->domes[0][j].position;
                        switch (work->command) {
                        case 0:
                        case 1:
                            work->comets[j].velocity.vy -= work->domes[0][j].scale.vz * work->height / 4096
                                * rsin(4096 * 20 / 360) / 4096;
                            switch (j) {
                            case 0:
                                work->comets[j].velocity.vz += (work->slot * 2 - 1) * work->domes[0][j].scale.vz
                                    * work->height / 4096 * rcos(-4096 * 20 / 360) / 4096;
                                work->comets[j].position.vx = work->domes[0][j].position.vx
                                    + (work->slot * 2 - 1) * work->domes[0][j].scale.vz * work->height / 4096
                                        * rsin(-4096 * 5 / 360) / 4096;
                                work->comets[j].velocity.vx = ((work->slot * 2 - 1) * 450 - work->comets[j].velocity.vz)
                                    / work->comets[j].duration * rsin(-4096 * 5 / 360) / rcos(-4096 * 5 / 360);
                                break;
                            case 1:
                                work->comets[j].velocity.vz += (work->slot * 2 - 1) * work->domes[0][j].scale.vz
                                    * work->height / 4096 * rcos(-4096 * 20 / 360) / 4096;
                                work->comets[j].position.vx = work->domes[0][j].position.vx
                                    + (work->slot * 2 - 1) * work->domes[0][j].scale.vz * work->height / 4096
                                        * rsin(-4096 * 25 / 360) / 4096;
                                work->comets[j].velocity.vx = ((work->slot * 2 - 1) * 450 - work->comets[j].velocity.vz)
                                    / work->comets[j].duration * rsin(-4096 * 25 / 360) / rcos(-4096 * 25 / 360);
                                break;
                            case 2:
                                work->comets[j].velocity.vz += (work->slot * 2 - 1) * work->domes[0][j].scale.vz
                                    * work->height / 4096 * rcos(-4096 * 20 / 360) / 4096;
                                work->comets[j].position.vx = work->domes[0][j].position.vx
                                    + (work->slot * 2 - 1) * work->domes[0][j].scale.vz * work->height / 4096
                                        * rsin(-4096 * 20 / 360) / 4096;
                                work->comets[j].velocity.vx = ((work->slot * 2 - 1) * 450 - work->comets[j].velocity.vz)
                                    / work->comets[j].duration * rsin(-4096 * 20 / 360) / rcos(-4096 * 20 / 360);
                                break;
                            }
                            break;
                        case 2:
                            work->comets[j].velocity.vy -= work->domes[0][j].scale.vz * work->height / 4096
                                * rsin(512) / 4096;
                            work->comets[j].velocity.vz += (work->slot * 2 - 1) * work->domes[0][j].scale.vz
                                * work->height / 4096 * rcos(-512) / 4096;
                            work->comets[j].position.vx = work->domes[0][j].position.vx
                                + (work->slot * 2 - 1) * work->domes[0][j].scale.vz * work->height / 4096
                                    * rsin(0) / 4096;
                            work->comets[j].velocity.vx = ((work->slot * 2 - 1) * 450 - work->comets[j].velocity.vz)
                                / work->comets[j].duration * rsin(0) / rcos(0);
                            break;
                        }
                        /* i is 0 here, but retail addresses the flag through it. */
                        work->domes[i][j].launched = 1;
                    }
                    work->domes[i][j].position.vx += work->domes[i][j].velocity.vx * step;
                    work->domes[i][j].position.vy += work->domes[i][j].velocity.vy * step;
                    work->domes[i][j].position.vz += work->domes[i][j].velocity.vz * step;
                    setVector(&work->domes[i][j].scale,
                        (work->domes[i][j].elapsed << 14) / work->domes[i][j].duration,
                        (work->domes[i][j].elapsed << 14) / work->domes[i][j].duration,
                        (work->domes[i][j].elapsed << 14) / work->domes[i][j].duration);
                    /* The shift keeps the doubling off the table colour. */
                    if (work->domes[i][j].duration / 2 < work->domes[i][j].elapsed) {
                        work->domes[i][j].color[0] = D_8013EC20[work->command].color[0]
                            * ((work->domes[i][j].duration - work->domes[i][j].elapsed) << 1) / work->domes[i][j].duration;
                        work->domes[i][j].color[1] = D_8013EC20[work->command].color[1]
                            * ((work->domes[i][j].duration - work->domes[i][j].elapsed) << 1) / work->domes[i][j].duration;
                        work->domes[i][j].color[2] = D_8013EC20[work->command].color[2]
                            * ((work->domes[i][j].duration - work->domes[i][j].elapsed) << 1) / work->domes[i][j].duration;
                    }
                    for (k = 0; k < 5; k++) {
                        for (l = 0; l < 8; l++) {
                            setPolyGT4(&work->strip);
                            work->strip.tpage = work->tpage[0];
                            work->strip.clut = work->clut[0];
                            setSemiTrans(&work->strip, 1);
                            setUV4(&work->strip, work->domes[i][j].u, work->domes[i][j].v,
                                work->domes[i][j].u + 31, work->domes[i][j].v,
                                work->domes[i][j].u, work->domes[i][j].v + 63,
                                work->domes[i][j].u + 31, work->domes[i][j].v + 63);
                            setRGB0(&work->strip, work->domes[i][j].color[0] * rsin((k << 11) / 5) / 4096,
                                work->domes[i][j].color[1] * rsin((k << 11) / 5) / 4096,
                                work->domes[i][j].color[2] * rsin((k << 11) / 5) / 4096);
                            setRGB1(&work->strip, work->domes[i][j].color[0] * rsin((k << 11) / 5) / 4096,
                                work->domes[i][j].color[1] * rsin((k << 11) / 5) / 4096,
                                work->domes[i][j].color[2] * rsin((k << 11) / 5) / 4096);
                            setRGB2(&work->strip, work->domes[i][j].color[0] * rsin(((k + 1) << 11) / 5) / 4096,
                                work->domes[i][j].color[1] * rsin(((k + 1) << 11) / 5) / 4096,
                                work->domes[i][j].color[2] * rsin(((k + 1) << 11) / 5) / 4096);
                            setRGB3(&work->strip, work->domes[i][j].color[0] * rsin(((k + 1) << 11) / 5) / 4096,
                                work->domes[i][j].color[1] * rsin(((k + 1) << 11) / 5) / 4096,
                                work->domes[i][j].color[2] * rsin(((k + 1) << 11) / 5) / 4096);
                            depth = RotAverage4(&work->dome[l][k], &work->dome[l + 1][k],
                                &work->dome[l][k + 1], &work->dome[l + 1][k + 1],
                                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    if (work->domes[i][j].stage == 1 && i < work->dome_count - 1) {
                        work->domes[i + 1][j].active = 1;
                    }
                    if (work->domes[i][j].stage < 3) {
                        work->domes[i][j].stage++;
                    }
                    work->domes[i][j].elapsed += step;
                    if (work->domes[i][j].duration < work->domes[i][j].elapsed) {
                        work->domes[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->copies; i++) {
            if (work->comets[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->comets[i].scale);
                unit.matrix.t[0] = work->comets[i].position.vx;
                /* A parabola from the launch height up to -100. The 5 * elapsed goes through
                 * its own variable, which keeps the 5 off the rise. */
                start = work->comets[i].velocity.vy;
                duration = work->comets[i].duration;
                rise = -100 - start;
                elapsed = work->comets[i].elapsed;
                unit.matrix.t[1] = rise * (ramp = elapsed * 5) * elapsed / (duration * duration * 3)
                    + -(rise * 2) / (duration * 3) * elapsed + start;
                unit.matrix.t[2] = ((work->slot * 2 - 1) * 450 - work->comets[i].velocity.vz) * work->comets[i].elapsed
                    / work->comets[i].duration + work->comets[i].velocity.vz;
                if (work->comets[i].elapsed == 0) {
                    for (k = 0; k < 2; k++) {
                        setVector(&work->trail[k][i], unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    }
                } else {
                    work->trail[1][i] = work->trail[0][i];
                    setVector(&work->trail[0][i], unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                }
                setVector(&work->comets[i].rotation,
                    ratan2(work->trail[0][i].vy - work->trail[1][i].vy, work->trail[0][i].vz - work->trail[1][i].vz) - 1024,
                    ratan2(work->trail[0][i].vx - work->trail[1][i].vx, work->trail[0][i].vz - work->trail[1][i].vz)
                        + work->slot * 2048,
                    0);
                RotMatrixZ(work->comets[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->comets[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->comets[i].rotation.vy, &unit.matrix);
                unit.super = work->comets[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->comets[i].position.vx += work->comets[i].velocity.vx * step;
                if (work->comets[i].stage == 0) {
                    applyVector(&work->comets[i].scale, 136, 136, 136, -=);
                    if (work->comets[i].scale.vx < 0x666) {
                        work->comets[i].stage = 1;
                    }
                } else {
                    applyVector(&work->comets[i].scale, 136, 136, 136, +=);
                    if (work->comets[i].scale.vx > 0x800) {
                        work->comets[i].stage = 0;
                    }
                }
                if (work->comets[i].elapsed < work->comets[i].duration / 5) {
                    work->comets[i].color[0] = D_8013EC20[work->command].color[0] * work->comets[i].elapsed * 5
                        / work->comets[i].duration;
                    work->comets[i].color[1] = D_8013EC20[work->command].color[1] * work->comets[i].elapsed * 5
                        / work->comets[i].duration;
                    work->comets[i].color[2] = D_8013EC20[work->command].color[2] * work->comets[i].elapsed * 5
                        / work->comets[i].duration;
                }
                for (k = 0; k < 16; k++) {
                    for (l = 0; l < 16; l++) {
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[1];
                        work->sprite.clut = work->clut[1];
                        setSemiTrans(&work->sprite, 1);
                        setUV4(&work->sprite, work->comets[i].u + l * 127 / 16, work->comets[i].v + k * 63 / 16,
                            work->comets[i].u + (l + 1) * 127 / 16, work->comets[i].v + k * 63 / 16,
                            work->comets[i].u + l * 127 / 16, work->comets[i].v + (k + 1) * 63 / 16,
                            work->comets[i].u + (l + 1) * 127 / 16, work->comets[i].v + (k + 1) * 63 / 16);
                        setRGB0(&work->sprite, work->comets[i].color[0], work->comets[i].color[1],
                            work->comets[i].color[2]);
                        depth = RotAverage4(&work->orb[l][k], &work->orb[l + 1][k],
                            &work->orb[l][k + 1], &work->orb[l + 1][k + 1],
                            (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->comets[i].elapsed += step;
                if (work->comets[i].duration < work->comets[i].elapsed) {
                    work->comets[i].active = 0;
                    for (k = 0; k < work->burst_count; k++) {
                        work->bursts[k][i].active = 1;
                        setVector(&work->bursts[k][i].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    }
                }
            } else {
                inactive++;
            }
        }
        for (k = 0; k < work->copies; k++) {
            for (i = 0; i < work->burst_count; i++) {
                if (work->bursts[i][k].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i][k].scale);
                    RotMatrixY(work->bursts[i][k].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][k].position.vx;
                    unit.matrix.t[1] = work->bursts[i][k].position.vy;
                    unit.matrix.t[2] = work->bursts[i][k].position.vz;
                    unit.super = work->bursts[i][k].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    switch (i) {
                    case 0:
                        if (work->bursts[i][k].elapsed < 20) {
                            work->bursts[i][k].scale.vy += step * 204;
                            if (work->bursts[i][k].scale.vy > 4096) {
                                work->bursts[i][k].scale.vy = 4096;
                            }
                        } else {
                            work->bursts[i][k].scale.vy = rsin(work->bursts[i][k].stage * 200 + 2048) / 32 + 4096;
                            work->bursts[i][k].stage += step;
                        }
                        break;
                    case 1:
                        if (work->bursts[i][k].elapsed < 20) {
                            work->bursts[i][k].scale.vy += step * 136;
                            if (work->bursts[i][k].scale.vy > 2730) {
                                work->bursts[i][k].scale.vy = 2730;
                            }
                        } else {
                            work->bursts[i][k].scale.vy = rsin(work->bursts[i][k].stage * 200) / 32 + 2730;
                            work->bursts[i][k].stage += step;
                        }
                        break;
                    }
                    /* As for the domes, the shift keeps the doubling off the colour. */
                    if (work->bursts[i][k].duration / 2 < work->bursts[i][k].elapsed) {
                        work->bursts[i][k].color[0] = D_8013EC20[work->command].color[3]
                            * ((work->bursts[i][k].duration - work->bursts[i][k].elapsed) << 1) / work->bursts[i][k].duration;
                        work->bursts[i][k].color[1] = D_8013EC20[work->command].color[4]
                            * ((work->bursts[i][k].duration - work->bursts[i][k].elapsed) << 1) / work->bursts[i][k].duration;
                        work->bursts[i][k].color[2] = D_8013EC20[work->command].color[5]
                            * ((work->bursts[i][k].duration - work->bursts[i][k].elapsed) << 1) / work->bursts[i][k].duration;
                    }
                    PushMatrix();
                    for (l = 0; l < 9; l++) {
                        m = D_800FE168;
                        RotMatrixY(rsin(work->bursts[i][k].elapsed * 160 + l * 500) / 15, &m);
                        for (j = 0; j < 9; j++) {
                            ApplyMatrixSV(&m, &work->cone[j][l], &work->spun[j][l]);
                        }
                    }
                    PopMatrix();
                    for (l = 0; l < 8; l++) {
                        for (j = 0; j < 8; j++) {
                            setPolyGT4(&work->strip);
                            setSemiTrans(&work->strip, 1);
                            work->strip.tpage = work->tpage[1];
                            work->strip.clut = work->clut[1];
                            setUV4(&work->strip, work->bursts[i][k].u + (8 - j) * 127 / 8,
                                work->bursts[i][k].v + (8 - l) * 63 / 8,
                                work->bursts[i][k].u + (7 - j) * 127 / 8, work->bursts[i][k].v + (8 - l) * 63 / 8,
                                work->bursts[i][k].u + (8 - j) * 127 / 8, work->bursts[i][k].v + (7 - l) * 63 / 8,
                                work->bursts[i][k].u + (7 - j) * 127 / 8, work->bursts[i][k].v + (7 - l) * 63 / 8);
                            setRGB0(&work->strip, work->bursts[i][k].color[0] / (l + 1),
                                work->bursts[i][k].color[1] / (l + 1), work->bursts[i][k].color[2] / (l + 1));
                            setRGB1(&work->strip, work->bursts[i][k].color[0] / (l + 1),
                                work->bursts[i][k].color[1] / (l + 1), work->bursts[i][k].color[2] / (l + 1));
                            setRGB2(&work->strip, work->bursts[i][k].color[0] / (l + 2),
                                work->bursts[i][k].color[1] / (l + 2), work->bursts[i][k].color[2] / (l + 2));
                            setRGB3(&work->strip, work->bursts[i][k].color[0] / (l + 2),
                                work->bursts[i][k].color[1] / (l + 2), work->bursts[i][k].color[2] / (l + 2));
                            depth = RotAverage4(&work->spun[j][l], &work->spun[j + 1][l],
                                &work->spun[j][l + 1], &work->spun[j + 1][l + 1],
                                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->bursts[i][k].elapsed += step;
                    if (work->bursts[i][k].duration < work->bursts[i][k].elapsed) {
                        work->bursts[i][k].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->copies; i++) {
            if (work->flashes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->flashes[i].scale);
                RotMatrixZ(work->flashes[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->flashes[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->flashes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->flashes[i].position.vx;
                unit.matrix.t[1] = work->flashes[i].position.vy;
                unit.matrix.t[2] = work->flashes[i].position.vz;
                unit.super = work->flashes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setVector(&work->flashes[i].scale, (work->flashes[i].elapsed << 13) / work->flashes[i].duration,
                    (work->flashes[i].elapsed << 14) / work->flashes[i].duration,
                    (work->flashes[i].elapsed << 13) / work->flashes[i].duration);
                work->flashes[i].color[0] = D_8013EC20[work->command].color[6]
                    * (work->flashes[i].duration - work->flashes[i].elapsed) / work->flashes[i].duration;
                work->flashes[i].color[1] = D_8013EC20[work->command].color[7]
                    * (work->flashes[i].duration - work->flashes[i].elapsed) / work->flashes[i].duration;
                work->flashes[i].color[2] = D_8013EC20[work->command].color[8]
                    * (work->flashes[i].duration - work->flashes[i].elapsed) / work->flashes[i].duration;
                for (k = 0; k < 8; k++) {
                    for (l = 0; l < 8; l++) {
                        work->line.attribute = 0x50000000;
                        work->line.r0 = work->flashes[i].color[0] * k / 8;
                        work->line.g0 = work->flashes[i].color[1] * k / 8;
                        work->line.b0 = work->flashes[i].color[2] * k / 8;
                        work->line.r1 = work->flashes[i].color[0] * (k + 1) / 8;
                        work->line.g1 = work->flashes[i].color[1] * (k + 1) / 8;
                        work->line.b1 = work->flashes[i].color[2] * (k + 1) / 8;
                        RotTransPers(&work->rays[l][k], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                        RotTransPers(&work->rays[l][k + 1], (PSXLONG *)&work->line.x1, &interpolation, &flag);
                        setVector(&vec, (work->rays[l][k].vx + work->rays[l][k + 1].vx) / 2,
                            (work->rays[l][k].vy + work->rays[l][k + 1].vy) / 2,
                            (work->rays[l][k].vz + work->rays[l][k + 1].vz) / 2);
                        depth = RotTransPers(&vec, &sxy, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->flashes[i].elapsed += step;
                if (work->flashes[i].duration < work->flashes[i].elapsed) {
                    work->flashes[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == (work->dome_count + 2 + work->burst_count) * work->copies) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

#include "../../types.h"
#include "variant657_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 657's entry: rings of Gouraud strips that grow out of a model
 * part, a pulsing band and a cap around each ring and a ring of spinning
 * shard sprites, all built from per-command colours. The setup also
 * prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model657State *work;
    MATRIX ls;
    MATRIX m;
    s32 uv[4];
    GsCOORDUNIT unit;
    Model657ViewSnapshot view;
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, i, step, inactive, distance;
    s32 j, k;
    s32 value, duration, lift;
    u8 r, g, b;
    s32 swell_x, swell_y;
    u32 tex;
    PSXLONG depth;

    work = (Model657State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model657State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model657State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FBC8[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013FBC8[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
            work->ring_count = 1;
            work->shard_count = 8;
            break;
        /* A separate, identical case: cross-jumping merges the bodies but
         * keeps the jump table. */
        case 6:
            work->ring_count = 1;
            work->shard_count = 8;
            break;
        }
        work->radius = D_8013FC1C[command].radius;
        work->height = D_8013FC1C[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            switch (command) {
            case 0:
                work->rings[i].duration = 286;
                break;
            case 1:
                work->rings[i].duration = 168;
                break;
            case 2:
                work->rings[i].duration = 250;
                break;
            case 3:
                work->rings[i].duration = 292;
                break;
            case 4:
                work->rings[i].duration = 304;
                break;
            case 5:
                work->rings[i].duration = 280;
                break;
            case 6:
            case 8:
                work->rings[i].duration = 64;
                break;
            case 7:
                work->rings[i].duration = 280;
                break;
            case 9:
                work->rings[i].duration = 306;
                break;
            case 10:
                work->rings[i].duration = 242;
                break;
            }
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013FC1C[command].color[0];
            work->rings[i].color[1] = D_8013FC1C[command].color[1];
            work->rings[i].color[2] = D_8013FC1C[command].color[2];
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            work->triggered[i] = 0;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].offset, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->ring_count; i++) {
            work->bands[i].active = 0;
            work->bands[i].elapsed = 0;
            work->bands[i].duration = work->rings[i].duration * 3 / 4;
            work->bands[i].mode = 1;
            work->bands[i].coord = Model_GetCameraCoordinateUnit();
            work->bands[i].color[0] = D_8013FC1C[command].color[3];
            work->bands[i].color[1] = D_8013FC1C[command].color[4];
            work->bands[i].color[2] = D_8013FC1C[command].color[5];
            work->bands[i].u = 0;
            work->bands[i].v = 0;
            setVector(&work->bands[i].scale, 4096, 4096, 4096);
            setVector(&work->bands[i].offset, 0, 0, 0);
            setVector(&work->bands[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bands[i].position, 0, 0, 0);
            work->bands[i].position.vy -= 350;
            work->bands[i].position.vz += work->direction * 450;
        }
        for (i = 0; i < work->ring_count; i++) {
            work->caps[i].active = 0;
            work->caps[i].elapsed = 0;
            work->caps[i].mode = 1;
            work->caps[i].duration = work->rings[i].duration;
            work->caps[i].coord = Model_GetCameraCoordinateUnit();
            work->caps[i].color[0] = D_8013FC1C[command].color[3];
            work->caps[i].color[1] = D_8013FC1C[command].color[4];
            work->caps[i].color[2] = D_8013FC1C[command].color[5];
            work->caps[i].u = 0;
            work->caps[i].v = 0;
            setVector(&work->caps[i].scale, 4096, 4096, 4096);
            setVector(&work->caps[i].offset, 0, 0, 0);
            setVector(&work->caps[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->caps[i].position, 0, 0, 0);
        }
        for (j = 0; j < work->ring_count; j++) {
            for (i = 0; i < work->shard_count; i++) {
                work->shards[i][j].active = 0;
                work->shards[i][j].elapsed = 0;
                work->shards[i][j].duration = work->rings[j].duration * 3 / 4;
                work->shards[i][j].mode = 1;
                work->shards[i][j].coord = Model_GetCameraCoordinateUnit();
                work->shards[i][j].color[0] = D_8013FC1C[command].color[6];
                work->shards[i][j].color[1] = D_8013FC1C[command].color[7];
                work->shards[i][j].color[2] = D_8013FC1C[command].color[8];
                work->shards[i][j].u = ((i * i) % 8) << 5;
                work->shards[i][j].v = 128;
                setVector(&work->shards[i][j].scale, 4096, 4096, 4096);
                if (i == 0) {
                    setVector(&work->shards[i][j].offset, 0, 0, 0);
                } else {
                    setVector(&work->shards[i][j].offset,
                        work->radius * (rsin((i - 1) * 4096 / (work->shard_count - 1)) << 1) / 4096,
                        work->radius * (rcos((i - 1) * 4096 / (work->shard_count - 1)) << 1) / 4096,
                        work->radius * ((i % 2) * 2 - 1) * rsin((i - 1) * 4096 / (work->shard_count - 1)) / 4096
                            - work->radius);
                }
                setVector(&work->shards[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->shards[i][j].position, 0, 0, 0);
            }
        }
        work->delay = D_8013FC1C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < work->ring_count; i++) {
            /* Each tripled sine goes through its own variable, which keeps
             * the 3 off the radius and keeps either temporary block-local. */
            for (j = 0, lift = 0; j < 5; lift -= 5, j++) {
                for (k = 0; k < 9; k++) {
                    setVector(&work->cap[k][j][i],
                        work->radius * (swell_x = rsin(j << 8) * 3) / 4096 * rsin(k << 9) / 4096,
                        work->radius * (swell_y = rsin(j << 8) * 3) / 4096 * rcos(k << 9) / 4096,
                        work->radius * lift / 4);
                }
            }
        }
        setVector(&work->corners[0], -(work->radius * 2), -(work->radius * 2), 0);
        setVector(&work->corners[1], work->radius * 2, -(work->radius * 2), 0);
        setVector(&work->corners[2], -(work->radius * 2), work->radius * 2, 0);
        setVector(&work->corners[3], work->radius * 2, work->radius * 2, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
    case 10:
        if (work->elapsed == 0) {
            for (i = 0; i < work->ring_count; i++) {
                work->rings[i].active = 1;
                work->caps[i].active = 1;
                work->flags[1] = 1;
            }
        }
        break;
    case 6:
        if (work->elapsed == 0) {
            work->bands[0].active = 1;
            work->bands[0].position.vz += work->direction * work->height / -5;
            for (i = 0; i < work->shard_count; i++) {
                work->shards[i][0].active = 1;
                PushMatrix();
                offset = work->shards[i][0].offset;
                m = D_800FE168;
                RotMatrixY(work->slot << 11, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[i][0].position);
                /* Adding 0 to vx leaves the original's dead load of it. */
                applyVector(&work->shards[i][0].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
            work->flags[1] = 1;
        }
        break;
    case 8:
        if (work->elapsed == 0) {
            work->bands[0].active = 1;
            work->bands[0].position.vy += 50;
            work->bands[0].position.vz += work->direction * work->height / -5;
            for (i = 0; i < work->shard_count; i++) {
                work->shards[i][0].active = 1;
                PushMatrix();
                offset = work->shards[i][0].offset;
                m = D_800FE168;
                RotMatrixY(work->slot << 11, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[i][0].position);
                applyVector(&work->shards[i][0].position, 0, -300, work->direction * 450, +=);
                PopMatrix();
            }
            work->flags[1] = 1;
        }
        break;
    }
    switch (work->command) {
    case 0:
    case 1:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 10);
        GsGetLwUnit(&unit, &ls);
        /* ABS inside the call negates the sum in place in the argument
         * register. */
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 2:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 12);
        GsGetLwUnit(&unit, &ls);
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 3:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
        GsGetLwUnit(&unit, &ls);
        ls.t[1] += 25;
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 4:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
        GsGetLwUnit(&unit, &ls);
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 5:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 7);
        GsGetLwUnit(&unit, &ls);
        ls.t[1] += 25;
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 7:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 7);
        GsGetLwUnit(&unit, &ls);
        ls.t[1] -= 25;
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 9:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
        GsGetLwUnit(&unit, &ls);
        ls.t[2] -= work->direction * 50;
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    case 10:
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 7);
        GsGetLwUnit(&unit, &ls);
        ls.t[1] += 10;
        ls.t[2] -= work->direction * 25;
        distance = SquareRoot0(ABS(ls.t[0] * ls.t[0] + (ls.t[1] + 350) * (ls.t[1] + 350)
            + (ls.t[2] - work->direction * 450) * (ls.t[2] - work->direction * 450)));
        for (i = 0; i < work->ring_count; i++) {
            setVector(&work->rings[i].position, ls.t[0], ls.t[1], ls.t[2]);
            setVector(&work->rings[i].rotation,
                ratan2(-(ls.t[1] + 350), ls.t[2] - work->direction * 450),
                ratan2(ls.t[0], ls.t[2] - work->direction * 450) + work->slot * 2048, 0);
            work->bands[i].rotation = work->rings[i].rotation;
            duration = work->direction * work->height / -5;
            setVector(&work->bands[i].position, duration * rsin(work->bands[i].rotation.vy) / 4096,
                work->direction * duration * rsin(work->bands[i].rotation.vx) / 4096 - 350,
                duration * rcos(work->bands[i].rotation.vy) / 4096 + work->direction * 450);
            work->caps[i].rotation = work->rings[i].rotation;
            work->caps[i].position = work->rings[i].position;
            for (j = 0; j < work->shard_count; j++) {
                PushMatrix();
                offset = work->shards[j][i].offset;
                m = D_800FE168;
                RotMatrixZ(work->rings[i].rotation.vz, &m);
                RotMatrixX(work->rings[i].rotation.vx, &m);
                RotMatrixY(work->rings[i].rotation.vy, &m);
                ApplyMatrixLV(&m, &offset, &work->shards[j][i].position);
                applyVector(&work->shards[j][i].position, 0, -350, work->direction * 450, +=);
                PopMatrix();
            }
        }
        break;
    default:
        distance = 0;
        break;
    }
    PushMatrix();
    view = *(Model657ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->ring_count; i++) {
        if (work->rings[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->rings[i].elapsed < work->rings[i].duration / 4) {
                value = (work->rings[i].elapsed << 14) / work->rings[i].duration;
            } else {
                value = 4096;
                if (work->triggered[i] == 0) {
                    work->bands[i].active = 1;
                    for (j = 0; j < work->shard_count; j++) {
                        work->shards[j][i].active = 1;
                    }
                    work->triggered[i] = 1;
                }
            }
            setVector(&work->rings[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->rings[i].scale);
            RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->rings[i].position.vx;
            unit.matrix.t[1] = work->rings[i].position.vy;
            unit.matrix.t[2] = work->rings[i].position.vz;
            unit.super = work->rings[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->rings[i].duration * 9 / 10 < work->rings[i].elapsed) {
                value = (work->rings[i].duration - work->rings[i].elapsed) * 10;
                duration = work->rings[i].duration;
                work->rings[i].color[0] = D_8013FC1C[work->command].color[0] * value / duration;
                work->rings[i].color[1] = D_8013FC1C[work->command].color[1] * value / duration;
                work->rings[i].color[2] = D_8013FC1C[work->command].color[2] * value / duration;
            }
            /* k runs outside j here, swapping their usual roles. */
            for (k = 0; k < 17; k++) {
                for (j = 0; j < 9; j++) {
                    value = work->radius / 4;
                    setVector(&work->ring[j][k][i],
                        (work->radius + value * rsin(work->rings[i].elapsed * 100 + k * -500) / 4096)
                            * rsin(j << 9) / 4096,
                        (work->radius + value * rsin(work->rings[i].elapsed * 100 + k * -500) / 4096)
                            * rcos(j << 9) / 4096,
                        distance * -k / 16);
                }
            }
            work->rings[i].v = (work->rings[i].elapsed * 4) % 64;
            for (j = 0; j < 16; j++) {
                for (k = 0; k < 8; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    uv[0] = (k % 4) * 63 / 4;
                    uv[1] = ((k % 4) + 1) * 63 / 4;
                    uv[2] = (j % 2) << 5;
                    uv[3] = ((j % 2) + 1) << 5;
                    setUV4(&work->strip, work->rings[i].u + uv[0], work->rings[i].v + uv[3],
                        work->rings[i].u + uv[1], work->rings[i].v + uv[3],
                        work->rings[i].u + uv[0], work->rings[i].v + uv[2],
                        work->rings[i].u + uv[1], work->rings[i].v + uv[2]);
                    if (j == 0) {
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                    } else {
                        setRGB0(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                        setRGB1(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    }
                    if (j == 15) {
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                    } else {
                        setRGB2(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                        setRGB3(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    }
                    depth = RotAverage4(&work->ring[k][j][i], &work->ring[k + 1][j][i],
                        &work->ring[k][j + 1][i], &work->ring[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->rings[i].elapsed += step;
            if (work->rings[i].duration < work->rings[i].elapsed) {
                work->rings[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->ring_count; i++) {
        if (work->bands[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            /* An if/else, not a preset, keeps the order of the hoisted &unit
             * and &unit.matrix spill slots. */
            if (work->bands[i].elapsed < work->bands[i].duration / 2) {
                value = (work->bands[i].elapsed << 13) / work->bands[i].duration + 0x400;
            } else {
                value = 0x1400;
            }
            setVector(&work->bands[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->bands[i].scale);
            RotMatrixZ(work->bands[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->bands[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->bands[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->bands[i].position.vx;
            unit.matrix.t[1] = work->bands[i].position.vy;
            unit.matrix.t[2] = work->bands[i].position.vz;
            unit.super = work->bands[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->bands[i].duration * 9 / 10 < work->bands[i].elapsed) {
                value = (work->bands[i].duration - work->bands[i].elapsed) * 10;
                duration = work->bands[i].duration;
                work->bands[i].color[0] = D_8013FC1C[work->command].color[3] * value / duration;
                work->bands[i].color[1] = D_8013FC1C[work->command].color[4] * value / duration;
                work->bands[i].color[2] = D_8013FC1C[work->command].color[5] * value / duration;
            }
            for (j = 0; j < 9; j++) {
                /* The shift keeps the step at 341 instead of folding it to 682. */
                duration = work->height * -rsin((j * 341 << 1) / 8) / 4096;
                for (k = 0; k < 17; k++) {
                    value = work->radius * j / 8 * rsin(work->bands[i].elapsed * 100 + (k << 10)) / 4096;
                    setVector(&work->band[k][j][i],
                        (work->height + value) * j / 8 * rsin(k << 8) / 4096,
                        (work->height + value) * j / 8 * rcos(k << 8) / 4096,
                        duration + value);
                }
            }
            work->bands[i].v = (work->bands[i].elapsed * 4) % 64;
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    uv[0] = (k % 4) * 63 / 4;
                    uv[1] = ((k % 4) + 1) * 63 / 4;
                    uv[2] = (j % 2) << 5;
                    uv[3] = ((j % 2) + 1) << 5;
                    setUV4(&work->strip, work->bands[i].u + uv[0], work->bands[i].v + uv[3],
                        work->bands[i].u + uv[1], work->bands[i].v + uv[3],
                        work->bands[i].u + uv[0], work->bands[i].v + uv[2],
                        work->bands[i].u + uv[1], work->bands[i].v + uv[2]);
                    if (j >= 4) {
                        value = (8 - j) * 2;
                        r = work->bands[i].color[0] * value / 8;
                        g = work->bands[i].color[1] * value / 8;
                        b = work->bands[i].color[2] * value / 8;
                    } else {
                        r = 0;
                        if (j == 0) {
                            g = 0;
                            b = 0;
                        } else {
                            r = work->bands[i].color[0];
                            g = work->bands[i].color[1];
                            b = work->bands[i].color[2];
                        }
                    }
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    if (j >= 4) {
                        value = (7 - j) * 2;
                        r = work->bands[i].color[0] * value / 8;
                        g = work->bands[i].color[1] * value / 8;
                        b = work->bands[i].color[2] * value / 8;
                    } else {
                        r = work->bands[i].color[0];
                        g = work->bands[i].color[1];
                        b = work->bands[i].color[2];
                    }
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->band[k][j][i], &work->band[k + 1][j][i],
                        &work->band[k][j + 1][i], &work->band[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->bands[i].elapsed += step;
            if (work->bands[i].duration < work->bands[i].elapsed) {
                work->bands[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->ring_count; i++) {
        if (work->caps[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->caps[i].elapsed < work->caps[i].duration / 4) {
                value = (work->caps[i].elapsed << 14) / work->caps[i].duration;
            } else {
                value = 4096;
            }
            setVector(&work->caps[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->caps[i].scale);
            RotMatrixZ(work->caps[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->caps[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->caps[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->caps[i].position.vx;
            unit.matrix.t[1] = work->caps[i].position.vy;
            unit.matrix.t[2] = work->caps[i].position.vz;
            unit.super = work->caps[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->caps[i].duration * 9 / 10 < work->caps[i].elapsed) {
                value = (work->caps[i].duration - work->caps[i].elapsed) * 10;
                duration = work->caps[i].duration;
                work->caps[i].color[0] = D_8013FC1C[work->command].color[3] * value / duration;
                work->caps[i].color[1] = D_8013FC1C[work->command].color[4] * value / duration;
                work->caps[i].color[2] = D_8013FC1C[work->command].color[5] * value / duration;
            }
            work->caps[i].v = (work->caps[i].elapsed * 4) % 64;
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 8; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    uv[0] = (k % 4) * 63 / 4;
                    uv[1] = ((k % 4) + 1) * 63 / 4;
                    uv[2] = (j % 2) << 5;
                    uv[3] = ((j % 2) + 1) << 5;
                    setUV4(&work->strip, work->caps[i].u + uv[0], work->caps[i].v + uv[3],
                        work->caps[i].u + uv[1], work->caps[i].v + uv[3],
                        work->caps[i].u + uv[0], work->caps[i].v + uv[2],
                        work->caps[i].u + uv[1], work->caps[i].v + uv[2]);
                    value = 4 - j;
                    r = work->caps[i].color[0] * value / 4;
                    g = work->caps[i].color[1] * value / 4;
                    b = work->caps[i].color[2] * value / 4;
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    value = 3 - j;
                    r = work->caps[i].color[0] * value / 4;
                    g = work->caps[i].color[1] * value / 4;
                    b = work->caps[i].color[2] * value / 4;
                    setRGB2(&work->strip, r, g, b);
                    setRGB3(&work->strip, r, g, b);
                    depth = RotAverage4(&work->cap[k][j][i], &work->cap[k + 1][j][i],
                        &work->cap[k][j + 1][i], &work->cap[k + 1][j + 1][i],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->caps[i].elapsed += step;
            if (work->caps[i].duration < work->caps[i].elapsed) {
                work->caps[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->ring_count; j++) {
        for (i = 0; i < work->shard_count; i++) {
            if (work->shards[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->shards[i][j].elapsed < work->shards[i][j].duration / 4) {
                    value = (work->shards[i][j].elapsed << 14) / work->shards[i][j].duration;
                } else {
                    value = 4096;
                }
                setVector(&work->shards[i][j].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->shards[i][j].scale);
                RotMatrixZ(work->shards[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->shards[i][j].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->shards[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->shards[i][j].position.vx;
                unit.matrix.t[1] = work->shards[i][j].position.vy;
                unit.matrix.t[2] = work->shards[i][j].position.vz;
                unit.super = work->shards[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->shards[i][j].duration * 9 / 10 < work->shards[i][j].elapsed) {
                    value = (work->shards[i][j].duration - work->shards[i][j].elapsed) * 10;
                    duration = work->shards[i][j].duration;
                    work->shards[i][j].color[0] = D_8013FC1C[work->command].color[6] * value / duration;
                    work->shards[i][j].color[1] = D_8013FC1C[work->command].color[7] * value / duration;
                    work->shards[i][j].color[2] = D_8013FC1C[work->command].color[8] * value / duration;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[1];
                work->sprite.clut = work->clut[1];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->shards[i][j].u, work->shards[i][j].v,
                    work->shards[i][j].u + 31, work->shards[i][j].v,
                    work->shards[i][j].u, work->shards[i][j].v + 31,
                    work->shards[i][j].u + 31, work->shards[i][j].v + 31);
                setRGB0(&work->sprite, work->shards[i][j].color[0], work->shards[i][j].color[1],
                    work->shards[i][j].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->shards[i][j].u += 32;
                if (work->shards[i][j].u >= 225) {
                    work->shards[i][j].u = 0;
                }
                work->shards[i][j].elapsed += step;
                if (work->shards[i][j].duration < work->shards[i][j].elapsed) {
                    work->shards[i][j].active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (work->flags[1] == 1 && inactive == work->ring_count * 3 + work->ring_count * work->shard_count) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}

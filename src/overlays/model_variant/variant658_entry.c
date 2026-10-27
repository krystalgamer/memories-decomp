#include "../../types.h"
#include "variant658_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 658's entry: one or two camera-facing halos (a sprite, a two-band
 * ring of strips and a cloud of shards), 128 bursts drawn as mirrored
 * Gouraud lines, 32 spark cones and a spinning flash, while the model's
 * ambient light is darkened and later restored. The setup also prints the
 * work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model658State *work;
    MATRIX ls;
    MATRIX m;
    s32 z[4];
    s32 x[2], y[2];
    GsCOORDUNIT unit;
    Model658ViewSnapshot view;
    SVECTOR v[4];
    SVECTOR rot;
    PSXLONG interpolation, flag;
    s32 result, step, inactive, skip;
    s32 i, j, k;
    s32 sign, phase, angle, quarter;
    s32 value, duration;
    u32 tex;
    PSXLONG depth;

    work = (Model658State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model658State));
        printf("E_DATA size = %x byte\n", sizeof(Model658State));
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->darken = 0;
        work->brighten = 0;
        work->fade = 0;
        work->projection = 100;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E03C[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E03C[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E03C[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
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
        case 11:
        case 12:
            work->halo_count = 1;
            work->burst_count = 128;
            work->spark_count = 32;
            break;
        case 6:
            work->halo_count = 2;
            work->burst_count = 128;
            work->spark_count = 32;
            break;
        }
        work->radius = D_8013E090[command].radius;
        work->height = D_8013E090[command].height;
        for (i = 0; i < work->halo_count; i++) {
            work->halos[i].active = 0;
            work->halos[i].elapsed = 0;
            work->halos[i].duration = 48;
            work->halos[i].mode = 0;
            work->halos[i].coord = Model_GetCameraCoordinateUnit();
            work->halos[i].color[0] = D_8013E090[command].color[0];
            work->halos[i].color[1] = D_8013E090[command].color[1];
            work->halos[i].color[2] = D_8013E090[command].color[2];
            work->halos[i].u = 0;
            work->halos[i].v = 0;
            setVector(&work->halos[i].scale, 4096, 4096, 4096);
            setVector(&work->halos[i].unknown_3C, 0, 0, 0);
            setVector(&work->halos[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->halos[i].position, 0, 150, 0);
                break;
            case 1:
                setVector(&work->halos[i].position, 0, 100, work->direction * 100);
                break;
            case 3:
            case 10:
                setVector(&work->halos[i].position, 0, -50, 0);
                break;
            case 8:
            case 12:
                setVector(&work->halos[i].position, 0, 50, 0);
                break;
            default:
                setVector(&work->halos[i].position, 0, 0, 0);
                break;
            }
            work->halos[i].position.vy -= 350;
            work->halos[i].position.vz += work->direction * 450;
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 64;
            work->bursts[i].color[0] = D_8013E090[command].color[3];
            work->bursts[i].color[1] = D_8013E090[command].color[4];
            work->bursts[i].color[2] = D_8013E090[command].color[5];
            sign = (i % 2) * 2 - 1;
            duration = sign * ((i % 10) * 16 + 160);
            value = sign * ((i % 9) * 160 / 9 + 160);
            phase = (i % 64) * 64;
            angle = i << 12;
            /* Adding the quarter turn through a variable keeps it out of the
             * phase sum, which the original adds last. */
            quarter = 1024;
            setVector(&work->bursts[i].start, value * rsin(phase + angle / work->burst_count) / 4096,
                duration * rcos(phase + angle / work->burst_count) / 4096, 1000);
            setVector(&work->bursts[i].end, value * rsin(phase + (angle / work->burst_count + quarter)) / 4096,
                duration * rcos(phase + (angle / work->burst_count + quarter)) / 4096, 100);
        }
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].duration = 64;
            work->sparks[i].color[0] = D_8013E090[command].color[3];
            work->sparks[i].color[1] = D_8013E090[command].color[4];
            work->sparks[i].color[2] = D_8013E090[command].color[5];
            work->sparks[i].u = 0;
            work->sparks[i].v = 64;
            setVector(&work->sparks[i].end, 0, 0, 100);
            setVector(&work->sparks[i].start, 0, 0, 2000);
        }
        work->flash.active = 0;
        work->flash.elapsed = 0;
        work->flash.duration = D_8013E090[command].emit_end + 64;
        work->flash.color[0] = D_8013E090[command].color[6];
        work->flash.color[1] = D_8013E090[command].color[7];
        work->flash.color[2] = D_8013E090[command].color[8];
        work->flash.u = 32;
        work->flash.v = 64;
        work->delay = D_8013E090[command].delay;
        for (i = 0; i < 6; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        setVector(&work->base, 0, 0, 0);
        setVector(&work->apex, 0, 0, work->height);
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], rsin(i * 256) * 320 / 4096, rcos(i * 256) * 320 / 4096, 0);
            setVector(&work->cone[i], rsin(i * 256) * 320 / 4096, rcos(i * 256) * 320 / 4096,
                work->height * 15);
        }
        value = 100;
        setVector(&work->flash_corners[0], -value, -value, 0);
        setVector(&work->flash_corners[1], value, -value, 0);
        setVector(&work->flash_corners[2], -value, value, 0);
        setVector(&work->flash_corners[3], value, value, 0);
        for (i = 0; i < 33; i++) {
            setVector(&work->inner[i], work->radius * rsin(i * 128) / 4096,
                work->radius * rcos(i * 128) / 4096, 0);
            setVector(&work->outer[i], work->radius * (rsin(i * 128) << 1) / 4096,
                work->radius * (rcos(i * 128) << 1) / 4096, 0);
            setVector(&work->middle[i], (work->inner[i].vx + work->outer[i].vx) / 2,
                (work->inner[i].vy + work->outer[i].vy) / 2,
                (work->inner[i].vz + work->outer[i].vz) / 2);
        }
        value = work->radius / 3;
        duration = work->radius * 4;
        setVector(&v[0], -value, 0, 0);
        setVector(&v[1], value, 0, 0);
        setVector(&v[2], 0, 0, duration);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                /* The cast keeps the -256 inside the subtracted term. */
                rot.vx = (i << 9) - (s16)((j % 4) * 128 - 256);
                rot.vy = (j << 9) - ((i % 5) << 9) / 5;
                rot.vz = j << 9;
                RotMatrix(&rot, &m);
                for (k = 0; k < 3; k++) {
                    PushMatrix();
                    ApplyMatrixSV(&m, &v[k], &work->shards[k][j][i]);
                    PopMatrix();
                }
            }
        }
    }
    if (work->fade == 0) {
        work->darken += step;
        if (D_8013E090[work->command].fade_duration < work->darken) {
            work->darken = D_8013E090[work->command].fade_duration;
            work->fade = 1;
        }
        value = 2048 - ((work->darken * 3) << 10) / D_8013E090[work->command].fade_duration;
        if (value < -1024) {
            value = -1024;
        }
        Model_SetSlotAmbient(2, value, value, value);
    }
    if (work->fade == 1) {
        Model_SetSlotAmbient(2, -1024, -1024, -1024);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed >= 243 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 1:
        if (work->elapsed >= 67 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 2:
        if (work->elapsed >= 141 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 3:
        if (work->elapsed >= 53 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 4:
        if (work->elapsed >= 143 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 5:
        if (work->elapsed >= 141 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 6:
        if (work->elapsed >= 59 && work->flags[2] == 0) {
            result = 4;
            work->halos[0].active = 1;
            work->halos[0].duration = 24;
            work->flags[2] = 1;
        }
        if (work->elapsed >= 81 && work->flags[5] == 0) {
            work->halos[1].active = 1;
            work->flags[5] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 7:
        if (work->elapsed >= 143 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 8:
        if (work->elapsed >= 141 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 9:
        if (work->elapsed >= 47 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 10:
        if (work->elapsed >= 101 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 11:
        if (work->elapsed >= 77 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    case 12:
        if (work->elapsed >= 207 && work->flags[2] == 0) {
            for (i = 0; i < work->halo_count; i++) {
                work->halos[i].active = 1;
            }
            work->flags[2] = 1;
            result = 1;
        }
        skip = 0;
        break;
    default:
        skip = 1;
        break;
    }
    if (skip == 0) {
        if (work->elapsed < D_8013E090[work->command].emit_end) {
            for (i = work->flags[0] * 4; i < (work->flags[0] + 1) * 4; i++) {
                work->bursts[i].active = 1;
                work->bursts[i].elapsed = 0;
                work->bursts[i].color[0] = D_8013E090[work->command].color[3];
                work->bursts[i].color[1] = D_8013E090[work->command].color[4];
                work->bursts[i].color[2] = D_8013E090[work->command].color[5];
            }
            work->flags[0]++;
            if ((work->burst_count >> 2) - 1 < work->flags[0]) {
                work->flags[0] = 0;
            }
        }
        if (work->elapsed < D_8013E090[work->command].emit_end) {
            if (work->flags[3] >= 25) {
                work->sparks[work->flags[1]].active = 1;
                work->sparks[work->flags[1]].elapsed = 0;
                work->sparks[work->flags[1]].color[0] = D_8013E090[work->command].color[3];
                work->sparks[work->flags[1]].color[1] = D_8013E090[work->command].color[4];
                work->sparks[work->flags[1]].color[2] = D_8013E090[work->command].color[5];
                work->flags[1]++;
                if (work->spark_count - 1 < work->flags[1]) {
                    work->flags[1] = 0;
                }
                work->flags[3] = 0;
            }
            work->flags[3] += step;
        }
        if (work->elapsed == 0) {
            work->flash.active = 1;
        }
    }
    PushMatrix();
    view = *(Model658ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->halo_count; i++) {
        if (work->halos[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            /* An if/else, not a preset 0x1400, keeps the original order of
             * the hoisted &unit and &unit.matrix spill slots. */
            if (work->halos[i].elapsed < work->halos[i].duration) {
                value = (work->halos[i].elapsed << 12) / work->halos[i].duration + 0x400;
            } else {
                value = 0x1400;
            }
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
                duration = work->halos[i].duration;
                work->halos[i].color[0] = D_8013E090[work->command].color[0] * value / duration;
                work->halos[i].color[1] = D_8013E090[work->command].color[1] * value / duration;
                work->halos[i].color[2] = D_8013E090[work->command].color[2] * value / duration;
            }
            setPolyFT4(&work->sprite);
            work->sprite.tpage = work->tpage[0];
            work->sprite.clut = work->clut[0];
            setSemiTrans(&work->sprite, 1);
            setUV4(&work->sprite, work->halos[i].u, work->halos[i].v,
                work->halos[i].u + 63, work->halos[i].v,
                work->halos[i].u, work->halos[i].v + 63,
                work->halos[i].u + 63, work->halos[i].v + 63);
            setRGB0(&work->sprite, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
            depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
            }
            for (j = 0; j < 32; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->halos[i].u, work->halos[i].v + 64,
                    work->halos[i].u, work->halos[i].v + 128,
                    work->halos[i].u + 31, work->halos[i].v + 64,
                    work->halos[i].u + 31, work->halos[i].v + 128);
                setRGB0(&work->strip, 0, 0, 0);
                setRGB2(&work->strip, 0, 0, 0);
                setRGB1(&work->strip, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                setRGB3(&work->strip, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                depth = RotAverage4(&work->inner[j], &work->middle[j], &work->inner[j + 1], &work->middle[j + 1],
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->halos[i].u, work->halos[i].v + 128,
                    work->halos[i].u, work->halos[i].v + 191,
                    work->halos[i].u + 31, work->halos[i].v + 128,
                    work->halos[i].u + 31, work->halos[i].v + 191);
                setRGB1(&work->strip, 0, 0, 0);
                setRGB3(&work->strip, 0, 0, 0);
                setRGB0(&work->strip, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                setRGB2(&work->strip, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                depth = RotAverage4(&work->middle[j], &work->outer[j], &work->middle[j + 1], &work->outer[j + 1],
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
            }
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->halos[i].scale);
            RotMatrixZ(work->halos[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->halos[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->halos[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->halos[i].position.vx;
            unit.matrix.t[1] = work->halos[i].position.vy;
            unit.matrix.t[2] = work->halos[i].position.vz;
            unit.super = work->halos[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            for (k = 0; k < 8; k++) {
                for (j = 0; j < 8; j++) {
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[0];
                    work->shard.clut = work->clut[0];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->halos[i].u + 64, work->halos[i].v,
                        work->halos[i].u + 127, work->halos[i].v,
                        work->halos[i].u + 96, work->halos[i].v + 63);
                    setRGB2(&work->shard, 0, 0, 0);
                    setRGB0(&work->shard, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                    setRGB1(&work->shard, work->halos[i].color[0], work->halos[i].color[1], work->halos[i].color[2]);
                    depth = RotAverage3(&work->shards[0][j][k], &work->shards[1][j][k], &work->shards[2][j][k],
                        (PSXLONG *)&work->shard.x0, (PSXLONG *)&work->shard.x1,
                        (PSXLONG *)&work->shard.x2, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shard, func_80058F10(), (u16)depth);
                    }
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
    for (i = 0; i < work->burst_count; i++) {
        if (work->bursts[i].active == 1) {
            if (work->bursts[i].elapsed < work->bursts[i].duration / 2) {
                value = work->bursts[i].elapsed * 2;
                duration = work->bursts[i].duration;
                work->bursts[i].color[0] = D_8013E090[work->command].color[3] * value / duration;
                work->bursts[i].color[1] = D_8013E090[work->command].color[4] * value / duration;
                work->bursts[i].color[2] = D_8013E090[work->command].color[5] * value / duration;
            }
            work->line.attribute = 0x50000000;
            work->line.r0 = work->bursts[i].color[0];
            work->line.g0 = work->bursts[i].color[1];
            work->line.b0 = work->bursts[i].color[2];
            work->line.r1 = 0;
            work->line.g1 = 0;
            work->line.b1 = 0;
            duration = work->bursts[i].start.vz + work->apex.vz;
            value = work->bursts[i].start.vz + work->base.vz
                + (work->bursts[i].end.vz - work->bursts[i].start.vz) * work->bursts[i].elapsed
                    / work->bursts[i].duration;
            if (value != 0 && duration != 0) {
                x[0] = work->bursts[i].start.vx * work->projection / value;
                y[0] = work->bursts[i].start.vy * work->projection / value;
                x[1] = work->bursts[i].start.vx * work->projection / duration;
                y[1] = work->bursts[i].start.vy * work->projection / duration;
                work->line.x0 = x[0] + 160;
                work->line.y0 = y[0] + 120;
                work->line.x1 = x[1] + 160;
                work->line.y1 = y[1] + 120;
                GsSortGLine(&work->line, func_80058F10(), 0xFFD);
                work->line.attribute = 0x50000000;
                work->line.r0 = work->bursts[i].color[0];
                work->line.g0 = work->bursts[i].color[1];
                work->line.b0 = work->bursts[i].color[2];
                work->line.r1 = 0;
                work->line.g1 = 0;
                work->line.b1 = 0;
                /* Mirrored copy; "* -1" keeps the full-word load of x. */
                work->line.x0 = x[0] * -1 + 160;
                work->line.y0 = y[0] + 120;
                work->line.x1 = x[1] * -1 + 160;
                work->line.y1 = y[1] + 120;
                GsSortGLine(&work->line, func_80058F10(), 0xFFD);
                x[0] = work->bursts[i].end.vx * work->projection / value;
                y[0] = work->bursts[i].end.vy * work->projection / value;
                x[1] = work->bursts[i].end.vx * work->projection / duration;
                y[1] = work->bursts[i].end.vy * work->projection / duration;
                work->line.x0 = x[0] + 160;
                work->line.y0 = y[0] + 120;
                work->line.x1 = x[1] + 160;
                work->line.y1 = y[1] + 120;
                GsSortGLine(&work->line, func_80058F10(), 0xFFD);
                work->line.attribute = 0x50000000;
                work->line.r0 = work->bursts[i].color[0];
                work->line.g0 = work->bursts[i].color[1];
                work->line.b0 = work->bursts[i].color[2];
                work->line.r1 = 0;
                work->line.g1 = 0;
                work->line.b1 = 0;
                work->line.x0 = x[0] * -1 + 160;
                work->line.y0 = y[0] + 120;
                work->line.x1 = x[1] * -1 + 160;
                work->line.y1 = y[1] + 120;
                GsSortGLine(&work->line, func_80058F10(), 0xFFD);
            }
            work->bursts[i].elapsed += step;
            if (work->bursts[i].duration < work->bursts[i].elapsed) {
                work->bursts[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->spark_count; i++) {
        if (work->sparks[i].active == 1) {
            if (work->sparks[i].elapsed < work->sparks[i].duration / 2) {
                value = work->sparks[i].elapsed * 2;
                duration = work->sparks[i].duration;
                work->sparks[i].color[0] = D_8013E090[work->command].color[3] * value / duration;
                work->sparks[i].color[1] = D_8013E090[work->command].color[4] * value / duration;
                work->sparks[i].color[2] = D_8013E090[work->command].color[5] * value / duration;
            }
            for (j = 0; j < 16; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->sparks[i].u, work->sparks[i].v,
                    work->sparks[i].u, work->sparks[i].v + 127,
                    work->sparks[i].u + 31, work->sparks[i].v,
                    work->sparks[i].u + 31, work->sparks[i].v + 127);
                setRGB0(&work->strip, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                setRGB2(&work->strip, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                setRGB1(&work->strip, 0, 0, 0);
                setRGB3(&work->strip, 0, 0, 0);
                value = work->sparks[i].start.vz
                    + (work->sparks[i].end.vz - work->sparks[i].start.vz) * work->sparks[i].elapsed
                        / work->sparks[i].duration;
                z[0] = work->ring[j].vz + value;
                z[1] = work->cone[j].vz + value;
                z[2] = work->ring[j + 1].vz + value;
                z[3] = work->cone[j + 1].vz + value;
                if (z[0] != 0 && z[1] != 0 && z[2] != 0 && z[3] != 0) {
                    work->strip.x0 = (work->sparks[i].start.vx + work->ring[j].vx) * work->projection / z[0] + 160;
                    work->strip.y0 = (work->sparks[i].start.vy + work->ring[j].vy) * work->projection / z[0] + 120;
                    work->strip.x1 = (work->sparks[i].start.vx + work->cone[j].vx) * work->projection / z[1] + 160;
                    work->strip.y1 = (work->sparks[i].start.vy + work->cone[j].vy) * work->projection / z[1] + 120;
                    work->strip.x2 = (work->sparks[i].start.vx + work->ring[j + 1].vx) * work->projection / z[2]
                        + 160;
                    work->strip.y2 = (work->sparks[i].start.vy + work->ring[j + 1].vy) * work->projection / z[2]
                        + 120;
                    work->strip.x3 = (work->sparks[i].start.vx + work->cone[j + 1].vx) * work->projection / z[3]
                        + 160;
                    work->strip.y3 = (work->sparks[i].start.vy + work->cone[j + 1].vy) * work->projection / z[3]
                        + 120;
                    GsSortPoly(&work->strip, func_80058F10(), 0xFFC);
                }
            }
            work->sparks[i].elapsed += step;
            if (work->sparks[i].duration < work->sparks[i].elapsed) {
                work->sparks[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    if (work->flash.active == 1) {
        value = 1;
        duration = 1;
        if (work->flash.elapsed < (u16)(work->flash.duration / 5)) {
            duration = work->flash.duration;
            value = work->flash.elapsed * 5;
        }
        if (work->flash.elapsed > work->flash.duration * 9 / 10) {
            value = (work->flash.duration - work->flash.elapsed) * 10;
            duration = work->flash.duration;
        }
        work->flash.color[0] = D_8013E090[work->command].color[6] * value / duration;
        work->flash.color[1] = D_8013E090[work->command].color[7] * value / duration;
        work->flash.color[2] = D_8013E090[work->command].color[8] * value / duration;
        setPolyFT4(&work->sprite);
        work->sprite.tpage = work->tpage[2];
        work->sprite.clut = work->clut[2];
        setSemiTrans(&work->sprite, 1);
        setUV4(&work->sprite, work->flash.u, work->flash.v,
            work->flash.u + 127, work->flash.v,
            work->flash.u, work->flash.v + 127,
            work->flash.u + 127, work->flash.v + 127);
        setRGB0(&work->sprite, work->flash.color[0], work->flash.color[1], work->flash.color[2]);
        m = D_800FE168;
        RotMatrixZ(work->flash.elapsed * 10, &m);
        for (i = 0; i < 4; i++) {
            PushMatrix();
            ApplyMatrixSV(&m, &work->flash_corners[i], &v[i]);
            PopMatrix();
        }
        work->sprite.x0 = v[0].vx + 160;
        work->sprite.y0 = v[0].vy + 120;
        work->sprite.x1 = v[1].vx + 160;
        work->sprite.y1 = v[1].vy + 120;
        work->sprite.x2 = v[2].vx + 160;
        work->sprite.y2 = v[2].vy + 120;
        work->sprite.x3 = v[3].vx + 160;
        work->sprite.y3 = v[3].vy + 120;
        GsSortPoly(&work->sprite, func_80058F10(), 0xFFE);
        work->flash.elapsed += step;
        if (work->flash.duration < work->flash.elapsed) {
            work->flash.active = 0;
        }
    } else {
        inactive++;
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->halo_count + work->burst_count + work->spark_count + 1) {
        if (work->flags[4] == 1) {
            result = 2;
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
        }
        if (work->fade == 1) {
            work->brighten += step;
            if (D_8013E090[work->command].fade_duration < work->brighten) {
                work->brighten = D_8013E090[work->command].fade_duration;
                work->fade = 2;
                work->flags[4] = 1;
            }
            value = ((work->brighten * 3) << 10) / D_8013E090[work->command].fade_duration - 1024;
            if (value > 2048) {
                value = 2048;
            }
            Model_SetSlotAmbient(2, value, value, value);
        }
    }
    return result;
}

#include "../../types.h"
#include "variant649_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 649's entry: a ring of up to 64 fading streaks spins between two
 * model joints, a textured dome opens over the other slot, an orb flies from
 * it to the midpoint of two more joints, and on its arrival a burst of
 * sparks follows while 32 spinning shards fly back. The opposing slot is
 * tinted while the streaks fade and its camera orbit is queued and restored.
 * The setup also prints the work-area size. */

/* Halving through an int parameter keeps the arithmetic shift of retail's
 * corner abscissas. */
static inline s32 Model649_NegHalf(s32 value)
{
    return -(value >> 1);
}

s32 func_8013B004(u8 *context, s32 command)
{
    Model649State *work;
    MATRIX ls, lw2, lw1;
    GsCOORDUNIT unit;
    Model649ViewSnapshot view;
    SVECTOR offset;
    VECTOR a, b, c, d, mid;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, value, den;
    u32 tex;
    PSXLONG depth;
    s32 red, green, edge_red;
    /* A byte, as retail truncates the blue shades before storing them. */
    u8 blue;
    u8 *bit;

    work = (Model649State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model649State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model649State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->target = work->slot == 0;
        Model_CopySlotPosition(work->target, (u16 *)&work->position);
        work->tint = 0x80;
        tex = ModelTexture_PackPageClutCopy(work->slot, 3, &D_8013DAAC[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAAC[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAAC[2]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAAC[0]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        if (command == 0) {
            work->streak_count = 64;
            work->shard_count = 32;
            work->spark_count = 32;
        }
        work->radius = D_8013DB00[command].radius;
        work->height = D_8013DB00[command].height;
        for (i = 0; i < work->streak_count; i++) {
            work->streaks[i].duration = 260;
            work->streaks[i].active = 0;
            work->streaks[i].elapsed = 0;
            work->streaks[i].mode = 1;
            work->streaks[i].coord = Model_GetCameraCoordinateUnit();
            work->streaks[i].color[0] = D_8013DB00[command].color[0];
            work->streaks[i].color[1] = D_8013DB00[command].color[1];
            work->streaks[i].color[2] = D_8013DB00[command].color[2];
            work->streaks[i].u = 0;
            work->streaks[i].v = 0x60;
            setVector(&work->streaks[i].scale, 4096, 4096, 4096);
            setVector(&work->streaks[i].unknown_3C, 4096, 4096, 4096);
            value = (i % 2) * 2 - 1;
            setVector(&work->streaks[i].rotation, value * (i << 12) / work->streak_count,
                (i << 12) / work->streak_count + work->slot * 2048, value * (i << 12) / work->streak_count);
            setVector(&work->streaks[i].position, 0, 0, 0);
        }
        work->canopy.active = 0;
        work->canopy.elapsed = 0;
        work->canopy.duration = 260;
        work->canopy.mode = 1;
        work->canopy.coord = Model_GetCameraCoordinateUnit();
        work->canopy.color[0] = D_8013DB00[command].color[3];
        work->canopy.color[1] = D_8013DB00[command].color[4];
        work->canopy.color[2] = D_8013DB00[command].color[5];
        work->canopy.v = 0x20;
        work->canopy.u = 0;
        setVector(&work->canopy.scale, 4096, 4096, 4096);
        setVector(&work->canopy.unknown_3C, 4096, 4096, 4096);
        setVector(&work->canopy.rotation, 0, work->slot * 2048, 0);
        setVector(&work->canopy.position, 0, 0, 0);
        work->orb.active = 0;
        work->orb.elapsed = 0;
        work->orb.duration = 400;
        work->orb.mode = 1;
        work->orb.coord = Model_GetCameraCoordinateUnit();
        work->orb.color[0] = D_8013DB00[command].color[0];
        work->orb.color[1] = D_8013DB00[command].color[1];
        work->orb.color[2] = D_8013DB00[command].color[2];
        work->orb.u = 0;
        work->orb.v = 0;
        setVector(&work->orb.scale, 0, 0, 0);
        setVector(&work->orb.unknown_3C, 0, 0, 0);
        setVector(&work->orb.rotation, 0, work->slot * 2048, 0);
        setVector(&work->orb.position, work->position.vx, work->position.vy, work->position.vz);
        for (i = 0; i < work->shard_count; i++) {
            work->shards[i].duration = 400;
            work->shards[i].active = 0;
            work->shards[i].elapsed = 0;
            work->shards[i].mode = 1;
            work->shards[i].coord = Model_GetCameraCoordinateUnit();
            work->shards[i].color[0] = D_8013DB00[command].color[0];
            work->shards[i].color[1] = D_8013DB00[command].color[1];
            work->shards[i].color[2] = D_8013DB00[command].color[2];
            work->shards[i].u = 0;
            work->shards[i].v = 0x60;
            setVector(&work->shards[i].scale, 0, 0, 0);
            setVector(&work->shards[i].unknown_3C, 0, 0, 0);
            value = (i % 2) * 2 - 1;
            setVector(&work->shards[i].rotation, value * (i << 12) / work->shard_count,
                (i << 12) / work->shard_count + work->slot * 2048, value * (i << 12) / work->shard_count);
            setVector(&work->shards[i].position, work->position.vx, work->position.vy, work->position.vz);
        }
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i].duration = 32;
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].color[0] = D_8013DB00[command].color[0];
            work->sparks[i].color[1] = D_8013DB00[command].color[1];
            work->sparks[i].color[2] = D_8013DB00[command].color[2];
            work->sparks[i].u = 0;
            work->sparks[i].v = 0x60;
            setVector(&work->sparks[i].scale, 4096, 4096, 4096);
            setVector(&work->sparks[i].unknown_3C, 4096, 4096, 4096);
            value = (i % 2) * 2 - 1;
            setVector(&work->sparks[i].rotation, value * ((i % 4) << 12) / 4,
                value * (i << 12) / work->spark_count + work->slot * 2048,
                (i << 10) / work->spark_count + 0x200);
            setVector(&work->sparks[i].position, 0, 0, 0);
        }
        work->delay = D_8013DB00[command].delay;
        /* Retail walks a byte pointer down from work + i and stores at the
         * flags offset: the strength-reduced form of flags[i] = 0. */
        for (i = 4, bit = (u8 *)work + i; i >= 0; i--) {
            bit[work->flags - (u8 *)work] = 0;
            bit--;
        }
        /* Field by field in this order: retail's radius loads and copies follow it. */
        work->corners[0].vz = -work->radius / 10;
        work->corners[1].vz = -work->radius / 5;
        work->corners[2].vz = work->radius / 10;
        work->corners[3].vz = work->radius / 5;
        work->corners[0].vy = 0;
        work->corners[1].vy = 0;
        work->corners[2].vy = 0;
        work->corners[3].vy = 0;
        work->corners[1].vx = work->radius;
        work->corners[0].vx = Model649_NegHalf(work->radius);
        work->corners[2].vx = Model649_NegHalf(work->radius);
        work->corners[3].vx = work->radius;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                setVector(&work->dome[j][i],
                    ((work->radius / 2) + work->radius * rsin(i * 64) / 4096) * rsin(j * 512) / 4096,
                    ((work->radius >> 2) + (work->radius >> 1) * rsin(i * 64) / 4096) * rcos(j * 512) / 4096,
                    work->height * -i / 8);
            }
        }
        setVector(&work->quad[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->quad[1], work->radius >> 1, -work->radius / 2, 0);
        setVector(&work->quad[2], -work->radius / 2, work->radius >> 1, 0);
        setVector(&work->quad[3], work->radius >> 1, work->radius >> 1, 0);
        setVector(&work->triangle[0], 0, 0, 0);
        setVector(&work->triangle[1], work->radius >> 1, work->radius * 3, 0);
        setVector(&work->triangle[2], -work->radius / 2, work->radius * 3, 0);
        setVector(&work->fan[0], work->radius * 3, 0, 0);
        setVector(&work->fan[1], 0, work->radius / 10, 0);
        setVector(&work->fan[2], 0, -work->radius / 10, 0);
        setVector(&work->fan[3], -(work->radius * 3), 0, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    if (work->command == 0) {
        if (work->elapsed == 0) {
            for (i = 0; i < work->streak_count; i++) {
                work->streaks[i].active = 1;
            }
            work->canopy.active = 1;
            work->orb.active = 1;
            for (i = 0; i < work->shard_count; i++) {
                work->shards[i].active = 1;
            }
            Model_SetSlotPlaybackDirection(work->slot, 0);
        }
        if (work->elapsed >= 266) {
            Model_SetSlotPlaybackDirection(work->slot, 1);
        }
    }
    PushMatrix();
    view = *(Model649ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
    GsGetLwUnit(&unit, &lw1);
    setVector(&a, lw1.t[0], lw1.t[1], lw1.t[2]);
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 4);
    GsGetLwUnit(&unit, &lw2);
    setVector(&b, lw2.t[0], lw2.t[1], lw2.t[2]);
    for (i = 0; i < work->streak_count; i++) {
        if (work->streaks[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->streaks[i].elapsed < work->streaks[i].duration / 10) {
                /* In-place divisions keep the numerator in retail's register. */
                value = work->streaks[i].elapsed * 40960;
                value /= work->streaks[i].duration;
                setVector(&work->streaks[i].scale, value, value, value);
            }
            ScaleMatrix(&unit.matrix, &work->streaks[i].scale);
            RotMatrixZ(work->streaks[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->streaks[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->streaks[i].rotation.vy, &unit.matrix);
            PushMatrix();
            lw2 = unit.matrix;
            MulMatrix0(&lw1, &lw2, &unit.matrix);
            PopMatrix();
            unit.matrix.t[0] = work->streaks[i].position.vx + a.vx + (b.vx - a.vx) * (i + 1) / work->streak_count;
            unit.matrix.t[1] = work->streaks[i].position.vy + a.vy + (b.vy - a.vy) * (i + 1) / work->streak_count;
            unit.matrix.t[2] = work->streaks[i].position.vz + a.vz + (b.vz - a.vz) * (i + 1) / work->streak_count;
            unit.super = work->streaks[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = ((i % 2) * 2 - 1) * (i % 16 + 10) * step;
            work->streaks[i].rotation.vx += value * 3;
            work->streaks[i].rotation.vy += value * 4;
            work->streaks[i].rotation.vz += value * 5;
            if (work->streaks[i].duration * 9 / 10 < work->streaks[i].elapsed) {
                value = (work->streaks[i].duration - work->streaks[i].elapsed) * 10;
                den = work->streaks[i].duration;
                work->streaks[i].color[0] = D_8013DB00[work->command].color[0] * value / den;
                work->streaks[i].color[1] = D_8013DB00[work->command].color[1] * value / den;
                work->streaks[i].color[2] = D_8013DB00[work->command].color[2] * value / den;
            }
            setPolyGT4(&work->strip);
            work->strip.tpage = work->tpage[0];
            work->strip.clut = work->clut[0];
            setSemiTrans(&work->strip, 1);
            setUV4(&work->strip, work->streaks[i].u, work->streaks[i].v,
                work->streaks[i].u, work->streaks[i].v + 127,
                work->streaks[i].u + 31, work->streaks[i].v,
                work->streaks[i].u + 31, work->streaks[i].v + 127);
            setRGB1(&work->strip, 0, 0, 0);
            setRGB3(&work->strip, 0, 0, 0);
            setRGB0(&work->strip, work->streaks[i].color[0], work->streaks[i].color[1], work->streaks[i].color[2]);
            setRGB2(&work->strip, work->streaks[i].color[0], work->streaks[i].color[1], work->streaks[i].color[2]);
            depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
            }
            if (work->streaks[i].duration / 2 < work->streaks[i].elapsed) {
                work->tint = ((work->streaks[i].duration - work->streaks[i].elapsed) << 8) / work->streaks[i].duration;
                Model_SetSlotTintTarget(work->target, 5, work->tint, work->tint, work->tint);
            }
            work->streaks[i].elapsed += step;
            if (work->streaks[i].duration < work->streaks[i].elapsed) {
                work->streaks[i].active = 0;
                Model_SetSlotTintTarget(work->target, 5, 0, 0, 0);
            }
        } else {
            inactive++;
        }
    }
    if (work->canopy.active == 1) {
        unit.flg = 0;
        unit.matrix = D_800FE168;
        if (work->canopy.elapsed < work->canopy.duration / 4) {
            value = (work->canopy.elapsed << 14);
            value /= work->canopy.duration;
            setVector(&work->canopy.scale, value, value, value);
        }
        ScaleMatrix(&unit.matrix, &work->canopy.scale);
        RotMatrixX(ratan2(work->position.vy - (b.vy + a.vy) / 2, work->position.vz - (b.vz + a.vz) / 2),
            &unit.matrix);
        RotMatrixY(work->canopy.rotation.vy
                + ratan2(work->position.vx - (b.vx + a.vx) / 2, work->position.vz - (b.vz + a.vz) / 2),
            &unit.matrix);
        unit.matrix.t[0] = (b.vx + a.vx) / 2;
        unit.matrix.t[1] = (b.vy + a.vy) / 2;
        unit.matrix.t[2] = (b.vz + a.vz) / 2;
        unit.super = work->canopy.coord;
        GsGetLsUnit(&unit, &ls);
        GsSetLsMatrix(&ls);
        if (work->canopy.duration * 9 / 10 < work->canopy.elapsed) {
            value = (work->canopy.duration - work->canopy.elapsed) * 10;
            den = work->canopy.duration;
            work->canopy.color[0] = D_8013DB00[work->command].color[3] * value / den;
            work->canopy.color[1] = D_8013DB00[work->command].color[4] * value / den;
            work->canopy.color[2] = D_8013DB00[work->command].color[5] * value / den;
        }
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                /* The original adds 254 to the byte coordinate (u - 2 modulo 256). */
                setUV4(&work->strip, work->canopy.u, work->canopy.v + 32,
                    work->canopy.u + 254, work->canopy.v + 32,
                    work->canopy.u, work->canopy.v,
                    work->canopy.u + 254, work->canopy.v);
                red = work->canopy.color[0] * (8 - i) / 8;
                green = work->canopy.color[1] * (8 - i) / 8;
                blue = work->canopy.color[2] * (8 - i) / 8;
                if (i == 0) {
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                } else {
                    setRGB0(&work->strip, red, green, blue);
                    setRGB1(&work->strip, red, green, blue);
                }
                edge_red = work->canopy.color[0] * (7 - i) / 8;
                green = work->canopy.color[1] * (7 - i) / 8;
                blue = work->canopy.color[2] * (7 - i) / 8;
                setRGB2(&work->strip, edge_red, green, blue);
                setRGB3(&work->strip, edge_red, green, blue);
                depth = RotAverage4(&work->dome[j][i], &work->dome[j + 1][i], &work->dome[j][i + 1],
                    &work->dome[j + 1][i + 1],
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
            }
        }
        work->canopy.v += step * 4;
        if (work->canopy.v >= 0x40) {
            work->canopy.v = 0x20;
        }
        work->canopy.elapsed += step;
        if (work->canopy.duration < work->canopy.elapsed) {
            work->canopy.active = 0;
        }
    } else {
        inactive++;
    }
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
    GsGetLwUnit(&unit, &ls);
    setVector(&c, ls.t[0], ls.t[1], ls.t[2]);
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 2);
    GsGetLwUnit(&unit, &ls);
    setVector(&d, ls.t[0], ls.t[1], ls.t[2]);
    setVector(&mid, (c.vx + d.vx) / 2, (c.vy + d.vy) / 2, (c.vz + d.vz) / 2 - work->direction * 100);
    if (work->orb.active == 1) {
        unit.flg = 0;
        unit.matrix = D_800FE168;
        if (work->orb.elapsed < work->orb.duration * 3 / 4) {
            value = (work->orb.elapsed << 14);
            value /= (work->orb.duration * 3);
            setVector(&work->orb.scale, value, value, value);
        }
        if (work->orb.duration * 9 / 10 < work->orb.elapsed) {
            value = (work->orb.duration - work->orb.elapsed) * 40960;
            value /= work->orb.duration;
            setVector(&work->orb.scale, value, value, value);
        }
        ScaleMatrix(&unit.matrix, &work->orb.scale);
        RotMatrixZ(work->orb.rotation.vz, &unit.matrix);
        RotMatrixX(work->orb.rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
        RotMatrixY(work->orb.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
        if (work->orb.elapsed < work->orb.duration * 3 / 4) {
            unit.matrix.t[0] = work->orb.position.vx;
            unit.matrix.t[1] = work->orb.position.vy;
            unit.matrix.t[2] = work->orb.position.vz;
        } else {
            if (work->flags[0] == 0) {
                setVector(&offset, 0, 0, 0);
                /* Built in `value` so it takes retail's a3, the orbit argument register. */
                value = work->orb.duration * 2;
                value += work->orb.duration;
                Model_QueueCameraOrbitFromPreset(work->slot, 1, &offset, value / 20);
                work->flags[0] = 1;
            }
            if (work->orb.elapsed < work->orb.duration * 9 / 10) {
                value = -(work->orb.elapsed * 20) + work->orb.duration * 9 * 2;
                den = work->orb.duration * 3;
            } else {
                value = 0;
                den = 1;
            }
            unit.matrix.t[0] = mid.vx + (work->orb.position.vx - mid.vx) * value / den;
            unit.matrix.t[1] = mid.vy + (work->orb.position.vy - mid.vy) * value / den;
            unit.matrix.t[2] = mid.vz + (work->orb.position.vz - mid.vz) * value / den;
        }
        unit.super = work->orb.coord;
        GsGetLsUnit(&unit, &ls);
        GsSetLsMatrix(&ls);
        setPolyFT4(&work->sprite);
        work->sprite.tpage = work->tpage[2];
        work->sprite.clut = work->clut[2];
        setSemiTrans(&work->sprite, 1);
        setRGB0(&work->sprite, work->orb.color[0], work->orb.color[1], work->orb.color[2]);
        setUV4(&work->sprite, work->orb.u, work->orb.v,
            work->orb.u + 31, work->orb.v,
            work->orb.u, work->orb.v + 31,
            work->orb.u + 31, work->orb.v + 31);
        depth = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3],
            (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
            &interpolation, &flag);
        if (flag >= 0) {
            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
        }
        work->orb.u += 32;
        if (work->orb.u == 0x60) {
            work->orb.u = 0;
        }
        work->orb.elapsed += step;
        if (work->orb.duration < work->orb.elapsed) {
            work->orb.active = 0;
            Model_SetSlotTintTarget(work->target, 0, 0x80, 0x80, 0x80);
            for (j = 0; j < work->spark_count; j++) {
                work->sparks[j].active = 1;
                setVector(&work->sparks[j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
            }
        }
    } else {
        inactive++;
    }
    for (i = 0; i < work->shard_count; i++) {
        if (work->shards[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->shards[i].elapsed < work->shards[i].duration * 3 / 4) {
                value = ((i % 8) * 292 + 0x800) * (work->shards[i].elapsed << 2);
                value /= (work->shards[i].duration * 3);
                setVector(&work->shards[i].scale, value, value, value);
            }
            if (work->shards[i].elapsed > work->shards[i].duration * 9 / 10) {
                value = (work->shards[i].duration - work->shards[i].elapsed) * 40960;
                value /= work->shards[i].duration;
                setVector(&work->shards[i].scale, value, value, value);
            }
            ScaleMatrix(&unit.matrix, &work->shards[i].scale);
            RotMatrixZ(work->shards[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->shards[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->shards[i].rotation.vy, &unit.matrix);
            if (work->shards[i].elapsed < work->shards[i].duration * 3 / 4) {
                unit.matrix.t[0] = work->shards[i].position.vx;
                unit.matrix.t[1] = work->shards[i].position.vy;
                unit.matrix.t[2] = work->shards[i].position.vz;
            } else {
                if (work->shards[i].elapsed < work->shards[i].duration * 9 / 10) {
                    value = -(work->shards[i].elapsed * 20) + work->shards[i].duration * 9 * 2;
                    den = work->shards[i].duration * 3;
                } else {
                    value = 0;
                    den = 1;
                }
                unit.matrix.t[0] = mid.vx + (work->shards[i].position.vx - mid.vx) * value / den;
                unit.matrix.t[1] = mid.vy + (work->shards[i].position.vy - mid.vy) * value / den;
                unit.matrix.t[2] = mid.vz + (work->shards[i].position.vz - mid.vz) * value / den;
            }
            unit.super = work->shards[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = ((i % 2) * 2 - 1) * (i % 8 + 20) * step;
            work->shards[i].rotation.vx += value * 3;
            work->shards[i].rotation.vy += value * 4;
            work->shards[i].rotation.vz += value * 5;
            setPolyGT3(&work->shard);
            work->shard.tpage = work->tpage[3];
            work->shard.clut = work->clut[3];
            setSemiTrans(&work->shard, 1);
            setUV3(&work->shard, work->shards[i].u + 16, work->shards[i].v,
                work->shards[i].u, work->shards[i].v + 127,
                work->shards[i].u + 31, work->shards[i].v + 127);
            setRGB1(&work->shard, 0, 0, 0);
            setRGB2(&work->shard, 0, 0, 0);
            setRGB0(&work->shard, work->shards[i].color[0], work->shards[i].color[1], work->shards[i].color[2]);
            depth = RotAverage3(&work->triangle[0], &work->triangle[1], &work->triangle[2],
                (PSXLONG *)&work->shard.x0, (PSXLONG *)&work->shard.x1, (PSXLONG *)&work->shard.x2,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->shard, func_80058F10(), (u16)depth);
            }
            work->shards[i].elapsed += step;
            if (work->shards[i].duration < work->shards[i].elapsed) {
                work->shards[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->spark_count; i++) {
        if (work->sparks[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = (work->sparks[i].elapsed << 12) / work->sparks[i].duration + 0x400;
            setVector(&work->sparks[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
            RotMatrixX(work->sparks[i].rotation.vx, &unit.matrix);
            RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
            RotMatrixY(work->sparks[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->sparks[i].position.vx;
            unit.matrix.t[1] = work->sparks[i].position.vy;
            unit.matrix.t[2] = work->sparks[i].position.vz;
            unit.super = work->sparks[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->sparks[i].duration * 4 / 5 < work->sparks[i].elapsed) {
                value = (work->sparks[i].duration - work->sparks[i].elapsed) * 5;
                den = work->sparks[i].duration;
                work->sparks[i].color[0] = D_8013DB00[work->command].color[0] * value / den;
                work->sparks[i].color[1] = D_8013DB00[work->command].color[1] * value / den;
                work->sparks[i].color[2] = D_8013DB00[work->command].color[2] * value / den;
            }
            for (j = 0; j < 2; j++) {
                setPolyGT3(&work->shard);
                work->shard.tpage = work->tpage[0];
                work->shard.clut = work->clut[0];
                setSemiTrans(&work->shard, 1);
                setUV3(&work->shard, work->sparks[i].u + 16, work->sparks[i].v,
                    work->sparks[i].u, work->sparks[i].v + 64,
                    work->sparks[i].u + 31, work->sparks[i].v + 64);
                setRGB0(&work->shard, 0, 0, 0);
                setRGB1(&work->shard, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                setRGB2(&work->shard, work->sparks[i].color[0], work->sparks[i].color[1], work->sparks[i].color[2]);
                depth = RotAverage3(&work->fan[j * 3], &work->fan[1], &work->fan[2],
                    (PSXLONG *)&work->shard.x0, (PSXLONG *)&work->shard.x1, (PSXLONG *)&work->shard.x2,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->shard, func_80058F10(), (u16)depth);
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
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->streak_count + 2 + work->shard_count + work->spark_count) {
        if (work->flags[4] == 20) {
            result = 1;
        }
        if (work->flags[4] == 21) {
            result = 2;
            Model_SetSlotTintTarget(work->target, 0, 0x80, 0x80, 0x80);
            setVector(&offset, 0, 0, 0);
            Model_SetCameraOrbitFromPreset(work->target, 0, &offset);
        }
        work->flags[4]++;
    }
    return result;
}

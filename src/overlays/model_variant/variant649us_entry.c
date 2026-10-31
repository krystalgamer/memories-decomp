#include "../../types.h"
#include "variant649us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 649's entry: spinning petals spread along the line between two
 * joints while a textured dome grows over the model; a card then flies from
 * the model to the midpoint of two other joints, shards spiral after it and
 * sparks burst where it lands. The setup also prints the work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL649US_STRING_PAD
#define MODEL649US_STRING_PAD ""
#endif
static const char D_8013DA94[24] = "E_DATA size = %x byte\n\0" MODEL649US_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model649UsState *work;
    MATRIX ls, m, lw;
    GsCOORDUNIT unit;
    Model649UsViewSnapshot view;
    SVECTOR zero;
    VECTOR near, far, first, second, mid;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, temp, den;
    u32 tex;
    PSXLONG depth;
    u8 red, green, blue;

    work = (Model649UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model649UsState));
#ifndef VERSION_FRENCH
        printf(D_8013DA94, sizeof(Model649UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->side = work->slot * 2 - 1;
        work->elapsed = 0;
        work->other = work->slot == 0;
        Model_CopySlotPosition(work->other, (u16 *)&work->anchor);
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
        switch (command) {
        case 0:
            work->petal_count = 0x40;
            work->shard_count = 0x20;
            work->spark_count = 0x20;
            break;
        }
        work->radius = D_8013DB00[command].radius;
        work->height = D_8013DB00[command].height;
        for (i = 0; i < work->petal_count; i++) {
            work->petals[i].duration = 0x104;
            work->petals[i].active = 0;
            work->petals[i].elapsed = 0;
            work->petals[i].mode = 1;
            work->petals[i].coord = Model_GetCameraCoordinateUnit();
            work->petals[i].red = D_8013DB00[command].color[0];
            work->petals[i].green = D_8013DB00[command].color[1];
            temp = i % 2 * 2 - 1;
            work->petals[i].blue = D_8013DB00[command].color[2];
            work->petals[i].u = 0;
            work->petals[i].v = 0x60;
            setVector(&work->petals[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->petals[i].growth, 0x1000, 0x1000, 0x1000);
            setVector(&work->petals[i].rotation, temp * (i << 12) / work->petal_count,
                (i << 12) / work->petal_count + work->slot * 2048,
                temp * (i << 12) / work->petal_count);
            setVector(&work->petals[i].position, 0, 0, 0);
        }
        work->dome_element.active = 0;
        work->dome_element.elapsed = 0;
        work->dome_element.duration = 0x104;
        work->dome_element.mode = 1;
        work->dome_element.coord = Model_GetCameraCoordinateUnit();
        work->dome_element.red = D_8013DB00[command].color[3];
        work->dome_element.green = D_8013DB00[command].color[4];
        work->dome_element.blue = D_8013DB00[command].color[5];
        work->dome_element.v = 0x20;
        work->dome_element.u = 0;
        setVector(&work->dome_element.scale, 0x1000, 0x1000, 0x1000);
        setVector(&work->dome_element.growth, 0x1000, 0x1000, 0x1000);
        setVector(&work->dome_element.rotation, 0, work->slot * 2048, 0);
        setVector(&work->dome_element.position, 0, 0, 0);
        work->card_element.active = 0;
        work->card_element.elapsed = 0;
        work->card_element.duration = 0x190;
        work->card_element.mode = 1;
        work->card_element.coord = Model_GetCameraCoordinateUnit();
        work->card_element.red = D_8013DB00[command].color[0];
        work->card_element.green = D_8013DB00[command].color[1];
        work->card_element.blue = D_8013DB00[command].color[2];
        work->card_element.u = 0;
        work->card_element.v = 0;
        setVector(&work->card_element.scale, 0, 0, 0);
        setVector(&work->card_element.growth, 0, 0, 0);
        setVector(&work->card_element.rotation, 0, work->slot * 2048, 0);
        setVector(&work->card_element.position, work->anchor.vx, work->anchor.vy, work->anchor.vz);
        for (i = 0; i < work->shard_count; i++) {
            work->shards[i].duration = 0x190;
            work->shards[i].active = 0;
            work->shards[i].elapsed = 0;
            work->shards[i].mode = 1;
            work->shards[i].coord = Model_GetCameraCoordinateUnit();
            work->shards[i].red = D_8013DB00[command].color[0];
            work->shards[i].green = D_8013DB00[command].color[1];
            temp = i % 2 * 2 - 1;
            work->shards[i].blue = D_8013DB00[command].color[2];
            work->shards[i].u = 0;
            work->shards[i].v = 0x60;
            setVector(&work->shards[i].scale, 0, 0, 0);
            setVector(&work->shards[i].growth, 0, 0, 0);
            setVector(&work->shards[i].rotation, temp * (i << 12) / work->shard_count,
                (i << 12) / work->shard_count + work->slot * 2048,
                temp * (i << 12) / work->shard_count);
            setVector(&work->shards[i].position, work->anchor.vx, work->anchor.vy, work->anchor.vz);
        }
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i].duration = 0x20;
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].red = D_8013DB00[command].color[0];
            work->sparks[i].green = D_8013DB00[command].color[1];
            work->sparks[i].blue = D_8013DB00[command].color[2];
            work->sparks[i].u = 0;
            work->sparks[i].v = 0x60;
            setVector(&work->sparks[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i].growth, 0x1000, 0x1000, 0x1000);
            temp = i % 2 * 2 - 1;
            setVector(&work->sparks[i].rotation, temp * ((i % 4) << 12) / 4,
                temp * (i << 12) / work->spark_count + work->slot * 2048,
                (i << 10) / work->spark_count + 0x200);
            setVector(&work->sparks[i].position, 0, 0, 0);
        }
        work->delay = D_8013DB00[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->petal[0], work->radius / -2, 0, -work->radius / 10);
        setVector(&work->petal[1], work->radius, 0, -work->radius / 5);
        setVector(&work->petal[2], work->radius / -2, 0, work->radius / 10);
        setVector(&work->petal[3], work->radius, 0, work->radius / 5);
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                setVector(&work->dome[j][i],
                    (work->radius / 2 + work->radius * rsin(i << 6) / 4096) * rsin(j << 9) / 4096,
                    (work->radius / 4 + work->radius / 2 * rsin(i << 6) / 4096) * rcos(j << 9) / 4096,
                    work->height * -i / 8);
            }
        }
        setVector(&work->square[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->square[3], work->radius / 2, work->radius / 2, 0);
        setVector(&work->tri[0], 0, 0, 0);
        setVector(&work->tri[1], work->radius / 2, work->radius * 3, 0);
        setVector(&work->tri[2], -work->radius / 2, work->radius * 3, 0);
        setVector(&work->spike[0], work->radius * 3, 0, 0);
        setVector(&work->spike[1], 0, work->radius / 10, 0);
        setVector(&work->spike[2], 0, -work->radius / 10, 0);
        setVector(&work->spike[3], -(work->radius * 3), 0, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                for (i = 0; i < work->petal_count; i++) {
                    work->petals[i].active = 1;
                }
                work->dome_element.active = 1;
                work->card_element.active = 1;
                for (i = 0; i < work->shard_count; i++) {
                    work->shards[i].active = 1;
                }
                Model_SetSlotPlaybackDirection(work->slot, 0);
            }
            if (work->elapsed >= 0x10A) {
                Model_SetSlotPlaybackDirection(work->slot, 1);
            }
            break;
        }
        PushMatrix();
        view = *(Model649UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 3);
        GsGetLwUnit(&unit, &lw);
        setVector(&near, lw.t[0], lw.t[1], lw.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 4);
        GsGetLwUnit(&unit, &m);
        setVector(&far, m.t[0], m.t[1], m.t[2]);
        for (i = 0; i < work->petal_count; i++) {
            if (work->petals[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->petals[i].elapsed < work->petals[i].duration / 10) {
                    temp = work->petals[i].elapsed * 10 << 12;
                    den = work->petals[i].duration;
                    temp /= den;
                    setVector(&work->petals[i].scale, temp, temp, temp);
                }
                ScaleMatrix(&unit.matrix, &work->petals[i].scale);
                RotMatrixZ(work->petals[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->petals[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->petals[i].rotation.vy, &unit.matrix);
                PushMatrix();
                m = unit.matrix;
                MulMatrix0(&lw, &m, &unit.matrix);
                PopMatrix();
                unit.matrix.t[0] = work->petals[i].position.vx + near.vx
                    + (far.vx - near.vx) * (i + 1) / work->petal_count;
                unit.matrix.t[1] = work->petals[i].position.vy + near.vy
                    + (far.vy - near.vy) * (i + 1) / work->petal_count;
                unit.matrix.t[2] = work->petals[i].position.vz + near.vz
                    + (far.vz - near.vz) * (i + 1) / work->petal_count;
                unit.super = work->petals[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                temp = (i % 2 * 2 - 1) * (i % 16 + 10) * step;
                work->petals[i].rotation.vx += temp * 3;
                work->petals[i].rotation.vy += temp * 4;
                work->petals[i].rotation.vz += temp * 5;
                if (work->petals[i].duration * 9 / 10 < work->petals[i].elapsed) {
                    temp = (work->petals[i].duration - work->petals[i].elapsed) * 10;
                    den = work->petals[i].duration;
                    work->petals[i].red = D_8013DB00[work->command].color[0] * temp / den;
                    work->petals[i].green = D_8013DB00[work->command].color[1] * temp / den;
                    work->petals[i].blue = D_8013DB00[work->command].color[2] * temp / den;
                }
                setPolyGT4(&work->quad);
                work->quad.tpage = work->tpage[0];
                work->quad.clut = work->clut[0];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->petals[i].u, work->petals[i].v, work->petals[i].u,
                    work->petals[i].v + 0x7F, work->petals[i].u + 0x1F, work->petals[i].v,
                    work->petals[i].u + 0x1F, work->petals[i].v + 0x7F);
                setRGB1(&work->quad, 0, 0, 0);
                setRGB3(&work->quad, 0, 0, 0);
                setRGB0(&work->quad, work->petals[i].red, work->petals[i].green, work->petals[i].blue);
                setRGB2(&work->quad, work->petals[i].red, work->petals[i].green, work->petals[i].blue);
                depth = RotAverage4(&work->petal[0], &work->petal[1], &work->petal[2], &work->petal[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1, (PSXLONG *)&work->quad.x2,
                    (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                if (work->petals[i].duration / 2 < work->petals[i].elapsed) {
                    work->tint = ((work->petals[i].duration - work->petals[i].elapsed) << 8)
                        / work->petals[i].duration;
                    Model_SetSlotTintTarget(work->other, 5, work->tint, work->tint, work->tint);
                }
                work->petals[i].elapsed += step;
                if (work->petals[i].duration < work->petals[i].elapsed) {
                    work->petals[i].active = 0;
                    Model_SetSlotTintTarget(work->other, 5, 0, 0, 0);
                }
            } else {
                inactive++;
            }
        }
        if (work->dome_element.active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            if (work->dome_element.elapsed < work->dome_element.duration / 4) {
                temp = work->dome_element.elapsed << 14;
                den = work->dome_element.duration;
                temp /= den;
                work->dome_element.scale.vx = temp;
                work->dome_element.scale.vy = temp;
                work->dome_element.scale.vz = temp;
            }
            ScaleMatrix(&unit.matrix, &work->dome_element.scale);
            RotMatrixX(ratan2(work->anchor.vy - (far.vy + near.vy) / 2,
                work->anchor.vz - (far.vz + near.vz) / 2), &unit.matrix);
            RotMatrixY(work->dome_element.rotation.vy
                + ratan2(work->anchor.vx - (far.vx + near.vx) / 2,
                    work->anchor.vz - (far.vz + near.vz) / 2), &unit.matrix);
            unit.matrix.t[0] = (far.vx + near.vx) / 2;
            unit.matrix.t[1] = (far.vy + near.vy) / 2;
            unit.matrix.t[2] = (far.vz + near.vz) / 2;
            unit.super = work->dome_element.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->dome_element.duration * 9 / 10 < work->dome_element.elapsed) {
                temp = (work->dome_element.duration - work->dome_element.elapsed) * 10;
                den = work->dome_element.duration;
                work->dome_element.red = D_8013DB00[work->command].color[3] * temp / den;
                work->dome_element.green = D_8013DB00[work->command].color[4] * temp / den;
                work->dome_element.blue = D_8013DB00[work->command].color[5] * temp / den;
            }
            for (i = 0; i < 8; i++) {
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->tpage[1];
                    work->quad.clut = work->clut[1];
                    setSemiTrans(&work->quad, 1);
                    setUV4(&work->quad, work->dome_element.u, work->dome_element.v + 0x20,
                        work->dome_element.u + 0xFE, work->dome_element.v + 0x20, work->dome_element.u,
                        work->dome_element.v, work->dome_element.u + 0xFE, work->dome_element.v);
                    red = work->dome_element.red * (8 - i) / 8;
                    green = work->dome_element.green * (8 - i) / 8;
                    blue = work->dome_element.blue * (8 - i) / 8;
                    if (i == 0) {
                        setRGB0(&work->quad, 0, 0, 0);
                        setRGB1(&work->quad, 0, 0, 0);
                    } else {
                        setRGB0(&work->quad, red, green, blue);
                        setRGB1(&work->quad, red, green, blue);
                    }
                    red = work->dome_element.red * (7 - i) / 8;
                    green = work->dome_element.green * (7 - i) / 8;
                    blue = work->dome_element.blue * (7 - i) / 8;
                    setRGB2(&work->quad, red, green, blue);
                    setRGB3(&work->quad, red, green, blue);
                    depth = RotAverage4(&work->dome[j][i], &work->dome[j + 1][i], &work->dome[j][i + 1],
                        &work->dome[j + 1][i + 1], (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->dome_element.v += step * 4;
            if (work->dome_element.v >= 0x40) {
                work->dome_element.v = 0x20;
            }
            work->dome_element.elapsed += step;
            if (work->dome_element.duration < work->dome_element.elapsed) {
                work->dome_element.active = 0;
            }
        } else {
            inactive++;
        }
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 1);
        GsGetLwUnit(&unit, &ls);
        setVector(&first, ls.t[0], ls.t[1], ls.t[2]);
        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 2);
        GsGetLwUnit(&unit, &ls);
        setVector(&second, ls.t[0], ls.t[1], ls.t[2]);
        setVector(&mid, (first.vx + second.vx) / 2, (first.vy + second.vy) / 2,
            (first.vz + second.vz) / 2 - work->side * 100);
        if (work->card_element.active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            den = work->card_element.duration * 3;
            if (work->card_element.elapsed < den / 4) {
                temp = work->card_element.elapsed << 14;
                temp /= den;
                setVector(&work->card_element.scale, temp, temp, temp);
            }
            if (work->card_element.duration * 9 / 10 < work->card_element.elapsed) {
                temp = (work->card_element.duration - work->card_element.elapsed) * 10 << 12;
                den = work->card_element.duration;
                temp /= den;
                setVector(&work->card_element.scale, temp, temp, temp);
            }
            ScaleMatrix(&unit.matrix, &work->card_element.scale);
            RotMatrixZ(work->card_element.rotation.vz, &unit.matrix);
            RotMatrixX(work->card_element.rotation.vx - (view.pitch - 0x1000) * work->side, &unit.matrix);
            RotMatrixY(work->card_element.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            if (work->card_element.elapsed < work->card_element.duration * 3 / 4) {
                unit.matrix.t[0] = work->card_element.position.vx;
                unit.matrix.t[1] = work->card_element.position.vy;
                unit.matrix.t[2] = work->card_element.position.vz;
            } else {
                if (work->flags[0] == 0) {
                    setVector(&zero, 0, 0, 0);
                    Model_QueueCameraOrbitFromPreset(work->slot, 1, &zero,
                        work->card_element.duration * 3 / 20);
                    work->flags[0] = 1;
                }
                if (work->card_element.elapsed < work->card_element.duration * 9 / 10) {
                    temp = -(work->card_element.elapsed * 20) + work->card_element.duration * 9 * 2;
                    den = work->card_element.duration * 3;
                } else {
                    temp = 0;
                    den = 1;
                }
                unit.matrix.t[0] = mid.vx + (work->card_element.position.vx - mid.vx) * temp / den;
                unit.matrix.t[1] = mid.vy + (work->card_element.position.vy - mid.vy) * temp / den;
                unit.matrix.t[2] = mid.vz + (work->card_element.position.vz - mid.vz) * temp / den;
            }
            unit.super = work->card_element.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            setPolyFT4(&work->card);
            work->card.tpage = work->tpage[2];
            work->card.clut = work->clut[2];
            setSemiTrans(&work->card, 1);
            setRGB0(&work->card, work->card_element.red, work->card_element.green, work->card_element.blue);
            setUV4(&work->card, work->card_element.u, work->card_element.v, work->card_element.u + 0x1F,
                work->card_element.v, work->card_element.u, work->card_element.v + 0x1F,
                work->card_element.u + 0x1F, work->card_element.v + 0x1F);
            depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                (PSXLONG *)&work->card.x0, (PSXLONG *)&work->card.x1, (PSXLONG *)&work->card.x2,
                (PSXLONG *)&work->card.x3, &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->card, func_80058F10(), (u16)depth);
            }
            work->card_element.u += 0x20;
            if (work->card_element.u == 0x60) {
                work->card_element.u = 0;
            }
            work->card_element.elapsed += step;
            if (work->card_element.duration < work->card_element.elapsed) {
                work->card_element.active = 0;
                Model_SetSlotTintTarget(work->other, 0, 0x80, 0x80, 0x80);
                for (j = 0; j < work->spark_count; j++) {
                    work->sparks[j].active = 1;
                    setVector(&work->sparks[j].position, unit.matrix.t[0], unit.matrix.t[1],
                        unit.matrix.t[2]);
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
                    temp = ((i % 8) * 0x124 + 0x800) * (work->shards[i].elapsed << 2);
                    den = work->shards[i].duration * 3;
                    temp /= den;
                    setVector(&work->shards[i].scale, temp, temp, temp);
                }
                if (work->shards[i].duration * 9 / 10 < work->shards[i].elapsed) {
                    temp = (work->shards[i].duration - work->shards[i].elapsed) * 10 << 12;
                    den = work->shards[i].duration;
                    temp /= den;
                    setVector(&work->shards[i].scale, temp, temp, temp);
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
                        temp = -(work->shards[i].elapsed * 20) + work->shards[i].duration * 9 * 2;
                        den = work->shards[i].duration * 3;
                    } else {
                        temp = 0;
                        den = 1;
                    }
                    unit.matrix.t[0] = mid.vx + (work->shards[i].position.vx - mid.vx) * temp / den;
                    unit.matrix.t[1] = mid.vy + (work->shards[i].position.vy - mid.vy) * temp / den;
                    unit.matrix.t[2] = mid.vz + (work->shards[i].position.vz - mid.vz) * temp / den;
                }
                unit.super = work->shards[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                temp = (i % 2 * 2 - 1) * (i % 8 + 20) * step;
                work->shards[i].rotation.vx += temp * 3;
                work->shards[i].rotation.vy += temp * 4;
                work->shards[i].rotation.vz += temp * 5;
                setPolyGT3(&work->shard);
                work->shard.tpage = work->tpage[3];
                work->shard.clut = work->clut[3];
                setSemiTrans(&work->shard, 1);
                setUV3(&work->shard, work->shards[i].u + 0x10, work->shards[i].v, work->shards[i].u,
                    work->shards[i].v + 0x7F, work->shards[i].u + 0x1F, work->shards[i].v + 0x7F);
                setRGB1(&work->shard, 0, 0, 0);
                setRGB2(&work->shard, 0, 0, 0);
                setRGB0(&work->shard, work->shards[i].red, work->shards[i].green, work->shards[i].blue);
                depth = RotAverage3(&work->tri[0], &work->tri[1], &work->tri[2], (PSXLONG *)&work->shard.x0,
                    (PSXLONG *)&work->shard.x1, (PSXLONG *)&work->shard.x2, &interpolation, &flag);
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
                temp = (work->sparks[i].elapsed << 12) / work->sparks[i].duration + 0x400;
                setVector(&work->sparks[i].scale, temp, temp, temp);
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
                    temp = (work->sparks[i].duration - work->sparks[i].elapsed) * 5;
                    den = work->sparks[i].duration;
                    work->sparks[i].red = D_8013DB00[work->command].color[0] * temp / den;
                    work->sparks[i].green = D_8013DB00[work->command].color[1] * temp / den;
                    work->sparks[i].blue = D_8013DB00[work->command].color[2] * temp / den;
                }
                for (j = 0; j < 2; j++) {
                    setPolyGT3(&work->shard);
                    work->shard.tpage = work->tpage[0];
                    work->shard.clut = work->clut[0];
                    setSemiTrans(&work->shard, 1);
                    setUV3(&work->shard, work->sparks[i].u + 0x10, work->sparks[i].v, work->sparks[i].u,
                        work->sparks[i].v + 0x40, work->sparks[i].u + 0x1F, work->sparks[i].v + 0x40);
                    setRGB0(&work->shard, 0, 0, 0);
                    setRGB1(&work->shard, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    setRGB2(&work->shard, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    depth = RotAverage3(&work->spike[j * 3], &work->spike[1], &work->spike[2],
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
        if (inactive == work->petal_count + 1 + 1 + work->shard_count + work->spark_count) {
            if (work->flags[4] == 0x14) {
                result = 1;
            }
            if (work->flags[4] == 0x15) {
                result = 2;
                Model_SetSlotTintTarget(work->other, 0, 0x80, 0x80, 0x80);
                setVector(&zero, 0, 0, 0);
                Model_SetCameraOrbitFromPreset(work->other, 0, &zero);
            }
            work->flags[4]++;
        }
    }
    return result;
}

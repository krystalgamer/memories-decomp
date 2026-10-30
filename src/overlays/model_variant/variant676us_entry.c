#include "../../types.h"
#include "variant676us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 676's entry. Command 0 flashes a sprite and a fan of shaded
 * triangles on a model point, then scatters a ring of shards and fires a
 * burst of spinning ribbons that settles into a ring of strips, a tumbling
 * lattice and an expanding wave; command 1 starts the ring and the wave at
 * a fixed offset. The setup also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator is
 * 0x8C. */
#ifndef MODEL676US_STRING_PAD
#define MODEL676US_STRING_PAD "\x8C"
#endif
#ifndef VERSION_FRENCH
static const char D_8013E478[24] = "E_DATA size = %x byte\n\0" MODEL676US_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model676UsState *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model676UsViewSnapshot view;
    SVECTOR points[2];
    SVECTOR *pt;
    u8 u[3], v[3];
    u8 r[2], g[2], b[2];
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, i, j, step, inactive;
    s32 k, n, d, s, t;
    s32 shade, delta;
    /* Reused scratch values; sharing them with the colour copies fixes the
       registers of the corner negations. */
    s32 tmp;
    s32 tmp2;
    u32 tex;
    PSXLONG depth;

    work = (Model676UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model676UsState));
#ifndef VERSION_FRENCH
        printf(D_8013E478, sizeof(Model676UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[3]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[0]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[5]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[2]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013E490[4]);
        work->tpage[4] = tex >> 16;
        work->clut[4] = tex;
        switch (command) {
        case 0:
            work->flash_count = 1;
            work->shard_count = 32;
            work->burst_count = 1;
            work->spoke_count = 16;
            break;
        case 1:
            work->flash_count = 0;
            work->shard_count = 0;
            work->burst_count = 1;
            work->spoke_count = 0;
            break;
        }
        work->radius = D_8013E538[command].radius;
        work->height = D_8013E538[command].height;
        for (i = 0; i < work->flash_count; i++) {
            work->flashes[i].active = 0;
            work->flashes[i].elapsed = 0;
            switch (command) {
            case 0:
                work->flashes[i].duration = 0xA0;
                break;
            }
            work->flashes[i].mode = 1;
            work->flashes[i].coord = Model_GetCameraCoordinateUnit();
            work->flashes[i].color[0] = D_8013E538[command].color[0];
            work->flashes[i].color[1] = D_8013E538[command].color[1];
            tmp2 = D_8013E538[command].color[2];
            work->flashes[i].color[2] = tmp2;
            work->flashes[i].u = 0x40;
            work->flashes[i].v = 0x20;
            setVector(&work->flashes[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->flashes[i].target, 0, 0, 0);
            setVector(&work->flashes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->flashes[i].position, 0, 0, 0);
            work->flashes[i].ticks = 0;
        }
        for (i = 0; i < work->shard_count; i++) {
            work->shards[i].duration = 0x40;
            work->shards[i].active = 0;
            work->shards[i].elapsed = 0;
            work->shards[i].mode = 1;
            work->shards[i].coord = Model_GetCameraCoordinateUnit();
            work->shards[i].color[0] = D_8013E538[command].color[0];
            work->shards[i].color[1] = D_8013E538[command].color[1];
            work->shards[i].color[2] = D_8013E538[command].color[2];
            work->shards[i].u = 0;
            work->shards[i].v = 0;
            setVector(&work->shards[i].scale, 0x1000, 0x1000, 0x1000);
            d = i % 2 * 2 - 1;
            n = work->radius * 2;
            PushMatrix();
            m = D_800FE168;
            RotMatrixZ(d * (i << 11) / work->shard_count, &m);
            RotMatrixY(d * (i % 16 << 12) / 16, &m);
            RotMatrixX(i % 16 << 8, &m);
            setVector(&offset, n, 0, 0);
            ApplyMatrixLV(&m, &offset, &work->shards[i].target);
            PopMatrix();
            setVector(&work->shards[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->shards[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].duration = 0x60;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013E538[command].color[3];
            work->bursts[i].color[1] = D_8013E538[command].color[4];
            work->bursts[i].color[2] = D_8013E538[command].color[5];
            work->bursts[i].u = 0x40;
            work->bursts[i].v = 0x20;
            setVector(&work->bursts[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->bursts[i].target, 0, -350, work->direction * 450);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i].position, 0, 0, 0);
            work->bursts[i].ticks = 0;
        }
        for (i = 0; i < work->burst_count; i++) {
            work->rings[i].duration = 0x80;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013E538[command].color[6];
            work->rings[i].color[1] = D_8013E538[command].color[7];
            tmp = D_8013E538[command].color[8];
            work->rings[i].color[2] = tmp;
            work->rings[i].u = 0;
            work->rings[i].v = 0x20;
            setVector(&work->rings[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].target, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->burst_count; i++) {
            work->waves[i].duration = 0x30;
            work->waves[i].active = 0;
            work->waves[i].elapsed = 0;
            work->waves[i].mode = 1;
            work->waves[i].coord = Model_GetCameraCoordinateUnit();
            work->waves[i].color[0] = D_8013E538[command].color[6];
            work->waves[i].color[1] = D_8013E538[command].color[7];
            work->waves[i].color[2] = D_8013E538[command].color[8];
            work->waves[i].u = 0;
            work->waves[i].v = 0x80;
            setVector(&work->waves[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->waves[i].target, 0, 0, 0);
            setVector(&work->waves[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->waves[i].position, 0, 0, 0);
        }
        work->delay = D_8013E538[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->radius;
        setVector(&work->corners[0], -n, -n, 0);
        setVector(&work->corners[1], n, -n, 0);
        setVector(&work->corners[2], -n, n, 0);
        setVector(&work->corners[3], n, n, 0);
        n = work->radius * 2;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
        }
        setVector(&work->centre, 0, 0, 0);
        n = work->radius / 3;
        /* The negative corner halves are stored after the radius reload. */
        tmp2 = -n;
        work->shard_corners[0].vz = 0;
        work->shard_corners[1].vz = 0;
        work->shard_corners[2].vz = 0;
        work->shard_corners[3].vz = 0;
        work->shard_corners[1].vx = n;
        work->shard_corners[2].vy = n;
        work->shard_corners[3].vx = n;
        work->shard_corners[3].vy = n;
        n = work->radius;
        d = work->radius / 10;
        tmp = -n;
        work->shard_corners[0].vx = tmp2;
        work->shard_corners[0].vy = tmp2;
        work->shard_corners[1].vy = tmp2;
        work->shard_corners[2].vx = tmp2;
        work->burst_corners[0].vz = 0;
        work->burst_corners[1].vz = 0;
        work->burst_corners[2].vz = 0;
        work->burst_corners[3].vz = 0;
        work->burst_corners[1].vx = n;
        work->burst_corners[2].vy = n;
        work->burst_corners[3].vx = n;
        work->burst_corners[3].vy = n;
        work->burst_corners[0].vx = tmp;
        work->burst_corners[0].vy = tmp;
        work->burst_corners[1].vy = tmp;
        work->burst_corners[2].vx = tmp;
        for (i = 0; i < 17; i++) {
            setVector(&work->arc_lower[i], n * rsin(i << 7) / 4096, d * -rsin(i << 7) / 4096,
                n * rcos(i << 7) / 4096);
            setVector(&work->arc_upper[i], n * rsin(i << 7) / 4096, d * rsin(i << 7) / 4096,
                n * rcos(i << 7) / 4096);
        }
        n = work->radius;
        d = work->height;
        setVector(&work->ring_corners[0], -n, -n, 0);
        setVector(&work->ring_corners[1], n, -n, 0);
        setVector(&work->ring_corners[2], -n, n, 0);
        setVector(&work->ring_corners[3], n, n, 0);
        n = work->radius / 5;
        for (j = 0; j < 8; j++) {
            for (i = 0; i < 8; i++) {
                for (k = 0; k < 5; k++) {
                    points[0].vx = k * d / 4;
                    points[0].vy = n * rcos(k << 8) / 4096 + (s = (k % 2 * 2 - 1) * n) * rcos(k << 8) / 4096;
                    points[0].vz = 0;
                    points[1].vx = k * d / 4;
                    points[1].vy = n * -rcos(k << 8) / 4096 + s * rcos(k << 8) / 4096;
                    points[1].vz = 0;
                    PushMatrix();
                    m = D_800FE168;
                    RotMatrixX(k << 10, &m);
                    s = i % 2 * 2 - 1;
                    RotMatrixZ(s * (i << 12) / 8, &m);
                    RotMatrixX(s * (i << 12) / 8, &m);
                    RotMatrixY(s * (i << 12) / 8, &m);
                    RotMatrixY(d + (j << 9), &m);
                    RotMatrixZ(d + s * -((j << 9) + j * 568), &m);
                    RotMatrixX(d + s * (j << 12) / 8 + j * 682, &m);
                    t = 0;
                    do {
                        ApplyMatrixSV(&m, &points[t], &work->lattice[t][k][i][j]);
                    } while (++t < 2);
                    PopMatrix();
                }
            }
        }
        n = work->radius;
        d = n * 2;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_inner[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
            setVector(&work->ring_outer[i], d * rsin(i << 8) / 4096, d * rcos(i << 8) / 4096, 0);
        }
        n = work->radius * 5 / 4;
        d = work->radius * 2;
        for (i = 0; i < 17; i++) {
            setVector(&work->wave_inner[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
            setVector(&work->wave_outer[i], d * rsin(i << 8) / 4096, d * rcos(i << 8) / 4096, 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                work->flashes[0].active = 1;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0xF);
            GsGetLwUnit(&unit, &ls);
            setVector(&work->flashes[0].position, ls.t[0], ls.t[1], ls.t[2]);
            break;
        case 1:
            if (work->elapsed == 0) {
                work->rings[0].active = 1;
                work->waves[0].active = 1;
                setVector(&work->rings[0].position, 0, -530, work->direction * 600);
                work->waves[0].position = work->rings[0].position;
            }
            break;
        }
        PushMatrix();
        view = *(Model676UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->flash_count; i++) {
            if (work->flashes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->flashes[i].elapsed < work->flashes[i].duration / 2) {
                    n = (work->flashes[i].elapsed << 13) / work->flashes[i].duration;
                } else {
                    n = 0x1000;
                }
                if ((work->flashes[i].ticks & 1) == 0) {
                    n /= 2;
                }
                setVector(&work->flashes[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->flashes[i].scale);
                RotMatrixZ(work->flashes[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->flashes[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->flashes[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->flashes[i].position.vx;
                unit.matrix.t[1] = work->flashes[i].position.vy;
                unit.matrix.t[2] = work->flashes[i].position.vz;
                unit.super = work->flashes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->flashes[i].u, work->flashes[i].v, work->flashes[i].u + 63,
                    work->flashes[i].v, work->flashes[i].u, work->flashes[i].v + 63,
                    work->flashes[i].u + 63, work->flashes[i].v + 63);
                setRGB0(&work->sprite, work->flashes[i].color[0], work->flashes[i].color[1],
                    work->flashes[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->flashes[i].elapsed < work->flashes[i].duration / 2) {
                    n = (work->flashes[i].elapsed << 13) / work->flashes[i].duration;
                } else {
                    n = 0x1000;
                }
                setVector(&work->flashes[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->flashes[i].scale);
                RotMatrixZ(work->flashes[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->flashes[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->flashes[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->flashes[i].position.vx;
                unit.matrix.t[1] = work->flashes[i].position.vy;
                unit.matrix.t[2] = work->flashes[i].position.vz;
                unit.super = work->flashes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                u[0] = 0x40;
                shade = (work->flashes[i].elapsed & 0x3F) * 63 / 64;
                v[0] = shade + 128;
                u[1] = 0x20;
                v[1] = shade + 192;
                u[2] = 0x5F;
                v[2] = shade + 192;
                for (k = 0; k < 16; k++) {
                    setPolyGT3(&work->fan);
                    work->fan.tpage = work->tpage[1];
                    work->fan.clut = work->clut[1];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, u[0], v[0], u[1], v[1], u[2], v[2]);
                    setRGB0(&work->fan, work->flashes[i].color[0], work->flashes[i].color[1],
                        work->flashes[i].color[2]);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&work->centre, &work->ring[k], &work->ring[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                }
                work->flashes[i].elapsed += step;
                work->flashes[i].ticks++;
                if (work->flashes[i].duration < work->flashes[i].elapsed) {
                    work->flashes[i].active = 0;
                    if (i == 0) {
                        unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0);
                        GsGetLwUnit(&unit, &ls);
                        for (k = 0; k < work->shard_count; k++) {
                            work->shards[k].active = 1;
                            setVector(&work->shards[k].position, ls.t[0], ls.t[1], ls.t[2]);
                        }
                        work->bursts[0].active = 1;
                        setVector(&work->bursts[0].position, ls.t[0], ls.t[1], ls.t[2]);
                    }
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->shard_count; i++) {
            if (work->shards[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->shards[i].scale);
                RotMatrixZ(work->shards[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->shards[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->shards[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->shards[i].position.vx
                    + work->shards[i].target.vx * work->shards[i].elapsed / work->shards[i].duration;
                unit.matrix.t[1] = work->shards[i].position.vy
                    + work->shards[i].target.vy * work->shards[i].elapsed / work->shards[i].duration;
                unit.matrix.t[2] = work->shards[i].position.vz
                    + work->shards[i].target.vz * work->shards[i].elapsed / work->shards[i].duration;
                unit.super = work->shards[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->shards[i].duration / 2 < work->shards[i].elapsed) {
                    n = (work->shards[i].duration - work->shards[i].elapsed) * 2;
                    d = work->shards[i].duration;
                    work->shards[i].color[0] = D_8013E538[work->command].color[0] * n / d;
                    work->shards[i].color[1] = D_8013E538[work->command].color[1] * n / d;
                    work->shards[i].color[2] = D_8013E538[work->command].color[2] * n / d;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[2];
                work->sprite.clut = work->clut[2];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->shards[i].u, work->shards[i].v, work->shards[i].u + 31,
                    work->shards[i].v, work->shards[i].u, work->shards[i].v + 31,
                    work->shards[i].u + 31, work->shards[i].v + 31);
                setRGB0(&work->sprite, work->shards[i].color[0], work->shards[i].color[1],
                    work->shards[i].color[2]);
                depth = RotAverage4(&work->shard_corners[0], &work->shard_corners[1],
                    &work->shard_corners[2], &work->shard_corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                if (work->shards[i].u < 0xE0) {
                    work->shards[i].u += 0x20;
                }
                work->shards[i].elapsed += step;
                if (work->shards[i].duration < work->shards[i].elapsed) {
                    work->shards[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->bursts[i].elapsed < work->bursts[i].duration / 8) {
                    n = (work->bursts[i].elapsed << 15) / work->bursts[i].duration;
                } else {
                    n = 0x1000;
                }
                if ((work->bursts[i].ticks & 1) == 0) {
                    n /= 2;
                }
                setVector(&work->bursts[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                if (work->bursts[i].elapsed < work->bursts[i].duration * 7 / 8) {
                    unit.matrix.t[0] = work->bursts[i].position.vx;
                    unit.matrix.t[1] = work->bursts[i].position.vy;
                    unit.matrix.t[2] = work->bursts[i].position.vz;
                } else {
                    d = -work->bursts[i].duration;
                    n = work->bursts[i].duration * 7 - work->bursts[i].elapsed * 8;
                    unit.matrix.t[0] = work->bursts[i].position.vx
                        + (work->bursts[i].target.vx - work->bursts[i].position.vx) * n / d;
                    unit.matrix.t[1] = work->bursts[i].position.vy
                        + (work->bursts[i].target.vy - work->bursts[i].position.vy) * n / d;
                    unit.matrix.t[2] = work->bursts[i].position.vz
                        + (work->bursts[i].target.vz - work->bursts[i].position.vz) * n / d;
                }
                setVector(&offset, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v, work->bursts[i].u + 63,
                    work->bursts[i].v, work->bursts[i].u, work->bursts[i].v + 63,
                    work->bursts[i].u + 63, work->bursts[i].v + 63);
                setRGB0(&work->sprite, work->bursts[i].color[0], work->bursts[i].color[1],
                    work->bursts[i].color[2]);
                depth = RotAverage4(&work->burst_corners[0], &work->burst_corners[1],
                    &work->burst_corners[2], &work->burst_corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                for (k = 0; k < work->spoke_count; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    if (work->bursts[i].elapsed < work->bursts[i].duration / 8) {
                        n = (work->bursts[i].elapsed << 15) / work->bursts[i].duration;
                    } else {
                        n = 0x1000;
                    }
                    d = n / 2 + n / 2 * rsin((k << 11) / work->spoke_count) / 4096;
                    setVector(&work->bursts[i].scale, d, d, d);
                    ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                    delta = k + 1;
                    RotMatrixY(work->bursts[i].elapsed * ((delta << 4) - delta) + (k << 12) / work->spoke_count,
                        &unit.matrix);
                    RotMatrixZ((k << 12) / work->spoke_count, &unit.matrix);
                    RotMatrixX((k << 12) / work->spoke_count, &unit.matrix);
                    unit.matrix.t[0] = offset.vx;
                    unit.matrix.t[1] = offset.vy;
                    unit.matrix.t[2] = offset.vz;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (t = 0, d = 8; t < 16; t++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[0];
                        work->strip.clut = work->clut[0];
                        setSemiTrans(&work->strip, 1);
                        u[0] = work->bursts[i].u + 64;
                        v[0] = work->bursts[i].v;
                        u[1] = work->bursts[i].u + 127;
                        v[1] = work->bursts[i].v + 63;
                        setUV4(&work->strip, u[0], v[0], u[1], v[0], u[0], v[1], u[1], v[1]);
                        {
                            s32 h = 8 - t;

                            n = 8 - (h >= 0 ? h : -h);
                        }
                        r[0] = work->bursts[i].color[0] * n / d;
                        g[0] = work->bursts[i].color[1] * n / d;
                        b[0] = work->bursts[i].color[2] * n / d;
                        {
                            s32 h = 7 - t;

                            n = d - (h >= 0 ? h : -h);
                        }
                        r[1] = work->bursts[i].color[0] * n / d;
                        g[1] = work->bursts[i].color[1] * n / d;
                        b[1] = work->bursts[i].color[2] * n / d;
                        setRGB0(&work->strip, r[0], g[0], b[0]);
                        setRGB1(&work->strip, r[0], g[0], b[0]);
                        setRGB2(&work->strip, r[1], g[1], b[1]);
                        setRGB3(&work->strip, r[1], g[1], b[1]);
                        depth = RotAverage4(&work->arc_lower[t], &work->arc_upper[t],
                            &work->arc_lower[t + 1], &work->arc_upper[t + 1],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->bursts[i].ticks++;
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                    work->rings[i].active = 1;
                    setVector(&work->rings[i].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    work->waves[i].active = 1;
                    work->waves[i].position = work->rings[i].position;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->rings[i].elapsed << 13) / work->rings[i].duration + 0x400;
                if ((work->rings[i].ticks & 1) == 0) {
                    n = (u32)n >> 1;
                }
                setVector(&work->rings[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                    n = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                    d = work->rings[i].duration;
                    work->rings[i].color[0] = D_8013E538[work->command].color[6] * n / d;
                    work->rings[i].color[1] = D_8013E538[work->command].color[7] * n / d;
                    work->rings[i].color[2] = D_8013E538[work->command].color[8] * n / d;
                }
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[3];
                work->sprite.clut = work->clut[3];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->rings[i].u, work->rings[i].v, work->rings[i].u + 63,
                    work->rings[i].v, work->rings[i].u, work->rings[i].v + 63,
                    work->rings[i].u + 63, work->rings[i].v + 63);
                setRGB0(&work->sprite, work->rings[i].color[0], work->rings[i].color[1],
                    work->rings[i].color[2]);
                depth = RotAverage4(&work->ring_corners[0], &work->ring_corners[1],
                    &work->ring_corners[2], &work->ring_corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[4];
                    work->strip.clut = work->clut[4];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->rings[i].color[0], work->rings[i].color[1],
                        work->rings[i].color[2]);
                    setRGB3(&work->strip, work->rings[i].color[0], work->rings[i].color[1],
                        work->rings[i].color[2]);
                    depth = RotAverage4(&work->ring_inner[k], &work->ring_inner[k + 1],
                        &work->ring_outer[k], &work->ring_outer[k + 1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].elapsed * 300, &unit.matrix);
                RotMatrixX(work->rings[i].elapsed * 350, &unit.matrix);
                RotMatrixY(work->rings[i].elapsed * 400, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 8; k++) {
                        for (t = 0; t < 4; t++) {
                            setPolyGT4(&work->strip);
                            setUV4(&work->strip, 0x80, 0x20, 0xC0, 0x20, 0x80, 0x5F, 0xBF, 0x5F);
                            work->strip.tpage = work->tpage[0];
                            work->strip.clut = work->clut[0];
                            setSemiTrans(&work->strip, 1);
                            r[0] = work->rings[i].color[0] * (4 - t) / 4;
                            g[0] = work->rings[i].color[1] * (4 - t) / 4;
                            b[0] = work->rings[i].color[2] * (4 - t) / 4;
                            r[1] = work->rings[i].color[0] * (3 - t) / 4;
                            g[1] = work->rings[i].color[1] * (3 - t) / 4;
                            b[1] = work->rings[i].color[2] * (3 - t) / 4;
                            setRGB0(&work->strip, r[0], g[0], b[0]);
                            setRGB1(&work->strip, r[0], g[0], b[0]);
                            setRGB2(&work->strip, r[1], g[1], b[1]);
                            setRGB3(&work->strip, r[1], g[1], b[1]);
                            depth = RotAverage4(&work->lattice[0][t][k][j], &work->lattice[1][t][k][j],
                                &work->lattice[0][t + 1][k][j], &work->lattice[1][t + 1][k][j],
                                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                }
                work->rings[i].ticks++;
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->waves[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = work->waves[i].elapsed * 0x3000 / work->waves[i].duration + 0x1000;
                if ((work->waves[i].ticks & 1) == 0) {
                    n = (u32)n >> 1;
                }
                setVector(&work->waves[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->waves[i].scale);
                RotMatrixZ(work->waves[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->waves[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->waves[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->waves[i].position.vx;
                unit.matrix.t[1] = work->waves[i].position.vy;
                unit.matrix.t[2] = work->waves[i].position.vz;
                unit.super = work->waves[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->waves[i].duration / 2 < work->waves[i].elapsed) {
                    n = (work->waves[i].duration - work->waves[i].elapsed) * 2;
                    d = work->waves[i].duration;
                    work->waves[i].color[0] = D_8013E538[work->command].color[6] * n / d;
                    work->waves[i].color[1] = D_8013E538[work->command].color[7] * n / d;
                    work->waves[i].color[2] = D_8013E538[work->command].color[8] * n / d;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[4];
                    work->strip.clut = work->clut[4];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->waves[i].u, work->waves[i].v, work->waves[i].u + 31,
                        work->waves[i].v, work->waves[i].u, work->waves[i].v + 127,
                        work->waves[i].u + 31, work->waves[i].v + 127);
                    setRGB0(&work->strip, 0, 0, 0);
                    setRGB1(&work->strip, 0, 0, 0);
                    setRGB2(&work->strip, work->waves[i].color[0], work->waves[i].color[1],
                        work->waves[i].color[2]);
                    setRGB3(&work->strip, work->waves[i].color[0], work->waves[i].color[1],
                        work->waves[i].color[2]);
                    depth = RotAverage4(&work->wave_inner[k], &work->wave_inner[k + 1],
                        &work->wave_outer[k], &work->wave_outer[k + 1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->waves[i].elapsed += step;
                if (work->waves[i].duration < work->waves[i].elapsed) {
                    work->waves[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->flash_count + work->shard_count + work->burst_count * 3) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}

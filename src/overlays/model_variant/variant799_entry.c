#include "../../types.h"
#include "variant799_entry.h"

/* Header 799's entry (partial reconstruction): up to six bolts leave a
 * model point at their own start times and fly towards a jittered point on
 * the opposing slot, shedding two sets of debris particles. */

static const VECTOR D_8013ED6C = {4096, 4096, 4096, 0};

/* Matching quirk: the shock-fan growth is indexed by "bolt - count", which
 * is always negative, so it really reads and writes the flight counters that
 * sit just below the growth table. */
#define GROWTH(i) work->u.timers.growth[(i) - work->config->count]

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model799State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_GT4 gt4;
    POLY_G3 g3;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_FT4 *ft4p;
    POLY_GT4 *gt4p;
    POLY_G3 *g3p;
    s32 slot;
    s32 i, j, k;
    s32 a, b;
    s32 otz;
    s16 sign;
    s32 edge;
    u16 half;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013ED6C;
    work = (Model799State *)context;
    ft4p = &ft4;
    gt4p = &gt4;
    g3p = &g3;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013EE40[command];
        for (i = 0; i < work->config->count; i++) {
            work->u.timers.growth[i] = 0;
            work->u.timers.ray_length[i] = 0;
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            setVector(&work->target[i], 0, 0, 0);
            setVector(&work->origin[i], 0, 0, 0);
            for (j = 0; j < work->config->ring_count; j++) {
                a = rand() % 4096;
                b = rand() % 4096;
                setVector(&work->ring[i][j],
                          work->config->size * ccos(a) / 4096 * ccos(b) / 4096,
                          work->config->size * csin(a) / 4096 * ccos(b) / 4096,
                          work->config->size * ccos(a) / 4096 * csin(b) / 4096);
            }
            for (j = 0; j < work->config->puff_count; j++) {
                a = rand() % 4096 - rand() % 4096;
                b = rand() % 4096 - rand() % 4096;
                k = rand() % 4096;
                setVector(&work->puffs[i][j], work->config->puff_spread * a / 4096,
                          work->config->puff_spread * b / 4096,
                          direction * work->config->puff_spread * k / 4096);
                setVector(&work->puff_velocity[i][j], work->config->puff_speed * a / 4096,
                          work->config->puff_speed * b / 4096,
                          direction * work->config->puff_speed * k / 4096);
                work->puff_frame[i][j] = 0;
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->chips[i][j], 0, 0, 0);
                setVector(&work->chip_velocity[i][j], (rand() - rand()) % 256 / 8,
                          -(rand() % 256 / 8 + 32), (rand() - rand()) % 256 / 8);
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->shards[i][j], 0, 0, 0);
                setVector(&work->shard_velocity[i][j], (rand() - rand()) % 256 * 36 / 256,
                          -(rand() % 256 * 36 / 256 + 36), (rand() - rand()) % 256 * 36 / 256);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->arrived[i] = 0;
            work->bolt[i].r = work->config->bolt_r;
            work->bolt[i].g = work->config->bolt_g;
            work->bolt[i].b = work->config->bolt_b;
            work->glow[i].r = work->config->glow_r;
            work->glow[i].g = work->config->glow_g;
            work->glow[i].b = work->config->glow_b;
            work->tail[i] = 0;
            work->trail_head[i] = 0;
            work->u.timers.flight[i] = 0;
            work->u.timers.flash_frame[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan[i][0], work->config->fan_radius * ccos(i * 256) / 4096, 0,
                      work->config->fan_radius * csin(i * 256) / 4096);
            setVector(&work->fan[i][1], work->config->fan_outer * ccos(i * 256) / 4096,
                      -work->config->fan_depth, work->config->fan_outer * csin(i * 256) / 4096);
        }
        for (i = 0; i < work->config->count; i++) {
            /* The spark table is indexed by spark only and shares its start
             * with the timers, which the init below resets. */
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->u.sparks[j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread));
                work->spark_frame[i][j] = 0;
            }
            work->spark_count[i] = 0;
        }
        work->ring_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->ring_clut = 0x3D28;
#else
        work->ring_clut = 0x3D20;
#endif
        for (i = 0; i < 7; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013ED7C[i]);
        }
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->u.timers.elapsed = 0;
        work->unknown_420C = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    setPolyFT4(ft4p);
    setPolyGT4(gt4p);
    setPolyG3(g3p);
    if (work->u.timers.elapsed < work->config->start[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -8);
    } else if (work->u.timers.elapsed < work->config->start[4]) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rot, -32);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rot, -8);
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 6) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 7;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->u.timers.elapsed < work->config->start[0]) {
        work->u.timers.elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if ((work->head == 0 && work->u.timers.elapsed >= work->config->start[0]) ||
        (work->head == 1 && work->u.timers.elapsed >= work->config->start[1]) ||
        (work->head == 2 && work->u.timers.elapsed >= work->config->start[2]) ||
        (work->head == 3 && work->u.timers.elapsed >= work->config->start[3]) ||
        (work->head == 4 && work->u.timers.elapsed >= work->config->start[4]) ||
        (work->head == 5 && work->u.timers.elapsed >= work->config->start[5])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        pos.vx += RANDOM_SPREAD(work->config->jitter);
        pos.vy = 0;
        pos.vz += -direction * work->config->jitter * (rand() % 4096) / 4096;
        setVector(&work->pos[work->head], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->head], (pos.vx - work->pos[work->head].vx) / work->config->steps,
                  (pos.vy - work->pos[work->head].vy) / work->config->steps,
                  (pos.vz - work->pos[work->head].vz) / work->config->steps);
        setVector(&work->angle[work->head], ratan2(work->step[work->head].vz, work->step[work->head].vy) - 0x400,
                  direction * ratan2(work->step[work->head].vx, work->step[work->head].vz), 0);
        addVector(&work->pos[work->head], &work->step[work->head]);
        setVector(&work->target[work->head], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 32; i++) {
            setVector(&work->trail[work->head][i], m.t[0], m.t[1], m.t[2]);
        }
        work->trail_head[work->head] = (work->trail_head[work->head] + 1) % 32;
        work->head++;
    }

    for (i = 0; i < work->head; i++) {
        if (work->arrived[i] == 0) {
            setPolyFT4(ft4p);
            ft4p->tpage = work->texture[1] >> 16;
            ft4p->clut = work->texture[1];
            setRGB0(ft4p, work->config->r, work->config->g, work->config->b);
            setVector(&quad[0], -work->config->size, -work->config->size, 0);
            setVector(&quad[1], work->config->size, -work->config->size, 0);
            setVector(&quad[2], -work->config->size, work->config->size, 0);
            setVector(&quad[3], work->config->size, work->config->size, 0);
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                              (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
            setUV4(ft4p, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
            }
        }

        if (work->bolt[i].r || work->bolt[i].g || work->bolt[i].b) {
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                copyVector(&pos, &work->pos[i]);
                addVector(&pos, &work->ring[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                ft4p->tpage = work->ring_tpage;
                ft4p->clut = work->ring_clut;
                setUV4(ft4p, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(ft4p, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                sign = 1;
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    quad[0].vx = -sign * work->config->ring_size;
                    edge = work->config->ring_size;
                    if (k - 2 >= 0) {
                        edge = -edge;
                    }
                    quad[0].vy = edge;
                    quad[0].vz = 0;
                    quad[1].vx = 0;
                    edge = work->config->ring_size;
                    if (k - 2 >= 0) {
                        edge = -edge;
                    }
                    quad[1].vy = edge;
                    quad[1].vz = 0;
                    setVector(&quad[2], -sign * work->config->ring_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                      (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
                    }
                }

                setRGB0(g3p, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB1(g3p, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB2(g3p, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->u.timers.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->u.timers.flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->u.timers.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->u.timers.flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.timers.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.timers.flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.timers.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->u.timers.elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->u.timers.elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.timers.flight[i]) * (-work->step[i].vz << 1));
                }
                for (k = 0; k < 2; k++) {
                    copyVector(&rot, &work->angle[i]);
                    rot.vz += k * 1024 % 4096;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3p->x0,
                                      (PSXLONG *)&g3p->x1, (PSXLONG *)&g3p->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3p, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3p->x0,
                                      (PSXLONG *)&g3p->x1, (PSXLONG *)&g3p->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3p, ot, (u16)otz, 3);
                    }
                }
            }
        }

        if (work->pos[i].vy < 0) {
            copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
            work->trail_head[i] = (work->trail_head[i] + 1) % 32;
            addVector(&work->pos[i], &work->step[i]);
            if (++work->u.timers.flight[i] == 8) {
                work->u.timers.flight[i] = 7;
            }
            if (work->pos[i].vy >= 0) {
                copyVector(&work->pos[i], &work->target[i]);
                work->arrived[i] = 1;
            }
        }

        setPolyG4(&g4);
        if (work->trail_head[i] > work->tail[i]) {
            if (work->bolt[i].r || work->bolt[i].g || work->bolt[i].b) {
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                k = 1;
                /* "- (tail + 1)" keeps the bound computed as (trail_head - 1) - tail. */
                for (j = work->trail_head[i] - 1; j >= work->trail_head[i] - (work->tail[i] + 1); j--) {
                    setRGB0(&g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB1(&g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB2(&g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setRGB3(&g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    setVector(&quad[0], work->trail[i][j].vx - work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx + work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx - work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx + work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    k++;
                }
                if (work->arrived[i]) {
                    if (work->bolt[i].r >= 16) {
                        work->bolt[i].r -= work->config->fade;
                    } else {
                        work->bolt[i].r = 0;
                    }
                    if (work->bolt[i].g >= 16) {
                        work->bolt[i].g -= work->config->fade;
                    } else {
                        work->bolt[i].g = 0;
                    }
                    if (work->bolt[i].b >= 16) {
                        work->bolt[i].b -= work->config->fade;
                    } else {
                        work->bolt[i].b = 0;
                    }
                } else {
                    work->tail[i]++;
                }
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        setPolyFT4(ft4p);
        ft4p->tpage = work->texture[2] >> 16;
        ft4p->clut = work->texture[2];
        setRGB0(ft4p, 0x80, 0x80, 0x80);
        setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
        setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
        setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
        for (j = 0; j < work->config->puff_count; j++) {
            if (work->puff_frame[i][j] < 16) {
                copyVector(&pos, &work->origin[i]);
                addVector(&pos, &work->puffs[i][j]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                  (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                setUV4(ft4p, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32 + 0x1F,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32 + 0x1F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 0);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->arrived[i] && (work->glow[i].r || work->glow[i].g || work->glow[i].b)) {
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            sign = 1;
            ft4p->tpage = work->ring_tpage;
            ft4p->clut = work->ring_clut;
            setUV4(ft4p, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(ft4p, work->glow[i].r, work->glow[i].g, work->glow[i].b);
            for (k = 0; k < 4; k++) {
                sign *= -1;
                quad[0].vx = -sign * work->config->glow_size;
                edge = work->config->glow_size;
                if (k - 2 >= 0) {
                    edge = -edge;
                }
                quad[0].vy = edge;
                quad[0].vz = 0;
                quad[1].vx = 0;
                edge = work->config->glow_size;
                if (k - 2 >= 0) {
                    edge = -edge;
                }
                quad[1].vy = edge;
                quad[1].vz = 0;
                setVector(&quad[2], -sign * work->config->glow_size, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                  (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
                }
            }
            sign = 1;
            for (k = 0; k < 4; k++) {
                sign *= -1;
                quad[0].vx = -sign * work->config->glow_size / 2;
                half = work->config->glow_size;
                quad[0].vy = (k - 2 >= 0) ? -half / 2 : half / 2;
                quad[0].vz = 0;
                quad[1].vx = 0;
                half = work->config->glow_size;
                quad[1].vy = (k - 2 >= 0) ? -half / 2 : half / 2;
                quad[1].vz = 0;
                setVector(&quad[2], -sign * work->config->glow_size / 2, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                  (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
                }
            }

            for (j = 0; j < 64; j++) {
                if (work->chips[i][j].vy <= 0) {
                    setVector(&quad[0], -24, -24, 0);
                    setVector(&quad[1], 24, -24, 0);
                    setVector(&quad[2], -24, 24, 0);
                    setVector(&quad[3], 24, 24, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&quad[k], &work->chips[i][j]);
                    }
                    a = rand() % 4;
                    setUV4(ft4p, a % 2 * 32, a / 2 * 32 + 0x40, a % 2 * 32 + 0x1F, a / 2 * 32 + 0x40,
                           a % 2 * 32, a / 2 * 32 + 0x5F, a % 2 * 32 + 0x1F, a / 2 * 32 + 0x5F);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                      (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
                    }
                    addVector(&work->chips[i][j], &work->chip_velocity[i][j]);
                    work->chip_velocity[i][j].vy += 4;
                }
            }

            setRGB0(ft4p, work->glow[i].r, work->glow[i].g, work->glow[i].b);
            ft4p->tpage = work->texture[0] >> 16;
            ft4p->clut = work->texture[0];
            setUV4(ft4p, 0xC0, 0, 0xFF, 0, 0xC0, 0x3F, 0xFF, 0x3F);
            for (j = 0; j < 64; j++) {
                if (work->shards[i][j].vy <= 0) {
                    setVector(&quad[0], -64, -64, 0);
                    setVector(&quad[1], 64, -64, 0);
                    setVector(&quad[2], -64, 64, 0);
                    setVector(&quad[3], 64, 64, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&quad[k], &work->shards[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                                      (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
                    }
                    addVector(&work->shards[i][j], &work->shard_velocity[i][j]);
                    work->shard_velocity[i][j].vy += 4;
                }
            }

            m = saved;
            setVector(&scale, 0x1000, work->u.timers.ray_length[i], 0x1000);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            ft4p->tpage = work->texture[work->u.timers.flash_frame[i] + 3] >> 16;
            ft4p->clut = work->texture[work->u.timers.flash_frame[i] + 3];
            setUV4(ft4p, work->u.timers.flash_frame[i] * 32, 0x40, work->u.timers.flash_frame[i] * 32 + 0x1F, 0x40,
                   work->u.timers.flash_frame[i] * 32, 0xBF, work->u.timers.flash_frame[i] * 32 + 0x1F, 0xBF);
            setRGB0(ft4p, work->glow[i].r, work->glow[i].g, work->glow[i].b);
            setVector(&quad[0], -64, -256, 0);
            setVector(&quad[1], 64, -256, 0);
            setVector(&quad[2], -64, 0, 0);
            setVector(&quad[3], 64, 0, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4p->x0,
                              (PSXLONG *)&ft4p->x1, (PSXLONG *)&ft4p->x2, (PSXLONG *)&ft4p->x3, &p, &flag);
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)ft4p, ot, (u16)otz, 1);
            }
            if (work->u.timers.flash_frame[i] < 3) {
                work->u.timers.flash_frame[i]++;
            }

            setVector(&rot, 0, 0, 0);
            setVector(&scale, GROWTH(i) + 0x1000, 0x1000, GROWTH(i) + 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setRGB0(gt4p, 0, 0, 0);
            setRGB1(gt4p, 0, 0, 0);
            setRGB2(gt4p, work->glow[i].r, work->glow[i].g, work->glow[i].b);
            setRGB3(gt4p, work->glow[i].r, work->glow[i].g, work->glow[i].b);
            gt4p->tpage = work->ring_tpage;
            gt4p->clut = work->ring_clut;
            setUV4(gt4p, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
            for (j = 0; j < 16; j++) {
                otz = RotAverage4(&work->fan[j][1], &work->fan[(j + 1) % 16][1], &work->fan[j][0],
                                  &work->fan[(j + 1) % 16][0], (PSXLONG *)&gt4p->x0, (PSXLONG *)&gt4p->x1,
                                  (PSXLONG *)&gt4p->x2, (PSXLONG *)&gt4p->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)gt4p, ot, (u16)otz, 1);
                }
            }
            setVector(&scale, GROWTH(i) / 2 + 0x1000, GROWTH(i) / 4 + 0x1000, GROWTH(i) / 2 + 0x1000);
            GsSetLsMatrix(&light);
            m = saved;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                otz = RotAverage4(&work->fan[j][1], &work->fan[(j + 1) % 16][1], &work->fan[j][0],
                                  &work->fan[(j + 1) % 16][0], (PSXLONG *)&gt4p->x0, (PSXLONG *)&gt4p->x1,
                                  (PSXLONG *)&gt4p->x2, (PSXLONG *)&gt4p->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)gt4p, ot, (u16)otz, 1);
                }
            }
            if (GROWTH(i) < 20000) {
                GROWTH(i) += work->config->grow;
            } else {
                GROWTH(i) += work->config->grow_late;
            }
            if (work->u.timers.ray_length[i] < 0x2000) {
                work->u.timers.ray_length[i] += 0x800;
            } else {
                work->u.timers.ray_length[i] += 0x100;
            }
            work->spark_count[i] += step;
            if (work->spark_count[i] > work->config->spark_count) {
                work->spark_count[i] = work->config->spark_count;
            }
            if (work->glow[i].r >= 16) {
                work->glow[i].r -= work->config->fade;
            } else {
                work->glow[i].r = 0;
            }
            if (work->glow[i].g >= 16) {
                work->glow[i].g -= work->config->fade;
            } else {
                work->glow[i].g = 0;
            }
            if (work->glow[i].b >= 16) {
                work->glow[i].b -= work->config->fade;
            } else {
                work->glow[i].b = 0;
            }
        }
    }

    work->u.timers.elapsed += step;
    PopMatrix();
    if (work->phase == 7 && work->glow[work->config->count - 1].r == 0 &&
        work->glow[work->config->count - 1].g == 0 && work->glow[work->config->count - 1].b == 0) {
        return 2;
    }
    if (work->arrived[work->config->count - 1] == 1 && work->phase == 5) {
        work->phase = 6;
        return 1;
    }
    if (work->arrived[4] == 1 && work->phase == 4) {
        work->phase = 5;
        return 4;
    }
    if (work->arrived[3] == 1 && work->phase == 3) {
        work->phase = 4;
        return 4;
    }
    if (work->arrived[2] == 1 && work->phase == 2) {
        work->phase = 3;
        return 4;
    }
    if (work->arrived[1] == 1 && work->phase == 1) {
        work->phase = 2;
        return 4;
    }
    if (work->arrived[0] == 1 && work->phase == 0) {
        work->phase = 1;
        return 4;
    }
    return 0;
}

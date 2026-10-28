#include "../../types.h"
#include "variant842_entry.h"

/* Header 842's entry: three bolts leave a model point at their own start
 * times and fly towards a jittered point on the opposing slot, wrapped in
 * a ring of quartered sprites and spinning Gouraud strips and trailing a
 * ribbon and smoke puffs. On arrival each flares into a quartered glow
 * with a fan of growing textured rays and animated sparks while the
 * ambient light dims and recovers. */

static const VECTOR D_8013E664 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model842State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_G4 g4;
    POLY_FT4 puff;
    POLY_FT4 glow;
    POLY_GT4 ray;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_GT4 *gt4;
    s32 slot;
    s32 i, j, k;
    s32 a, b;
    s32 otz;
    s16 sign;
    s32 edge;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013E664;
    work = (Model842State *)context;
    gt4 = &ray;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013E700[command];
        for (i = 0; i < work->config->count; i++) {
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
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->arrived[i] = 0;
            work->bolt_r[i] = work->config->bolt_r;
            work->bolt_g[i] = work->config->bolt_g;
            work->bolt_b[i] = work->config->bolt_b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->ray_r[i] = work->config->ray_r;
            work->ray_g[i] = work->config->ray_g;
            work->ray_b[i] = work->config->ray_b;
            work->tail[i] = 0;
            work->trail_head[i] = 0;
            work->flight[i] = 0;
            work->ray_length[i] = 0;
            for (j = 0; j < 16; j++) {
                setVector(&work->rays[i][j], rand() % 4096, rand() % 4096, rand() % 4096);
            }
        }
        for (i = 0; i < work->config->count; i++) {
            for (j = 0; j < work->config->spark_count; j++) {
                setVector(&work->sparks[i][j], RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread),
                          RANDOM_SPREAD(work->config->spark_spread));
                work->spark_frame[i][j] = 0;
            }
            work->spark_count[i] = 0;
        }
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E674[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E674[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E674[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E674[3]);
        work->texture[4] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E674[4]);
        work->ring_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->ring_clut = 0x3D28;
#else
        work->ring_clut = 0x3D20;
#endif
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->unknown_ABE = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start[0]) {
        Model_QueueCameraOrbitFromPreset(slot, 4, &rot, -8);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, -8);
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 3) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 4;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, 0x800, work->ambient, work->ambient);
    if (work->elapsed < work->config->start[0]) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if ((work->head == 0 && work->elapsed >= work->config->start[0]) ||
        (work->head == 1 && work->elapsed >= work->config->start[1]) ||
        (work->head == 2 && work->elapsed >= work->config->start[2])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        setVector(&work->target[work->head], RANDOM_SPREAD(work->config->jitter),
                  pos.vy + RANDOM_SPREAD(work->config->jitter), pos.vz);
        setVector(&work->pos[work->head], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->head], (work->target[work->head].vx - work->pos[work->head].vx) / work->config->steps,
                  (work->target[work->head].vy - work->pos[work->head].vy) / work->config->steps,
                  (work->target[work->head].vz - work->pos[work->head].vz) / work->config->steps);
        setVector(&work->angle[work->head], ratan2(work->step[work->head].vz, work->step[work->head].vy) - 0x400,
                  direction * ratan2(work->step[work->head].vx, work->step[work->head].vz), 0);
        addVector(&work->pos[work->head], &work->step[work->head]);
        for (i = 0; i < 32; i++) {
            setVector(&work->trail[work->head][i], m.t[0], m.t[1], m.t[2]);
        }
        work->trail_head[work->head] = (work->trail_head[work->head] + 1) % 32;
        work->head++;
    }

    for (i = 0; i < work->head; i++) {
        if (work->arrived[i] == 0) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
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
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
        }

        if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
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
                setPolyFT4(&ft4);
                ft4.tpage = work->ring_tpage;
                ft4.clut = work->ring_clut;
                setUV4(&ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(&ft4, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                /* Each quarter is drawn separately; the edge temporary keeps one
                 * store per conditional negation. */
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
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                }

                setPolyG3(&g3);
                setRGB0(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * (-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * (-work->step[i].vz << 1));
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
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                }
            }
        }

        if (direction > 0) {
            if (work->pos[i].vz < work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 32;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz >= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        } else {
            if (work->pos[i].vz > work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 32;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->flight[i] == 8) {
                    work->flight[i] = 7;
                }
                if (work->pos[i].vz <= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        }

        setPolyG4(&g4);
        if (work->trail_head[i] > work->tail[i]) {
            if (work->bolt_r[i] || work->bolt_g[i] || work->bolt_b[i]) {
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
                    setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
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
                    if (work->bolt_r[i] >= 16) {
                        work->bolt_r[i] -= work->config->fade;
                    } else {
                        work->bolt_r[i] = 0;
                    }
                    if (work->bolt_g[i] >= 16) {
                        work->bolt_g[i] -= work->config->fade;
                    } else {
                        work->bolt_g[i] = 0;
                    }
                    if (work->bolt_b[i] >= 16) {
                        work->bolt_b[i] -= work->config->fade;
                    } else {
                        work->bolt_b[i] = 0;
                    }
                } else {
                    work->tail[i]++;
                }
            }
        }

        setVector(&scale, 0x1000, 0x1000, 0x1000);
        setVector(&rot, 0, 0, 0);
        setPolyFT4(&puff);
        puff.tpage = work->texture[4] >> 16;
        puff.clut = work->texture[4];
        setRGB0(&puff, 0x80, 0x80, 0x80);
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
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&puff.x0,
                                  (PSXLONG *)&puff.x1, (PSXLONG *)&puff.x2, (PSXLONG *)&puff.x3, &p, &flag);
                if (work->puff_frame[i][j] < 8) {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x40,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x5F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x5F);
                } else {
                    setUV4(&puff, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x60,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, 0x7F,
                           work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, 0x7F);
                }
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&puff, ot, (u16)otz, 0);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->arrived[i] && (work->glow_r[i] || work->glow_g[i] || work->glow_b[i])) {
            setPolyFT4(&glow);
            glow.tpage = work->ring_tpage;
            glow.clut = work->ring_clut;
            setUV4(&glow, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(&glow, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
            sign = 1;
            GsSetLsMatrix(&light);
            RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
            saved = m;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 4; j++) {
                sign *= -1;
                quad[0].vx = -sign * work->config->glow_size;
                edge = work->config->glow_size;
                if (j - 2 >= 0) {
                    edge = -edge;
                }
                quad[0].vy = edge;
                quad[0].vz = 0;
                quad[1].vx = 0;
                edge = work->config->glow_size;
                if (j - 2 >= 0) {
                    edge = -edge;
                }
                quad[1].vy = edge;
                quad[1].vz = 0;
                setVector(&quad[2], -sign * work->config->glow_size, 0, 0);
                setVector(&quad[3], 0, 0, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                }
            }

            setPolyGT4(gt4);
            gt4->tpage = work->ring_tpage;
            gt4->clut = work->ring_clut;
            setUV4(gt4, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
            setRGB0(gt4, 0, 0, 0);
            setRGB1(gt4, 0, 0, 0);
            setRGB2(gt4, work->glow_r[i] / 2, work->glow_g[i] / 2, work->glow_b[i] / 2);
            setRGB3(gt4, work->glow_r[i] / 2, work->glow_g[i] / 2, work->glow_b[i] / 2);
            setVector(&quad[0], -12, -256, 0);
            setVector(&quad[1], 12, -256, 0);
            setVector(&quad[2], -16, 0, 0);
            setVector(&quad[3], 16, 0, 0);
            setVector(&scale, 0x1000, work->ray_length[i], 0x1000);
            for (j = 0; j < 16; j++) {
                m = saved;
                RotMatrix(&work->rays[i][j], &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4->x0,
                                  (PSXLONG *)&gt4->x1, (PSXLONG *)&gt4->x2, (PSXLONG *)&gt4->x3, &p, &flag);
                setSemiTrans(gt4, 1);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(gt4, ot, (u16)otz);
                }
            }
            if (work->ray_length[i] < 0x2000) {
                work->ray_length[i] += 0x800;
            } else {
                work->ray_length[i] += 0x40;
            }

            setPolyFT4(&glow);
            setRGB0(&glow, 0x80, 0x80, 0x80);
            glow.tpage = work->texture[3] >> 16;
            glow.clut = work->texture[3];
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&rot, 0, 0, 0);
            for (j = 0; j < work->spark_count[i]; j++) {
                if (work->spark_frame[i][j] < 8) {
                    copyVector(&pos, &work->target[i]);
                    addVector(&pos, &work->sparks[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    if (work->spark_frame[i][j] < 4) {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0x80,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x80,
                               work->spark_frame[i][j] % 4 * 32 + 0x80, 0x9F,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x9F);
                    } else {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0xA0,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xA0,
                               work->spark_frame[i][j] % 4 * 32 + 0x80, 0xBF,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xBF);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    }
                    work->spark_frame[i][j]++;
                }
            }
            work->spark_count[i] += step;
            if (work->spark_count[i] > work->config->spark_count) {
                work->spark_count[i] = work->config->spark_count;
            }

            if (work->glow_r[i] >= 16) {
                work->glow_r[i] -= work->config->fade;
            } else {
                work->glow_r[i] = 0;
            }
            if (work->glow_g[i] >= 16) {
                work->glow_g[i] -= work->config->fade;
            } else {
                work->glow_g[i] = 0;
            }
            if (work->glow_b[i] >= 16) {
                work->glow_b[i] -= work->config->fade;
            } else {
                work->glow_b[i] = 0;
            }
            if (work->ray_r[i] >= 16) {
                work->ray_r[i] -= work->config->ray_fade;
            } else {
                work->ray_r[i] = 0;
            }
            if (work->ray_g[i] >= 16) {
                work->ray_g[i] -= work->config->ray_fade;
            } else {
                work->ray_g[i] = 0;
            }
            if (work->ray_b[i] >= 16) {
                work->ray_b[i] -= work->config->ray_fade;
            } else {
                work->ray_b[i] = 0;
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 4 && work->glow_r[work->config->count - 1] == 0 &&
        work->glow_g[work->config->count - 1] == 0 && work->glow_b[work->config->count - 1] == 0) {
        return 2;
    }
    if (work->arrived[work->config->count - 1] == 1 && work->phase == 2) {
        work->phase = 3;
        return 1;
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

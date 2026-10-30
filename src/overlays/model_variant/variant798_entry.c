#include "../../types.h"
#include "variant798_entry.h"

/* Header 798's entry: once the start time passes a single bolt leaves a
 * model point with a burst of launch debris and flies towards a jittered
 * point on the opposing slot, wrapped in a ring of four-way flares and
 * spinning Gouraud strips and trailing a ribbon and smoke puffs. On arrival
 * it flares into a glow with impact chips, dust, shards and animated
 * sparks, 64 spinning semi-transparent blades and a pulsing shock fan while
 * the ambient light dims and recovers. */

static const VECTOR D_8013F984 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model798State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4_poly;
    POLY_G3 g3_poly;
    POLY_G4 g4_poly;
    POLY_GT4 gt4_poly;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    POLY_FT4 *ft4;
    POLY_G3 *g3;
    POLY_G4 *g4;
    POLY_GT4 *gt4;
    s32 slot;
    s32 i, j, k, n;
    s32 a, b;
    s32 otz;
    s16 sign;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013F984;
    work = (Model798State *)context;
    ft4 = &ft4_poly;
    g3 = &g3_poly;
    g4 = &g4_poly;
    gt4 = &gt4_poly;
    direction = (slot = Model_GetActiveSlotIndex()) * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013FAC8[command];
        for (i = 0; i < work->config->count; i++) {
            work->u.n.pulse[i] = 0;
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
            for (j = 0; j < 16; j++) {
                setVector(&work->chips[i][j], 0, 0, 0);
                setVector(&work->chip_velocity[i][j], ((rand() - rand()) % 256) / 16,
                          ((rand() - rand()) % 256) / 16, ((rand() - rand()) % 256) / 16);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->shards[i][j], 0, 0, 0);
                setVector(&work->shard_velocity[i][j], ((rand() - rand()) % 256) * 12 / 256,
                          ((rand() - rand()) % 256) * 12 / 256, ((rand() - rand()) % 256) * 12 / 256);
                work->shard_frame[i] = 0;
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->dust[i][j], 0, 0, 0);
                setVector(&work->dust_velocity[i][j], ((rand() - rand()) % 256) * 24 / 256,
                          ((rand() - rand()) % 256) * 24 / 256, ((rand() - rand()) % 256) * 24 / 256);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->debris[i][j], 0, 0, 0);
                setVector(&work->debris_velocity[i][j], ((rand() - rand()) % 256) / 16,
                          ((rand() - rand()) % 256) / 16, ((rand() - rand()) % 256) / 16);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->spin[i][j], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                          (rand() - rand()) % 4096);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->arrived[i] = 0;
            work->bolt[i].r = work->config->bolt_r;
            work->bolt[i].g = work->config->bolt_g;
            work->bolt[i].b = work->config->bolt_b;
            work->glow[i].r = work->config->glow_r;
            work->glow[i].g = work->config->glow_g;
            work->glow[i].b = work->config->glow_b;
            work->flash[i].r = work->config->glow_r;
            work->flash[i].g = work->config->glow_g;
            work->flash[i].b = work->config->glow_b;
            work->tail[i] = 0;
            work->trail_head[i] = 0;
            work->growth[i] = 0;
            work->u.n.flight[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan[i].inner, work->config->fan_inner * ccos(i * 256) / 4096,
                      work->config->fan_inner * csin(i * 256) / 4096, 0);
            setVector(&work->fan[i].outer, work->config->fan_outer * ccos(i * 256) / 4096,
                      work->config->fan_outer * csin(i * 256) / 4096, -direction * work->config->fan_depth);
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
        work->fan_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->fan_clut = 0x3D28;
#else
        work->fan_clut = 0x3D20;
#endif
        for (i = 0; i < 11; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013F994[i]);
        }
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->unknown_1448 = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 4, &rot, -16);
    } else if (work->elapsed < work->config->approach) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, work->config->steps / 2);
    } else if (work->elapsed < work->config->turn) {
        Model_QueueCameraOrbitFromPreset(slot, 3, &rot, 0);
    } else if (work->elapsed < work->config->end) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -8);
    } else if (work->elapsed < work->config->launch[2]) {
        Model_QueueCameraOrbitFromPreset(slot, 1, &rot, 0);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, -8);
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 4) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 5;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, 0x800, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    setPolyFT4(ft4);
    setPolyG3(g3);
    setPolyG4(g4);
    setPolyGT4(gt4);
    if ((work->head == 0 && work->elapsed >= work->config->start) ||
        (work->head == 1 && work->elapsed >= work->config->launch[0]) ||
        (work->head == 2 && work->elapsed >= work->config->launch[1]) ||
        (work->head == 3 && work->elapsed >= work->config->launch[2])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        pos.vz += -direction * (rot.vz / 2) * (rand() % 4096) / 4096;
        setVector(&work->pos[work->head], m.t[0], m.t[1], m.t[2]);
        setVector(&work->step[work->head], (pos.vx - work->pos[work->head].vx) / work->config->steps,
                  (pos.vy - work->pos[work->head].vy) / work->config->steps,
                  (pos.vz - work->pos[work->head].vz) / work->config->steps);
        setVector(&work->angle[work->head], ratan2(work->step[work->head].vz, work->step[work->head].vy) - 0x400,
                  direction * ratan2(work->step[work->head].vx, work->step[work->head].vz), 0);
        addVector(&work->pos[work->head], &work->step[work->head]);
        setVector(&work->target[work->head], pos.vx, pos.vy, pos.vz);
        for (i = 0; i < 16; i++) {
            setVector(&work->trail[work->head][i], m.t[0], m.t[1], m.t[2]);
        }
        work->trail_head[work->head] = (work->trail_head[work->head] + 1) % 16;
        work->head++;
    }

    for (i = 0; i < work->head; i++) {
        if (work->arrived[i] == 0) {
            ft4->tpage = work->texture[0] >> 16;
            ft4->clut = work->texture[0];
            setRGB0(ft4, work->config->r, work->config->g, work->config->b);
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
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                              (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
            setUV4(ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
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
                sign = 1;
                ft4->tpage = work->fan_tpage;
                ft4->clut = work->fan_clut;
                setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(ft4, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * work->config->flare_size,
                              (k - 2 >= 0 ? -1 : 1) * work->config->flare_size, 0);
                    setVector(&quad[1], 0, (k - 2 >= 0 ? -1 : 1) * work->config->flare_size, 0);
                    setVector(&quad[2], -sign * work->config->flare_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }

                setRGB0(g3, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB1(g3, work->bolt[i].r, work->bolt[i].g, work->bolt[i].b);
                setRGB2(g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->u.n.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->u.n.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->u.n.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->u.n.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.n.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.n.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.n.flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->u.n.flight[i]) * ((s32)((u32)-work->step[i].vz << 1)));
                }
                for (k = 0; k < 4; k++) {
                    copyVector(&rot, &work->angle[i]);
                    rot.vz += k * 512 % 4096;
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3->x0, (PSXLONG *)&g3->x1,
                                      (PSXLONG *)&g3->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3, ot, (u16)otz, 3);
                    }
                    otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3->x0, (PSXLONG *)&g3->x1,
                                      (PSXLONG *)&g3->x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g3, ot, (u16)otz, 3);
                    }
                }
            }
        }

        if (direction > 0) {
            if (work->pos[i].vz < work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->u.n.flight[i] == 8) {
                    work->u.n.flight[i] = 7;
                }
                if (work->pos[i].vz >= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        } else {
            if (work->pos[i].vz > work->target[i].vz) {
                copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                addVector(&work->pos[i], &work->step[i]);
                if (++work->u.n.flight[i] == 8) {
                    work->u.n.flight[i] = 7;
                }
                if (work->pos[i].vz <= work->target[i].vz) {
                    copyVector(&work->pos[i], &work->target[i]);
                    work->arrived[i] = 1;
                }
            }
        }

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
                for (j = work->trail_head[i] - 1; j >= work->trail_head[i] - (work->tail[i] + 1); j--) {
                    setRGB0(g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB1(g4, work->bolt[i].r / k, work->bolt[i].g / k, work->bolt[i].b / k);
                    setRGB2(g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setRGB3(g4, work->bolt[i].r / (k + 1), work->bolt[i].g / (k + 1), work->bolt[i].b / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->size / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->config->size / (k + 1), work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4->x0,
                                      (PSXLONG *)&g4->x1, (PSXLONG *)&g4->x2, (PSXLONG *)&g4->x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g4, ot, (u16)otz, 1);
                    }
                    setVector(&quad[0], work->trail[i][j].vx - work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx + work->config->size / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx - work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx + work->config->size / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4->x0,
                                      (PSXLONG *)&g4->x1, (PSXLONG *)&g4->x2, (PSXLONG *)&g4->x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)g4, ot, (u16)otz, 1);
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
        ft4->tpage = work->texture[1] >> 16;
        ft4->clut = work->texture[1];
        setRGB0(ft4, 0x80, 0x80, 0x80);
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
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                  (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                setUV4(ft4, work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x40, work->puff_frame[i][j] / 8 * 32 + 0x1F,
                       work->puff_frame[i][j] / 2 % 4 * 32 + 0x5F, work->puff_frame[i][j] / 8 * 32 + 0x1F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 0);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->flash[i].r || work->flash[i].g || work->flash[i].b) {
            ft4->tpage = work->fan_tpage;
            ft4->clut = work->fan_clut;
            setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
            setRGB0(ft4, work->flash[i].r, work->flash[i].g, work->flash[i].b);
            GsSetLsMatrix(&light);
            RotTrans(&work->origin[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                sign = 1;
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * 12, k - 2 >= 0 ? -12 : 12, 0);
                    setVector(&quad[1], 0, k - 2 >= 0 ? -12 : 12, 0);
                    setVector(&quad[2], -sign * 12, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    for (n = 0; n < 4; n++) {
                        addVector(&quad[n], &work->debris[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }
                addVector(&work->debris[i][j], &work->debris_velocity[i][j]);
            }
            if (work->flash[i].r >= 16) {
                work->flash[i].r -= 15;
            } else {
                work->flash[i].r = 0;
            }
            if (work->flash[i].g >= 16) {
                work->flash[i].g -= 15;
            } else {
                work->flash[i].g = 0;
            }
            if (work->flash[i].b >= 16) {
                work->flash[i].b -= 15;
            } else {
                work->flash[i].b = 0;
            }
        }

        if (work->arrived[i]) {
            if (work->glow[i].r || work->glow[i].g || work->glow[i].b) {
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                saved = m;
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                sign = 1;
                ft4->tpage = work->fan_tpage;
                ft4->clut = work->fan_clut;
                setUV4(ft4, 0x40, 0, 0x7F, 0, 0x40, 0x3F, 0x7F, 0x3F);
                setRGB0(ft4, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                for (k = 0; k < 4; k++) {
                    sign *= -1;
                    setVector(&quad[0], -sign * work->config->glow_size,
                              (k - 2 >= 0 ? -1 : 1) * work->config->glow_size, 0);
                    setVector(&quad[1], 0, (k - 2 >= 0 ? -1 : 1) * work->config->glow_size, 0);
                    setVector(&quad[2], -sign * work->config->glow_size, 0, 0);
                    setVector(&quad[3], 0, 0, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }

                for (n = 0; n < 16; n++) {
                    sign = 1;
                    for (k = 0; k < 4; k++) {
                        sign *= -1;
                        setVector(&quad[0], -sign * 16, k - 2 >= 0 ? -16 : 16, 0);
                        setVector(&quad[1], 0, k - 2 >= 0 ? -16 : 16, 0);
                        setVector(&quad[2], -sign * 16, 0, 0);
                        setVector(&quad[3], 0, 0, 0);
                        for (j = 0; j < 4; j++) {
                            addVector(&quad[j], &work->chips[i][n]);
                        }
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                          (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p,
                                          &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                        }
                    }
                    addVector(&work->chips[i][n], &work->chip_velocity[i][n]);
                }

                ft4->tpage = work->texture[10] >> 16;
                ft4->clut = work->texture[10];
                setRGB0(ft4, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                setUV4(ft4, 0xC0, 0, 0xFF, 0, 0xC0, 0x3F, 0xFF, 0x3F);
                for (j = 0; j < 64; j++) {
                    setVector(&quad[0], -64, -64, 0);
                    setVector(&quad[1], 64, -64, 0);
                    setVector(&quad[2], -64, 64, 0);
                    setVector(&quad[3], 64, 64, 0);
                    for (k = 0; k < 4; k++) {
                        addVector(&quad[k], &work->dust[i][j]);
                    }
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                      (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                    addVector(&work->dust[i][j], &work->dust_velocity[i][j]);
                }

                ft4->tpage = work->texture[1] >> 16;
                ft4->clut = work->texture[1];
                setRGB0(ft4, 0x80, 0x80, 0x80);
                if (work->shard_frame[i] < 16) {
                    setUV4(ft4, work->shard_frame[i] / 2 % 4 * 32 + 0x40, work->shard_frame[i] / 8 * 32,
                           work->shard_frame[i] / 2 % 4 * 32 + 0x5F, work->shard_frame[i] / 8 * 32,
                           work->shard_frame[i] / 2 % 4 * 32 + 0x40, work->shard_frame[i] / 8 * 32 + 0x1F,
                           work->shard_frame[i] / 2 % 4 * 32 + 0x5F, work->shard_frame[i] / 8 * 32 + 0x1F);
                    for (j = 0; j < 16; j++) {
                        setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
                        setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
                        setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
                        setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
                        for (k = 0; k < 4; k++) {
                            addVector(&quad[k], &work->shards[i][j]);
                        }
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                          (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p,
                                          &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                        }
                        addVector(&work->shards[i][j], &work->shard_velocity[i][j]);
                    }
                    work->shard_frame[i]++;
                }

                setRGB0(ft4, 0x40, 0x40, 0x40);
                for (j = 0; j < work->spark_count[i]; j++) {
                    if (work->spark_frame[i][j] < 8) {
                        ft4->tpage = work->texture[work->spark_frame[i][j] + 2] >> 16;
                        ft4->clut = work->texture[work->spark_frame[i][j] + 2];
                        setUV4(ft4, work->spark_frame[i][j] % 4 * 64, work->spark_frame[i][j] / 4 * 64 + 0x60,
                               work->spark_frame[i][j] % 4 * 64 + 0x3F, work->spark_frame[i][j] / 4 * 64 + 0x60,
                               work->spark_frame[i][j] % 4 * 64, work->spark_frame[i][j] / 4 * 64 + 0x9F,
                               work->spark_frame[i][j] % 4 * 64 + 0x3F, work->spark_frame[i][j] / 4 * 64 + 0x9F);
                        setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                        setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                        setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                        setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                        for (n = 0; n < 4; n++) {
                            addVector(&quad[n], &work->sparks[i][j]);
                        }
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4->x0,
                                          (PSXLONG *)&ft4->x1, (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p,
                                          &flag);
                        if (otz >= 0 && flag >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                        }
                        work->spark_frame[i][j]++;
                    }
                }

                gt4->tpage = work->fan_tpage;
                gt4->clut = work->fan_clut;
                setSemiTrans(gt4, 1);
                setUV4(gt4, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
                setVector(&scale, 0x1000, work->growth[i], 0x1000);
                setRGB0(gt4, 0, 0, 0);
                setRGB1(gt4, 0, 0, 0);
                setRGB2(gt4, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                setRGB3(gt4, work->glow[i].r, work->glow[i].g, work->glow[i].b);
                setVector(&quad[0], -4, -0xC0, 0);
                setVector(&quad[1], 4, -0xC0, 0);
                setVector(&quad[2], -12, 0, 0);
                setVector(&quad[3], 12, 0, 0);
                for (j = 0; j < 32; j++) {
                    m = saved;
                    RotMatrix(&work->spin[i][j], &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&gt4->x0,
                                      (PSXLONG *)&gt4->x1, (PSXLONG *)&gt4->x2, (PSXLONG *)&gt4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        GsSortPoly(gt4, ot, (u16)otz);
                    }
                }

                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->u.rows[1][i - work->config->count] + 0x1000,
                          work->u.rows[1][i - work->config->count] + 0x1000,
                          work->u.rows[1][i - work->config->count] / 8 + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB0(ft4, work->glow[i].r / 4, work->glow[i].g / 4, work->glow[i].b / 4);
                ft4->tpage = work->fan_tpage;
                ft4->clut = work->fan_clut;
                setUV4(ft4, 0, 0x80, 0x1F, 0x80, 0, 0xFF, 0x1F, 0xFF);
                for (j = 0; j < 16; j++) {
                    otz = RotAverage4(&work->fan[j].outer, &work->fan[(j + 1) % 16].outer, &work->fan[j].inner,
                                      &work->fan[(j + 1) % 16].inner, (PSXLONG *)&ft4->x0, (PSXLONG *)&ft4->x1,
                                      (PSXLONG *)&ft4->x2, (PSXLONG *)&ft4->x3, &p, &flag);
                    if (otz >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)ft4, ot, (u16)otz, 1);
                    }
                }
                if (work->u.rows[1][i - work->config->count] < 20000) {
                    work->u.rows[1][i - work->config->count] += work->config->pulse_fast;
                } else {
                    work->u.rows[1][i - work->config->count] += work->config->pulse_slow;
                }
                work->spark_count[i] += 8;
                if (work->spark_count[i] > work->config->spark_count) {
                    work->spark_count[i] = work->config->spark_count;
                }
                if (work->growth[i] < 0x2000) {
                    work->growth[i] += 0x800;
                } else {
                    work->growth[i] += 0x100;
                }
                if (work->glow[i].r >= 16) {
                    work->glow[i].r -= 15;
                } else {
                    work->glow[i].r = 0;
                }
                if (work->glow[i].g >= 16) {
                    work->glow[i].g -= 15;
                } else {
                    work->glow[i].g = 0;
                }
                if (work->glow[i].b >= 16) {
                    work->glow[i].b -= 15;
                } else {
                    work->glow[i].b = 0;
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 5 && work->glow[work->config->count - 1].r == 0 &&
        work->glow[work->config->count - 1].g == 0 && work->glow[work->config->count - 1].b == 0) {
        return 2;
    }
    if (work->arrived[work->config->count - 1] == 1 && work->phase == 3) {
        work->phase = 4;
        return 1;
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

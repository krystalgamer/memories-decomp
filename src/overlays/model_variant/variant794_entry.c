#include "../../types.h"
#include "variant794_entry.h"

/* Header 794's entry: every period a bolt leaves a model point and flies
 * towards a jittered point on the opposing slot, wrapped in a ring of
 * sprites and spinning Gouraud strips and trailing a fading ribbon and
 * smoke puffs. On arrival it flares into a glow with two fans of rays and
 * animated sparks while the ambient light dims and recovers. */

static const VECTOR gModelVariant794_D3854 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant794_HandleRequest(u8 *context, s32 command)
{
    Model794State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_G4 g4;
    POLY_FT4 puff;
    POLY_FT4 glow;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j, k;
    s32 a, b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant794_D3854;
    work = (Model794State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant794_D38F0[command];
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
                /* The depth spread reuses K, which fixes the register allocation. */
                k = rand() % 4096;
                setVector(&work->puffs[i][j], work->config->puff_spread * a / 4096,
                          work->config->puff_spread * b / 4096,
                          direction * work->config->puff_spread * k / 4096);
                setVector(&work->puff_velocity[i][j], work->config->puff_speed * a / 4096,
                          work->config->puff_speed * b / 4096,
                          direction * work->config->puff_speed * k / 4096);
                work->puff_frame[i][j] = 0;
            }
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread));
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
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant794_D3864[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant794_D3864[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant794_D3864[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant794_D3864[3]);
        work->texture[4] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant794_D3864[4]);
        work->ambient = 0x800;
        work->head = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->unknown_30EC = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 14, &rot, -8);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 16);
    }
    if (work->head == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 3;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->start && work->elapsed < work->config->end &&
        work->frames % work->config->period == 0 && work->head < work->config->count) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin[work->head], m.t[0], m.t[1], m.t[2]);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
        pos.vx += RANDOM_SPREAD(rot.vx / 2);
        pos.vy += RANDOM_SPREAD(rot.vy / 2);
        pos.vz += RANDOM_SPREAD(rot.vz / 2);
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
        /* The original indexes the old trail head with the spent loop
         * counter. */
        work->trail_head[work->head] = (work->trail_head[work->head] + 1) % 32;
        work->head++;
    }

    for (i = 0; i < work->head; i++) {
        if (work->arrived[i] == 0) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[3] >> 16;
            ft4.clut = work->texture[3];
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
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
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
                ft4.tpage = work->texture[2] >> 16;
                ft4.clut = work->texture[2];
                setRGB0(&ft4, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
                setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB1(&g3, work->bolt_r[i], work->bolt_g[i], work->bolt_b[i]);
                setRGB2(&g3, 0, 0, 0);
                setVector(&quad[0], 0, -work->config->strip_width, 0);
                setVector(&quad[1], 0, work->config->strip_width, 0);
                /* The doubled depth is a shift of the negated step; "* 2" would
                 * fold into the quotient instead. */
                if (j % 4 == 0) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (13 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 1) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (11 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 2) {
                    setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
                } else if (j % 4 == 3) {
                    setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * -work->step[i].vz);
                    setVector(&quad[3], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                              -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                              work->config->steps / (12 - work->flight[i]) * ((u32)-work->step[i].vz << 1));
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
                if (ABS(work->pos[i].vz) >= ABS(work->target[i].vz)) {
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
                /* Writing the bound as "- (tail + 1)" keeps "trail_head - 1" computed first. */
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
        puff.tpage = work->texture[0] >> 16;
        puff.clut = work->texture[0];
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
                    Graphics_SubmitDrawModePacket((u32 *)&puff, ot, (u16)otz, 1);
                }
                work->puff_frame[i][j]++;
                addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
            }
        }

        if (work->arrived[i]) {
            if (work->glow_r[i] || work->glow_g[i] || work->glow_b[i]) {
                setPolyFT4(&glow);
                glow.tpage = work->texture[2] >> 16;
                glow.clut = work->texture[2];
                setRGB0(&glow, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                setUV4(&glow, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                }

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->ray_r[i], work->ray_g[i], work->ray_b[i]);
                setRGB2(&g3, work->ray_r[i], work->ray_g[i], work->ray_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    setVector(&quad[0], -work->config->ray_width, 0, 0);
                    setVector(&quad[1], work->config->ray_width, 0, 0);
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }
                setVector(&scale, 0xC00, 0xC00, 0xC00);
                GsSetLsMatrix(&light);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    setVector(&quad[0], -work->config->ray_width, 0, 0);
                    setVector(&quad[1], work->config->ray_width, 0, 0);
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }

                setPolyFT4(&glow);
                glow.tpage = work->texture[1] >> 16;
                glow.clut = work->texture[1];
                setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                for (j = 0; j < work->spark_count[i]; j++) {
                    if (work->spark_frame[i][j] < 8) {
                        setRGB0(&glow, 0x80, 0x80, 0x80);
                        copyVector(&pos, &work->target[i]);
                        addVector(&pos, &work->sparks[i][j]);
                        GsSetLsMatrix(&light);
                        RotTrans(&pos, (VECTOR *)m.t, &flag);
                        RotMatrix(&rot, &m);
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                          (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p,
                                          &flag);
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
    }

    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->phase == 3 && work->glow_r[work->head - 1] == 0 && work->glow_g[work->head - 1] == 0 &&
        work->glow_b[work->head - 1] == 0) {
        return 2;
    }
    if (work->elapsed > work->config->end && work->arrived[work->head - 1] == 1 && work->phase == 0) {
        work->phase = 2;
        return 1;
    }
    if (work->arrived[0] == 1 && work->phase == 0) {
        return 4;
    }
    return 0;
}

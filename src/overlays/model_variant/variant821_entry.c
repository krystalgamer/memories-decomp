#include "../../types.h"
#include "variant821_entry.h"

/* Header 821's entry: three volleys of up to ten bolts leave model points
 * on the active slot every period and fly towards the opposing slot or the
 * ground near it, each wrapped in a ring of sprites and spinning Gouraud
 * strips and trailing a ribbon and a pair of flares. A bolt that lands
 * bursts into animated sparks, rising smoke and falling debris while the
 * ambient light dims and recovers. */

static const VECTOR gModelVariant821_D3920 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

/* The submissions below are unbraced on purpose: the block notes braces add
 * lengthen the shared "&p" lifetime enough for loop.c to hoist it out of
 * the draw loop, which changes the register allocation. */

s32 ModelVariant821_HandleRequest(u8 *context, s32 command)
{
    Model821State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 g, i, j, k;
    s32 b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant821_D3920;
    work = (Model821State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant821_D39BC[command];
        for (g = 0; g < 3; g++) {
            work->head[g] = 0;
            for (i = 0; i < 10; i++) {
                setVector(&work->pos[g][i], 0, 0, 0);
                setVector(&work->step[g][i], 0, 0, 0);
                setVector(&work->target[g][i], 0, 0, 0);
                setVector(&work->angle[g][i], 0, 0, 0);
                work->state[g][i] = 0;
                work->r[g][i] = work->config->r;
                work->g[g][i] = work->config->g;
                work->b[g][i] = work->config->b;
                work->flight[g][i] = 0;
                work->trail_head[g][i] = 0;
                work->tail[g][i] = 0;
                work->spark_count[g][i] = 0;
                work->puff_frame[g][i] = 0;
                work->flare_frame[g][i] = 0;
                for (j = 0; j < 16; j++) {
                    setVector(&work->trail[g][i][j], 0, 0, 0);
                }
                /* Reusing K for the first angle fixes the register allocation. */
                for (j = 0; j < 8; j++) {
                    k = rand() % 4096;
                    b = rand() % 4096;
                    setVector(&work->ring[g][i][j],
                              work->config->size * 3 / 4 * ccos(k) / 4096 * ccos(b) / 4096,
                              work->config->size * 3 / 4 * csin(k) / 4096 * ccos(b) / 4096,
                              work->config->size * 3 / 4 * ccos(k) / 4096 * csin(b) / 4096);
                }
                for (j = 0; j < 6; j++) {
                    setVector(&work->sparks[g][i][j], 0, 0, 0);
                    work->spark_frame[g][i][j] = rand() % 4;
                }
                for (j = 0; j < 5; j++) {
                    setVector(&work->puffs[g][i][j], RANDOM_SPREAD(work->config->puff_spread),
                              -work->config->puff_spread * (rand() % 4096) / 4096,
                              RANDOM_SPREAD(work->config->puff_spread));
                }
                for (j = 0; j < 8; j++) {
                    setVector(&work->debris[g][i][j], 0, 0, 0);
                    setVector(&work->debris_velocity[g][i][j], RANDOM_SPREAD(work->config->debris_spread),
                              -work->config->debris_spread * (rand() % 4096) / 4096,
                              RANDOM_SPREAD(work->config->debris_spread));
                }
                for (j = 0; j < 2; j++) {
                    setVector(&work->flares[g][i][j], 0, 0, 0);
                    setVector(&work->flare_velocity[g][i][j], RANDOM_SPREAD(work->config->flare_spread),
                              RANDOM_SPREAD(work->config->flare_spread),
                              direction * work->config->flare_spread * (rand() % 4096) / 4096);
                }
            }
        }
        for (g = 0; g < 5; g++) {
            work->texture[g] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant821_D3930[g]);
        }
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant821_D3930[3]);
        work->texture[5] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant821_D3930[3]);
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->frames = 0;
        work->side = rand() % 2;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->side == 0) {
        if (work->elapsed < work->config->start) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -12);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -64);
        }
    } else {
        if (work->elapsed < work->config->start) {
            Model_QueueCameraOrbitFromPreset(slot, 3, &rot, -12);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -64);
        }
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 32;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step * 32;
        } else {
            work->phase = 3;
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
    if (work->elapsed <= work->config->end && work->frames % work->config->period == 0) {
        for (g = 0; g < 3; g++) {
            if (work->head[g] < 10) {
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part[g]), &m);
                setVector(&pos, m.t[0], m.t[1], m.t[2]);
                for (i = 0; i < 2; i++) {
                    setVector(&work->flares[g][work->head[g]][i], m.t[0], m.t[1], m.t[2] + direction * 32);
                }
                for (i = 0; i < 16; i++) {
                    setVector(&work->trail[g][work->head[g]][i], m.t[0], m.t[1], m.t[2]);
                }
                work->trail_head[g][work->head[g]] = (work->trail_head[g][work->head[g]] + 1) % 16;
                copyVector(&work->pos[g][work->head[g]], &pos);
                Model_CopySlotPosition(slot ^ 1, (u16 *)&pos);
                Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rot);
                if (rand() % 100 > work->config->chance) {
                    setVector(&work->target[g][work->head[g]], pos.vx + RANDOM_SPREAD(work->config->spread), 0,
                              pos.vz + RANDOM_SPREAD(work->config->spread));
                    work->state[g][work->head[g]] = 0;
                    for (i = 0; i < 6; i++) {
                        setVector(&work->sparks[g][work->head[g]][i], RANDOM_SPREAD(work->config->spark_spread),
                                  -work->config->spark_height, RANDOM_SPREAD(work->config->spark_spread));
                    }
                } else {
                    setVector(&work->target[g][work->head[g]], pos.vx + RANDOM_SPREAD(rot.vx / 2),
                              pos.vy + RANDOM_SPREAD(rot.vy / 2),
                              pos.vz + rot.vz / 2 * -direction * (rand() % 4096) / 4096);
                    work->state[g][work->head[g]] = 1;
                    for (i = 0; i < 6; i++) {
                        setVector(&work->sparks[g][work->head[g]][i], RANDOM_SPREAD(work->config->spark_spread),
                                  RANDOM_SPREAD(work->config->spark_spread),
                                  RANDOM_SPREAD(work->config->spark_spread));
                    }
                }
                setVector(&work->step[g][work->head[g]],
                          (work->target[g][work->head[g]].vx - work->pos[g][work->head[g]].vx) / work->config->steps,
                          (work->target[g][work->head[g]].vy - work->pos[g][work->head[g]].vy) / work->config->steps,
                          (work->target[g][work->head[g]].vz - work->pos[g][work->head[g]].vz) / work->config->steps);
                setVector(&work->angle[g][work->head[g]],
                          ratan2(work->step[g][work->head[g]].vz, work->step[g][work->head[g]].vy) - 0x400,
                          direction * ratan2(work->step[g][work->head[g]].vx, work->step[g][work->head[g]].vz), 0);
                work->head[g]++;
            }
        }
    }

    for (g = 0; g < 3; g++) {
        for (i = 0; i < work->head[g]; i++) {
            if (work->state[g][i] < 2) {
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
                setVector(&quad[0], -work->config->size, -work->config->size, 0);
                setVector(&quad[1], work->config->size, -work->config->size, 0);
                setVector(&quad[2], -work->config->size, work->config->size, 0);
                setVector(&quad[3], work->config->size, work->config->size, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[g][i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x80, 0x77, 0xBF, 0x77, 0x80, 0xB6, 0xBF, 0xB6);
                if (otz >= 0)
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }

            if (work->r[g][i] || work->g[g][i] || work->b[g][i]) {
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->r[g][i], work->g[g][i], work->b[g][i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[g][i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 8; j++) {
                    setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
                    setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
                    setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
                    setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
                    addVector(&quad[0], &work->ring[g][i][j]);
                    addVector(&quad[1], &work->ring[g][i][j]);
                    addVector(&quad[2], &work->ring[g][i][j]);
                    addVector(&quad[3], &work->ring[g][i][j]);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, 0, 0x77, 0x3F, 0x77, 0, 0xB6, 0x3F, 0xB6);
                    if (otz >= 0)
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }

                for (j = 0; j < 8; j++) {
                    setPolyG3(&g3);
                    setRGB0(&g3, work->r[g][i], work->g[g][i], work->b[g][i]);
                    setRGB1(&g3, work->r[g][i], work->g[g][i], work->b[g][i]);
                    setRGB2(&g3, 0, 0, 0);
                    setVector(&quad[0], 0, -work->config->strip_width, 0);
                    setVector(&quad[1], 0, work->config->strip_width, 0);
                    if (i % 4 == 0) {
                        setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                                  work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                                  work->config->steps / (13 - work->flight[g][i]) * -work->step[g][i].vz);
                        setVector(&quad[3], quad[2].vx, quad[2].vy, quad[2].vz * 2);
                    } else if (i % 4 == 1) {
                        setVector(&quad[2], -work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                                  work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                                  work->config->steps / (11 - work->flight[g][i]) * -work->step[g][i].vz);
                        setVector(&quad[3], quad[2].vx, quad[2].vy, quad[2].vz * 2);
                    } else if (i % 4 == 2) {
                        setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                                  -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                                  work->config->steps / (12 - work->flight[g][i]) * -work->step[g][i].vz);
                        setVector(&quad[3], quad[2].vx, quad[2].vy, quad[2].vz * 2);
                    } else if (i % 4 == 3) {
                        setVector(&quad[2], work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                                  -work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                                  work->config->steps / (12 - work->flight[g][i]) * -work->step[g][i].vz);
                        setVector(&quad[3], quad[2].vx, quad[2].vy, quad[2].vz * 2);
                    }
                    for (k = 0; k < 2; k++) {
                        copyVector(&pos, &work->pos[g][i]);
                        addVector(&pos, &work->ring[g][i][j]);
                        copyVector(&rot, &work->angle[g][i]);
                        rot.vz += k * 1024 % 4096;
                        GsSetLsMatrix(&light);
                        RotTrans(&pos, (VECTOR *)m.t, &flag);
                        RotMatrix(&rot, &m);
                        MulMatrix2(&light, &m);
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                          (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                        if (otz >= 0)
                            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                        otz = RotAverage3(&quad[0], &quad[1], &quad[3], (PSXLONG *)&g3.x0,
                                          (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                        if (otz >= 0)
                            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                    }
                }

                if (work->state[g][i] >= 2) {
                    setPolyFT4(&ft4);
                    ft4.clut = work->texture[4];
                    ft4.tpage = work->texture[4] >> 16;
                    setRGB0(&ft4, work->r[g][i], work->g[g][i], work->b[g][i]);
                    setVector(&pos, 0, 0, 0);
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    GsSetLsMatrix(&light);
                    RotTrans(&work->target[g][i], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    for (j = 0; j < work->spark_count[g][i]; j++) {
                        setVector(&quad[0], -work->config->spark_width, -work->config->spark_height, 0);
                        setVector(&quad[1], work->config->spark_width, -work->config->spark_height, 0);
                        setVector(&quad[2], -work->config->spark_width, work->config->spark_height, 0);
                        setVector(&quad[3], work->config->spark_width, work->config->spark_height, 0);
                        addVector(&quad[0], &work->sparks[g][i][j]);
                        addVector(&quad[1], &work->sparks[g][i][j]);
                        addVector(&quad[2], &work->sparks[g][i][j]);
                        addVector(&quad[3], &work->sparks[g][i][j]);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                        setUV4(&ft4, work->spark_frame[g][i][j] % 4 * 64, 0,
                               work->spark_frame[g][i][j] % 4 * 64 + 0x3F, 0,
                               work->spark_frame[g][i][j] % 4 * 64, 0x56,
                               work->spark_frame[g][i][j] % 4 * 64 + 0x3F, 0x56);
                        if (otz >= 0)
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                        work->spark_frame[g][i][j] = (work->spark_frame[g][i][j] + 1) % 4;
                    }

                    if (work->puff_frame[g][i] < 16) {
                        GsSetLsMatrix(&light);
                        GsSetLsMatrix(&m);
                        ft4.clut = work->texture[3];
                        ft4.tpage = work->texture[3] >> 16;
                        setRGB0(&ft4, 0x40, 0x40, 0x40);
                        for (j = 0; j < 5; j++) {
                            setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
                            setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
                            setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
                            setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
                            addVector(&quad[0], &work->puffs[g][i][j]);
                            addVector(&quad[1], &work->puffs[g][i][j]);
                            addVector(&quad[2], &work->puffs[g][i][j]);
                            addVector(&quad[3], &work->puffs[g][i][j]);
                            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p,
                                              &flag);
                            setUV4(&ft4, work->puff_frame[g][i] / 2 * 32, 0x57,
                                   work->puff_frame[g][i] / 2 * 32 + 0x1F, 0x57,
                                   work->puff_frame[g][i] / 2 * 32, 0x76,
                                   work->puff_frame[g][i] / 2 * 32 + 0x1F, 0x76);
                            if (otz >= 0)
                                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                            work->puffs[g][i][j].vy -= work->config->puff_rise;
                        }
                        work->puff_frame[g][i]++;
                    }

                    ft4.clut = work->texture[1];
                    ft4.tpage = work->texture[1] >> 16;
                    setRGB0(&ft4, work->r[g][i], work->g[g][i], work->b[g][i]);
                    for (j = 0; j < 8; j++) {
                        setVector(&quad[0], -work->config->debris_size, -work->config->debris_size, 0);
                        setVector(&quad[1], work->config->debris_size, -work->config->debris_size, 0);
                        setVector(&quad[2], -work->config->debris_size, work->config->debris_size, 0);
                        setVector(&quad[3], work->config->debris_size, work->config->debris_size, 0);
                        addVector(&quad[0], &work->debris[g][i][j]);
                        addVector(&quad[1], &work->debris[g][i][j]);
                        addVector(&quad[2], &work->debris[g][i][j]);
                        addVector(&quad[3], &work->debris[g][i][j]);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                        setUV4(&ft4, 0, 0x77, 0x3F, 0x77, 0, 0xB6, 0x3F, 0xB6);
                        if (otz >= 0)
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                        addVector(&work->debris[g][i][j], &work->debris_velocity[g][i][j]);
                        work->debris_velocity[g][i][j].vy++;
                    }
                    work->spark_count[g][i] += step;
                    if (work->spark_count[g][i] > 6) {
                        work->spark_count[g][i] = 6;
                    }
                    if (work->r[g][i] >= 16) {
                        work->r[g][i] -= work->config->fade;
                    } else {
                        work->r[g][i] = 0;
                    }
                    if (work->g[g][i] >= 16) {
                        work->g[g][i] -= work->config->fade;
                    } else {
                        work->g[g][i] = 0;
                    }
                    if (work->b[g][i] >= 16) {
                        work->b[g][i] -= work->config->fade;
                    } else {
                        work->b[g][i] = 0;
                    }
                }
            }

            if (work->state[g][i] < 2) {
                copyVector(&work->trail[g][i][work->trail_head[g][i]], &work->pos[g][i]);
                work->trail_head[g][i] = (work->trail_head[g][i] + 1) % 16;
                addVector(&work->pos[g][i], &work->step[g][i]);
                if (++work->flight[g][i] == 8) {
                    work->flight[g][i] = 7;
                }
                if (work->state[g][i] == 0 && work->pos[g][i].vy >= 0) {
                    work->state[g][i] += 2;
                }
                if (work->state[g][i] == 1) {
                    if (direction > 0) {
                        if (work->pos[g][i].vz > work->target[g][i].vz) {
                            work->state[g][i] += 2;
                            if (work->phase == 0) {
                                work->phase = 1;
                            }
                        }
                    } else {
                        if (work->pos[g][i].vz < work->target[g][i].vz) {
                            work->state[g][i] += 2;
                            if (work->phase == 0) {
                                work->phase = 1;
                            }
                        }
                    }
                }
            }

            if (work->trail_head[g][i] > work->tail[g][i] && (work->r[g][i] || work->g[g][i] || work->b[g][i])) {
                setPolyG4(&g4);
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
                for (j = work->trail_head[g][i] - 1; j >= work->trail_head[g][i] - (work->tail[g][i] + 1); j--) {
                    setRGB0(&g4, work->r[g][i] / k, work->g[g][i] / k, work->b[g][i] / k);
                    setRGB1(&g4, work->r[g][i] / k, work->g[g][i] / k, work->b[g][i] / k);
                    setRGB2(&g4, work->r[g][i] / (k + 1), work->g[g][i] / (k + 1), work->b[g][i] / (k + 1));
                    setRGB3(&g4, work->r[g][i] / (k + 1), work->g[g][i] / (k + 1), work->b[g][i] / (k + 1));
                    setVector(&quad[0], work->trail[g][i][j].vx, work->trail[g][i][j].vy - work->config->size / k,
                              work->trail[g][i][j].vz);
                    setVector(&quad[1], work->trail[g][i][j].vx, work->trail[g][i][j].vy + work->config->size / k,
                              work->trail[g][i][j].vz);
                    setVector(&quad[2], work->trail[g][i][j - 1].vx,
                              work->trail[g][i][j - 1].vy - work->config->size / (k + 1), work->trail[g][i][j - 1].vz);
                    setVector(&quad[3], work->trail[g][i][j - 1].vx,
                              work->trail[g][i][j - 1].vy + work->config->size / (k + 1), work->trail[g][i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0)
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    setVector(&quad[0], work->trail[g][i][j].vx - work->config->size / k, work->trail[g][i][j].vy,
                              work->trail[g][i][j].vz);
                    setVector(&quad[1], work->trail[g][i][j].vx + work->config->size / k, work->trail[g][i][j].vy,
                              work->trail[g][i][j].vz);
                    setVector(&quad[2], work->trail[g][i][j - 1].vx - work->config->size / (k + 1),
                              work->trail[g][i][j - 1].vy, work->trail[g][i][j - 1].vz);
                    setVector(&quad[3], work->trail[g][i][j - 1].vx + work->config->size / (k + 1),
                              work->trail[g][i][j - 1].vy, work->trail[g][i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0)
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    k++;
                }
                if (work->state[g][i] < 2) {
                    work->tail[g][i]++;
                }
            }

            if (work->flare_frame[g][i] < 8) {
                setPolyFT4(&ft4);
                ft4.clut = work->texture[5];
                ft4.tpage = work->texture[5] >> 16;
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                setUV4(&ft4, work->flare_frame[g][i] * 32, 0x57, work->flare_frame[g][i] * 32 + 0x1F, 0x57,
                       work->flare_frame[g][i] * 32, 0x76, work->flare_frame[g][i] * 32 + 0x1F, 0x76);
                setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
                setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
                setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
                setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
                for (j = 0; j < 2; j++) {
                    GsSetLsMatrix(&light);
                    RotTrans(&work->flares[g][i][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    if (otz >= 0)
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    addVector(&work->flares[g][i][j], &work->flare_velocity[g][i][j]);
                }
                work->flare_frame[g][i]++;
            }
        }
    }

    work->elapsed += step;
    work->frames++;
    PopMatrix();
    if (work->r[0][work->head[0] - 1] == 0 && work->g[0][work->head[0] - 1] == 0 &&
        work->b[0][work->head[0] - 1] == 0 && work->r[1][work->head[1] - 1] == 0 &&
        work->g[1][work->head[1] - 1] == 0 && work->b[1][work->head[1] - 1] == 0 &&
        work->r[2][work->head[2] - 1] == 0 && work->g[2][work->head[2] - 1] == 0 &&
        work->b[2][work->head[2] - 1] == 0 && work->phase == 3) {
        return 2;
    }
    if (work->elapsed >= work->config->end && work->state[0][work->head[0] - 1] >= 2 &&
        work->state[1][work->head[1] - 1] >= 2 && work->state[2][work->head[2] - 1] >= 2 && work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    if (work->phase == 1) {
        return 4;
    }
    return 0;
}

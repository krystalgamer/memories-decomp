#include "../../types.h"
#include "variant785_entry.h"

/* Header 785's entry: two groups of projectiles leave a model point with a
 * random spread and arc under gravity, each wrapped in a ring of sprites
 * and trailing swirling strips and a fading ribbon, while smoke puffs drift
 * from the origin and a flash flickers. Landed first-group projectiles
 * bloom into a three-layer growing fan with animated sparks; second-group
 * ones into a two-layer fan. The ambient light dims and recovers. */

static const VECTOR gModelVariant785_D3CAC = {4096, 4096, 4096, 0};

s32 ModelVariant785_HandleRequest(u8 *context, s32 command)
{
    Model785State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_G4 g4;
    POLY_GT4 gt4;
    POLY_FT4 glow;
    POLY_FT4 puff;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    SVECTOR head;
    SVECTOR prev;
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, g;
    s32 j, k;
    s32 b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = gModelVariant785_D3CAC;
    work = (Model785State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant785_D3D48[command];
        for (i = 0; i < work->config->count[0]; i++) {
            work->glow_r[i] = work->config->r;
            work->glow_g[i] = work->config->g;
            work->glow_b[i] = work->config->b;
            work->fans.s.growth[i] = 0;
            work->spark_count[i] = 0;
            for (j = 0; j < work->config->spark_count; j++) {
                work->spark_frame[i][j] = 0;
            }
        }
        for (i = 0; i < work->config->count[1]; i++) {
            work->glow2_r[i] = work->config->r;
            work->glow2_g[i] = work->config->g;
            work->glow2_b[i] = work->config->b;
            work->fans.s.growth2[i] = 0;
            for (j = 0; j < work->config->spark2_count; j++) {
                work->spark2_frame[i][j] = 0;
            }
            work->spark2_count[i] = 0;
        }
        for (k = 0; k < 2; k++) {
            for (i = 0; i < work->config->count[k]; i++) {
                work->bolt_r[i][k] = work->config->r;
                work->bolt_g[i][k] = work->config->g;
                work->bolt_b[i][k] = work->config->b;
                work->tail[i][k] = 0;
                work->strip[i][k] = 0;
                work->trail_head[i][k] = 0;
                work->arrived[i][k] = 0;
                setVector(&work->pos[i][k], 0, 0, 0);
                setVector(&work->step[i][k],
                          work->config->spread[k] * ((rand() - rand()) % 4096) / 4096 / work->config->steps,
                          -(work->config->lift[k] / 6 * (rand() % 8)) / work->config->steps,
                          direction * (work->config->spread[k] * (rand() % 4096) / 4096) / work->config->steps);
                for (j = 0; j < work->config->ring_count; j++) {
                    g = rand() % 4096;
                    b = rand() % 4096;
                    setVector(&work->ring[i][j][k],
                              work->config->size[k] * ccos(g) / 4096 * ccos(b) / 4096,
                              work->config->size[k] * csin(g) / 4096 * ccos(b) / 4096,
                              work->config->size[k] * ccos(g) / 4096 * csin(b) / 4096);
                }
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->fan_inner[i], work->config->fan_inner * ccos(i * 256 % 4096) / 4096, 0,
                      work->config->fan_inner * csin(i * 256 % 4096) / 4096);
            setVector(&work->fan_outer[i], work->config->fan_outer * ccos(i * 256 % 4096) / 4096, -work->config->fan_depth,
                      work->config->fan_outer * csin(i * 256 % 4096) / 4096);
            setVector(&work->fan2_inner[i], work->config->fan2_inner * ccos(i * 256 % 4096) / 4096, 0,
                      work->config->fan2_inner * csin(i * 256 % 4096) / 4096);
            setVector(&work->fan2_outer[i], work->config->fan2_outer * ccos(i * 256 % 4096) / 4096, -work->config->fan2_depth,
                      work->config->fan2_outer * csin(i * 256 % 4096) / 4096);
        }
        for (i = 0; i < work->config->spark_count; i++) {
            setVector(&work->sparks[i], work->config->fan_outer * ((rand() - rand()) % 4096) / 4096, 0,
                      work->config->fan_outer * ((rand() - rand()) % 4096) / 4096);
        }
        for (i = 0; i < work->config->spark2_count; i++) {
            setVector(&work->sparks2[i], work->config->fan2_outer * ((rand() - rand()) % 4096) / 4096, 0,
                      work->config->fan2_outer * ((rand() - rand()) % 4096) / 4096);
        }
        for (i = 0; i < work->config->puff_count; i++) {
            g = rand() % 4096 - rand() % 4096;
            b = rand() % 4096 - rand() % 4096;
            k = rand() % 4096;
            setVector(&work->puffs[i], work->config->puff_spread * g / 4096, work->config->puff_spread * b / 4096,
                      direction * work->config->puff_spread * k / 4096);
            setVector(&work->puff_velocity[i], work->config->puff_speed * g / 4096,
                      work->config->puff_speed * b / 4096, direction * work->config->puff_speed * k / 4096);
            work->puff_frame[i] = 0;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant785_D3CBC[i]);
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->flash_frame = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 32);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 32);
    }
    if (work->phase == 0) {
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
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    if (work->phase == 0) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&work->origin, m.t[0], m.t[1], m.t[2]);
        for (g = 0; g < 2; g++) {
            for (i = 0; i < work->config->count[g]; i++) {
                setVector(&work->pos[i][g], m.t[0], m.t[1], m.t[2]);
                for (j = 0; j < 64; j++) {
                    setVector(&work->trail[i][j][g], m.t[0], m.t[1], m.t[2]);
                }
            }
        }
        work->phase = 1;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();

    setPolyFT4(&puff);
    puff.tpage = work->texture[4] >> 16;
    puff.clut = work->texture[4];
    setRGB0(&puff, 0x80, 0x80, 0x80);
    setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
    setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
    setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
    setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
    for (i = 0; i < work->config->puff_count; i++) {
        if (work->puff_frame[i] < 16) {
            copyVector(&pos, &work->origin);
            addVector(&pos, &work->puffs[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&puff.x0, (PSXLONG *)&puff.x1,
                              (PSXLONG *)&puff.x2, (PSXLONG *)&puff.x3, &p, &flag);
            if (work->puff_frame[i] < 8) {
                setUV4(&puff, work->puff_frame[i] / 2 % 4 * 32 + 0x40, 0x40, work->puff_frame[i] / 2 % 4 * 32 + 0x5F,
                       0x40, work->puff_frame[i] / 2 % 4 * 32 + 0x40, 0x5F, work->puff_frame[i] / 2 % 4 * 32 + 0x5F,
                       0x5F);
            } else {
                setUV4(&puff, work->puff_frame[i] / 2 % 4 * 32 + 0x40, 0x60, work->puff_frame[i] / 2 % 4 * 32 + 0x5F,
                       0x60, work->puff_frame[i] / 2 % 4 * 32 + 0x40, 0x7F, work->puff_frame[i] / 2 % 4 * 32 + 0x5F,
                       0x7F);
            }
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&puff, ot, (u16)otz, 1);
            }
            work->puff_frame[i]++;
            addVector(&work->puffs[i], &work->puff_velocity[i]);
        }
    }

    if (work->flash_frame < 16) {
        setPolyFT4(&glow);
        glow.tpage = work->texture[0] >> 16;
        glow.clut = work->texture[0];
        setRGB0(&glow, 0x80, 0x80, 0x80);
        setVector(&quad[0], -work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[1], work->config->flash_size, -work->config->flash_size, 0);
        setVector(&quad[2], -work->config->flash_size, work->config->flash_size, 0);
        setVector(&quad[3], work->config->flash_size, work->config->flash_size, 0);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                          (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
        if (work->flash_frame < 8) {
            setUV4(&glow, work->flash_frame / 2 % 4 * 32 + 0x80, 0x80, work->flash_frame / 2 % 4 * 32 + 0x9F, 0x80,
                   work->flash_frame / 2 % 4 * 32 + 0x80, 0x9F, work->flash_frame / 2 % 4 * 32 + 0x9F, 0x9F);
        } else {
            setUV4(&glow, work->flash_frame / 2 % 4 * 32 + 0x80, 0xA0, work->flash_frame / 2 % 4 * 32 + 0x9F, 0xA0,
                   work->flash_frame / 2 % 4 * 32 + 0x80, 0xBF, work->flash_frame / 2 % 4 * 32 + 0x9F, 0xBF);
        }
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
        }
        work->flash_frame++;
    }

    for (g = 0; g < 2; g++) {
        for (i = 0; i < work->config->count[g]; i++) {
            if (work->arrived[i][g] == 0) {
                setPolyFT4(&ft4);
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                ft4.tpage = work->texture[3] >> 16;
                ft4.clut = work->texture[3];
                setVector(&quad[0], -work->config->size[g], -work->config->size[g], 0);
                setVector(&quad[1], work->config->size[g], -work->config->size[g], 0);
                setVector(&quad[2], -work->config->size[g], work->config->size[g], 0);
                setVector(&quad[3], work->config->size[g], work->config->size[g], 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][g], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                setSemiTrans(&ft4, 0);
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, (u16)otz);
                }
            }
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                copyVector(&pos, &work->pos[i][g]);
                addVector(&pos, &work->ring[i][j][g]);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->bolt_r[i][g], work->bolt_g[i][g], work->bolt_b[i][g]);
                setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
                setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
                setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                  (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                setSemiTrans(&ft4, 1);
                if (otz >= 0) {
                    GsSortPoly(&ft4, ot, (u16)otz);
                }
            }
            if (work->pos[i][g].vy < 0) {
                copyVector(&work->trail[i][work->trail_head[i][g]][g], &work->pos[i][g]);
                work->trail_head[i][g] = (work->trail_head[i][g] + 1) % 64;
                addVector(&work->pos[i][g], &work->step[i][g]);
                work->step[i][g].vy += work->config->gravity;
                if (work->pos[i][g].vy > 0) {
                    work->arrived[i][g] = 1;
                    work->pos[i][g].vy = 0;
                }
            }
            if (work->bolt_r[i][g] || work->bolt_g[i][g] || work->bolt_b[i][g]) {
                setVector(&pos, 0, 0, 0);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&pos, (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                if (work->trail_head[i][g] > work->strip[i][g]) {
                    setPolyG3(&g3);
                    setRGB0(&g3, work->bolt_r[i][g], work->bolt_g[i][g], work->bolt_b[i][g]);
                    setRGB1(&g3, work->bolt_r[i][g], work->bolt_g[i][g], work->bolt_b[i][g]);
                    setRGB2(&g3, 0, 0, 0);
                    for (j = 0; j < work->config->ring_count; j++) {
                        copyVector(&head, &work->trail[i][work->trail_head[i][g] - work->strip[i][g]][g]);
                        addVector(&head, &work->ring[i][j][g]);
                        copyVector(&prev, &work->trail[i][work->trail_head[i][g] - 1][g]);
                        addVector(&prev, &work->ring[i][j][g]);
                        setVector(&quad[0], prev.vx, prev.vy - work->config->strip_width, prev.vz);
                        setVector(&quad[1], prev.vx, prev.vy + work->config->strip_width, prev.vz);
                        setVector(&quad[2], head.vx + work->config->strip_radius * ccos(work->elapsed * 64 % 4096) / 4096,
                                  head.vy + work->config->strip_radius * csin(work->elapsed * 64 % 4096) / 4096,
                                  head.vz);
                        otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                          (PSXLONG *)&g3.x2, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                        }
                        setVector(&quad[0], prev.vx - work->config->strip_width, prev.vy, prev.vz);
                        setVector(&quad[1], prev.vx + work->config->strip_width, prev.vy, prev.vz);
                        otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                          (PSXLONG *)&g3.x2, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                        }
                    }
                }
                setPolyG4(&g4);
                if (work->trail_head[i][g] >= work->tail[i][g]) {
                    setVector(&pos, 0, 0, 0);
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    GsSetLsMatrix(&light);
                    k = 1;
                    for (j = work->trail_head[i][g] - 1; work->trail_head[i][g] - work->tail[i][g] < j; j--) {
                        setRGB0(&g4, work->bolt_r[i][g] / k, work->bolt_g[i][g] / k, work->bolt_b[i][g] / k);
                        setRGB1(&g4, work->bolt_r[i][g] / k, work->bolt_g[i][g] / k, work->bolt_b[i][g] / k);
                        setRGB2(&g4, work->bolt_r[i][g] / (k + 1), work->bolt_g[i][g] / (k + 1),
                                work->bolt_b[i][g] / (k + 1));
                        setRGB3(&g4, work->bolt_r[i][g] / (k + 1), work->bolt_g[i][g] / (k + 1),
                                work->bolt_b[i][g] / (k + 1));
                        setVector(&quad[0], work->trail[i][j][g].vx, work->trail[i][j][g].vy - work->config->size[g] / k,
                                  work->trail[i][j][g].vz);
                        setVector(&quad[1], work->trail[i][j][g].vx, work->trail[i][j][g].vy + work->config->size[g] / k,
                                  work->trail[i][j][g].vz);
                        setVector(&quad[2], work->trail[i][j - 1][g].vx,
                                  work->trail[i][j - 1][g].vy - work->config->size[g] / (k + 1),
                                  work->trail[i][j - 1][g].vz);
                        setVector(&quad[3], work->trail[i][j - 1][g].vx,
                                  work->trail[i][j - 1][g].vy + work->config->size[g] / (k + 1),
                                  work->trail[i][j - 1][g].vz);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                          (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                        }
                        setVector(&quad[0], work->trail[i][j][g].vx - work->config->size[g] / k, work->trail[i][j][g].vy,
                                  work->trail[i][j][g].vz);
                        setVector(&quad[1], work->trail[i][j][g].vx + work->config->size[g] / k, work->trail[i][j][g].vy,
                                  work->trail[i][j][g].vz);
                        setVector(&quad[2], work->trail[i][j - 1][g].vx - work->config->size[g] / (k + 1),
                                  work->trail[i][j - 1][g].vy, work->trail[i][j - 1][g].vz);
                        setVector(&quad[3], work->trail[i][j - 1][g].vx + work->config->size[g] / (k + 1),
                                  work->trail[i][j - 1][g].vy, work->trail[i][j - 1][g].vz);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                          (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                        }
                        k++;
                    }
                }
                if (work->arrived[i][g]) {
                    if (work->bolt_r[i][g] >= 16) {
                        work->bolt_r[i][g] -= work->config->fade;
                    } else {
                        work->bolt_r[i][g] = 0;
                    }
                    if (work->bolt_g[i][g] >= 16) {
                        work->bolt_g[i][g] -= work->config->fade;
                    } else {
                        work->bolt_g[i][g] = 0;
                    }
                    if (work->bolt_b[i][g] >= 16) {
                        work->bolt_b[i][g] -= work->config->fade;
                    } else {
                        work->bolt_b[i][g] = 0;
                    }
                } else {
                    work->tail[i][g]++;
                    if (++work->strip[i][g] >= 6) {
                        work->strip[i][g] = 5;
                    }
                }
            }
        }
    }

    for (i = 0; i < work->config->count[0]; i++) {
        if (work->arrived[i][0]) {
            setPolyFT4(&glow);
            glow.tpage = work->texture[0] >> 16;
            glow.clut = work->texture[0];
            setRGB0(&glow, 0xFF, 0xFF, 0xFF);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&quad[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&quad[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&quad[3], work->config->spark_size, work->config->spark_size, 0);
            for (j = 0; j < work->spark_count[i]; j++) {
                if (work->spark_frame[i][j] < 8) {
                    copyVector(&pos, &work->pos[i][0]);
                    addVector(&pos, &work->sparks[j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    if (work->spark_frame[i][j] < 4) {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0x80,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x80, work->spark_frame[i][j] % 4 * 32 + 0x80,
                               0x9F, work->spark_frame[i][j] % 4 * 32 + 0x9F, 0x9F);
                    } else {
                        setUV4(&glow, work->spark_frame[i][j] % 4 * 32 + 0x80, 0xA0,
                               work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xA0, work->spark_frame[i][j] % 4 * 32 + 0x80,
                               0xBF, work->spark_frame[i][j] % 4 * 32 + 0x9F, 0xBF);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    }
                    work->spark_frame[i][j]++;
                }
            }
            if (work->glow_r[i] || work->glow_g[i] || work->glow_b[i]) {
                setPolyGT4(&gt4);
                gt4.tpage = work->texture[2] >> 16;
                gt4.clut = work->texture[2];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setRGB2(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB3(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, (work->fans.s.growth[i] >> 4) + 0x1000, work->fans.s.growth[i] + 0x1000,
                          (work->fans.s.growth[i] >> 4) + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][0], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, (work->fans.s.growth[i] >> 3) + 0x1000, work->fans.s.growth[i] >> 2,
                          (work->fans.s.growth[i] >> 3) + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                setVector(&scale, (work->fans.s.growth[i] >> 1) + 0x1000, work->fans.s.growth[i] >> 3,
                          (work->fans.s.growth[i] >> 1) + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan_outer[15], &work->fan_outer[0], &work->fan_inner[15],
                                          &work->fan_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan_outer[j], &work->fan_outer[j + 1], &work->fan_inner[j],
                                          &work->fan_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                if (++work->spark_count[i] > work->config->spark_count) {
                    work->spark_count[i] = work->config->spark_count;
                }
                if (work->fans.s.growth[i] < 20000) {
                    work->fans.s.growth[i] += work->config->grow;
                } else {
                    work->fans.s.growth[i] += work->config->grow_late;
                    if (work->glow_r[i] >= 16) {
                        work->glow_r[i] -= work->config->glow_fade;
                    } else {
                        work->glow_r[i] = 0;
                    }
                    if (work->glow_g[i] >= 16) {
                        work->glow_g[i] -= work->config->glow_fade;
                    } else {
                        work->glow_g[i] = 0;
                    }
                    if (work->glow_b[i] >= 16) {
                        work->glow_b[i] -= work->config->glow_fade;
                    } else {
                        work->glow_b[i] = 0;
                    }
                }
            }
        }
    }

    for (i = 0; i < work->config->count[1]; i++) {
        if (work->arrived[i][1]) {
            setPolyFT4(&glow);
            glow.tpage = work->texture[0] >> 16;
            glow.clut = work->texture[0];
            setRGB0(&glow, 0xFF, 0xFF, 0xFF);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            setVector(&quad[0], -work->config->spark2_size, -work->config->spark2_size, 0);
            setVector(&quad[1], work->config->spark2_size, -work->config->spark2_size, 0);
            setVector(&quad[2], -work->config->spark2_size, work->config->spark2_size, 0);
            setVector(&quad[3], work->config->spark2_size, work->config->spark2_size, 0);
            for (j = 0; j < work->spark2_count[i]; j++) {
                if (work->spark2_frame[i][j] < 8) {
                    copyVector(&pos, &work->pos[i][1]);
                    addVector(&pos, &work->sparks2[j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                      (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    if (work->spark2_frame[i][j] < 4) {
                        setUV4(&glow, work->spark2_frame[i][j] % 4 * 32 + 0x80, 0x80,
                               work->spark2_frame[i][j] % 4 * 32 + 0x9F, 0x80,
                               work->spark2_frame[i][j] % 4 * 32 + 0x80, 0x9F,
                               work->spark2_frame[i][j] % 4 * 32 + 0x9F, 0x9F);
                    } else {
                        setUV4(&glow, work->spark2_frame[i][j] % 4 * 32 + 0x80, 0xA0,
                               work->spark2_frame[i][j] % 4 * 32 + 0x9F, 0xA0,
                               work->spark2_frame[i][j] % 4 * 32 + 0x80, 0xBF,
                               work->spark2_frame[i][j] % 4 * 32 + 0x9F, 0xBF);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    }
                    work->spark2_frame[i][j]++;
                }
            }
            if (work->glow2_r[i] || work->glow2_g[i] || work->glow2_b[i]) {
                setPolyGT4(&gt4);
                gt4.tpage = work->texture[2] >> 16;
                gt4.clut = work->texture[2];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setRGB2(&gt4, work->glow2_r[i], work->glow2_g[i], work->glow2_b[i]);
                setRGB3(&gt4, work->glow2_r[i], work->glow2_g[i], work->glow2_b[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, (work->fans.s.growth2[i] >> 3) + 0x1000, work->fans.s.growth2[i] + 0x1000,
                          (work->fans.s.growth2[i] >> 3) + 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][1], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan2_outer[15], &work->fan2_outer[0], &work->fan2_inner[15],
                                          &work->fan2_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan2_outer[j], &work->fan2_outer[j + 1], &work->fan2_inner[j],
                                          &work->fan2_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                /* The second layer scales by the first group's growth, indexed by this
                 * group's counter: from i == 3 on that is the second group's growth. */
                setVector(&scale, (work->fans.all[i] >> 1) + 0x1000, work->fans.all[i] >> 3,
                          (work->fans.all[i] >> 1) + 0x1000);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->fan2_outer[15], &work->fan2_outer[0], &work->fan2_inner[15],
                                          &work->fan2_inner[0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->fan2_outer[j], &work->fan2_outer[j + 1], &work->fan2_inner[j],
                                          &work->fan2_inner[j + 1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0, 0x90, 0x7F, 0x90, 0, 0xDF, 0x7F, 0xDF);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                    }
                }
                if (++work->spark2_count[i] > work->config->spark2_count) {
                    work->spark2_count[i] = work->config->spark2_count;
                }
                if (work->fans.s.growth2[i] < 20000) {
                    work->fans.s.growth2[i] += work->config->grow;
                } else {
                    work->fans.s.growth2[i] += work->config->grow_late;
                    if (work->glow2_r[i] >= 16) {
                        work->glow2_r[i] -= work->config->glow_fade;
                    } else {
                        work->glow2_r[i] = 0;
                    }
                    if (work->glow2_g[i] >= 16) {
                        work->glow2_g[i] -= work->config->glow_fade;
                    } else {
                        work->glow2_g[i] = 0;
                    }
                    if (work->glow2_b[i] >= 16) {
                        work->glow2_b[i] -= work->config->glow_fade;
                    } else {
                        work->glow2_b[i] = 0;
                    }
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 4) {
        return 2;
    }
    if (work->phase == 2 && work->elapsed > work->config->end) {
        work->phase = 3;
        return 0;
    }
    if (work->arrived[0][0] == 2 && work->arrived[1][0] == 2 && work->arrived[2][0] == 2 && work->phase == 1) {
        work->phase = 2;
        return 1;
    }
    if (work->arrived[0][0] == 1) {
        work->arrived[0][0] = 2;
        return 4;
    }
    if (work->arrived[1][0] == 1) {
        work->arrived[1][0] = 2;
        return 4;
    }
    if (work->arrived[2][0] == 1) {
        work->arrived[2][0] = 2;
        return 4;
    }
    return 0;
}

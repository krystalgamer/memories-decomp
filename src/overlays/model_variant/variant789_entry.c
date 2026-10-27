#include "../../types.h"
#include "variant789_entry.h"

/* Header 789's entry: at six launch times a flare leaves a model point,
 * rises along a sine arc and flies off with a fading, shrinking trail of
 * sprites. When it lands it opens into a glow with two fans of rays, a
 * textured ring and a pulsing sprite that grow and fade while the ambient
 * light dims and recovers. */

static const VECTOR D_8013CDBC = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Model789State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_FT4 glow;
    POLY_GT4 gt4;
    SVECTOR rot;
    VECTOR scale;
    SVECTOR quad[4];
    SVECTOR delta;
    PSXLONG flag;
    PSXLONG p;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i, j;
    s32 w;
    s32 otz;

    memset(&rot, 0, 8);
    scale = D_8013CDBC;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    work = (Model789State *)context;
    if (command >= 0) {
        work->config = &D_8013CE20[command];
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->velocity[i], RANDOM_SPREAD(work->config->spread),
                      RANDOM_SPREAD(work->config->spread) - 350, direction * 450);
            if (!(rand() & 1)) {
                work->arc[i].vx = work->config->lift + (rand() - rand()) % (work->config->lift / 2);
            } else {
                work->arc[i].vx = -(work->config->lift + (rand() - rand()) % (work->config->lift / 2));
            }
            work->arc[i].vy = -(work->config->lift + (rand() - rand()) % (work->config->lift / 4));
            work->arc[i].vz = -direction * (work->config->lift + (rand() - rand()) % (work->config->lift / 4));
            setVector(&work->start[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            work->frame[i] = 0;
            work->scale[i] = 0x1000;
            work->state[i] = 0;
            work->unknown_810[i] = 0;
            work->r[i] = work->config->r;
            work->g[i] = work->config->g;
            work->b[i] = work->config->b;
            work->glow_r[i] = work->config->glow_r;
            work->glow_g[i] = work->config->glow_g;
            work->glow_b[i] = work->config->glow_b;
            work->ray[i] = 0x80;
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread));
            }
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring[i][0], work->config->ring_outer * ccos(i << 8) / 4096,
                      work->config->ring_outer * csin(i << 8) / 4096, 0);
            setVector(&work->ring[i][1], work->config->ring_inner * ccos(i << 8) / 4096,
                      work->config->ring_inner * csin(i << 8) / 4096, 0);
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CDCC[i]);
        }
        work->ambient = 0x800;
        work->count = 0;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->time[0]) {
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(2, 8, &rot, 16);
        } else {
            Model_QueueCameraOrbitFromPreset(2, 6, &rot, 16);
        }
    } else if (direction > 0) {
        Model_QueueCameraOrbitFromPreset(2, 10, &rot, 8);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 12, &rot, 8);
    }
    if (work->count == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step * 64;
        } else {
            work->phase = 2;
            work->ambient = 0x800;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->time[0]) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if ((work->count == 0 && work->elapsed >= work->config->time[0]) ||
        (work->count == 1 && work->elapsed >= work->config->time[1]) ||
        (work->count == 2 && work->elapsed >= work->config->time[2]) ||
        (work->count == 3 && work->elapsed >= work->config->time[3]) ||
        (work->count == 4 && work->elapsed >= work->config->time[4]) ||
        (work->count == 5 && work->elapsed >= work->config->time[5])) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part[work->count % 3]), &m);
        setVector(&work->pos[work->count], m.t[0], m.t[1], m.t[2]);
        setVector(&work->start[work->count], m.t[0], m.t[1], m.t[2]);
        work->state[work->count] = 1;
        work->count++;
    }

    for (i = 0; i < work->count; i++) {
        if (work->state[i] == 1 || work->state[i] == 2) {
            if (work->frame[i] < work->config->steps) {
                if (work->frame[i] <= work->config->steps / 2) {
                    copyVector(&work->pos[i], &work->start[i]);
                    /* The arc phase reuses J, which fixes the register allocation. */
                    j = 4096 / work->config->steps * work->frame[i] % 4096;
                    delta.vx = work->arc[i].vx * csin(j / 2) / 4096;
                    delta.vy = work->arc[i].vy * csin(j / 2) / 4096;
                    delta.vz = work->arc[i].vz * csin(j) / 4096;
                    setVector(&work->angle[i], ratan2(delta.vz, delta.vy) - 0x400,
                              ratan2(delta.vx, delta.vz) + 0x800, 0x400 - ratan2(delta.vy, delta.vx));
                    addVector(&work->pos[i], &delta);
                } else {
                    if (work->state[i] == 1) {
                        copyVector(&work->start[i], &work->velocity[i]);
                        work->velocity[i].vx -= work->pos[i].vx;
                        work->velocity[i].vx /= work->config->steps / 2;
                        work->velocity[i].vy -= work->pos[i].vy;
                        work->velocity[i].vy /= work->config->steps / 2;
                        work->velocity[i].vz -= work->pos[i].vz;
                        work->velocity[i].vz /= work->config->steps / 2;
                        work->state[i] = 2;
                    }
                    setVector(&work->angle[i], ratan2(work->velocity[i].vz, work->velocity[i].vy) - 0x400,
                              direction * ratan2(work->velocity[i].vx, work->velocity[i].vz),
                              ratan2(work->velocity[i].vy, work->velocity[i].vx) + 0x400);
                    addVector(&work->pos[i], &work->velocity[i]);
                }
                work->frame[i]++;
            } else {
                work->pos[i].vz = direction * 450;
                work->state[i] = 3;
            }
        }
        if (work->state[i] >= 3) {
            /* applyVector, not addVector: only the position goes through a pointer. */
            applyVector(&work->pos[i], work->velocity[i].vx, work->velocity[i].vy, work->velocity[i].vz, +=);
            if (work->scale[i] < 12000) {
                work->scale[i] += work->config->grow;
            } else {
                work->scale[i] += work->config->slow_grow;
                if (work->r[i] >= 16) {
                    work->r[i] -= work->config->fade;
                } else {
                    work->r[i] = 0;
                }
                if (work->g[i] >= 16) {
                    work->g[i] -= work->config->fade;
                } else {
                    work->g[i] = 0;
                }
                if (work->b[i] >= 16) {
                    work->b[i] -= work->config->fade;
                } else {
                    work->b[i] = 0;
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
                if (work->ray[i] >= 16) {
                    work->ray[i] -= work->config->fade;
                } else {
                    work->ray[i] = 0;
                }
            }
        }
    }

    for (i = 0; i < work->count; i++) {
        /* A once-set, unallocated constant: reload rematerialises it at each
         * use, giving the copied 0x7F of both sprites' right-hand UVs. */
        w = 0x7F;
        if (work->r[i] || work->g[i] || work->b[i]) {
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->r[i], work->g[i], work->b[i]);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < work->config->trail_count; j++) {
                setRGB0(&ft4, work->r[i] / (j + 1), work->g[i] / (j + 1), work->b[i] / (j + 1));
                setVector(&quad[0], j * 8 - work->config->width, 0,
                          -direction * (work->config->depth + work->config->depth * j));
                setVector(&quad[1], j * 8 - work->config->width, 0,
                          direction * (work->config->depth - work->config->depth * j));
                setVector(&quad[2], work->config->width - j * 8, 0,
                          -direction * (work->config->depth + work->config->depth * j));
                setVector(&quad[3], work->config->width - j * 8, 0,
                          direction * (work->config->depth - work->config->depth * j));
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0, 0, w, 0, 0, 0xBF, w, 0xBF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }
            }
        }
        if (work->state[i] >= 3) {
            /* The original tests the third ray level, not this flare's. */
            if (work->glow_r[i] || work->glow_g[i] || work->glow_b[i] || work->ray[2]) {
                setPolyG3(&g3);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setVector(&rot, 0, 0, 0);
                setVector(&quad[0], -work->config->ray_width, 0, 0);
                setVector(&quad[1], work->config->ray_width, 0, 0);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&g3, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->start[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }

                setPolyGT4(&gt4);
                gt4.tpage = work->texture[1] >> 16;
                gt4.clut = work->texture[1];
                setRGB0(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB1(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB2(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setRGB3(&gt4, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                setUV4(&gt4, 0x80, 0x40, 0x9F, 0x40, 0x80, 0xBF, 0x9F, 0xBF);
                setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->ring[15][0], &work->ring[0][0], &work->ring[15][1],
                                          &work->ring[0][1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->ring[j][0], &work->ring[j + 1][0], &work->ring[j][1],
                                          &work->ring[j + 1][1], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                    }
                }

                setVector(&scale, 0x800, 0x800, 0x800);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->ray[i], work->ray[i], work->ray[i]);
                setRGB2(&g3, work->ray[i], work->ray[i], work->ray[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }

                setVector(&scale, 0x1000, 0x1000, 0x1000);
                setPolyFT4(&glow);
                glow.tpage = work->texture[2] >> 16;
                glow.clut = work->texture[2];
                setUV4(&glow, 0x40, 0, w, 0, 0x40, 0x3F, w, 0x3F);
                setRGB0(&glow, work->glow_r[i], work->glow_g[i], work->glow_b[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->start[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                }
                setVector(&scale, 0x800, 0x800, 0x800);
                m = saved;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0,
                                  (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    for (i = 0; i < work->count; i++) {
        if (work->state[i] == 3) {
            if (i == work->config->count - 1) {
                work->state[i] = 4;
                work->phase = 1;
                return 1;
            }
            work->state[i] = 4;
            return 4;
        }
    }
    return 0;
}

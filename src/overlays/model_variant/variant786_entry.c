#include "../../types.h"
#include "variant786_entry.h"

/* Header 786's entry: a volley of bolts launches one every third frame from a
 * model point and drifts across the field with a fading 16-point ribbon trail.
 * Bolts that reach their limit burst into a glow with a fan of rays and a
 * ring of rising animated sprites while the ambient light dims and
 * recovers. */

static const VECTOR D_8013CD98 = {4096, 4096, 4096, 0};

/* The flight step is reduced in place through its address; the bolt
 * position is read by value, which keeps it out of the address register. */
#define DIV_VECTOR(v, d) ((v)->vx /= (d), (v)->vy /= (d), (v)->vz /= (d))
#define SUB_VECTOR(v0, v1) ((v0)->vx -= (v1).vx, (v0)->vy -= (v1).vy, (v0)->vz -= (v1).vz)

s32 func_8013B004(u8 *context, s32 command)
{
    Model786State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_FT4 glow;
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
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CD98;
    work = (Model786State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CDFC[command];
        for (i = 0; i < work->config->count; i++) {
            j = (rand() - rand()) % 4096;
            k = (rand() - rand()) % 4096;
            setVector(&work->pos[i], work->config->spread * j / 4096, work->config->spread * k / 4096, 0);
            setVector(&work->step[i], work->config->drift * j / 4096, work->config->drift * k / 4096 - 350,
                      direction * 450);
            for (j = 0; j < work->config->ring_count; j++) {
                setVector(&work->ring[i][j], work->config->ring_spread * ((rand() - rand()) % 4096) / 4096,
                          work->config->ring_spread * ((rand() - rand()) % 4096) / 4096, 0);
                work->ring_frame[i][j] = 0;
            }
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], work->config->ray_spread * (rand() % 4096 - rand() % 4096) / 4096,
                          work->config->ray_spread * (rand() % 4096 - rand() % 4096) / 4096, 0);
            }
            for (j = 0; j < 16; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->ring_shown[i] = 0;
            work->tail[i] = 0;
            work->state[i] = 0;
            work->glow[i] = 0xFF;
            work->trail_head[i] = 0;
            work->bolt_r[i] = work->config->bolt_r;
            work->bolt_g[i] = work->config->bolt_g;
            work->bolt_b[i] = work->config->bolt_b;
            work->size[i] = work->config->size + (rand() - rand()) % (work->config->size / 4);
            setVector(&work->angle[i], 0, 0, rand() % 4096);
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CDA8[i]);
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        work->active = 0;
        work->stage = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
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
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < work->active; i++) {
        if (work->state[i] < 2) {
            setPolyFT4(&ft4);
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            ft4.clut = work->texture[2];
            ft4.tpage = work->texture[2] >> 16;
            setVector(&quad[0], -work->size[i], -work->size[i], 0);
            setVector(&quad[1], work->size[i], -work->size[i], 0);
            setVector(&quad[2], -work->size[i], work->size[i], 0);
            setVector(&quad[3], work->size[i], work->size[i], 0);
            GsSetLsMatrix(&light);
            if (i == work->active - 1 && work->state[i] == 0) {
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
                setVector(&pos, m.t[0], m.t[1], m.t[2]);
                addVector(&work->pos[i], &pos);
                SUB_VECTOR(&work->step[i], work->pos[i]);
                DIV_VECTOR(&work->step[i], work->config->steps);
                work->state[i] = 1;
            }
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0, 0x2F, 0, 0, 0x2F, 0x2F, 0x2F);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
            if (direction < 0) {
                if (direction * 450 < work->pos[i].vz) {
                    copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                    work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                    addVector(&work->pos[i], &work->step[i]);
                    if (work->pos[i].vz < direction * 450) {
                        work->pos[i].vz = direction * 450;
                    }
                } else {
                    work->state[i] = 2;
                }
            }
            if (direction > 0) {
                if (work->pos[i].vz < direction * 450) {
                    copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
                    work->trail_head[i] = (work->trail_head[i] + 1) % 16;
                    addVector(&work->pos[i], &work->step[i]);
                    if (direction * 450 < work->pos[i].vz) {
                        work->pos[i].vz = direction * 450;
                    }
                } else {
                    work->state[i] = 2;
                }
            }
        } else {
            if (work->glow[i] != 0) {
                setPolyG3(&g3);
                setVector(&quad[0], -work->config->ray_width, 0, 0);
                setVector(&quad[1], work->config->ray_width, 0, 0);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->glow[i], work->glow[i], work->glow[i]);
                setRGB2(&g3, work->glow[i], work->glow[i], work->glow[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                                      (PSXLONG *)&g3.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                    }
                }
                setPolyFT4(&glow);
                setRGB0(&glow, work->glow[i], work->glow[i], work->glow[i]);
                glow.clut = work->texture[1];
                glow.tpage = work->texture[1] >> 16;
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                                  (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                setUV4(&glow, 0x18, 0, 0x57, 0, 0x18, 0x3F, 0x57, 0x3F);
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                if (work->glow[i] >= 16) {
                    work->glow[i] -= work->config->fade;
                } else {
                    work->glow[i] = 0;
                }
            }
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->config->r, work->config->g, work->config->b);
            ft4.clut = work->texture[2];
            ft4.tpage = work->texture[0] >> 16;
            setVector(&quad[0], -work->config->ring_size, -work->config->ring_size, 0);
            setVector(&quad[1], work->config->ring_size, -work->config->ring_size, 0);
            setVector(&quad[2], -work->config->ring_size, work->config->ring_size, 0);
            setVector(&quad[3], work->config->ring_size, work->config->ring_size, 0);
            if (++work->ring_shown[i] > work->config->ring_count) {
                work->ring_shown[i] = work->config->ring_count;
            }
            for (j = 0; j < work->ring_shown[i]; j++) {
                if (work->ring_frame[i][j] < 16) {
                    GsSetLsMatrix(&light);
                    copyVector(&pos, &work->pos[i]);
                    addVector(&pos, &work->ring[i][j]);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    /* The retail code tests the diagonal frame [i][i], not [i][j]. */
                    if (work->ring_frame[i][i] < 8) {
                        setUV4(&ft4, work->ring_frame[i][j] / 2 % 4 * 32, 0x40,
                               work->ring_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x40,
                               work->ring_frame[i][j] / 2 % 4 * 32, 0x5F,
                               work->ring_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x5F);
                    } else {
                        setUV4(&ft4, work->ring_frame[i][j] / 2 % 4 * 32, 0x60,
                               work->ring_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x60,
                               work->ring_frame[i][j] / 2 % 4 * 32, 0x7F,
                               work->ring_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x7F);
                    }
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    work->ring_frame[i][j]++;
                    work->ring[i][j].vz += direction * work->config->ring_speed;
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
                for (j = work->trail_head[i] - 1; j >= work->trail_head[i] - work->tail[i]; j--) {
                    setRGB0(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB1(&g4, work->bolt_r[i] / k, work->bolt_g[i] / k, work->bolt_b[i] / k);
                    setRGB2(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setRGB3(&g4, work->bolt_r[i] / (k + 1), work->bolt_g[i] / (k + 1), work->bolt_b[i] / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->size[i] / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->size[i] / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->size[i] / (k + 1), work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->size[i] / (k + 1), work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    setVector(&quad[0], work->trail[i][j].vx - work->size[i] / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx + work->size[i] / k, work->trail[i][j].vy,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx - work->size[i] / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx + work->size[i] / (k + 1),
                              work->trail[i][j - 1].vy, work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0,
                                      (PSXLONG *)&g4.x1, (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    k++;
                }
                if (work->state[i] >= 2) {
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
    }

    if (work->stage == 0) {
        if (++work->active > work->config->count) {
            work->active = work->config->count;
        }
        work->stage = 1;
    } else if (work->stage == 1) {
        work->stage = 2;
    } else if (work->stage == 2) {
        work->stage = 0;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    if (work->phase == 1) {
        return 0;
    }
    if (work->state[work->config->count - 1] < 2) {
        return work->state[0] >= 2 ? 4 : 0;
    }
    work->phase = 1;
    return 1;
}

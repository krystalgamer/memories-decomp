#include "../../types.h"
#include "variant781_entry.h"
#include "../../psyq/abs.h"

/* Header 781's entry: two orbs leave each of two model parts, ringed by
 * sixteen spinning sparks and ray fans, while the camera pans and the
 * ambient light dims. Each orb that lands opens into a growing textured
 * halo ring, a stack of fading glows and a spray of falling debris. */

static const VECTOR D_8013CDC4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model781State *work;
    MATRIX light;
    MATRIX m;
    POLY_FT4 ft4;
    POLY_G3 g3;
    POLY_GT4 gt4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j, k, l;
    s32 b;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013CDC4;
    work = (Model781State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CE28[command];
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                setVector(&work->pos[i][j], 0, 0, 0);
                setVector(&work->velocity[i][j], 0, 0, 0);
                work->arrived[i][j] = 0;
                work->scale[i][j] = 0x1000;
                work->angle[i][j] = 0;
                work->height[i][j] = work->config->height;
                work->r[i][j] = 0;
                work->g[i][j] = 0;
                work->b[i][j] = 0;
                for (k = 0; k < 16; k++) {
                    /* The spark angles reuse L, and the ring angle below
                     * reuses J, which fixes the register allocation. */
                    l = rand() % 4096;
                    b = rand() % 4096;
                    setVector(&work->sparks[i][j][k],
                              work->config->size * ccos(l) / 4096 * ccos(b) / 4096,
                              work->config->size * csin(l) / 4096 * ccos(b) / 4096,
                              work->config->size * ccos(l) / 4096 * csin(b) / 4096);
                }
                for (k = 0; k < 32; k++) {
                    setVector(&work->debris[i][j][k], 0, -350, direction * 450);
                    setVector(&work->debris_velocity[i][j][k],
                              work->config->debris_speed * ((rand() - rand()) % 4096) / 4096,
                              work->config->debris_speed * ((rand() - rand()) % 4096) / 4096,
                              -direction * work->config->debris_speed * (rand() % 4096) / 4096);
                }
            }
        }
        for (i = 0; i < 16; i++) {
            j = (i << 8) % 4096;
            setVector(&work->ring[i][0], work->config->ring_outer * ccos(j) / 4096,
                      work->config->ring_outer * csin(j) / 4096, -direction * work->config->ring_depth);
            setVector(&work->ring[i][1], work->config->ring_inner * ccos(j) / 4096,
                      work->config->ring_inner * csin(j) / 4096, 0);
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CDD4[i]);
        }
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->orbit_end) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else if (work->elapsed < work->config->pan_end) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -work->config->flight);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rot, -16);
    }
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step << 6;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 0x800) {
            work->ambient += step << 6;
        } else {
            work->phase = 2;
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
    if (work->elapsed <= work->config->time[1][1]) {
        for (i = 0; i < 2; i++) {
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part[i]), &m);
            setVector(&pos, m.t[0], m.t[1], m.t[2]);
            for (j = 0; j < 2; j++) {
                if (work->elapsed <= work->config->time[i][j]) {
                    if (work->r[i][j] < work->config->r - 31) {
                        work->r[i][j] += work->config->color_step;
                    } else {
                        work->r[i][j] = work->config->r;
                    }
                    if (work->g[i][j] < work->config->g - 31) {
                        work->g[i][j] += work->config->color_step;
                    } else {
                        work->g[i][j] = work->config->g;
                    }
                    if (work->b[i][j] < work->config->b - 31) {
                        work->b[i][j] += work->config->color_step;
                    } else {
                        work->b[i][j] = work->config->b;
                    }
                    setVector(&work->pos[i][j], m.t[0], m.t[1], m.t[2]);
                    setVector(&work->velocity[i][j], -work->pos[i][j].vx / work->config->flight,
                              (-350 - work->pos[i][j].vy) / work->config->flight,
                              (direction * 450 - work->pos[i][j].vz) / work->config->flight);
                }
            }
        }
    }

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (work->arrived[i][j] == 0) {
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                /* Clearing the rotation first places its stores between
                 * the sprite's length and code bytes. */
                setVector(&rot, 0, 0, 0);
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setRGB0(&ft4, 0x80, 0x80, 0x80);
                setVector(&quad[0], -work->config->size, -work->config->size, 0);
                setVector(&quad[1], work->config->size, -work->config->size, 0);
                setVector(&quad[2], -work->config->size, work->config->size, 0);
                setVector(&quad[3], work->config->size, work->config->size, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->pos[i][j], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                setSemiTrans(&ft4, 0);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 0);
                }
            }
            if (work->r[i][j] != 0 || work->g[i][j] != 0 || work->b[i][j] != 0) {
                setVector(&rot, 0, 0, 0);
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[1] >> 16;
                ft4.clut = work->texture[1];
                setRGB0(&ft4, work->r[i][j], work->g[i][j], work->b[i][j]);
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                for (k = 0; k < 16; k++) {
                    copyVector(&pos, &work->pos[i][j]);
                    addVector(&pos, &work->sparks[i][j][k]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                    setSemiTrans(&ft4, 1);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                }

                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->r[i][j], work->g[i][j], work->b[i][j]);
                setRGB2(&g3, work->r[i][j], work->g[i][j], work->b[i][j]);
                setVector(&quad[0], work->config->orbit * ccos(work->elapsed * 64 % 4096) / 4096,
                          -work->height[i][j], work->config->orbit * csin(work->elapsed * 64 % 4096) / 4096);
                setVector(&quad[1], -work->config->size / 4, 0, 0);
                setVector(&quad[2], work->config->size / 4, 0, 0);
                for (k = 0; k < 16; k++) {
                    setVector(&rot, 0, 0, 0);
                    for (l = 0; l < 4; l++) {
                        rot.vx = work->angle[i][j];
                        rot.vy += (l << 9) % 4096;
                        copyVector(&pos, &work->pos[i][j]);
                        addVector(&pos, &work->sparks[i][j][k]);
                        GsSetLsMatrix(&light);
                        RotTrans(&pos, (VECTOR *)m.t, &flag);
                        RotMatrix(&rot, &m);
                        MulMatrix2(&light, &m);
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                          (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                        setSemiTrans(&g3, 1);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 3);
                        }
                    }
                }

                if (work->arrived[i][j] != 0) {
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, work->scale[i][j], work->scale[i][j], work->scale[i][j]);
                    setPolyGT4(&gt4);
                    gt4.tpage = work->texture[2] >> 16;
                    gt4.clut = work->texture[2];
                    setRGB0(&gt4, work->r[i][j] / 3, work->g[i][j] / 3, work->b[i][j] / 3);
                    setRGB1(&gt4, work->r[i][j] / 3, work->g[i][j] / 3, work->b[i][j] / 3);
                    setRGB2(&gt4, 0, 0, 0);
                    setRGB3(&gt4, 0, 0, 0);
                    copyVector(&pos, &work->pos[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    saved = m;
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    for (k = 0; k < 16; k++) {
                        if (k == 15) {
                            otz = RotAverage4(&work->ring[15][1], &work->ring[0][1], &work->ring[15][0],
                                              &work->ring[0][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                              (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                        } else {
                            otz = RotAverage4(&work->ring[k][1], &work->ring[k + 1][1], &work->ring[k][0],
                                              &work->ring[k + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                              (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                        }
                        setUV4(&gt4, 0xC0, 0, 0xDF, 0, 0xC0, 0x7F, 0xDF, 0x7F);
                        setSemiTrans(&gt4, 1);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 3);
                        }
                    }

                    setPolyFT4(&ft4);
                    ft4.tpage = work->texture[1] >> 16;
                    ft4.clut = work->texture[1];
                    setRGB0(&ft4, work->r[i][j] / 2, work->g[i][j] / 2, work->b[i][j] / 2);
                    setVector(&quad[0], -work->config->halo_size, -work->config->halo_size, 0);
                    setVector(&quad[1], work->config->halo_size, -work->config->halo_size, 0);
                    setVector(&quad[2], -work->config->halo_size, work->config->halo_size, 0);
                    setVector(&quad[3], work->config->halo_size, work->config->halo_size, 0);
                    for (k = 0; k < 4; k++) {
                        setVector(&scale, work->scale[i][j] / (k + 1), work->scale[i][j] / (k + 1),
                                  work->scale[i][j] / (k + 1));
                        GsSetLsMatrix(&light);
                        m = saved;
                        ScaleMatrix(&m, &scale);
                        GsSetLsMatrix(&m);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                          (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                        setUV4(&ft4, 0x20, 0, 0x5F, 0, 0x20, 0x3F, 0x5F, 0x3F);
                        setSemiTrans(&ft4, 1);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                        }
                    }

                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    setVector(&rot, 0, 0, 0);
                    setPolyFT4(&ft4);
                    ft4.tpage = work->texture[0] >> 16;
                    ft4.clut = work->texture[0];
                    setRGB0(&ft4, work->r[i][j], work->g[i][j], work->b[i][j]);
                    setVector(&quad[0], -work->config->debris_size, -work->config->debris_size, 0);
                    setVector(&quad[1], work->config->debris_size, -work->config->debris_size, 0);
                    setVector(&quad[2], -work->config->debris_size, work->config->debris_size, 0);
                    setVector(&quad[3], work->config->debris_size, work->config->debris_size, 0);
                    for (k = 0; k < 32; k++) {
                        if (work->debris[i][j][k].vy < 0) {
                            GsSetLsMatrix(&light);
                            RotTrans(&work->debris[i][j][k], (VECTOR *)m.t, &flag);
                            RotMatrix(&rot, &m);
                            ScaleMatrix(&m, &scale);
                            GsSetLsMatrix(&m);
                            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                              (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3,
                                              &p, &flag);
                            setUV4(&ft4, 0, 0, 0x3F, 0, 0, 0x3F, 0x3F, 0x3F);
                            setSemiTrans(&ft4, 1);
                            if (otz >= 0) {
                                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 0);
                            }
                            addVector(&work->debris[i][j][k], &work->debris_velocity[i][j][k]);
                            work->debris_velocity[i][j][k].vy += 2;
                        }
                    }
                }
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
            }

            if (work->config->time[i][j] < work->elapsed && work->arrived[i][j] == 0) {
                addVector(&work->pos[i][j], &work->velocity[i][j]);
                work->height[i][j] += 12;
                if (ABS(work->angle[i][j]) < 0x400) {
                    work->angle[i][j] += direction * (step << 7);
                } else {
                    work->angle[i][j] = direction << 10;
                }
                if (direction > 0) {
                    if (work->pos[i][j].vz > 450) {
                        work->arrived[i][j] = 1;
                    }
                } else if (work->pos[i][j].vz < direction * 450) {
                    work->arrived[i][j] = 1;
                }
            } else if (work->arrived[i][j] != 0) {
                if (work->scale[i][j] < 0x3000) {
                    work->scale[i][j] += work->config->grow;
                } else {
                    work->scale[i][j] += work->config->slow_grow;
                }
                if (work->r[i][j] >= 16) {
                    work->r[i][j] -= work->config->fade;
                } else {
                    work->r[i][j] = 0;
                }
                if (work->g[i][j] >= 16) {
                    work->g[i][j] -= work->config->fade;
                } else {
                    work->g[i][j] = 0;
                }
                if (work->b[i][j] >= 16) {
                    work->b[i][j] -= work->config->fade;
                } else {
                    work->b[i][j] = 0;
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && work->r[1][1] == 0 && work->g[1][1] == 0 && work->b[1][1] == 0) {
        return 2;
    }
    if (work->arrived[1][1] == 1) {
        work->arrived[1][1] = 2;
        work->phase = 1;
        return 1;
    }
    if (work->arrived[1][0] == 1) {
        work->arrived[1][0] = 2;
        return 4;
    }
    if (work->arrived[0][1] == 1) {
        work->arrived[0][1] = 2;
        return 4;
    }
    if (work->arrived[0][0] == 1) {
        work->arrived[0][0] = 2;
        return 4;
    }
    return 0;
}

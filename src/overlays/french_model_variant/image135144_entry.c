#include "../../types.h"
#include "image135144_entry.h"

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 func_8013B004(u8 *context, s32 command)
{
    Image135144State *work;
    MATRIX light;
    MATRIX m;
    MATRIX saved;
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
    s32 slot;
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013D18C;
    work = (Image135144State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D1F0[command];
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->pos[i], 0, 0, 0);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            work->level[i] = 128;
            work->arrived[i] = 0;
            work->trail_head[i] = 1;
            work->tail[i] = 0;
            work->r[i] = work->config->r;
            work->g[i] = work->config->g;
            work->b[i] = work->config->b;
            work->inner_r[i] = work->config->inner_r;
            work->inner_g[i] = work->config->inner_g;
            work->inner_b[i] = work->config->inner_b;
            work->unknown_r[i] = work->config->unknown_r;
            work->unknown_g[i] = work->config->unknown_g;
            work->unknown_b[i] = work->config->unknown_b;
            for (j = 0; j < work->config->puff_count; j++) {
                setVector(&work->puffs[i][j], 0, 0, 0);
                setVector(&work->puff_velocity[i][j], RANDOM_SPREAD(work->config->puff_speed),
                          RANDOM_SPREAD(work->config->puff_speed),
                          RANDOM_SPREAD(work->config->puff_speed));
            }
            for (j = 0; j < work->config->ray_count; j++) {
                setVector(&work->rays[i][j], RANDOM_SPREAD(work->config->ray_spread),
                          RANDOM_SPREAD(work->config->ray_spread),
                          -direction * work->config->ray_spread * (rand() % 4096) / 4096);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D19C[i]);
        }
        work->ambient = 2048;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->launch) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rot, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rot, -12);
    }
    if (work->phase == 0) {
        if (work->ambient > -512) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -512;
        }
    } else if (work->arrived[0] == 2) {
        if (work->ambient < 2048) {
            work->ambient += step * 64;
        } else {
            work->arrived[0] = 4;
            work->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();

    if (work->elapsed <= work->config->charge_end) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->charge_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->angle[i],
                      direction * (direction * ratan2(pos.vz - rot.vz, pos.vy - rot.vy) - 1024), 0, 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, 128, 128, 128);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->width, 0);
            setVector(&quad[1], 0, -work->config->width, direction * work->config->length);
            setVector(&quad[2], 0, work->config->width, 0);
            setVector(&quad[3], 0, work->config->width, direction * work->config->length);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                              (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag) + 2;
            setUV4(&ft4, 0, 64, 95, 64, 0, 95, 95, 95);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }

    if (work->elapsed <= work->config->launch) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &m);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->aim_part), &m);
        setVector(&rot, m.t[0], m.t[1], m.t[2]);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->angle[i],
                      direction * (direction * ratan2(rot.vz - pos.vz, rot.vy - pos.vy) - 1024), 0, 0);
            setPolyFT4(&ft4);
            setRGB0(&ft4, 128, 128, 128);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->width, 0);
            setVector(&quad[1], 0, -work->config->width, direction * work->config->length);
            setVector(&quad[2], 0, work->config->width, 0);
            setVector(&quad[3], 0, work->config->width, direction * work->config->length);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                              (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 64, 95, 64, 0, 95, 95, 95);
            setSemiTrans(&ft4, 0);
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
            copyVector(&work->pos[i], &pos);
            setVector(&work->step[i], 0, work->config->speed >> 4, direction * work->config->speed);
            for (j = 0; j < 32; j++) {
                copyVector(&work->trail[i][j], &work->pos[i]);
            }
        }
        PopMatrix();
        work->elapsed += step;
        return 0;
    }

    for (i = 0; i < work->config->count; i++) {
        if (work->level[i]) {
            setPolyFT4(&ft4);
            setRGB0(&ft4, work->level[i], work->level[i], work->level[i]);
            ft4.tpage = work->texture[2] >> 16;
            ft4.clut = work->texture[2];
            setVector(&quad[0], 0, -work->config->width, 0);
            setVector(&quad[1], 0, -work->config->width, direction * work->config->length);
            setVector(&quad[2], 0, work->config->width, 0);
            setVector(&quad[3], 0, work->config->width, direction * work->config->length);
            GsSetLsMatrix(&light);
            RotTrans(&work->pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&work->angle[i], &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                              (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 64, 95, 64, 0, 95, 95, 95);
            if (work->arrived[i] == 0) {
                setSemiTrans(&ft4, 0);
            } else {
                setSemiTrans(&ft4, 1);
            }
            if (otz >= 0) {
                GsSortPoly(&ft4, ot, (u16)otz);
            }
            if (work->arrived[i] == 0) {
                copyVector(&work->origin[i], &work->pos[i]);
                for (j = 0; j < work->config->puff_count; j++) {
                    copyVector(&work->puffs[i][j], &work->pos[i]);
                }
            }
            copyVector(&work->trail[i][work->trail_head[i]], &work->pos[i]);
            work->trail_head[i] = (work->trail_head[i] + 1) % 32;
            addVector(&work->pos[i], &work->step[i]);
            if (ABS(work->pos[i].vz) > 450 && work->arrived[i] == 0) {
                work->arrived[i] = 1;
            }
            if (work->arrived[i]) {
                if (work->level[i] >= 16) {
                    work->level[i] -= work->config->fade;
                } else {
                    work->level[i] = 0;
                }
            }
        }

        if (work->trail_head[i] > work->tail[i] && (work->r[i] || work->g[i] || work->b[i])) {
            setPolyG4(&g4);
            setVector(&pos, 0, 0, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = work->trail_head[i] - 1, k = 1;
                 j >= work->trail_head[i] - (work->tail[i] + 1); j--, k++) {
                setRGB0(&g4, work->r[i] / k, work->g[i] / k, work->b[i] / k);
                setRGB1(&g4, work->r[i] / k, work->g[i] / k, work->b[i] / k);
                setRGB2(&g4, work->r[i] / (k + 1), work->g[i] / (k + 1), work->b[i] / (k + 1));
                setRGB3(&g4, work->r[i] / (k + 1), work->g[i] / (k + 1), work->b[i] / (k + 1));
                setVector(&quad[0], work->trail[i][j].vx,
                          work->trail[i][j].vy - (u16)(work->config->width / 3) / k,
                          work->trail[i][j].vz);
                setVector(&quad[1], work->trail[i][j].vx,
                          work->trail[i][j].vy + (u16)(work->config->width / 3) / k,
                          work->trail[i][j].vz);
                /* The native ribbon keeps the current point's Y at both ends. */
                setVector(&quad[2], work->trail[i][j - 1].vx,
                          work->trail[i][j].vy - (u16)(work->config->width / 3) / (k + 1),
                          work->trail[i][j - 1].vz);
                setVector(&quad[3], work->trail[i][j - 1].vx,
                          work->trail[i][j].vy + (u16)(work->config->width / 3) / (k + 1),
                          work->trail[i][j - 1].vz);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                                  (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                  (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                }
            }
        }

        if (work->arrived[i]) {
            if (work->r[i] || work->g[i] || work->b[i]) {
                setVector(&rot, 0, 0, 0);
                setVector(&pos, 0, 0, 0);
                setPolyFT4(&glow);
                setRGB0(&glow, work->r[i], work->g[i], work->b[i]);
                glow.tpage = work->texture[0] >> 16;
                glow.clut = work->texture[0];
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                GsSetLsMatrix(&light);
                RotTrans(&work->origin[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                                  (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                                  (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                setUV4(&glow, 0, 0, 63, 0, 0, 63, 63, 63);
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                setPolyG3(&g3);
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->r[i], work->g[i], work->b[i]);
                setRGB2(&g3, work->r[i], work->g[i], work->b[i]);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                }
                setRGB0(&g3, 0, 0, 0);
                setRGB1(&g3, work->inner_r[i], work->inner_g[i], work->inner_b[i]);
                setRGB2(&g3, work->inner_r[i], work->inner_g[i], work->inner_b[i]);
                m = saved;
                setVector(&scale, 2048, 2048, 2048);
                GsSetLsMatrix(&light);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    copyVector(&quad[0], &work->rays[i][j]);
                    setVector(&quad[1], 0, -work->config->ray_width, 0);
                    setVector(&quad[2], 0, work->config->ray_width, 0);
                    otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0,
                                      (PSXLONG *)&g3.x1, (PSXLONG *)&g3.x2, &p, &flag);
                    Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
                }
                setVector(&scale, 4096, 4096, 4096);
                for (j = 0; j < work->config->puff_count; j++) {
                    glow.clut = work->texture[1];
                    glow.tpage = work->texture[1] >> 16;
                    GsSetLsMatrix(&light);
                    RotTrans(&work->puffs[i][j], (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
                    setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
                    setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
                    setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3],
                                      (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1,
                                      (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &p, &flag);
                    setUV4(&glow, 128 + work->elapsed % 3 * 32, 0, 159 + work->elapsed % 3 * 32, 0,
                           128 + work->elapsed % 3 * 32, 31, 159 + work->elapsed % 3 * 32, 31);
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)otz, 1);
                    addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
                }
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
                if (work->inner_r[i] >= 16) {
                    work->inner_r[i] -= work->config->fade;
                } else {
                    work->inner_r[i] = 0;
                }
                if (work->inner_g[i] >= 16) {
                    work->inner_g[i] -= work->config->fade;
                } else {
                    work->inner_g[i] = 0;
                }
                if (work->inner_b[i] >= 16) {
                    work->inner_b[i] -= work->config->fade;
                } else {
                    work->inner_b[i] = 0;
                }
                if (work->unknown_r[i] >= 16) {
                    work->unknown_r[i] -= work->config->fade;
                } else {
                    work->unknown_r[i] = 0;
                }
                if (work->unknown_g[i] >= 16) {
                    work->unknown_g[i] -= work->config->fade;
                } else {
                    work->unknown_g[i] = 0;
                }
                if (work->unknown_b[i] >= 16) {
                    work->unknown_b[i] -= work->config->fade;
                } else {
                    work->unknown_b[i] = 0;
                }
            }
        } else {
            work->tail[i]++;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->arrived[0] == 4 && !work->r[0] && !work->g[0] && !work->b[0]) {
        return 2;
    }
    if (work->arrived[0] == 1) {
        work->phase = 1;
        work->arrived[0] = 2;
        return 1;
    }
    return 0;
}

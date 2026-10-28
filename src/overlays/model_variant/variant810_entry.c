#include "../../types.h"
#include "variant810_entry.h"

/* Header 810's entry: once each side's start time passes, a spinning
 * Gouraud ring fan and a glow sprite follow a model point and leave a
 * fading ribbon trail, while a second ring fan and smoke puffs rise from
 * another model point. When the side ends, a four-way burst fan and a
 * flare flash in front of the slot while the ambient light dims and
 * recovers. */

static const VECTOR D_8013DA70 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model810State *work;
    MATRIX light;
    MATRIX m;
    POLY_GT4 gt4;
    POLY_FT4 ft4;
    POLY_G4 g4;
    SVECTOR rot;
    SVECTOR pos;
    VECTOR scale;
    SVECTOR quad[4];
    SVECTOR ring_pos[2];
    SVECTOR halo_pos[2];
    MATRIX saved;
    PSXLONG flag;
    PSXLONG p;
    s32 slot;
    s32 direction;
    s32 step;
    GsOT *ot;
    s32 i, j, k;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013DA70;
    work = (Model810State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013DAF0[command];
        for (i = 0; i < 16; i++) {
            j = i * 256 % 4096;
            setVector(&work->ring[i][0], work->config->inner_radius * ccos(j) / 4096,
                      work->config->inner_radius * csin(j) / 4096, -direction * work->config->inner_depth);
            setVector(&work->ring[i][1], work->config->inner_outer * ccos(j) / 4096,
                      work->config->inner_outer * csin(j) / 4096, -direction * work->config->outer_depth);
            setVector(&work->ring_outer[i][0], work->config->outer_radius * ccos(j) / 4096,
                      work->config->outer_radius * csin(j) / 4096, -direction * work->config->inner_depth);
            setVector(&work->ring_outer[i][1], work->config->outer_outer * ccos(j) / 4096,
                      work->config->outer_outer * csin(j) / 4096, -direction * work->config->outer_depth * 2);
            setVector(&work->halo[i][0], work->config->halo_radius * ccos(j) / 4096,
                      work->config->halo_radius * csin(j) / 4096, 0);
            setVector(&work->halo[i][1], work->config->halo_outer * ccos(j) / 4096,
                      work->config->halo_outer * csin(j) / 4096, direction * work->config->halo_depth);
            setVector(&work->halo_outer[i][0], work->config->halo2_radius * ccos(j) / 4096,
                      work->config->halo2_radius * csin(j) / 4096, 0);
            setVector(&work->halo_outer[i][1], work->config->halo2_outer * ccos(j) / 4096,
                      work->config->halo2_outer * csin(j) / 4096, direction * work->config->halo_depth * 2);
            setVector(&work->burst[i][0], work->config->burst_radius * ccos(j) / 4096,
                      work->config->burst_radius * csin(j) / 4096, 0);
            setVector(&work->burst[i][1], work->config->burst_outer * ccos(j) / 4096,
                      work->config->burst_outer * csin(j) / 4096, -direction * work->config->burst_depth);
        }
        for (i = 0; i < 2; i++) {
            work->ring_r[i] = work->config->r;
            work->ring_g[i] = work->config->g;
            work->ring_b[i] = work->config->b;
            work->halo_r[i] = work->config->r;
            work->halo_g[i] = work->config->g;
            work->halo_b[i] = work->config->b;
            work->flare_r[i] = work->config->flare_r;
            work->flare_g[i] = work->config->flare_g;
            work->flare_b[i] = work->config->flare_b;
            work->ring_scale[i] = 0x400;
            work->burst_scale[i] = 0;
            work->tail[i] = 0;
            work->head[i] = 1;
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            for (j = 0; j < work->config->puff_count; j++) {
                setVector(&work->puffs[i][j], 0, 0, 0);
                setVector(&work->puff_velocity[i][j],
                          work->config->puff_speed * ((rand() - rand()) % 4096) / 4096,
                          work->config->puff_speed * ((rand() - rand()) % 4096) / 4096,
                          -direction * work->config->puff_speed * (rand() % 4096) / 4096);
                work->puff_frame[i][j] = 0;
            }
        }
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DA80[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DA80[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013DA80[2]);
        work->texture[3] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 0, &D_8013DA80[3]);
        work->ambient = 0x800;
        work->phase = 0;
        work->elapsed = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->start[1]) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rot, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rot, -16);
    }
    if (work->phase == 0) {
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

    if (work->elapsed < work->config->start[0]) {
        PushMatrix();
        light = *(MATRIX *)Model_GetLightSourceMatrix();
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&light);
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part[i]), &m);
            setVector(&ring_pos[i], m.t[0], m.t[1], m.t[2]);
            copyVector(&work->trail[i][0], &ring_pos[i]);
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }

    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < 2; i++) {
        if (work->elapsed >= work->config->start[i] && (work->ring_r[i] || work->ring_g[i] || work->ring_b[i])) {
            GsSetLsMatrix(&light);
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part[i]), &m);
            setVector(&ring_pos[i], m.t[0], m.t[1], m.t[2]);
            if (work->elapsed <= work->config->end[i]) {
                copyVector(&work->trail[i][work->head[i]], &ring_pos[i]);
                work->head[i] = (work->head[i] + 1) % 32;
            }

            setPolyGT4(&gt4);
            gt4.tpage = work->texture[1] >> 16;
            gt4.clut = work->texture[1];
            setRGB0(&gt4, 0, 0, 0);
            setRGB1(&gt4, 0, 0, 0);
            setRGB2(&gt4, work->ring_r[i], work->ring_g[i], work->ring_b[i]);
            setRGB3(&gt4, work->ring_r[i], work->ring_g[i], work->ring_b[i]);
            setVector(&rot, 0, 0, work->elapsed * work->config->spin);
            setVector(&scale, work->ring_scale[i], work->ring_scale[i], work->ring_scale[i]);
            GsSetLsMatrix(&light);
            RotTrans(&ring_pos[i], (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->ring[15][1], &work->ring[0][1], &work->ring[15][0], &work->ring[0][0],
                                      (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                      (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->ring[j][1], &work->ring[j + 1][1], &work->ring[j][0],
                                      &work->ring[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0, 0xFF, 0, 0x80, 0x4F, 0xFF, 0x4F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->ring_outer[15][1], &work->ring_outer[0][1], &work->ring_outer[15][0],
                                      &work->ring_outer[0][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->ring_outer[j][1], &work->ring_outer[j + 1][1], &work->ring_outer[j][0],
                                      &work->ring_outer[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0, 0xFF, 0, 0x80, 0x4F, 0xFF, 0x4F);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }

            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->ring_r[i], work->ring_g[i], work->ring_b[i]);
            setVector(&quad[0], -work->config->halo_size, -work->config->halo_size,
                      -direction * work->config->inner_depth);
            setVector(&quad[1], work->config->halo_size, -work->config->halo_size,
                      -direction * work->config->inner_depth);
            setVector(&quad[2], -work->config->halo_size, work->config->halo_size,
                      -direction * work->config->inner_depth);
            setVector(&quad[3], work->config->halo_size, work->config->halo_size,
                      -direction * work->config->inner_depth);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x40, 0x3F, 0x40, 0, 0x7F, 0x3F, 0x7F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }

            if (work->head[i] > work->tail[i]) {
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
                for (j = work->head[i] - 1; j >= -1 - work->tail[i] + work->head[i]; j--) {
                    setRGB0(&g4, work->ring_r[i] / k, work->ring_g[i] / k, work->ring_b[i] / k);
                    setRGB1(&g4, work->ring_r[i] / k, work->ring_g[i] / k, work->ring_b[i] / k);
                    setRGB2(&g4, work->ring_r[i] / (k + 1), work->ring_g[i] / (k + 1), work->ring_b[i] / (k + 1));
                    setRGB3(&g4, work->ring_r[i] / (k + 1), work->ring_g[i] / (k + 1), work->ring_b[i] / (k + 1));
                    setVector(&quad[0], work->trail[i][j].vx, work->trail[i][j].vy - work->config->ribbon_width / k,
                              work->trail[i][j].vz);
                    setVector(&quad[1], work->trail[i][j].vx, work->trail[i][j].vy + work->config->ribbon_width / k,
                              work->trail[i][j].vz);
                    setVector(&quad[2], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy - work->config->ribbon_width / (k + 1),
                              work->trail[i][j - 1].vz);
                    setVector(&quad[3], work->trail[i][j - 1].vx,
                              work->trail[i][j - 1].vy + work->config->ribbon_width / (k + 1),
                              work->trail[i][j - 1].vz);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&g4.x0, (PSXLONG *)&g4.x1,
                                      (PSXLONG *)&g4.x2, (PSXLONG *)&g4.x3, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&g4, ot, (u16)otz, 1);
                    }
                    k++;
                }
            }

            if (work->elapsed <= work->config->end[i]) {
                work->tail[i]++;
                if (work->ring_scale[i] < 0x1000) {
                    work->ring_scale[i] += 0x800;
                } else {
                    work->ring_scale[i] = 0x1000;
                }
            } else {
                if (work->ring_scale[i] > 0) {
                    work->ring_scale[i] -= 0x400;
                } else {
                    work->ring_scale[i] = 0;
                }
                if (work->ring_r[i] >= 32) {
                    work->ring_r[i] -= work->config->ring_fade;
                } else {
                    work->ring_r[i] = 0;
                }
                if (work->ring_g[i] >= 32) {
                    work->ring_g[i] -= work->config->ring_fade;
                } else {
                    work->ring_g[i] = 0;
                }
                if (work->ring_b[i] >= 32) {
                    work->ring_b[i] -= work->config->ring_fade;
                } else {
                    work->ring_b[i] = 0;
                }
            }
        }

        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->puff_part[i]), &m);
        setVector(&halo_pos[i], m.t[0], m.t[1], m.t[2]);
        if (work->elapsed >= work->config->start[i]) {
            if (work->halo_r[i] || work->halo_g[i] || work->halo_b[i]) {
                setPolyGT4(&gt4);
                gt4.tpage = work->texture[1] >> 16;
                gt4.clut = work->texture[1];
                setRGB0(&gt4, 0, 0, 0);
                setRGB1(&gt4, 0, 0, 0);
                setRGB2(&gt4, work->halo_r[i], work->halo_g[i], work->halo_b[i]);
                setRGB3(&gt4, work->halo_r[i], work->halo_g[i], work->halo_b[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, 0x1000, 0x1000, 0x1000);
                GsSetLsMatrix(&light);
                RotTrans(&halo_pos[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                saved = m;
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->halo[15][1], &work->halo[0][1], &work->halo[15][0],
                                          &work->halo[0][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->halo[j][1], &work->halo[j + 1][1], &work->halo[j][0],
                                          &work->halo[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                          (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0x80, 0, 0xFF, 0, 0x80, 0x4F, 0xFF, 0x4F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                    }
                }
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->halo_outer[15][1], &work->halo_outer[0][1],
                                          &work->halo_outer[15][0], &work->halo_outer[0][0], (PSXLONG *)&gt4.x0,
                                          (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->halo_outer[j][1], &work->halo_outer[j + 1][1],
                                          &work->halo_outer[j][0], &work->halo_outer[j + 1][0], (PSXLONG *)&gt4.x0,
                                          (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                    }
                    setUV4(&gt4, 0x80, 0, 0xFF, 0, 0x80, 0x4F, 0xFF, 0x4F);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                    }
                }
                if (work->halo_r[i] >= 16) {
                    work->halo_r[i] -= work->config->fade;
                } else {
                    work->halo_r[i] = 0;
                }
                if (work->halo_g[i] >= 16) {
                    work->halo_g[i] -= work->config->fade;
                } else {
                    work->halo_g[i] = 0;
                }
                if (work->halo_b[i] >= 16) {
                    work->halo_b[i] -= work->config->fade;
                } else {
                    work->halo_b[i] = 0;
                }
            }
        }

        if (work->elapsed >= work->config->start[i]) {
            setPolyFT4(&ft4);
            ft4.tpage = work->texture[3] >> 16;
            ft4.clut = work->texture[3];
            setRGB0(&ft4, 0x80, 0x80, 0x80);
            setVector(&quad[0], -work->config->puff_size, -work->config->puff_size, 0);
            setVector(&quad[1], work->config->puff_size, -work->config->puff_size, 0);
            setVector(&quad[2], -work->config->puff_size, work->config->puff_size, 0);
            setVector(&quad[3], work->config->puff_size, work->config->puff_size, 0);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            for (j = 0; j < work->config->puff_count; j++) {
                if (work->puff_frame[i][j] < 16) {
                    copyVector(&pos, &halo_pos[i]);
                    addVector(&pos, &work->puffs[i][j]);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                      (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    if (work->puff_frame[i][j] < 8) {
                        setUV4(&ft4, work->puff_frame[i][j] / 2 % 4 * 32, 0,
                               work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0,
                               work->puff_frame[i][j] / 2 % 4 * 32, 0x1F,
                               work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x1F);
                    } else {
                        setUV4(&ft4, work->puff_frame[i][j] / 2 % 4 * 32, 0x20,
                               work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x20,
                               work->puff_frame[i][j] / 2 % 4 * 32, 0x3F,
                               work->puff_frame[i][j] / 2 % 4 * 32 + 0x1F, 0x3F);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                    }
                    work->puff_frame[i][j]++;
                    addVector(&work->puffs[i][j], &work->puff_velocity[i][j]);
                }
            }
        }

        if (work->elapsed >= work->config->end[i]) {
            setPolyGT4(&gt4);
            gt4.tpage = work->texture[2] >> 16;
            gt4.clut = work->texture[2];
            setRGB0(&gt4, 0, 0, 0);
            setRGB1(&gt4, 0, 0, 0);
            setRGB2(&gt4, work->flare_r[i], work->flare_g[i], work->flare_b[i]);
            setRGB3(&gt4, work->flare_r[i], work->flare_g[i], work->flare_b[i]);
            setVector(&pos, 0, -350, direction * 450);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, work->burst_scale[i], work->burst_scale[i], work->burst_scale[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            saved = m;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->burst[15][1], &work->burst[0][1], &work->burst[15][0], &work->burst[0][0],
                                      (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                      (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->burst[j][1], &work->burst[j + 1][1], &work->burst[j][0],
                                      &work->burst[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0x50, 0x9F, 0x50, 0x80, 0xCF, 0x9F, 0xCF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }
            setVector(&scale, work->burst_scale[i] / 2, work->burst_scale[i] / 2, work->burst_scale[i] * 2);
            GsSetLsMatrix(&light);
            m = saved;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->burst[15][1], &work->burst[0][1], &work->burst[15][0], &work->burst[0][0],
                                      (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                      (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->burst[j][1], &work->burst[j + 1][1], &work->burst[j][0],
                                      &work->burst[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0x50, 0x9F, 0x50, 0x80, 0xCF, 0x9F, 0xCF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }
            setVector(&scale, work->burst_scale[i] * 2, work->burst_scale[i] * 2, work->burst_scale[i] / 2);
            GsSetLsMatrix(&light);
            m = saved;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->burst[15][1], &work->burst[0][1], &work->burst[15][0], &work->burst[0][0],
                                      (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                      (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->burst[j][1], &work->burst[j + 1][1], &work->burst[j][0],
                                      &work->burst[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0x50, 0x9F, 0x50, 0x80, 0xCF, 0x9F, 0xCF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }
            setVector(&scale, work->burst_scale[i], work->burst_scale[i], work->burst_scale[i] * 3 / 2);
            GsSetLsMatrix(&light);
            m = saved;
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            for (j = 0; j < 16; j++) {
                if (j == 15) {
                    otz = RotAverage4(&work->burst[15][1], &work->burst[0][1], &work->burst[15][0], &work->burst[0][0],
                                      (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1, (PSXLONG *)&gt4.x2,
                                      (PSXLONG *)&gt4.x3, &p, &flag);
                } else {
                    otz = RotAverage4(&work->burst[j][1], &work->burst[j + 1][1], &work->burst[j][0],
                                      &work->burst[j + 1][0], (PSXLONG *)&gt4.x0, (PSXLONG *)&gt4.x1,
                                      (PSXLONG *)&gt4.x2, (PSXLONG *)&gt4.x3, &p, &flag);
                }
                setUV4(&gt4, 0x80, 0x50, 0x9F, 0x50, 0x80, 0xCF, 0x9F, 0xCF);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gt4, ot, (u16)otz, 1);
                }
            }

            setPolyFT4(&ft4);
            ft4.tpage = work->texture[0] >> 16;
            ft4.clut = work->texture[0];
            setRGB0(&ft4, work->flare_r[i], work->flare_g[i], work->flare_b[i]);
            setVector(&quad[0], -work->config->flare_size, -work->config->flare_size, 0);
            setVector(&quad[1], work->config->flare_size, -work->config->flare_size, 0);
            setVector(&quad[2], -work->config->flare_size, work->config->flare_size, 0);
            setVector(&quad[3], work->config->flare_size, work->config->flare_size, 0);
            setVector(&pos, 0, -350, direction * 450);
            setVector(&rot, 0, 0, 0);
            setVector(&scale, 0x1000, 0x1000, 0x1000);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                              (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
            setUV4(&ft4, 0, 0x40, 0x3F, 0x40, 0, 0x7F, 0x3F, 0x7F);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
            }
            if (work->burst_scale[i] < 0x3000) {
                work->burst_scale[i] += 0x1000;
            } else {
                work->burst_scale[i] += 0x80;
            }
            if (work->flare_r[i] >= 16) {
                work->flare_r[i] -= work->config->fade;
            } else {
                work->flare_r[i] = 0;
            }
            if (work->flare_g[i] >= 16) {
                work->flare_g[i] -= work->config->fade;
            } else {
                work->flare_g[i] = 0;
            }
            if (work->flare_b[i] >= 16) {
                work->flare_b[i] -= work->config->fade;
            } else {
                work->flare_b[i] = 0;
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3 && work->flare_r[1] == 0 && work->flare_g[1] == 0 && work->flare_b[1] == 0) {
        return 2;
    }
    if (work->elapsed >= work->config->end[0]) {
        if (work->phase == 1) {
            work->phase = 2;
            return 1;
        }
        if (work->phase == 0) {
            work->phase = 1;
            return 4;
        }
    }
    return 0;
}

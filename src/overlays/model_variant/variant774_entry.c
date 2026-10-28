#include "../../types.h"
#include "variant774_entry.h"

/* Header 774's entry: a halo grows on a model point while a ring of spinning
 * blades forms around the slot; one by one the blades swing out and fly to
 * the opposing side trailing ribbons, then each bursts into a growing glow
 * with rays and a textured double ring while the ambient light dims and
 * recovers. */

static const VECTOR D_8013E374 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model774State *work;
    MATRIX light;
    MATRIX m;
    POLY_G3 g3;
    POLY_FT4 flare;
    POLY_F4 f4;
    POLY_FT4 ft4;
    POLY_G3 ray;
    POLY_FT4 halo;
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
    s32 slot;
    s32 otz;

    memset(&rot, 0, 8);
    memset(&pos, 0, 8);
    scale = D_8013E374;
    work = (Model774State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013E3D8[command];
        setVector(&work->blade[0], 0, 0, 0);
        setVector(&work->blade[1], 0, -work->config->height, 0);
        setVector(&work->blade[2], 0, 0, direction * work->config->front);
        setVector(&work->blade[3], 0, 0, direction * -work->config->back);
        for (i = 0; i < work->config->count; i++) {
            j = 4096 / work->config->count;
            setVector(&work->pos[i], work->config->radius * ccos(j * i % 4096) / 4096,
                      work->config->radius * csin(j * i % 4096) / 4096 - 350, -direction * 450);
            copyVector(&work->origin[i], &work->pos[i]);
            setVector(&work->step[i], 0, 0, 0);
            setVector(&work->angle[i], 0, 0, 0);
            setVector(&work->target[i], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                      work->config->spread * ((rand() - rand()) % 4096) / 4096 - 350, direction * 450);
            work->frame[i] = 0;
            work->scale[i] = 0x1000;
            work->state[i] = 0;
            work->head[i] = 0;
            work->trail_level[i] = 0x60;
            work->glow[i] = 0x40;
            for (j = 0; j < 32; j++) {
                setVector(&work->trail[i][j], 0, 0, 0);
                setVector(&work->rays[i][j],
                          work->config->ray_spread * (rand() % 4096 - rand() % 4096) / 4096,
                          work->config->ray_spread * (rand() % 4096 - rand() % 4096) / 4096, 0);
            }
        }
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 16; j++) {
                setVector(&work->inner[i][j], work->config->inner_radius * ccos(j * 256 % 4096) / 4096,
                          work->config->inner_radius * csin(j * 256 % 4096) / 4096,
                          -direction * work->config->inner_radius * ccos(256) / 4096);
                setVector(&work->outer[i][j], work->config->outer_radius * ccos(j * 256 % 4096) / 4096,
                          work->config->outer_radius * csin(j * 256 % 4096) / 4096,
                          -direction * work->config->outer_radius * ccos(256) / 4096);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E384[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->ambient = 0x800;
        work->phase = 0;
        work->level = 0;
        work->active = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->ambient > -0x200) {
            work->ambient -= step << 6;
        } else {
            work->ambient = -0x200;
        }
    } else if (work->phase == 2) {
        if (work->ambient < 0x800) {
            work->ambient += step << 6;
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
    if (work->phase == 0) {
        work->level += 4;
        if (work->level > 0x70) {
            work->level = 0x70;
            work->phase = 1;
        }
    } else if (work->state[work->config->count - 1] == 3) {
        if (work->level != 0) {
            work->level -= 4;
            if (work->level < 15) {
                work->level = 0;
            }
        }
    }
    if (work->level != 0) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->part), &m);
        setPolyFT4(&halo);
        setVector(&pos, m.t[0], m.t[1], m.t[2]);
        halo.tpage = work->texture[1] >> 16;
        halo.clut = work->texture[1];
        setUV4(&halo, 0, 32, 63, 32, 0, 95, 63, 95);
        setRGB0(&halo, work->level * 2, work->level * 2, work->level);
        GsSetLsMatrix(&light);
        RotTrans(&pos, (VECTOR *)m.t, &flag);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        GsSetLsMatrix(&m);
        setVector(&quad[0], -work->config->halo_size, -work->config->halo_size, 0);
        setVector(&quad[1], work->config->halo_size, -work->config->halo_size, 0);
        setVector(&quad[2], -work->config->halo_size, work->config->halo_size, 0);
        setVector(&quad[3], work->config->halo_size, work->config->halo_size, 0);
        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&halo.x0,
                          (PSXLONG *)&halo.x1, (PSXLONG *)&halo.x2, (PSXLONG *)&halo.x3, &p, &flag);
        if (otz >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&halo, ot, (u16)otz, 1);
        }
    }

    for (i = 0; i < work->config->count; i++) {
        if (work->state[i] < 2) {
            setPolyG3(&g3);
            setPolyFT4(&flare);
            setRGB0(&g3, work->level, work->level, 0);
            setRGB1(&g3, 0, 0, 0);
            setRGB2(&g3, 0, 0, 0);
            setRGB0(&flare, work->level, work->level, work->level);
            flare.tpage = work->texture[2] >> 16;
            flare.clut = work->texture[2];
            copyVector(&rot, &work->angle[i]);
            rot.vz += work->elapsed * work->config->spin % 4096;
            copyVector(&pos, &work->pos[i]);
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            setRGB0(&g3, work->level, work->level, work->level);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            setVector(&quad[0], -work->config->size, -work->config->size, 0);
            setVector(&quad[1], work->config->size, -work->config->size, 0);
            setVector(&quad[2], -work->config->size, work->config->size, 0);
            setVector(&quad[3], work->config->size, work->config->size, 0);
            otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&flare.x0,
                              (PSXLONG *)&flare.x1, (PSXLONG *)&flare.x2, (PSXLONG *)&flare.x3, &p, &flag);
            setUV4(&flare, work->elapsed % 3 * 32, 0, work->elapsed % 3 * 32 + 31, 0,
                   work->elapsed % 3 * 32, 31, work->elapsed % 3 * 32 + 31, 31);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)otz, 1);
            }

            setRGB0(&g3, work->level, work->level, 0);
            copyVector(&rot, &work->angle[i]);
            rot.vz += work->elapsed * work->config->spin % 4096 + 0x555;
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            setRGB0(&g3, work->level, work->level, work->level);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }

            setRGB0(&g3, work->level, work->level, 0);
            copyVector(&rot, &work->angle[i]);
            rot.vz += work->elapsed * work->config->spin % 4096 + 0xAAB;
            GsSetLsMatrix(&light);
            RotTrans(&pos, (VECTOR *)m.t, &flag);
            RotMatrix(&rot, &m);
            MulMatrix2(&light, &m);
            ScaleMatrix(&m, &scale);
            GsSetLsMatrix(&m);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            setRGB0(&g3, work->level, work->level, work->level);
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[2]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
            copyVector(&quad[0], &work->blade[0]);
            copyVector(&quad[1], &work->blade[1]);
            copyVector(&quad[2], &work->blade[3]);
            quad[0].vx *= 2;
            quad[0].vy *= 2;
            otz = RotAverage3(&quad[0], &quad[1], &quad[2], (PSXLONG *)&g3.x0, (PSXLONG *)&g3.x1,
                              (PSXLONG *)&g3.x2, &p, &flag);
            if (otz >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&g3, ot, (u16)otz, 1);
            }
        }
    }

    if (work->config->launch < work->elapsed) {
        setPolyF4(&f4);
        for (i = 0; i < work->config->count; i++) {
            if (work->head[i] >= 5) {
                if (work->trail_level[i] != 0) {
                    setVector(&pos, 0, 0, 0);
                    setVector(&rot, 0, 0, 0);
                    setVector(&scale, 0x1000, 0x1000, 0x1000);
                    GsSetLsMatrix(&light);
                    RotTrans(&pos, (VECTOR *)m.t, &flag);
                    RotMatrix(&rot, &m);
                    MulMatrix2(&light, &m);
                    ScaleMatrix(&m, &scale);
                    GsSetLsMatrix(&m);
                    /* Indexing the older point as J - 1 keeps GCC from strength-reducing
                     * the trail address, as retail does. */
                    for (j = work->head[i] - 1, k = 1; j >= work->head[i] - 4; j--, k++) {
                        setRGB0(&f4, work->trail_level[i] / k * 2, work->trail_level[i] / k * 2,
                                work->trail_level[i] / k);
                        setVector(&quad[0], work->trail[i][j].vx,
                                  work->trail[i][j].vy - work->config->size / 4, work->trail[i][j].vz);
                        setVector(&quad[1], work->trail[i][j].vx,
                                  work->trail[i][j].vy + work->config->size / 4, work->trail[i][j].vz);
                        setVector(&quad[2], work->trail[i][j - 1].vx,
                                  work->trail[i][j - 1].vy - work->config->size / 8, work->trail[i][j - 1].vz);
                        setVector(&quad[3], work->trail[i][j - 1].vx,
                                  work->trail[i][j - 1].vy + work->config->size / 8, work->trail[i][j - 1].vz);
                        otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&f4.x0,
                                          (PSXLONG *)&f4.x1, (PSXLONG *)&f4.x2, (PSXLONG *)&f4.x3, &p, &flag);
                        if (otz >= 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&f4, ot, (u16)otz, 1);
                        }
                    }
                    if (work->pos[i].vz == direction * 450) {
                        if (work->trail_level[i] >= 16) {
                            work->trail_level[i] -= 12;
                        } else {
                            work->trail_level[i] = 0;
                        }
                    }
                }
            }
        }
    }

    if (work->config->launch < work->elapsed) {
        for (i = 0; i < work->active; i++) {
            if (ABS(work->pos[i].vz) <= 450 && work->state[i] < 2) {
                work->step[i].vz = work->target[i].vz - work->origin[i].vz;
                if (work->frame[i] < work->config->steps / 2) {
                    work->pos[i].vx = work->origin[i].vx;
                    work->pos[i].vy = work->origin[i].vy;
                    /* Each blade swings on its own path. The "- -(...)" and
                     * "r - (c - C)" spellings stop GCC from reassociating the
                     * ratan2 offsets, matching retail's evaluation order. */
                    if (i == 0) {
                        work->step[i].vx = -work->config->swing * csin(-(work->frame[i] * 64) % 4096) / 4096 + 1;
                        work->step[i].vy = 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  0,
                                  csin(work->frame[i] * 64) / 4 - ratan2(work->step[i].vx, work->step[i].vz),
                                  0);
                    } else if (i == 1) {
                        work->step[i].vx = -work->config->swing * csin(-(work->frame[i] * 64) % 4096) / 4096 + 1;
                        work->step[i].vy = work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (ratan2(work->step[i].vz, work->step[i].vy) - (csin(work->frame[i] * 64) * 768 / 4096 - 0x200)),
                                  csin(work->frame[i] * 64) * 768 / 4096 - -(-ratan2(work->step[i].vx, work->step[i].vz) + 0x200),
                                  0);
                    } else if (i == 2) {
                        work->step[i].vx = 1;
                        work->step[i].vy = work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (csin(-(work->frame[i] * 64)) * 1280 / 4096 - -(ratan2(work->step[i].vz, work->step[i].vy) + 0x400)),
                                  0,
                                  0);
                    } else if (i == 3) {
                        work->step[i].vx = work->config->swing * csin(-(work->frame[i] * 64) % 4096) / 4096 + 1;
                        work->step[i].vy = work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (csin(-(work->frame[i] * 64)) * 768 / 4096 - -(ratan2(work->step[i].vz, work->step[i].vy) + 0x200)),
                                  csin(-(work->frame[i] * 64)) * 768 / 4096 - -(-ratan2(work->step[i].vx, work->step[i].vz) + -0x200),
                                  0);
                    } else if (i == 4) {
                        work->step[i].vx = work->config->swing * csin(-(work->frame[i] * 64) % 4096) / 4096 + 1;
                        work->step[i].vy = 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  0,
                                  csin(-(work->frame[i] * 64)) / 4 - ratan2(work->step[i].vx, work->step[i].vz),
                                  0);
                    } else if (i == 5) {
                        work->step[i].vx = work->config->swing * csin(-(work->frame[i] * 64) % 4096) / 4096 + 1;
                        work->step[i].vy = -work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (csin(work->frame[i] * 64) * 768 / 4096 - -(ratan2(work->step[i].vz, work->step[i].vy) + 0x600)),
                                  csin(-(work->frame[i] * 64)) * 768 / 4096 - -(-ratan2(work->step[i].vx, work->step[i].vz) + -0x200),
                                  0);
                    } else if (i == 6) {
                        work->step[i].vx = 1;
                        work->step[i].vy = -work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (csin(work->frame[i] * 64) * 1280 / 4096 - -(ratan2(work->step[i].vz, work->step[i].vy) + 0x400)),
                                  0,
                                  0);
                    } else if (i == 7) {
                        work->step[i].vx = work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vy = -work->config->swing * csin(work->frame[i] * 64 % 4096) / 4096 + 1;
                        work->step[i].vz /= work->config->steps * 8;
                        setVector(&work->angle[i],
                                  -direction * (ratan2(work->step[i].vz, work->step[i].vy) - (csin(-(work->frame[i] * 64)) * 768 / 4096 - 0x600)),
                                  csin(work->frame[i] * 64) * 768 / 4096 - -(-ratan2(work->step[i].vx, work->step[i].vz) + 0x200),
                                  0);
                    }
                } else {
                    if (work->state[i] == 0) {
                        work->origin[i].vx = work->pos[i].vx;
                        work->origin[i].vy = work->pos[i].vy;
                        work->origin[i].vz = work->pos[i].vz;
                        work->state[i] = 1;
                    }
                    work->step[i].vx = work->target[i].vx - work->origin[i].vx;
                    work->step[i].vx /= work->config->steps / 8;
                    work->step[i].vy = work->target[i].vy - work->origin[i].vy;
                    work->step[i].vy /= work->config->steps / 8;
                    work->step[i].vz = work->target[i].vz - work->origin[i].vz;
                    work->step[i].vz /= work->config->steps / 8;
                    setVector(&work->angle[i], ratan2(work->step[i].vz, work->step[i].vy) - 0x400,
                              -ratan2(work->step[i].vx, work->step[i].vz), 0);
                }
                addVector(&work->pos[i], &work->step[i]);
                copyVector(&work->trail[i][work->head[i]], &work->pos[i]);
                work->head[i] = (work->head[i] + 1) % 32;
                work->frame[i]++;
                if (ABS(work->pos[i].vz) >= 450) {
                    work->pos[i].vz = direction * 450;
                    work->state[i] = 2;
                }
            } else if (work->state[i] == 1) {
                work->state[i] = 2;
            }
        }
        if (work->frames % work->config->period == 0 && work->active < work->config->count) {
            work->active++;
        }
        work->frames++;
    }

    for (i = 0; i < work->config->count; i++) {
        setPolyFT4(&ft4);
        setPolyG3(&ray);
        ft4.tpage = work->texture[1] >> 16;
        ft4.clut = work->texture[1];
        setUV4(&ft4, 0, 32, 63, 32, 0, 95, 63, 95);
        if (work->state[i] >= 3) {
            if (work->glow[i] != 0) {
                setVector(&quad[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&quad[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&quad[3], work->config->glow_size, work->config->glow_size, 0);
                setRGB0(&ft4, work->glow[i] * 2, work->glow[i] * 2, work->glow[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                otz = RotAverage4(&quad[0], &quad[1], &quad[2], &quad[3], (PSXLONG *)&ft4.x0,
                                  (PSXLONG *)&ft4.x1, (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                if (otz >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 1);
                }
                setVector(&quad[0], -work->config->ray_width, 0, 0);
                setVector(&quad[1], work->config->ray_width, 0, 0);
                setRGB0(&ray, 0, 0, 0);
                setRGB1(&ray, work->glow[i] * 2, work->glow[i] * 2, work->glow[i]);
                setRGB2(&ray, work->glow[i] * 2, work->glow[i] * 2, work->glow[i]);
                setVector(&rot, 0, 0, 0);
                setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < work->config->ray_count; j++) {
                    otz = RotAverage3(&work->rays[i][j], &quad[0], &quad[1], (PSXLONG *)&ray.x0,
                                      (PSXLONG *)&ray.x1, (PSXLONG *)&ray.x2, &p, &flag);
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ray, ot, (u16)otz, 1);
                    }
                }
                setPolyFT4(&ft4);
                ft4.tpage = work->texture[0] >> 16;
                ft4.clut = work->texture[0];
                setUV4(&ft4, 0x80, 0, 0x9F, 0, 0x80, 0x7F, 0x9F, 0x7F);
                setRGB0(&ft4, work->glow[i] * 2, work->glow[i] * 2, work->glow[i]);
                setVector(&scale, work->scale[i], work->scale[i], work->scale[i]);
                GsSetLsMatrix(&light);
                RotTrans(&work->target[i], (VECTOR *)m.t, &flag);
                RotMatrix(&rot, &m);
                MulMatrix2(&light, &m);
                ScaleMatrix(&m, &scale);
                GsSetLsMatrix(&m);
                for (j = 0; j < 16; j++) {
                    if (j == 15) {
                        otz = RotAverage4(&work->inner[i][15], &work->inner[i][0], &work->outer[i][15],
                                          &work->outer[i][0], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    } else {
                        otz = RotAverage4(&work->inner[i][j], &work->inner[i][j + 1], &work->outer[i][j],
                                          &work->outer[i][j + 1], (PSXLONG *)&ft4.x0, (PSXLONG *)&ft4.x1,
                                          (PSXLONG *)&ft4.x2, (PSXLONG *)&ft4.x3, &p, &flag);
                    }
                    if (otz >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&ft4, ot, (u16)otz, 3);
                    }
                }
                if (work->scale[i] < 0x5DC0) {
                    work->scale[i] += work->config->grow;
                } else {
                    work->scale[i] += work->config->fade_grow;
                    if (work->glow[i] >= 16) {
                        work->glow[i] -= 4;
                    } else {
                        work->glow[i] = 0;
                    }
                }
            }
        }
    }

    work->elapsed += step;
    PopMatrix();
    for (i = 0; i < work->config->count - 1; i++) {
        if (work->state[i] == 2) {
            work->state[i] = 3;
            return 4;
        }
    }
    if (work->state[work->config->count - 1] == 2 && work->phase == 1) {
        work->phase = 2;
        work->state[work->config->count - 1] = 3;
        return 1;
    }
    return (work->phase == 3) * 2;
}
